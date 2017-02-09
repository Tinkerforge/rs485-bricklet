/* rs485-bricklet
 * Copyright (C) 2017 Ishraq Ibne Ashraf <ishraq@tinkerforge.com>
 *
 * modbus.h: Modbus implementation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "modbus.h"

#include "bricklib2/utility/crc16.h"
#include "bricklib2/hal/system_timer/system_timer.h"

#include "xmc_uart.h"
#include "xmc_scu.h"

void modbus_init(RS485 *rs485) {
  if(rs485->baudrate > 19200) {
    rs485->modbus_rtu.time_4_chars_us = 1750;
  }
  else {
    // Always one start bit.
    uint8_t bit_count = 1;

    bit_count += rs485->wordlength;

    if(rs485->parity != PARITY_NONE) {
      bit_count++;
    }

    bit_count += rs485->stopbits;

    rs485->modbus_rtu.time_4_chars_us = (4 * bit_count * 1000000) / rs485->baudrate;
  }

  rs485->modbus_rtu.request.id = 0;
  rs485->modbus_rtu.tx_done = false;
  rs485->modbus_rtu.rx_rb_last_length = 0;
  rs485->modbus_rtu.request.cb_invoke = false;
  rs485->modbus_rtu.request.rx_frame = &rs485->buffer[0];
  rs485->modbus_rtu.state_wire = MODBUS_RTU_WIRE_STATE_INIT;
  rs485->modbus_rtu.time_ref_go_to_state_idle = system_timer_get_ms();
  rs485->modbus_rtu.request.state = MODBUS_REQUEST_PROCESS_STATE_READY;
  rs485->modbus_rtu.request.tx_frame = &rs485->buffer[rs485->buffer_size_rx];

  memset(rs485->modbus_rtu.request.rx_frame, 0, rs485->buffer_size_rx);
  memset(rs485->modbus_rtu.request.tx_frame, 0, RS485_BUFFER_SIZE - rs485->buffer_size_rx);
  memset(&rs485->modbus_rtu.slave_error_counters,
         0,
         sizeof(RS485ModbusSlaveErrorCounters));
  memset(&rs485->modbus_rtu.master_error_counters,
         0,
         sizeof(RS485ModbusMasterErrorCounters));
}

void modbus_clear_request(RS485 *rs485) {
  rs485->modbus_rtu.request.id++;

  ringbuffer_init(&rs485->ringbuffer_rx, rs485->buffer_size_rx, &rs485->buffer[0]);
  ringbuffer_init(&rs485->ringbuffer_tx,
                  RS485_BUFFER_SIZE-rs485->buffer_size_rx,
                  &rs485->buffer[rs485->buffer_size_rx]);

  rs485->modbus_rtu.rx_rb_last_length = 0;
  rs485->modbus_rtu.request.cb_invoke = false;
  rs485->modbus_rtu.request.state = MODBUS_REQUEST_PROCESS_STATE_READY;
}

bool modbus_slave_check_address(RS485 *rs485) {
  return (rs485->modbus_rtu.request.rx_frame[0] == rs485->modbus_slave_address);
}

void modbus_start_tx_from_buffer(RS485 *rs485) {
  rs485->modbus_rtu.state_wire = MODBUS_RTU_WIRE_STATE_TX;

	XMC_USIC_CH_TXFIFO_EnableEvent(RS485_USIC, XMC_USIC_CH_TXFIFO_EVENT_CONF_STANDARD);
	XMC_USIC_CH_TriggerServiceRequest(RS485_USIC, RS485_SERVICE_REQUEST_TX);
}

bool modbus_check_frame_checksum(RS485 *rs485) {
  uint8_t checksum_gen[2];

  crc16(rs485->modbus_rtu.request.rx_frame,
        rs485->modbus_rtu.rx_rb_last_length - 2,
        &checksum_gen[0]);

  uint8_t *checksum_recvd =
    &rs485->modbus_rtu.request.rx_frame[rs485->modbus_rtu.rx_rb_last_length - 2];

  return (checksum_recvd[0] == checksum_gen[0] && checksum_recvd[1] == checksum_gen[1]);
}

void modbus_update_rtu_wire_state_machine(RS485 *rs485) {
  uint16_t rx_rb_used = 0;

	if(rs485->modbus_rtu.state_wire == MODBUS_RTU_WIRE_STATE_INIT) {
    if(system_timer_is_time_elapsed_ms(rs485->modbus_rtu.time_ref_go_to_state_idle,
                                       rs485->modbus_rtu.time_4_chars_us/1000 + 1)) {
      rs485->modbus_rtu.state_wire = MODBUS_RTU_WIRE_STATE_IDLE;
    }
	}
	else if(rs485->modbus_rtu.state_wire == MODBUS_RTU_WIRE_STATE_IDLE) {
    if(rs485->modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_READY) {
      return;
    }

    rx_rb_used = ringbuffer_get_used(&rs485->ringbuffer_rx);

    if(rx_rb_used == 0) {
      return;
    }

    if((rx_rb_used > 0) &&
       (rx_rb_used > rs485->modbus_rtu.rx_rb_last_length)) {
        rs485->modbus_rtu.rx_rb_last_length = rx_rb_used;
        rs485->modbus_rtu.time_ref_go_to_state_idle = system_timer_get_ms();

        return;
    }

    if((rx_rb_used == rs485->modbus_rtu.rx_rb_last_length) &&
       system_timer_is_time_elapsed_ms(rs485->modbus_rtu.time_ref_go_to_state_idle,
                                       rs485->modbus_rtu.time_4_chars_us/1000 + 1)) {
      if(rs485->mode == MODE_MODBUS_SLAVE) {
        if(!modbus_slave_check_address(rs485)) {
          // The frame is not for this slave.
          modbus_clear_request(rs485);

          return;
        }

        if(!modbus_check_frame_checksum(rs485)) {
          // Frame checksum mismatch.
          modbus_clear_request(rs485);

          return;
        }

        modbus_init_new_request(rs485,
                                MODBUS_REQUEST_PROCESS_STATE_SLAVE_PROCESSING_REQUEST,
                                rx_rb_used);
      }
    }
	}
	else if(rs485->modbus_rtu.state_wire == MODBUS_RTU_WIRE_STATE_TX) {
    if(rs485->modbus_rtu.tx_done) {
      rs485->modbus_rtu.tx_done = false;
      rs485->modbus_rtu.state_wire = MODBUS_RTU_WIRE_STATE_IDLE;

      if(rs485->mode == MODE_MODBUS_SLAVE) {
        // In slave mode completed TX means end of a request handling.
        modbus_clear_request(rs485);
      }
    }
	}
}

void modbus_report_exception(RS485 *rs485, uint8_t function_code, uint8_t exception_code) {
  ModbusExceptionResponse modbus_exception_response;
  uint8_t *modbus_exception_response_ptr = (uint8_t *)&modbus_exception_response;

  modbus_exception_response.address = rs485->modbus_slave_address;
  modbus_exception_response.function_code = function_code + 0x80;
  modbus_exception_response.exception_code = exception_code;

  crc16(modbus_exception_response_ptr,
        sizeof(ModbusExceptionResponse) - 2,
        modbus_exception_response.checksum);

  ringbuffer_init(&rs485->ringbuffer_tx,
                  RS485_BUFFER_SIZE - rs485->buffer_size_rx,
                  &rs485->buffer[rs485->buffer_size_rx]);

  for(uint8_t i = 0; i < sizeof(ModbusExceptionResponse); i++) {
    ringbuffer_add(&rs485->ringbuffer_tx, modbus_exception_response_ptr[i]);
  }

  modbus_start_tx_from_buffer(rs485);
}

void modbus_init_new_request(RS485 *rs485, RS485ModbusRequestState state, uint16_t length) {
  rs485->modbus_rtu.request.id++;
  rs485->modbus_rtu.request.state = state;
  rs485->modbus_rtu.request.length = length;
  rs485->modbus_rtu.request.cb_invoke = true;
}
