/* rs485-bricklet
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 * Copyright (C) 2017 Ishraq Ibne Ashraf <ishraq@tinkerforge.com>
 *
 * rs485.h: RS485 handling
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

#ifndef RS485_H
#define RS485_H

#include "configs/config_rs485.h"

#include "bricklib2/utility/ringbuffer.h"
#include "bricklib2/utility/led_flicker.h"

#include <stdint.h>

#define RS485_BUFFER_SIZE 1024*10

#define RS485_OVERSAMPLING 16
#define RS485_BAUDRATE_MIN 100
#define RS485_BAUDRATE_MAX 2000000

#define RS485_MODBUS_RTU_FRAME_SIZE_MAX 256

// Modbus specific.
#define MODBUS_FC_READ_COILS 0x01
#define MODBUS_FC_WRITE_SINGLE_COIL 0x05

#define MODBUS_DEFAULT_SLAVE_ADDRESS 1
#define MODBUS_DEFAULT_MASTER_REQUEST_TIMEOUT 1000 // In milliseconds.

typedef enum {
	MODE_RS485 = 0,
	MODE_MODBUS_MASTER = 1,
	MODE_MODBUS_SLAVE = 2,
} RS485Mode;

typedef enum {
	PARITY_NONE = 0,
	PARITY_ODD = 1,
	PARITY_EVEN = 2,
} RS485Parity;

typedef enum {
	STOPBITS_1 = 1,
	STOPBITS_2 = 2,
} RS485Stopbits;

typedef enum {
	WORDLENGTH_5 = 5,
	WORDLENGTH_6 = 6,
	WORDLENGTH_7 = 7,
	WORDLENGTH_8 = 8,
} RS485Wordlength;

typedef enum {
	DUPLEX_HALF = 0,
	DUPLEX_FULL = 1,
} RS485Duplex;

// TODO: Can we detect more errors? Framing error?
typedef enum {
	ERROR_OVERRUN = 1 << 0,
	ERROR_PARITY  = 1 << 1,
} RS485Error;

typedef enum {
	MODBUS_RTU_WIRE_STATE_INIT = 0,
	MODBUS_RTU_WIRE_STATE_IDLE = 1,
	MODBUS_RTU_WIRE_STATE_RX = 2,
	MODBUS_RTU_WIRE_STATE_TX = 3,
} RS485ModbusRTUWireState;

typedef enum {
	MODBUS_REQUEST_PROCESS_STATE_READY = 0,
	MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE = 1,
	MODBUS_REQUEST_PROCESS_STATE_SLAVE_PROCESSING_REQUEST = 2,
} RS485ModbusRequestState;

typedef struct {
	uint8_t id;
	bool cb_invoke;
	uint16_t length;
	uint8_t *rx_frame;
	uint8_t *tx_frame;
	RS485ModbusRequestState state;
} RS485ModbusRequest;

typedef struct {
	uint32_t error_count_timeout;
	uint32_t error_count_checksum;
	uint32_t error_count_invalid_fc;
	uint32_t error_count_frame_size;
	uint32_t error_count_invalid_data;
	uint32_t error_count_fc_not_implemented;
	uint32_t error_count_invalid_slave_address;
} RS485ModbusMasterErrorCounters;

typedef struct {
	uint32_t error_count_timeout;
	uint32_t error_count_checksum;
	uint32_t error_count_frame_size;
	uint32_t error_count_invalid_data;
	uint32_t error_count_fc_not_implemented;
} RS485ModbusSlaveErrorCounters;

typedef struct {
	bool tx_done;
	uint32_t time_4_chars_us;
	uint16_t rx_rb_last_length;
	RS485ModbusRequest request;
	uint32_t time_ref_go_to_state_idle;
	RS485ModbusRTUWireState state_wire;
	RS485ModbusSlaveErrorCounters slave_error_counters;
	RS485ModbusMasterErrorCounters master_error_counters;
} RS485ModbusRTU;

typedef struct {
	RS485Mode mode;
	uint32_t baudrate;
	RS485Parity parity;
	RS485Duplex duplex;
	RS485Stopbits stopbits;
	uint16_t buffer_size_rx;
	Ringbuffer ringbuffer_tx;
	Ringbuffer ringbuffer_rx;

	// Callback configuration.
	bool read_callback_enabled;

	RS485Wordlength wordlength;
	uint32_t error_count_parity;
	uint32_t error_count_overrun;
	LEDFlickerState red_led_state;
	uint32_t duplex_shift_setting;
	LEDFlickerState yellow_led_state;
	bool error_count_callback_enabled;

	// Modbus specific.
	RS485ModbusRTU modbus_rtu;
	uint8_t modbus_slave_address;
	uint32_t modbus_master_request_timeout;

	uint8_t buffer[RS485_BUFFER_SIZE];
} RS485;

void rs485_init(RS485 *rs485);
void rs485_tick(RS485 *rs485);
void rs485_init_buffer(RS485 *rs485);
void rs485_apply_configuration(RS485 *rs485);

#endif
