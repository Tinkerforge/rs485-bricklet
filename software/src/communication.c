/* rs485-bricklet
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 * Copyright (C) 2017 Ishraq Ibne Ashraf <ishraq@tinkerforge.com>
 *
 * communication.c: TFP protocol message handling
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

#include "communication.h"
#include "configs/config.h"

#include "rs485.h"
#include "modbus.h"

#include "bricklib2/hal/system_timer/system_timer.h"
#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/utility/ringbuffer.h"
#include "bricklib2/utility/util_definitions.h"
#include "bricklib2/utility/communication_callback.h"
#include "bricklib2/bootloader/bootloader.h"
#include "bricklib2/utility/crc16.h"

#include "xmc_usic.h"
#include "xmc_uart.h"

#define MESSAGE_LENGTH 60

extern RS485 rs485;
extern RS485ModbusStreamChunking stream_chunking;

static bool modbus_slave_check_current_request(const uint8_t request_id) {
	if((rs485.mode != MODE_MODBUS_SLAVE_RTU) ||
	   (rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_SLAVE_PROCESSING_REQUEST) ||
	   (request_id != rs485.modbus_rtu.request.id)) {
	 return false;
	}

	return true;
}

static void modbus_store_tx_frame_data_bytes(const uint8_t *data,
                                             const uint16_t length) {
	for(uint16_t i = 0; i < length; i++) {
		ringbuffer_add(&rs485.ringbuffer_tx, data[i]);
	}
}

static void modbus_store_tx_frame_data_shorts(const uint16_t *data,
                                              const uint16_t length) {
	uint16_t u16_network_order = 0;
	uint8_t *_data = (uint8_t *)&u16_network_order;

	for(uint16_t i = 0; i < length; i++) {
		u16_network_order = HTONS(data[i]);

		ringbuffer_add(&rs485.ringbuffer_tx, _data[0]);
		ringbuffer_add(&rs485.ringbuffer_tx, _data[1]);
	}
}

static void modbus_add_tx_frame_checksum(void) {
	uint8_t checksum_gen[2];

	crc16(rs485.ringbuffer_tx.buffer,
	      ringbuffer_get_used(&rs485.ringbuffer_tx),
	      &checksum_gen[0]);

	ringbuffer_add(&rs485.ringbuffer_tx, checksum_gen[0]);
	ringbuffer_add(&rs485.ringbuffer_tx, checksum_gen[1]);
}

static bool send_stream_chunks(uint8_t function_code, void *cb) {
	uint16_t temp1 = 0;

	if(!rs485.modbus_rtu.request.stream_chunking->in_progress) {
		modbus_clear_request(&rs485);

		return true;
	}

	if(rs485.modbus_rtu.request.stream_chunking->chunk_total == 0 ||
	   rs485.modbus_rtu.request.stream_chunking->stream_total == 0) {
		modbus_clear_request(&rs485);

		return true;
	}

	if(!bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		return false;
	}

	if(function_code == FID_CALLBACK_MODBUS_READ_COILS_RESPONSE_LOW_LEVEL) {
		ModbusReadCoilsResponseLowLevelCallback *_cb = (ModbusReadCoilsResponseLowLevelCallback *)cb;

		if(rs485.modbus_rtu.request.stream_chunking->chunk_current < rs485.modbus_rtu.request.stream_chunking->chunk_total) {
			_cb->stream_total_length = rs485.modbus_rtu.request.stream_chunking->stream_total;
			_cb->stream_chunk_offset = rs485.modbus_rtu.request.stream_chunking->chunk_current * sizeof(_cb->stream_chunk_data);

			if((_cb->stream_chunk_offset == 0) ||
			   ((_cb->stream_total_length - _cb->stream_chunk_offset) > sizeof(_cb->stream_chunk_data))) {
					memcpy(&_cb->stream_chunk_data,
					       &rs485.modbus_rtu.request.rx_frame[_cb->stream_chunk_offset + 3],
					       sizeof(_cb->stream_chunk_data));
			}
			else {
				memcpy(&_cb->stream_chunk_data,
				       &rs485.modbus_rtu.request.rx_frame[_cb->stream_chunk_offset + 3],
				       _cb->stream_total_length - _cb->stream_chunk_offset);
			}

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t *)_cb,
			                                       sizeof(ModbusReadCoilsResponseLowLevelCallback));
		}

		if(rs485.modbus_rtu.request.stream_chunking->chunk_current == rs485.modbus_rtu.request.stream_chunking->chunk_total - 1) {
			modbus_clear_request(&rs485);
		}
		else {
			rs485.modbus_rtu.request.stream_chunking->chunk_current++;
		}

		return true;
	}
	else if(function_code == FID_CALLBACK_MODBUS_READ_HOLDING_REGISTERS_RESPONSE_LOW_LEVEL) {
		ModbusReadHoldingRegistersResponseLowLevelCallback *_cb = (ModbusReadHoldingRegistersResponseLowLevelCallback *)cb;

		if(rs485.modbus_rtu.request.stream_chunking->chunk_current < rs485.modbus_rtu.request.stream_chunking->chunk_total) {
			_cb->stream_total_length = rs485.modbus_rtu.request.stream_chunking->stream_total;
			_cb->stream_chunk_offset = rs485.modbus_rtu.request.stream_chunking->chunk_current * (sizeof(_cb->stream_chunk_data) / 2);

			if((_cb->stream_chunk_offset == 0) ||
			   ((_cb->stream_total_length - _cb->stream_chunk_offset) > (sizeof(_cb->stream_chunk_data) / 2))) {
			     uint16_t *_data = (uint16_t *)&rs485.modbus_rtu.request.rx_frame[(_cb->stream_chunk_offset * 2) + 3];

			     for(uint16_t i = 0; i < (sizeof(_cb->stream_chunk_data) / 2); i++) {
			     _cb->stream_chunk_data[i] = NTOHS(_data[i]);
			    }
			}
			else {
				uint16_t *_data = (uint16_t *)&rs485.modbus_rtu.request.rx_frame[(_cb->stream_chunk_offset * 2) + 3];

				for(uint16_t i = 0; i < _cb->stream_total_length - _cb->stream_chunk_offset; i++) {
					_cb->stream_chunk_data[i] = NTOHS(_data[i]);
				}
			}

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t *)_cb,
			                                       sizeof(ModbusReadHoldingRegistersResponseLowLevelCallback));
		}

		if(rs485.modbus_rtu.request.stream_chunking->chunk_current == rs485.modbus_rtu.request.stream_chunking->chunk_total - 1) {
			modbus_clear_request(&rs485);
		}
		else {
			rs485.modbus_rtu.request.stream_chunking->chunk_current++;
		}

		return true;
	}
	else if(function_code == FID_CALLBACK_MODBUS_WRITE_MULTIPLE_COILS_REQUEST_LOW_LEVEL) {
		ModbusWriteMultipleCoilsRequestLowLevelCallback *_cb = (ModbusWriteMultipleCoilsRequestLowLevelCallback *)cb;

		if(rs485.modbus_rtu.request.stream_chunking->chunk_current < rs485.modbus_rtu.request.stream_chunking->chunk_total) {
			_cb->stream_total_length = rs485.modbus_rtu.request.stream_chunking->stream_total;
			_cb->stream_chunk_offset = rs485.modbus_rtu.request.stream_chunking->chunk_current * sizeof(_cb->stream_chunk_data);

			if((_cb->stream_chunk_offset == 0) ||
			   ((_cb->stream_total_length - _cb->stream_chunk_offset) > sizeof(_cb->stream_chunk_data))) {
					memcpy(&_cb->stream_chunk_data,
					       &rs485.modbus_rtu.request.rx_frame[_cb->stream_chunk_offset + 7],
					       sizeof(_cb->stream_chunk_data));
			}
			else {
				memcpy(&_cb->stream_chunk_data,
				       &rs485.modbus_rtu.request.rx_frame[_cb->stream_chunk_offset + 7],
				       _cb->stream_total_length - _cb->stream_chunk_offset);
			}

			_cb->request_id = rs485.modbus_rtu.request.id;

			memcpy(&temp1, &rs485.modbus_rtu.request.rx_frame[2], 2);
			_cb->starting_address = NTOHS(temp1);

			memcpy(&temp1, &rs485.modbus_rtu.request.rx_frame[4], 2);
			_cb->count = NTOHS(temp1);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t *)_cb,
			                                       sizeof(ModbusWriteMultipleCoilsRequestLowLevelCallback));
		}

		if(rs485.modbus_rtu.request.stream_chunking->chunk_current == rs485.modbus_rtu.request.stream_chunking->chunk_total - 1) {
			memset(&stream_chunking, 0, sizeof(RS485ModbusStreamChunking));

			rs485.modbus_rtu.request.cb_invoke = false;
			rs485.modbus_rtu.state_wire = MODBUS_RTU_WIRE_STATE_IDLE;
			rs485.modbus_rtu.request.master_request_timed_out = false;
		}
		else {
			rs485.modbus_rtu.request.stream_chunking->chunk_current++;
		}

		return true;
	}
	else if(function_code == FID_CALLBACK_MODBUS_WRITE_MULTIPLE_REGISTERS_REQUEST_LOW_LEVEL) {
		ModbusWriteMultipleRegistersRequestLowLevelCallback *_cb = (ModbusWriteMultipleRegistersRequestLowLevelCallback *)cb;

		if(rs485.modbus_rtu.request.stream_chunking->chunk_current < rs485.modbus_rtu.request.stream_chunking->chunk_total) {
			_cb->stream_total_length = rs485.modbus_rtu.request.stream_chunking->stream_total;
			_cb->stream_chunk_offset = rs485.modbus_rtu.request.stream_chunking->chunk_current * (sizeof(_cb->stream_chunk_data) / 2);

			if((_cb->stream_chunk_offset == 0) ||
			   ((_cb->stream_total_length - _cb->stream_chunk_offset) > (sizeof(_cb->stream_chunk_data) / 2))) {
					memcpy(&_cb->stream_chunk_data,
					       &rs485.modbus_rtu.request.rx_frame[(_cb->stream_chunk_offset * 2) + 7],
					       sizeof(_cb->stream_chunk_data));
			}
			else {
				memcpy(&_cb->stream_chunk_data,
				       &rs485.modbus_rtu.request.rx_frame[(_cb->stream_chunk_offset * 2) + 7],
				       (_cb->stream_total_length - _cb->stream_chunk_offset) * 2);
			}

			_cb->request_id = rs485.modbus_rtu.request.id;

			memcpy(&temp1, &rs485.modbus_rtu.request.rx_frame[2], 2);
			_cb->starting_address = NTOHS(temp1);

			memcpy(&temp1, &rs485.modbus_rtu.request.rx_frame[4], 2);
			_cb->count = NTOHS(temp1);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t *)_cb,
			                                       sizeof(ModbusWriteMultipleRegistersRequestLowLevelCallback));
		}

		if(rs485.modbus_rtu.request.stream_chunking->chunk_current == rs485.modbus_rtu.request.stream_chunking->chunk_total - 1) {
			memset(&stream_chunking, 0, sizeof(RS485ModbusStreamChunking));

			rs485.modbus_rtu.request.cb_invoke = false;
			rs485.modbus_rtu.state_wire = MODBUS_RTU_WIRE_STATE_IDLE;
			rs485.modbus_rtu.request.master_request_timed_out = false;
		}
		else {
			rs485.modbus_rtu.request.stream_chunking->chunk_current++;
		}

		return true;
	}
	else {
		return false;
	}
}

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_WRITE: return write(message, response);
		case FID_READ: return read(message, response);
		case FID_ENABLE_READ_CALLBACK: return enable_read_callback(message);
		case FID_DISABLE_READ_CALLBACK: return disable_read_callback(message);
		case FID_IS_READ_CALLBACK_ENABLED: return is_read_callback_enabled(message, response);
		case FID_SET_RS485_CONFIGURATION: return set_rs485_configuration(message);
		case FID_GET_RS485_CONFIGURATION: return get_rs485_configuration(message, response);

		// Modbus specific.
		case FID_SET_MODBUS_CONFIGURATION: return set_modbus_configuration(message);
		case FID_GET_MODBUS_CONFIGURATION: return get_modbus_configuration(message, response);

		case FID_SET_MODE: return set_mode(message);
		case FID_GET_MODE: return get_mode(message, response);
		case FID_APPLY_CONFIGURATION : return apply_configuration(message);
		case FID_SET_COMMUNICATION_LED_CONFIG: return set_communication_led_config(message);
		case FID_GET_COMMUNICATION_LED_CONFIG: return get_communication_led_config(message, response);
		case FID_SET_ERROR_LED_CONFIG: return set_error_led_config(message);
		case FID_GET_ERROR_LED_CONFIG: return get_error_led_config(message, response);
		case FID_SET_BUFFER_CONFIG: return set_buffer_config(message);
		case FID_GET_BUFFER_CONFIG: return get_buffer_config(message, response);
		case FID_GET_BUFFER_STATUS: return get_buffer_status(message, response);
		case FID_ENABLE_ERROR_COUNT_CALLBACK: return enable_error_count_callback(message);
		case FID_DISABLE_ERROR_COUNT_CALLBACK: return disable_error_count_callback(message);
		case FID_IS_ERROR_COUNT_CALLBACK_ENABLED: return is_error_count_callback_enabled(message, response);
		case FID_GET_ERROR_COUNT: return get_error_count(message, response);

		// Modbus specific.
		case FID_MODBUS_REPORT_EXCEPTION: return modbus_report_exception(message);
		case FID_GET_MODBUS_COMMON_ERROR_COUNT: return get_modbus_common_error_count(message, response);
		case FID_MODBUS_ANSWER_READ_COILS_REQUEST_LOW_LEVEL: return modbus_answer_read_coils_request_low_level(message);
		case FID_MODBUS_READ_COILS: return modbus_read_coils(message, response);
		case FID_MODBUS_ANSWER_READ_HOLDING_REGISTERS_REQUEST_LOW_LEVEL: return modbus_answer_read_holding_registers_request_low_level(message);
		case FID_MODBUS_READ_HOLDING_REGISTERS: return modbus_read_holding_registers(message, response);
		case FID_MODBUS_ANSWER_WRITE_SINGLE_COIL_REQUEST: return modbus_answer_write_single_coil_request(message);
		case FID_MODBUS_WRITE_SINGLE_COIL: return modbus_write_single_coil(message, response);
		case FID_MODBUS_ANSWER_WRITE_SINGLE_REGISTER_REQUEST: return modbus_answer_write_single_register_request(message);
		case FID_MODBUS_WRITE_SINGLE_REGISTER: return modbus_write_single_register(message, response);
		case FID_MODBUS_ANSWER_WRITE_MULTIPLE_COILS_REQUEST: return modbus_answer_write_multiple_coils_request(message);
		case FID_MODBUS_WRITE_MULTIPLE_COILS_LOW_LEVEL: return modbus_write_multiple_coils_low_level(message, response);
		case FID_MODBUS_ANSWER_WRITE_MULTIPLE_REGISTERS_REQUEST: return modbus_answer_write_multiple_registers_request(message);
		case FID_MODBUS_WRITE_MULTIPLE_REGISTERS_LOW_LEVEL: return modbus_write_multiple_registers_low_level(message, response);

		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}

BootloaderHandleMessageResponse write(const Write *data, WriteResponse *response) {
	response->header.length = sizeof(WriteResponse);

	const uint8_t length = MIN(data->length, MESSAGE_LENGTH);
	uint8_t written = 0;
	for(; written < length; written++) {
		if(!ringbuffer_add(&rs485.ringbuffer_tx, data->message[written])) {
			break;
		}
	}

	if(written != 0) {
		XMC_USIC_CH_TXFIFO_EnableEvent(RS485_USIC, XMC_USIC_CH_TXFIFO_EVENT_CONF_STANDARD);
		XMC_USIC_CH_TriggerServiceRequest(RS485_USIC, RS485_SERVICE_REQUEST_TX);
	}

	response->written = written;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse read(const Read *data, ReadResponse *response) {
	response->header.length = sizeof(ReadResponse);

	// Read bytes available in ringbuffer
	uint8_t read = 0;
	for(; read < MESSAGE_LENGTH; read++) {
		if(!ringbuffer_get(&rs485.ringbuffer_rx, (uint8_t*)&response->message[read])) {
			break;
		}
	}

	// Fill rest with 0
	for(uint8_t i = read; i < MESSAGE_LENGTH; i++) {
		response->message[read] = 0;
	}

	response->length = read;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse enable_read_callback(const EnableReadCallback *data) {
	rs485.read_callback_enabled = true;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse disable_read_callback(const DisableReadCallback *data) {
	rs485.read_callback_enabled = false;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse is_read_callback_enabled(const IsReadCallbackEnabled *data, IsReadCallbackEnabledResponse *response) {
	response->header.length = sizeof(IsReadCallbackEnabledResponse);
	response->enabled = rs485.read_callback_enabled;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_rs485_configuration(const SetRS485Configuration *data) {
	if((data->parity >= 3) ||
		 (data->stopbits != 1 && data->stopbits != 2) ||
		 (data->wordlength < 5 || data->wordlength > 8)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	rs485.baudrate   = BETWEEN(RS485_BAUDRATE_MIN, data->baudrate, RS485_BAUDRATE_MAX);
	rs485.parity     = data->parity;
	rs485.stopbits   = data->stopbits;
	rs485.wordlength = data->wordlength;
	rs485.duplex     = data->duplex;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_rs485_configuration(const GetRS485Configuration *data, GetRS485ConfigurationResponse *response) {
	response->header.length = sizeof(GetRS485ConfigurationResponse);
	response->baudrate      = rs485.baudrate;
	response->parity        = rs485.parity;
	response->stopbits      = rs485.stopbits;
	response->wordlength    = rs485.wordlength;
	response->duplex        = rs485.duplex;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

// Modbus specific.
BootloaderHandleMessageResponse set_modbus_configuration(const SetModbusConfiguration *data) {
	if(data->slave_address > 247 || data->slave_address == 0) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	rs485.modbus_slave_address = data->slave_address;
	rs485.modbus_master_request_timeout = data->master_request_timeout;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_modbus_configuration(const GetModbusConfiguration *data, GetModbusConfigurationResponse *response) {
	response->header.length          = sizeof(GetModbusConfigurationResponse);
	response->slave_address          = rs485.modbus_slave_address;
	response->master_request_timeout = rs485.modbus_master_request_timeout;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_mode(const SetMode *data) {
	rs485.mode = data->mode;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_mode(const GetMode *data, GetModeResponse *response) {
	response->header.length = sizeof(GetModeResponse);
	response->mode          = rs485.mode;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse apply_configuration(const ApplyConfiguration *data) {
	rs485_apply_configuration(&rs485);

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse set_communication_led_config(const SetCommunicationLEDConfig *data) {
	if(data->config > COMMUNICATION_LED_CONFIG_SHOW_HEARTBEAT) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	rs485.yellow_led_state.config = data->config;

	// Set LED according to value
	if(rs485.yellow_led_state.config == COMMUNICATION_LED_CONFIG_OFF) {
		XMC_GPIO_SetOutputHigh(RS485_LED_YELLOW_PIN);
	} else {
		XMC_GPIO_SetOutputLow(RS485_LED_YELLOW_PIN);
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_communication_led_config(const GetCommunicationLEDConfig *data, GetCommunicationLEDConfigResponse *response) {
	response->header.length = sizeof(GetCommunicationLEDConfigResponse);
	response->config        = rs485.yellow_led_state.config;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_error_led_config(const SetErrorLEDConfig *data) {
	if(data->config > ERROR_LED_CONFIG_SHOW_HEARTBEAT) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	/*
	 * The error config and flicker config are not the same, we save
	 * the "show error" option as "external".
	 */
	if(data->config == ERROR_LED_CONFIG_SHOW_ERROR) {
		rs485.red_led_state.config = LED_FLICKER_CONFIG_EXTERNAL;
	} else {
		rs485.red_led_state.config = data->config;
	}

	// Set LED according to value
	if(rs485.red_led_state.config == ERROR_LED_CONFIG_OFF ||
		 rs485.red_led_state.config == LED_FLICKER_CONFIG_EXTERNAL) {
		XMC_GPIO_SetOutputHigh(RS485_LED_RED_PIN);
	} else {
		XMC_GPIO_SetOutputLow(RS485_LED_RED_PIN);
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_error_led_config(const GetErrorLEDConfig *data, GetErrorLEDConfigResponse *response) {
	response->header.length = sizeof(GetErrorLEDConfigResponse);

	if(rs485.red_led_state.config == LED_FLICKER_CONFIG_EXTERNAL) {
		response->config = ERROR_LED_CONFIG_SHOW_ERROR;
	}
	else {
		response->config = rs485.red_led_state.config;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_buffer_config(const SetBufferConfig *data) {
	if((data->receive_buffer_size < 1024) ||
		 (data->send_buffer_size < 1024) ||
		 (data->receive_buffer_size + data->send_buffer_size != RS485_BUFFER_SIZE)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	rs485.buffer_size_rx = data->receive_buffer_size;

	rs485_init_buffer(&rs485);

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_buffer_config(const GetBufferConfig *data, GetBufferConfigResponse *response) {
	response->header.length       = sizeof(GetBufferConfigResponse);
	response->receive_buffer_size = rs485.buffer_size_rx;
	response->send_buffer_size    = RS485_BUFFER_SIZE - rs485.buffer_size_rx;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse get_buffer_status(const GetBufferStatus *data, GetBufferStatusResponse *response) {
	response->header.length       = sizeof(GetBufferStatusResponse);
	response->send_buffer_used    = ringbuffer_get_used(&rs485.ringbuffer_tx);
	response->receive_buffer_used = ringbuffer_get_used(&rs485.ringbuffer_rx);

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse enable_error_count_callback(const EnableErrorCountCallback *data) {
	rs485.error_count_callback_enabled = true;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse disable_error_count_callback(const DisableErrorCountCallback *data) {
	rs485.error_count_callback_enabled = false;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse is_error_count_callback_enabled(const IsErrorCountCallbackEnabled *data, IsErrorCountCallbackEnabledResponse *response) {
	response->header.length = sizeof(IsErrorCountCallbackEnabledResponse);
	response->enabled       = rs485.error_count_callback_enabled;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse get_error_count(const GetErrorCount *data, GetErrorCountResponse *response) {
	response->header.length       = sizeof(GetErrorCountResponse);
	response->overrun_error_count = rs485.error_count_overrun;
	response->parity_error_count  = rs485.error_count_parity;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

// Modbus specific.
BootloaderHandleMessageResponse get_modbus_common_error_count(const GetModbusCommonErrorCount *data, GetModbusCommonErrorCountResponse *response) {
	response->header.length                    = sizeof(GetModbusCommonErrorCountResponse);
	response->timeout_error_count              = rs485.modbus_common_error_counters.timeout;
	response->checksum_error_count             = rs485.modbus_common_error_counters.checksum;
	response->frame_too_big_error_count        = rs485.modbus_common_error_counters.frame_too_big;
	response->illegal_function_error_count     = rs485.modbus_common_error_counters.illegal_function;
	response->illegal_data_address_error_count = rs485.modbus_common_error_counters.illegal_data_address;
	response->illegal_data_value_error_count   = rs485.modbus_common_error_counters.illegal_data_value;
	response->slave_device_failure_error_count = rs485.modbus_common_error_counters.slave_device_failure;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse modbus_report_exception(const ModbusReportException *data) {
	// It is allowed to report an exception only in slave mode.
	if(rs485.mode != MODE_MODBUS_SLAVE_RTU) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_SLAVE_PROCESSING_REQUEST) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(!modbus_slave_check_current_request(data->request_id)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	_modbus_report_exception(&rs485, rs485.modbus_rtu.request.rx_frame[1], data->exception_code);

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse
modbus_answer_read_coils_request_low_level(const ModbusAnswerReadCoilsRequestLowLevel *data) {
	// This function can be invoked only in slave mode.

	uint16_t count = 0;
	uint16_t expected_bytes = 0;

	if(rs485.mode != MODE_MODBUS_SLAVE_RTU){
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(!modbus_slave_check_current_request(data->request_id)){
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(rs485.modbus_rtu.request.rx_frame[1] != MODBUS_FC_READ_COILS) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(data->stream_total_length > (RS485_MODBUS_RTU_FRAME_SIZE_MAX - 5)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	memcpy(&count, &rs485.modbus_rtu.request.rx_frame[4], 2);

	// Fix endianness (BE->LE).
	count = NTOHS(count);

	expected_bytes = count / 8;

	if((count % 8) != 0) {
		expected_bytes ++;
	}

	if(data->stream_total_length != expected_bytes) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	// The first chunk.
	if(data->stream_chunk_offset == 0) {
		ringbuffer_add(&rs485.ringbuffer_tx, rs485.modbus_slave_address);
		ringbuffer_add(&rs485.ringbuffer_tx, (uint8_t)MODBUS_FC_READ_COILS);
		ringbuffer_add(&rs485.ringbuffer_tx, expected_bytes);

		if(data->stream_total_length <= sizeof(data->stream_chunk_data)) {
			// All data fits in the first chunk there will not be other chunks.
			modbus_store_tx_frame_data_bytes(data->stream_chunk_data, data->stream_total_length);

			modbus_add_tx_frame_checksum();

			if(rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
				return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
			}

			modbus_start_tx_from_buffer(&rs485);

			return HANDLE_MESSAGE_RESPONSE_EMPTY;
		}
	}

	if((data->stream_chunk_offset + sizeof(data->stream_chunk_data)) <= data->stream_total_length) {
		modbus_store_tx_frame_data_bytes(data->stream_chunk_data, sizeof(data->stream_chunk_data));

		if((data->stream_chunk_offset + sizeof(data->stream_chunk_data)) == data->stream_total_length) {
			// All data of the frame is in the buffer except checksum.
			modbus_add_tx_frame_checksum();

			if(rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
				return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
			}

			modbus_start_tx_from_buffer(&rs485);
		}
	}
	else {
		modbus_store_tx_frame_data_bytes(data->stream_chunk_data, data->stream_total_length - data->stream_chunk_offset);

		// All data of the frame is in the buffer except checksum.
		modbus_add_tx_frame_checksum();

		if(rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
			return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
		}

		modbus_start_tx_from_buffer(&rs485);
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse modbus_read_coils(const ModbusReadCoils *data, ModbusReadCoilsResponse *response) {
	// This function can be invoked only in master mode.

	uint8_t _fc = 0;

	ModbusReadCoils _data;

	response->request_id = 0;
	response->header.length = sizeof(ModbusReadCoilsResponse);

	if(rs485.mode != MODE_MODBUS_MASTER_RTU ||
		 rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_READY ||
		 rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {

		return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
	}

	if(data->count < 1 || data->count > 2000) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	modbus_init_new_request(&rs485,
	                        MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE,
	                        ((sizeof(ModbusReadCoils) - sizeof(TFPMessageHeader)) + 3));

	response->request_id = rs485.modbus_rtu.request.id;

  _fc = (uint8_t)MODBUS_FC_READ_COILS;

	// Constructing the frame in the TX buffer.
	modbus_store_tx_frame_data_bytes(&data->slave_address, 1); // Slave address.
	modbus_store_tx_frame_data_bytes(&_fc, 1); // Function code.

	memcpy(&_data, data, sizeof(ModbusReadCoils));

	if(_data.starting_address > 0) {
		_data.starting_address--;
	}

	// Fix endianness (LE->BE).
	_data.count = HTONS(_data.count);
	_data.starting_address = HTONS(_data.starting_address);

	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.starting_address, 2);
	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.count, 2);

	// Calculate checksum and put it at the end of the TX buffer.
	modbus_add_tx_frame_checksum();

	// Start master request timeout timing.
	rs485.modbus_rtu.request.time_ref_master_request_timeout = system_timer_get_ms();

	// Start TX.
	modbus_start_tx_from_buffer(&rs485);

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse
modbus_answer_read_holding_registers_request_low_level(const ModbusAnswerReadHoldingRegistersRequestLowLevel *data) {
	// This function can be invoked only in slave mode.

	uint16_t count = 0;
	uint16_t _stream_chunk_data[29];

	if(rs485.mode != MODE_MODBUS_SLAVE_RTU) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(!modbus_slave_check_current_request(data->request_id)){
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(rs485.modbus_rtu.request.rx_frame[1] != MODBUS_FC_READ_HOLDING_REGISTERS) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if((data->stream_total_length * 2) > (RS485_MODBUS_RTU_FRAME_SIZE_MAX - 5)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	memcpy(&count, &rs485.modbus_rtu.request.rx_frame[4], 2);

	// Fix endianness (BE->LE).
	count = NTOHS(count);

	if(data->stream_total_length != count) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	/*
	 * FIXME: Strange memcpy() problem.
	 *
	 * For undetermined reason calling modbus_store_tx_frame_data_shorts()
	 * with data->stream_chunk_data as argument doesn't work and shows unspecified
	 * behaviour.
	 *
	 * Copying the data to a stack local variable and then calling the function
	 * seems to work.
	 */

	memcpy(_stream_chunk_data, data->stream_chunk_data, sizeof(data->stream_chunk_data));

	// The first chunk.
	if(data->stream_chunk_offset == 0) {
		ringbuffer_add(&rs485.ringbuffer_tx, rs485.modbus_slave_address);
		ringbuffer_add(&rs485.ringbuffer_tx, (uint8_t)MODBUS_FC_READ_HOLDING_REGISTERS);
		ringbuffer_add(&rs485.ringbuffer_tx, count * 2);

		if(data->stream_total_length <= sizeof(data->stream_chunk_data) / 2) {
			// All data fits in the first chunk there will not be other chunks.
			modbus_store_tx_frame_data_shorts(_stream_chunk_data, count);

			modbus_add_tx_frame_checksum();

			if(rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
				return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
			}

			modbus_start_tx_from_buffer(&rs485);

			return HANDLE_MESSAGE_RESPONSE_EMPTY;
		}
	}

	if((data->stream_chunk_offset + (sizeof(data->stream_chunk_data) / 2)) <= data->stream_total_length) {
		// We can copy all the data available in this chunk.
		modbus_store_tx_frame_data_shorts(_stream_chunk_data, (sizeof(data->stream_chunk_data) / 2));

		if((data->stream_chunk_offset + (sizeof(data->stream_chunk_data) / 2)) == data->stream_total_length) {
			/*
			 * This is the last chunk of the stream. All data of the frame is in the
			 * buffer except checksum.
			 */
			modbus_add_tx_frame_checksum();

			if(rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
				return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
			}

			modbus_start_tx_from_buffer(&rs485);
		}
	}
	else {
		/*
		 * This is the last chunk of the stream but we have to calculate how much
		 * to copy from the chunk.
		 */
		modbus_store_tx_frame_data_shorts(_stream_chunk_data, data->stream_total_length - data->stream_chunk_offset);

		// All data of the frame is in the buffer except checksum.
		modbus_add_tx_frame_checksum();

		if(rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
			return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
		}

		modbus_start_tx_from_buffer(&rs485);
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse modbus_read_holding_registers(const ModbusReadHoldingRegisters *data, ModbusReadHoldingRegistersResponse *response) {
	// This function can be invoked only in master mode.

	uint8_t _fc = 0;

	response->request_id = 0;
	ModbusReadHoldingRegisters _data;

	response->header.length = sizeof(ModbusReadHoldingRegistersResponse);

	if(rs485.mode != MODE_MODBUS_MASTER_RTU ||
		 rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_READY ||
		 rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
		return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
	}

	if(data->count < 1 || data->count > 125) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	modbus_init_new_request(&rs485,
	                        MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE,
	                        ((sizeof(ModbusReadHoldingRegisters) - sizeof(TFPMessageHeader)) + 3));

	response->request_id = rs485.modbus_rtu.request.id;

  _fc = (uint8_t)MODBUS_FC_READ_HOLDING_REGISTERS;

	// Constructing the frame in the TX buffer.
	modbus_store_tx_frame_data_bytes(&data->slave_address, 1); // Slave address.
	modbus_store_tx_frame_data_bytes(&_fc, 1); // Function code.

	memcpy(&_data, data, sizeof(ModbusReadHoldingRegisters));

	if(_data.starting_address > 0) {
		_data.starting_address--;
	}

	// Fix endianness (LE->BE).
	_data.count = HTONS(_data.count);
	_data.starting_address = HTONS(_data.starting_address);

	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.starting_address, 2);
	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.count, 2);

	// Calculate checksum and put it at the end of the TX buffer.
	modbus_add_tx_frame_checksum();

	// Start master request timeout timing.
	rs485.modbus_rtu.request.time_ref_master_request_timeout = system_timer_get_ms();

	// Start TX.
	modbus_start_tx_from_buffer(&rs485);

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse modbus_answer_write_single_coil_request(const ModbusAnswerWriteSingleCoilRequest *data) {
	// This function can be invoked only in slave mode.

	uint8_t _fc;
	ModbusAnswerWriteSingleCoilRequest _data;

	if(rs485.mode != MODE_MODBUS_SLAVE_RTU) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(!modbus_slave_check_current_request(data->request_id)){
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(rs485.modbus_rtu.request.rx_frame[1] != MODBUS_FC_WRITE_SINGLE_COIL) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(data->coil_value != 0x0000 && data->coil_value != 0xFF00) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	_fc = (uint8_t)MODBUS_FC_WRITE_SINGLE_COIL;

	// Constructing the frame in the TX buffer.
	modbus_store_tx_frame_data_bytes(&rs485.modbus_slave_address, 1); // Slave address.
	modbus_store_tx_frame_data_bytes(&_fc, 1); // Function code.

	memcpy(&_data, data, sizeof(ModbusAnswerWriteSingleCoilRequest));

	// Fix endianness (LE->BE).
	_data.coil_address = HTONS(data->coil_address);
	_data.coil_value = HTONS(data->coil_value);

	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.coil_address, 2);
	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.coil_value, 2);

	// Calculate checksum and put it at the end of the TX buffer.
	modbus_add_tx_frame_checksum();

	// Start master request timeout timing.
	rs485.modbus_rtu.request.time_ref_master_request_timeout = system_timer_get_ms();

	// Start TX.
	modbus_start_tx_from_buffer(&rs485);

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse modbus_write_single_coil(const ModbusWriteSingleCoil *data,
                                                         ModbusWriteSingleCoilResponse *response) {
	// This function can be invoked only in master mode.

	uint8_t _fc = 0;
	ModbusWriteSingleCoil _data;

	response->request_id = 0;
	response->header.length = sizeof(ModbusWriteSingleCoil);

	if(rs485.mode != MODE_MODBUS_MASTER_RTU ||
	   rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_READY ||
	   rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
	  return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
	}

	if(data->coil_value != 0x0000 && data->coil_value != 0xFF00) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	modbus_init_new_request(&rs485,
	                        MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE,
	                        ((sizeof(ModbusWriteSingleCoil) - sizeof(TFPMessageHeader)) + 3));

	response->request_id = rs485.modbus_rtu.request.id;

	_fc = (uint8_t)MODBUS_FC_WRITE_SINGLE_COIL;

	// Constructing the frame in the TX buffer.
	modbus_store_tx_frame_data_bytes(&data->slave_address, 1); // Slave address.
	modbus_store_tx_frame_data_bytes(&_fc, 1); // Function code.

	memcpy(&_data, data, sizeof(ModbusWriteSingleCoil));

	if(_data.coil_address > 0) {
		_data.coil_address--;
	}

	// Fix endianness (LE->BE).
	_data.coil_address = HTONS(_data.coil_address);
	_data.coil_value = HTONS(_data.coil_value);

	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.coil_address, 2);
	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.coil_value, 2);

	// Calculate checksum and put it at the end of the TX buffer.
	modbus_add_tx_frame_checksum();

	// Start master request timeout timing.
	rs485.modbus_rtu.request.time_ref_master_request_timeout = system_timer_get_ms();

	// Start TX.
	modbus_start_tx_from_buffer(&rs485);

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse modbus_answer_write_single_register_request(const ModbusAnswerWriteSingleRegisterRequest *data) {
	// This function can be invoked only in slave mode.

	uint8_t _fc;
	ModbusAnswerWriteSingleRegisterRequest _data;

	if(rs485.mode != MODE_MODBUS_SLAVE_RTU) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(!modbus_slave_check_current_request(data->request_id)){
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(rs485.modbus_rtu.request.rx_frame[1] != MODBUS_FC_WRITE_SINGLE_REGISTER) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	_fc = (uint8_t)MODBUS_FC_WRITE_SINGLE_REGISTER;

	// Constructing the frame in the TX buffer.
	modbus_store_tx_frame_data_bytes(&rs485.modbus_slave_address, 1); // Slave address.
	modbus_store_tx_frame_data_bytes(&_fc, 1); // Function code.

	memcpy(&_data, data, sizeof(ModbusAnswerWriteSingleRegisterRequest));

	// Fix endianness (LE->BE).
	_data.register_address = HTONS(data->register_address);
	_data.register_value = HTONS(data->register_value);

	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.register_address, 2);
	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.register_value, 2);

	// Calculate checksum and put it at the end of the TX buffer.
	modbus_add_tx_frame_checksum();

	// Start master request timeout timing.
	rs485.modbus_rtu.request.time_ref_master_request_timeout = system_timer_get_ms();

	// Start TX.
	modbus_start_tx_from_buffer(&rs485);

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse modbus_write_single_register(const ModbusWriteSingleRegister *data,
                                                             ModbusWriteSingleRegisterResponse *response) {
	//This function can be invoked only in master mode.

	uint8_t _fc = 0;

	ModbusWriteSingleRegister _data;

	response->request_id = 0;
	response->header.length = sizeof(ModbusWriteSingleRegister);

	if(rs485.mode != MODE_MODBUS_MASTER_RTU ||
	   rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_READY ||
	   rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
	  return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
	}

	modbus_init_new_request(&rs485,
	                        MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE,
	                        ((sizeof(ModbusWriteSingleRegister) - sizeof(TFPMessageHeader)) + 3));

	response->request_id = rs485.modbus_rtu.request.id;

	_fc = (uint8_t)MODBUS_FC_WRITE_SINGLE_REGISTER;

	// Constructing the frame in the TX buffer.
	modbus_store_tx_frame_data_bytes(&data->slave_address, 1); // Slave address.
	modbus_store_tx_frame_data_bytes(&_fc, 1); // Function code.

	memcpy(&_data, data, sizeof(ModbusWriteSingleRegister));

	if(_data.register_address > 0) {
		_data.register_address--;
	}

	// Fix endianness (LE->BE).
	_data.register_address = HTONS(_data.register_address);
	_data.register_value = HTONS(_data.register_value);

	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.register_address, 2);
	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.register_value, 2);

	// Calculate checksum and put it at the end of the TX buffer.
	modbus_add_tx_frame_checksum();

	// Start master request timeout timing.
	rs485.modbus_rtu.request.time_ref_master_request_timeout = system_timer_get_ms();

	// Start TX.
	modbus_start_tx_from_buffer(&rs485);

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse modbus_answer_write_multiple_coils_request(const ModbusAnswerWriteMultipleCoilsRequest *data) {
	// This function can be invoked only in slave mode.

	uint8_t _fc;
	ModbusAnswerWriteMultipleCoilsRequest _data;

	if(rs485.mode != MODE_MODBUS_SLAVE_RTU) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(!modbus_slave_check_current_request(data->request_id)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(rs485.modbus_rtu.request.rx_frame[1] != MODBUS_FC_WRITE_MULTIPLE_COILS) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	memcpy(&_data, data, sizeof(ModbusAnswerWriteMultipleCoilsRequest));

	// Fix endianness (LE->BE).
	_data.starting_address = HTONS(data->starting_address);
	_data.count = HTONS(data->count);

	_fc = (uint8_t)MODBUS_FC_WRITE_MULTIPLE_COILS;

	// Constructing the frame in the TX buffer.
	modbus_store_tx_frame_data_bytes(&rs485.modbus_slave_address, 1); // Slave address.
	modbus_store_tx_frame_data_bytes(&_fc, 1); // Function code.
	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.starting_address, 2);
	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.count, 2);

	// Calculate checksum and put it at the end of the TX buffer.
	modbus_add_tx_frame_checksum();

	// Start TX.
	modbus_start_tx_from_buffer(&rs485);

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse modbus_write_multiple_coils_low_level(const ModbusWriteMultipleCoilsLowLevel *data,
                                                                      ModbusWriteMultipleCoilsLowLevelResponse *response) {
	// This function can be invoked only in master mode.

	uint8_t _fc = 0;

	ModbusWriteMultipleCoilsLowLevel _data;

	response->request_id = 0;
	response->header.length = sizeof(ModbusWriteMultipleCoilsLowLevelResponse);

	if(rs485.mode != MODE_MODBUS_MASTER_RTU ||
	   rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE ||
	   rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_READY) {
	  return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
	}

	if(data->stream_total_length > (RS485_MODBUS_RTU_FRAME_SIZE_MAX - 9)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(data->count < 1 || data->count > 1968) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	memcpy(&_data, data, sizeof(ModbusWriteMultipleCoilsLowLevel));

	// The first chunk.
	if(data->stream_chunk_offset == 0) {
		_fc = (uint8_t)MODBUS_FC_WRITE_MULTIPLE_COILS;

		if(data->starting_address > 0) {
			_data.starting_address--;
		}

		// Fix endianness (LE->BE).
		_data.starting_address = HTONS(data->starting_address);
		_data.count = HTONS(data->count);

		// Constructing the frame in the TX buffer.
		modbus_store_tx_frame_data_bytes(&data->slave_address, 1); // Slave address.
		modbus_store_tx_frame_data_bytes(&_fc, 1); // Function code.
		modbus_store_tx_frame_data_bytes((uint8_t *)&_data.starting_address, 2); // Starting address.
		modbus_store_tx_frame_data_bytes((uint8_t *)&_data.count, 2); // Count.
		modbus_store_tx_frame_data_bytes((uint8_t *)&data->stream_total_length, 1); // Byte count.

		if(data->stream_total_length <= sizeof(data->stream_chunk_data)) {
			// All data fits in the first chunk there will not be other chunks.
			modbus_store_tx_frame_data_bytes(data->stream_chunk_data, data->stream_total_length);

			modbus_add_tx_frame_checksum();

			if(rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
				return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
			}

			modbus_init_new_request(&rs485,
			                        MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE,
			                        data->stream_total_length + 9);

			modbus_start_tx_from_buffer(&rs485);

			response->request_id = rs485.modbus_rtu.request.id;

			// Start master request timeout timing.
			rs485.modbus_rtu.request.time_ref_master_request_timeout = system_timer_get_ms();

			return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
		}
	}

	if((data->stream_chunk_offset + sizeof(data->stream_chunk_data)) <= data->stream_total_length) {
		modbus_store_tx_frame_data_bytes(data->stream_chunk_data, sizeof(data->stream_chunk_data));

		if((data->stream_chunk_offset + sizeof(data->stream_chunk_data)) == data->stream_total_length) {
			// All data of the frame is in the buffer except checksum.
			modbus_add_tx_frame_checksum();

			if(rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
				return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
			}

			modbus_init_new_request(&rs485,
			                        MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE,
			                        data->stream_total_length + 9);

			response->request_id = rs485.modbus_rtu.request.id;

			// Start master request timeout timing.
			rs485.modbus_rtu.request.time_ref_master_request_timeout = system_timer_get_ms();

			modbus_start_tx_from_buffer(&rs485);
		}
	}
	else {
		modbus_store_tx_frame_data_bytes(data->stream_chunk_data, data->stream_total_length - data->stream_chunk_offset);

		// All data of the frame is in the buffer except checksum.
		modbus_add_tx_frame_checksum();

		if(rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
			return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
		}

		modbus_init_new_request(&rs485,
		                        MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE,
		                        data->stream_total_length + 9);

		response->request_id = rs485.modbus_rtu.request.id;

		// Start master request timeout timing.
		rs485.modbus_rtu.request.time_ref_master_request_timeout = system_timer_get_ms();

		modbus_start_tx_from_buffer(&rs485);
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse modbus_answer_write_multiple_registers_request(const ModbusAnswerWriteMultipleRegistersRequest *data) {
	// This function can be invoked only in slave mode.

	uint8_t _fc;
	ModbusAnswerWriteMultipleRegistersRequest _data;

	if(rs485.mode != MODE_MODBUS_SLAVE_RTU) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(!modbus_slave_check_current_request(data->request_id)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(rs485.modbus_rtu.request.rx_frame[1] != MODBUS_FC_WRITE_MULTIPLE_REGISTERS) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	memcpy(&_data, data, sizeof(ModbusAnswerWriteMultipleRegistersRequest));

	// Fix endianness (LE->BE).
	_data.starting_address = HTONS(data->starting_address);
	_data.count = HTONS(data->count);

	_fc = (uint8_t)MODBUS_FC_WRITE_MULTIPLE_REGISTERS;

	// Constructing the frame in the TX buffer.
	modbus_store_tx_frame_data_bytes(&rs485.modbus_slave_address, 1); // Slave address.
	modbus_store_tx_frame_data_bytes(&_fc, 1); // Function code.
	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.starting_address, 2);
	modbus_store_tx_frame_data_bytes((uint8_t *)&_data.count, 2);

	// Calculate checksum and put it at the end of the TX buffer.
	modbus_add_tx_frame_checksum();

	// Start TX.
	modbus_start_tx_from_buffer(&rs485);

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse modbus_write_multiple_registers_low_level(const ModbusWriteMultipleRegistersLowLevel *data,
                                                                          ModbusWriteMultipleRegistersLowLevelResponse *response) {
	// This function can be invoked only in master mode.

	uint8_t _fc = 0;
	uint8_t byte_count = 0;
	uint16_t _stream_chunk_data[27];

	ModbusWriteMultipleRegistersLowLevel _data;

	response->request_id = 0;
	response->header.length = sizeof(ModbusWriteMultipleRegistersLowLevelResponse);

	if(rs485.mode != MODE_MODBUS_MASTER_RTU ||
	   rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE ||
	   rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_READY) {
	  return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
	}

	if((data->stream_total_length * 2) > (RS485_MODBUS_RTU_FRAME_SIZE_MAX - 9)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(data->count < 1 || data->count > 123) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	memcpy(&_data, data, sizeof(ModbusWriteMultipleRegistersLowLevel));

	// The first chunk.
	if(data->stream_chunk_offset == 0) {
		_fc = (uint8_t)MODBUS_FC_WRITE_MULTIPLE_REGISTERS;

		if(data->starting_address > 0) {
			_data.starting_address--;
		}

		// Fix endianness (LE->BE).
		_data.starting_address = HTONS(data->starting_address);
		_data.count = HTONS(data->count);

		byte_count = data->stream_total_length * 2;

		// Constructing the frame in the TX buffer.
		modbus_store_tx_frame_data_bytes(&data->slave_address, 1); // Slave address.
		modbus_store_tx_frame_data_bytes(&_fc, 1); // Function code.
		modbus_store_tx_frame_data_bytes((uint8_t *)&_data.starting_address, 2); // Starting address.
		modbus_store_tx_frame_data_bytes((uint8_t *)&_data.count, 2); // Count.
		modbus_store_tx_frame_data_bytes((uint8_t *)&byte_count, 1); // Byte count.

		if((data->stream_total_length * 2) <= sizeof(data->stream_chunk_data)) {
			// All data fits in the first chunk there will not be other chunks.

			/*
			 * FIXME: Strange memcpy() problem.
			 *
			 * For undetermined reason calling modbus_store_tx_frame_data_shorts()
			 * with data->stream_chunk_data as argument doesn't work and shows unspecified
			 * behaviour.
			 *
			 * Copying the data to a stack local variable and then calling the function
			 * seems to work.
			 */
			memcpy(&_stream_chunk_data, &data->stream_chunk_data, sizeof(data->stream_chunk_data));

			modbus_store_tx_frame_data_shorts(_stream_chunk_data, data->stream_total_length);

			modbus_add_tx_frame_checksum();

			if(rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
				return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
			}

			modbus_init_new_request(&rs485,
			                        MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE,
			                        (data->stream_total_length * 2) + 9);

			modbus_start_tx_from_buffer(&rs485);

			response->request_id = rs485.modbus_rtu.request.id;

			// Start master request timeout timing.
			rs485.modbus_rtu.request.time_ref_master_request_timeout = system_timer_get_ms();

			return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
		}
	}

	if((data->stream_chunk_offset + (sizeof(data->stream_chunk_data) / 2)) <= data->stream_total_length) {
		/*
		 * FIXME: Strange memcpy() problem.
		 *
		 * For undetermined reason calling modbus_store_tx_frame_data_shorts()
		 * with data->stream_chunk_data as argument doesn't work and shows unspecified
		 * behaviour.
		 *
		 * Copying the data to a stack local variable and then calling the function
		 * seems to work.
		 */
		memcpy(&_stream_chunk_data, &data->stream_chunk_data, sizeof(data->stream_chunk_data));

		modbus_store_tx_frame_data_shorts(_stream_chunk_data, (sizeof(data->stream_chunk_data) / 2));

		if((data->stream_chunk_offset + (sizeof(data->stream_chunk_data) / 2)) == data->stream_total_length) {
			// All data of the frame is in the buffer except checksum.
			modbus_add_tx_frame_checksum();

			if(rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
				return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
			}

			modbus_init_new_request(&rs485,
			                        MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE,
			                        (data->stream_total_length * 2) + 9);

			response->request_id = rs485.modbus_rtu.request.id;

			// Start master request timeout timing.
			rs485.modbus_rtu.request.time_ref_master_request_timeout = system_timer_get_ms();

			modbus_start_tx_from_buffer(&rs485);
		}
	}
	else {
		/*
		 * FIXME: Strange memcpy() problem.
		 *
		 * For undetermined reason calling modbus_store_tx_frame_data_shorts()
		 * with data->stream_chunk_data as argument doesn't work and shows unspecified
		 * behaviour.
		 *
		 * Copying the data to a stack local variable and then calling the function
		 * seems to work.
		 */
		memcpy(&_stream_chunk_data, &data->stream_chunk_data, sizeof(data->stream_chunk_data));

		modbus_store_tx_frame_data_shorts(_stream_chunk_data, (data->stream_total_length - data->stream_chunk_offset));

		// All data of the frame is in the buffer except checksum.
		modbus_add_tx_frame_checksum();

		if(rs485.modbus_rtu.state_wire != MODBUS_RTU_WIRE_STATE_IDLE) {
			return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
		}

		modbus_init_new_request(&rs485,
		                        MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE,
		                        (data->stream_total_length * 2) + 9);

		response->request_id = rs485.modbus_rtu.request.id;

		// Start master request timeout timing.
		rs485.modbus_rtu.request.time_ref_master_request_timeout = system_timer_get_ms();

		modbus_start_tx_from_buffer(&rs485);
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

bool handle_read_callback_callback(void) {
	static bool is_buffered = false;
	static ReadCallbackCallback cb;

	static uint32_t last_used = 0;
	static uint32_t last_time = 0;

	if(rs485.mode == MODE_MODBUS_SLAVE_RTU || rs485.mode == MODE_MODBUS_MASTER_RTU) {
		return false;
	}

	if(!rs485.read_callback_enabled) {
		return false;
	}

	if(!is_buffered) {
		const uint32_t used         = ringbuffer_get_used(&rs485.ringbuffer_rx);
		const uint32_t ms_per_2byte = (2*8000/rs485.baudrate) + 1;
		const bool     time_elapsed = system_timer_is_time_elapsed_ms(last_time, ms_per_2byte);
		const bool     no_change    = (last_used == used);

		last_used = used;

		// We update the time if the buffer is empty or the time has elapsed
		// the buffer fill level changed
		if(used == 0 || time_elapsed || !no_change) {
			last_time = system_timer_get_ms();
		}
		// We send a read callback if there is data in the buffer and it hasn't changed
		// for at least two bytes worth of time or if there are 60 or more bytes in the buffer
		if((used > 0 && no_change && time_elapsed) || used >= 60) {
			tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(ReadCallbackCallback), FID_CALLBACK_READ_CALLBACK);
			// Read bytes available in ringbuffer
			uint8_t read = 0;
			for(; read < MESSAGE_LENGTH; read++) {
				if(!ringbuffer_get(&rs485.ringbuffer_rx, (uint8_t*)&cb.message[read])) {
					break;
				}
			}

			// Fill rest with 0
			for(uint8_t i = read; i < MESSAGE_LENGTH; i++) {
				cb.message[read] = 0;
			}

			cb.length = read;

			is_buffered = true;
		}
		else {
			is_buffered = false;
			return false;
		}
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(ReadCallbackCallback));
		is_buffered = false;

		return true;
	}
	else {
		is_buffered = true;
	}

	return false;
}

bool handle_error_count_callback(void) {
	static bool is_buffered = false;
	static ErrorCountCallback cb;

	static uint32_t last_error_count_parity = 0;
	static uint32_t last_error_count_overrun = 0;
	static uint32_t last_time = 0;

	if(!rs485.error_count_callback_enabled) {
		return false;
	}

	if(!is_buffered) {
		if(system_timer_is_time_elapsed_ms(last_time, CALLBACK_ERROR_COUNT_DEBOUNCE_MS) && (rs485.error_count_overrun != last_error_count_overrun || rs485.error_count_parity != last_error_count_parity)) {
			last_time = system_timer_get_ms();

			tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(ErrorCountCallback), FID_CALLBACK_ERROR_COUNT);
			cb.overrun_error_count = rs485.error_count_overrun;
			cb.parity_error_count  = rs485.error_count_parity;

			is_buffered = true;
		}
		else {
			is_buffered = false;
			return false;
		}
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(ErrorCountCallback));

		is_buffered = false;

		return true;
	}
	else {
		is_buffered = true;
	}

	return false;
}

// Modbus specific.
bool handle_modbus_read_coils_request_callback(void) {
	// This callback is processed only in slave mode.

	static ModbusReadCoilsRequestCallback cb;

	if((rs485.mode != MODE_MODBUS_SLAVE_RTU) ||
	   (rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_SLAVE_PROCESSING_REQUEST) ||
	   (rs485.modbus_rtu.request.rx_frame[1] != MODBUS_FC_READ_COILS) ||
	   !rs485.modbus_rtu.request.cb_invoke) {
	  return false;
	}

	memcpy(&cb.count, &rs485.modbus_rtu.request.rx_frame[4], 2);

	// Fix endianness (BE->LE).
	cb.count = NTOHS(cb.count);

	if(cb.count < 1 || cb.count > 2000) {
		rs485.modbus_common_error_counters.illegal_data_value++;

		_modbus_report_exception(&rs485, rs485.modbus_rtu.request.rx_frame[1], (uint8_t)MODBUS_EC_ILLEGAL_DATA_VALUE);

		return true;
	}

	 // Read data from rx_frame form a TFP callback packet and send.
	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		tfp_make_default_header(&cb.header,
		                        bootloader_get_uid(),
		                        sizeof(ModbusReadCoilsRequestCallback),
		                        FID_CALLBACK_MODBUS_READ_COILS_REQUEST);

		cb.request_id = rs485.modbus_rtu.request.id;

		memcpy(&cb.starting_address, &rs485.modbus_rtu.request.rx_frame[2], 2);

		// Fix endianness (BE->LE).
		cb.starting_address = NTOHS(cb.starting_address);

		bootloader_spitfp_send_ack_and_message(&bootloader_status,
		                                       (uint8_t*)&cb,
		                                       sizeof(ModbusReadCoilsRequestCallback));

		rs485.modbus_rtu.request.cb_invoke = false;

		return true;
	}

	return false;
}

bool handle_modbus_read_coils_response_low_level_callback(void) {
	// This callback is processed only in master mode.

	uint16_t chunks = 0;
	static ModbusReadCoilsResponseLowLevelCallback cb;

	if((rs485.mode != MODE_MODBUS_MASTER_RTU) ||
	   (rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE) ||
	   (rs485.modbus_rtu.request.tx_frame[1] != MODBUS_FC_READ_COILS) ||
	   !rs485.modbus_rtu.request.cb_invoke) {
	  return false;
	}

	cb.request_id = rs485.modbus_rtu.request.id;
	cb.exception_code = 0;

	tfp_make_default_header(&cb.header,
	                        bootloader_get_uid(),
	                        sizeof(ModbusReadCoilsResponseLowLevelCallback),
	                        FID_CALLBACK_MODBUS_READ_COILS_RESPONSE_LOW_LEVEL);

	// Check if the request has timed out.
	if(rs485.modbus_rtu.request.master_request_timed_out) {
		cb.exception_code = (int8_t)MODBUS_EC_TIMEOUT;
		cb.stream_total_length = 0;
		cb.stream_chunk_offset = 0;

		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			modbus_clear_request(&rs485);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusReadCoilsResponseLowLevelCallback));
			return true;
		}
		else {
			return false;
		}
	}

	// Check if the slave response is an exception.
	if(rs485.modbus_rtu.request.rx_frame[1] == rs485.modbus_rtu.request.tx_frame[1] + 0x80) {
		cb.exception_code = rs485.modbus_rtu.request.rx_frame[2];
		cb.stream_total_length = 0;
		cb.stream_chunk_offset = 0;

		if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_FUNCTION) {
			rs485.modbus_common_error_counters.illegal_function++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_DATA_ADDRESS) {
			rs485.modbus_common_error_counters.illegal_data_address++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_DATA_VALUE) {
			rs485.modbus_common_error_counters.illegal_data_value++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_SLAVE_DEVICE_FAILURE) {
			rs485.modbus_common_error_counters.slave_device_failure++;
		}

		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			modbus_clear_request(&rs485);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusReadCoilsResponseLowLevelCallback));
			return true;
		}
		else {
			return false;
		}
	}

	// Data to be handled.
	cb.stream_chunk_offset = 0;
	cb.stream_total_length = rs485.modbus_rtu.request.rx_frame[2];

	if(cb.stream_total_length <= sizeof(cb.stream_chunk_data)) {
		// Fits in one packet.
		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			memcpy(&cb.stream_chunk_data, &rs485.modbus_rtu.request.rx_frame[3], cb.stream_total_length);

			modbus_clear_request(&rs485);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusReadCoilsResponseLowLevelCallback));

			return true;
		}
		else {
			return false;
		}
	}
	else {
		// Need more than one packet to send the data.
		if(!rs485.modbus_rtu.request.stream_chunking->in_progress) {
			chunks = rs485.modbus_rtu.request.rx_frame[2] / sizeof(cb.stream_chunk_data);
			chunks = ((rs485.modbus_rtu.request.rx_frame[2] % sizeof(cb.stream_chunk_data)) > 0) ? chunks + 1 : chunks;

			rs485.modbus_rtu.request.stream_chunking->in_progress = true;
			rs485.modbus_rtu.request.stream_chunking->chunk_current = 0;
			rs485.modbus_rtu.request.stream_chunking->chunk_total = chunks;
			rs485.modbus_rtu.request.stream_chunking->stream_total = rs485.modbus_rtu.request.rx_frame[2];
		}

		return send_stream_chunks(FID_CALLBACK_MODBUS_READ_COILS_RESPONSE_LOW_LEVEL, &cb);
	}

	return false;
}

bool handle_modbus_read_holding_registers_request_callback(void) {
	// This callback is processed only in slave mode.

	static ModbusReadHoldingRegistersRequestCallback cb;

	if((rs485.mode != MODE_MODBUS_SLAVE_RTU) ||
	   (rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_SLAVE_PROCESSING_REQUEST) ||
	   (rs485.modbus_rtu.request.rx_frame[1] != MODBUS_FC_READ_HOLDING_REGISTERS) ||
	   !rs485.modbus_rtu.request.cb_invoke) {
	  return false;
	}

	memcpy(&cb.count, &rs485.modbus_rtu.request.rx_frame[4], 2);

	// Fix endianness (BE->LE).
	cb.count = NTOHS(cb.count);

	if(cb.count < 1 || cb.count > 125) {
		rs485.modbus_common_error_counters.illegal_data_value++;

		_modbus_report_exception(&rs485, rs485.modbus_rtu.request.rx_frame[1], (uint8_t)MODBUS_EC_ILLEGAL_DATA_VALUE);

		return true;
	}

	 // Read data from rx_frame, form a TFP callback packet and send.
	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		tfp_make_default_header(&cb.header,
		                        bootloader_get_uid(),
		                        sizeof(ModbusReadHoldingRegistersRequestCallback),
		                        FID_CALLBACK_MODBUS_READ_HOLDING_REGISTERS_REQUEST);

		cb.request_id = rs485.modbus_rtu.request.id;

		memcpy(&cb.starting_address, &rs485.modbus_rtu.request.rx_frame[2], 2);

		// Fix endianness (BE->LE).
		cb.starting_address = NTOHS(cb.starting_address);

		bootloader_spitfp_send_ack_and_message(&bootloader_status,
		                                       (uint8_t*)&cb,
		                                       sizeof(ModbusReadHoldingRegistersRequestCallback));

		rs485.modbus_rtu.request.cb_invoke = false;

		return true;
	}

	return false;
}

bool handle_modbus_read_holding_registers_response_low_level_callback(void) {
	// This callback is processed only in master mode.

	uint16_t chunks = 0;
	static ModbusReadHoldingRegistersResponseLowLevelCallback cb;

	if((rs485.mode != MODE_MODBUS_MASTER_RTU) ||
	   (rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE) ||
	   (rs485.modbus_rtu.request.tx_frame[1] != MODBUS_FC_READ_HOLDING_REGISTERS) ||
	   !rs485.modbus_rtu.request.cb_invoke) {
		return false;
	}

	cb.request_id = rs485.modbus_rtu.request.id;
	cb.exception_code = 0;

	tfp_make_default_header(&cb.header,
	                        bootloader_get_uid(),
	                        sizeof(ModbusReadHoldingRegistersResponseLowLevelCallback),
	                        FID_CALLBACK_MODBUS_READ_HOLDING_REGISTERS_RESPONSE_LOW_LEVEL);

	// Check if the request has timed out.
	if(rs485.modbus_rtu.request.master_request_timed_out) {
		cb.exception_code = (int8_t)MODBUS_EC_TIMEOUT;
		cb.stream_total_length = 0;
		cb.stream_chunk_offset = 0;

		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			modbus_clear_request(&rs485);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusReadHoldingRegistersResponseLowLevelCallback));

			return true;
		}
		else {
			return false;
		}
	}

	// Check if the slave response is an exception.
	if(rs485.modbus_rtu.request.rx_frame[1] == rs485.modbus_rtu.request.tx_frame[1] + 0x80) {
		cb.exception_code = rs485.modbus_rtu.request.rx_frame[2];
		cb.stream_total_length = 0;
		cb.stream_chunk_offset = 0;

		if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_FUNCTION) {
			rs485.modbus_common_error_counters.illegal_function++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_DATA_ADDRESS) {
			rs485.modbus_common_error_counters.illegal_data_address++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_DATA_VALUE) {
			rs485.modbus_common_error_counters.illegal_data_value++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_SLAVE_DEVICE_FAILURE) {
			rs485.modbus_common_error_counters.slave_device_failure++;
		}

		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			modbus_clear_request(&rs485);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusReadHoldingRegistersResponseLowLevelCallback));
			return true;
		}
		else {
			return false;
		}
	}

	// Data to be handled.
	cb.stream_chunk_offset = 0;
	cb.stream_total_length = rs485.modbus_rtu.request.rx_frame[2] / 2;

	if(cb.stream_total_length <= (sizeof(cb.stream_chunk_data) / 2)) {
		// Fits in one packet.
		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			uint16_t *_data = (uint16_t *)&rs485.modbus_rtu.request.rx_frame[3];

			for(uint16_t i = 0; i < cb.stream_total_length; i++) {
				// Before copying convert the data to host order from network order.
				cb.stream_chunk_data[i] = NTOHS(_data[i]);
			}

			modbus_clear_request(&rs485);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusReadHoldingRegistersResponseLowLevelCallback));

			return true;
		}
		else {
			return false;
		}
	}
	else {
		// Need more than one packet to send the data.
		if(!rs485.modbus_rtu.request.stream_chunking->in_progress) {
			chunks = rs485.modbus_rtu.request.rx_frame[2] / sizeof(cb.stream_chunk_data);
			chunks = ((rs485.modbus_rtu.request.rx_frame[2] % sizeof(cb.stream_chunk_data)) > 0) ? chunks + 1 : chunks;

			rs485.modbus_rtu.request.stream_chunking->in_progress = true;
			rs485.modbus_rtu.request.stream_chunking->chunk_current = 0;
			rs485.modbus_rtu.request.stream_chunking->chunk_total = chunks;
			rs485.modbus_rtu.request.stream_chunking->stream_total = rs485.modbus_rtu.request.rx_frame[2] / 2;
		}

		return send_stream_chunks(FID_CALLBACK_MODBUS_READ_HOLDING_REGISTERS_RESPONSE_LOW_LEVEL, &cb);
	}

	return false;
}

bool handle_modbus_write_single_coil_request_callback(void) {
	// This callback is processed only in slave mode.

	static ModbusWriteSingleCoilRequestCallback cb;

	if((rs485.mode != MODE_MODBUS_SLAVE_RTU) ||
	   (rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_SLAVE_PROCESSING_REQUEST) ||
	   (rs485.modbus_rtu.request.rx_frame[1] != MODBUS_FC_WRITE_SINGLE_COIL) ||
	   !rs485.modbus_rtu.request.cb_invoke) {
	  return false;
	}

  memcpy(&cb.coil_address, &rs485.modbus_rtu.request.rx_frame[2], 2);
  memcpy(&cb.coil_value, &rs485.modbus_rtu.request.rx_frame[4], 2);

	// Fix endianness (BE->LE).
	cb.coil_address = NTOHS(cb.coil_address);
	cb.coil_value = NTOHS(cb.coil_value);

	if(cb.coil_value != 0x0000 && cb.coil_value != 0xFF00) {
		rs485.modbus_common_error_counters.illegal_data_value++;

		_modbus_report_exception(&rs485, rs485.modbus_rtu.request.rx_frame[1], (uint8_t)MODBUS_EC_ILLEGAL_DATA_VALUE);

		return true;
	}

	 // Read data from rx_frame, form a TFP callback packet and send.
	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		tfp_make_default_header(&cb.header,
		                        bootloader_get_uid(),
		                        sizeof(ModbusWriteSingleCoilRequestCallback),
		                        FID_CALLBACK_MODBUS_WRITE_SINGLE_COIL_REQUEST);

		cb.request_id = rs485.modbus_rtu.request.id;

		bootloader_spitfp_send_ack_and_message(&bootloader_status,
		                                       (uint8_t*)&cb,
		                                       sizeof(ModbusWriteSingleCoilRequestCallback));

		rs485.modbus_rtu.request.cb_invoke = false;

		return true;
	}
	else {
		return false;
  }
}

bool handle_modbus_write_single_coil_response_callback(void) {
	// This callback is processed only in master mode.

	ModbusWriteSingleCoilResponseCallback cb;

	if((rs485.mode != MODE_MODBUS_MASTER_RTU) ||
	   (rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE) ||
	   (rs485.modbus_rtu.request.tx_frame[1] != MODBUS_FC_WRITE_SINGLE_COIL) ||
	   !rs485.modbus_rtu.request.cb_invoke) {
		return false;
	}

	cb.request_id = rs485.modbus_rtu.request.id;
	cb.exception_code = 0;

	tfp_make_default_header(&cb.header,
	                        bootloader_get_uid(),
	                        sizeof(ModbusWriteSingleCoilResponseCallback),
	                        FID_CALLBACK_MODBUS_WRITE_SINGLE_COIL_RESPONSE);

	// Check if the request has timed out.
	if(rs485.modbus_rtu.request.master_request_timed_out) {
	   cb.exception_code = (int8_t)MODBUS_EC_TIMEOUT;
	   cb.coil_address = 0;
	   cb.coil_value = 0;

		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			modbus_clear_request(&rs485);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusWriteSingleCoilResponseCallback));

			return true;
		}
		else {
			return false;
		}
	}

	// Check if the slave response is an exception.
	if(rs485.modbus_rtu.request.rx_frame[1] == rs485.modbus_rtu.request.tx_frame[1] + 0x80) {
		cb.exception_code = rs485.modbus_rtu.request.rx_frame[2];

		if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_FUNCTION) {
			rs485.modbus_common_error_counters.illegal_function++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_DATA_ADDRESS) {
			rs485.modbus_common_error_counters.illegal_data_address++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_DATA_VALUE) {
			rs485.modbus_common_error_counters.illegal_data_value++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_SLAVE_DEVICE_FAILURE) {
			rs485.modbus_common_error_counters.slave_device_failure++;
		}

		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			modbus_clear_request(&rs485);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusWriteSingleCoilResponseCallback));
			return true;
		}
		else {
			return false;
		}
	}

	memcpy(&cb.coil_address, &rs485.modbus_rtu.request.rx_frame[2], 2);
	memcpy(&cb.coil_value, &rs485.modbus_rtu.request.rx_frame[4], 2);

	cb.coil_address = NTOHS(cb.coil_address);
	cb.coil_value = NTOHS(cb.coil_value);

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
	   modbus_clear_request(&rs485);

	  bootloader_spitfp_send_ack_and_message(&bootloader_status,
	                                         (uint8_t*)&cb,
	                                         sizeof(ModbusWriteSingleCoilResponseCallback));

	  return true;
	}
	else {
		return false;
	}
}

bool handle_modbus_write_single_register_request_callback(void) {
	// This callback is processed only in slave mode.

	static ModbusWriteSingleRegisterRequestCallback cb;

	if((rs485.mode != MODE_MODBUS_SLAVE_RTU) ||
	   (rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_SLAVE_PROCESSING_REQUEST) ||
	   (rs485.modbus_rtu.request.rx_frame[1] != MODBUS_FC_WRITE_SINGLE_REGISTER) ||
	   !rs485.modbus_rtu.request.cb_invoke) {
	  return false;
	}

  memcpy(&cb.register_address, &rs485.modbus_rtu.request.rx_frame[2], 2);
  memcpy(&cb.register_value, &rs485.modbus_rtu.request.rx_frame[4], 2);

	// Fix endianness (BE->LE).
	cb.register_address = NTOHS(cb.register_address);
	cb.register_value = NTOHS(cb.register_value);

	 // Read data from rx_frame, form a TFP callback packet and send.
	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		tfp_make_default_header(&cb.header,
		                        bootloader_get_uid(),
		                        sizeof(ModbusWriteSingleRegisterRequestCallback),
		                        FID_CALLBACK_MODBUS_WRITE_SINGLE_REGISTER_REQUEST);

		cb.request_id = rs485.modbus_rtu.request.id;

		bootloader_spitfp_send_ack_and_message(&bootloader_status,
		                                       (uint8_t*)&cb,
		                                       sizeof(ModbusWriteSingleRegisterRequestCallback));

		rs485.modbus_rtu.request.cb_invoke = false;

		return true;
	}
	else {
		return false;
  }
}

bool handle_modbus_write_single_register_response_callback(void) {
	// This callback is processed only in master mode.

	ModbusWriteSingleRegisterResponseCallback cb;

	if((rs485.mode != MODE_MODBUS_MASTER_RTU) ||
		 (rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE) ||
		 (rs485.modbus_rtu.request.tx_frame[1] != MODBUS_FC_WRITE_SINGLE_REGISTER) ||
		 !rs485.modbus_rtu.request.cb_invoke) {
		return false;
	}

	cb.request_id = rs485.modbus_rtu.request.id;
	cb.exception_code = 0;

	tfp_make_default_header(&cb.header,
	                        bootloader_get_uid(),
	                        sizeof(ModbusWriteSingleRegisterResponseCallback),
	                        FID_CALLBACK_MODBUS_WRITE_SINGLE_REGISTER_RESPONSE);

	// Check if the request has timed out.
	if(rs485.modbus_rtu.request.master_request_timed_out) {
	   cb.exception_code = (int8_t)MODBUS_EC_TIMEOUT;
	   cb.register_address = 0;
	   cb.register_value = 0;

		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			modbus_clear_request(&rs485);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusWriteSingleRegisterResponseCallback));

			return true;
		}
		else {
			return false;
		}
	}

	// Check if the slave response is an exception.
	if(rs485.modbus_rtu.request.rx_frame[1] == rs485.modbus_rtu.request.tx_frame[1] + 0x80) {
		cb.exception_code = rs485.modbus_rtu.request.rx_frame[2];

		if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_FUNCTION) {
			rs485.modbus_common_error_counters.illegal_function++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_DATA_ADDRESS) {
			rs485.modbus_common_error_counters.illegal_data_address++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_DATA_VALUE) {
			rs485.modbus_common_error_counters.illegal_data_value++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_SLAVE_DEVICE_FAILURE) {
			rs485.modbus_common_error_counters.slave_device_failure++;
		}

		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			modbus_clear_request(&rs485);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusWriteSingleRegisterResponseCallback));
			return true;
		}
		else {
			return false;
		}
	}

	memcpy(&cb.register_address, &rs485.modbus_rtu.request.rx_frame[2], 2);
	memcpy(&cb.register_value, &rs485.modbus_rtu.request.rx_frame[4], 2);

	cb.register_address = NTOHS(cb.register_address);
	cb.register_value = NTOHS(cb.register_value);

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		modbus_clear_request(&rs485);

		bootloader_spitfp_send_ack_and_message(&bootloader_status,
	                                         (uint8_t*)&cb,
	                                         sizeof(ModbusWriteSingleRegisterResponseCallback));

	  return true;
	}
	else {
		return false;
	}
}

bool handle_modbus_write_multiple_coils_request_low_level_callback(void) {
	// This callback is processed only in slave mode.

	uint16_t chunks = 0;
	static ModbusWriteMultipleCoilsRequestLowLevelCallback cb;

	if((rs485.mode != MODE_MODBUS_SLAVE_RTU) ||
	   (rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_SLAVE_PROCESSING_REQUEST) ||
	   (rs485.modbus_rtu.request.rx_frame[1] != MODBUS_FC_WRITE_MULTIPLE_COILS) ||
	   !rs485.modbus_rtu.request.cb_invoke) {
	  return false;
	}

	memcpy(&cb.count, &rs485.modbus_rtu.request.rx_frame[4], 2);

	// Fix endianness (BE->LE).
	cb.count = NTOHS(cb.count);

	if(cb.count < 1 || cb.count > 1968) {
		rs485.modbus_common_error_counters.illegal_data_value++;

		_modbus_report_exception(&rs485, rs485.modbus_rtu.request.rx_frame[1], (uint8_t)MODBUS_EC_ILLEGAL_DATA_VALUE);

		rs485.modbus_rtu.request.cb_invoke = false;

		return true;
	}

	memcpy(&cb.starting_address, &rs485.modbus_rtu.request.rx_frame[2], 2);

	// Fix endianness (BE->LE).
	cb.starting_address = NTOHS(cb.starting_address);

	cb.request_id = rs485.modbus_rtu.request.id;

	// Data to be handled.
	cb.stream_chunk_offset = 0;
	cb.stream_total_length = rs485.modbus_rtu.request.rx_frame[6];

	tfp_make_default_header(&cb.header,
	                        bootloader_get_uid(),
	                        sizeof(ModbusWriteMultipleCoilsRequestLowLevelCallback),
	                        FID_CALLBACK_MODBUS_WRITE_MULTIPLE_COILS_REQUEST_LOW_LEVEL);

	if(cb.stream_total_length <= sizeof(cb.stream_chunk_data)) {
		// Fits in one packet.
		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			memcpy(&cb.stream_chunk_data, &rs485.modbus_rtu.request.rx_frame[7], cb.stream_total_length);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusWriteMultipleCoilsRequestLowLevelCallback));

			rs485.modbus_rtu.request.cb_invoke = false;

			return true;
		}
		else {
			return false;
		}
	}
	else {
		// Need more than one packet to send the data.
		if(!rs485.modbus_rtu.request.stream_chunking->in_progress) {
			chunks = rs485.modbus_rtu.request.rx_frame[6] / sizeof(cb.stream_chunk_data);
			chunks = ((rs485.modbus_rtu.request.rx_frame[6] % sizeof(cb.stream_chunk_data)) > 0) ? chunks + 1 : chunks;

			rs485.modbus_rtu.request.stream_chunking->in_progress = true;
			rs485.modbus_rtu.request.stream_chunking->chunk_current = 0;
			rs485.modbus_rtu.request.stream_chunking->chunk_total = chunks;
			rs485.modbus_rtu.request.stream_chunking->stream_total = rs485.modbus_rtu.request.rx_frame[6];
		}

		return send_stream_chunks(FID_CALLBACK_MODBUS_WRITE_MULTIPLE_COILS_REQUEST_LOW_LEVEL, &cb);
	}
}

bool handle_modbus_write_multiple_coils_response_callback(void) {
	// This callback is processed only in master mode.

	ModbusWriteMultipleCoilsResponseCallback cb;

	if((rs485.mode != MODE_MODBUS_MASTER_RTU) ||
	   (rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE) ||
	   (rs485.modbus_rtu.request.tx_frame[1] != MODBUS_FC_WRITE_MULTIPLE_COILS) ||
	   !rs485.modbus_rtu.request.cb_invoke) {
	  return false;
	}

	cb.request_id = rs485.modbus_rtu.request.id;
	cb.exception_code = 0;

	tfp_make_default_header(&cb.header,
	                        bootloader_get_uid(),
	                        sizeof(ModbusWriteMultipleCoilsResponseCallback),
	                        FID_CALLBACK_MODBUS_WRITE_MULTIPLE_COILS_RESPONSE);

	// Check if the request has timed out.
	if(rs485.modbus_rtu.request.master_request_timed_out) {
	   cb.exception_code = (int8_t)MODBUS_EC_TIMEOUT;
	   cb.starting_address = 0;
	   cb.count = 0;

		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			modbus_clear_request(&rs485);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusWriteMultipleCoilsResponseCallback));

			return true;
		}
		else {
			return false;
		}
	}

	// Check if the slave response is an exception.
	if(rs485.modbus_rtu.request.rx_frame[1] == rs485.modbus_rtu.request.tx_frame[1] + 0x80) {
		cb.exception_code = rs485.modbus_rtu.request.rx_frame[2];

		if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_FUNCTION) {
			rs485.modbus_common_error_counters.illegal_function++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_DATA_ADDRESS) {
			rs485.modbus_common_error_counters.illegal_data_address++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_DATA_VALUE) {
			rs485.modbus_common_error_counters.illegal_data_value++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_SLAVE_DEVICE_FAILURE) {
			rs485.modbus_common_error_counters.slave_device_failure++;
		}

		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			modbus_clear_request(&rs485);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusWriteMultipleCoilsResponseCallback));

			return true;
		}
		else {
			return false;
		}
	}

	memcpy(&cb.starting_address, &rs485.modbus_rtu.request.rx_frame[2], 2);
	memcpy(&cb.count, &rs485.modbus_rtu.request.rx_frame[4], 2);

	cb.starting_address = NTOHS(cb.starting_address);
	cb.count = NTOHS(cb.count);

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
	   modbus_clear_request(&rs485);

	  bootloader_spitfp_send_ack_and_message(&bootloader_status,
	                                         (uint8_t*)&cb,
	                                         sizeof(ModbusWriteMultipleCoilsResponseCallback));

	  return true;
	}
	else {
		return false;
	}
}

bool handle_modbus_write_multiple_registers_request_low_level_callback(void) {
	// This callback is processed only in slave mode.

	uint16_t chunks = 0;
	static ModbusWriteMultipleRegistersRequestLowLevelCallback cb;

	if((rs485.mode != MODE_MODBUS_SLAVE_RTU) ||
	   (rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_SLAVE_PROCESSING_REQUEST) ||
	   (rs485.modbus_rtu.request.rx_frame[1] != MODBUS_FC_WRITE_MULTIPLE_REGISTERS) ||
	   !rs485.modbus_rtu.request.cb_invoke) {
	  return false;
	}

	memcpy(&cb.count, &rs485.modbus_rtu.request.rx_frame[4], 2);

	// Fix endianness (BE->LE).
	cb.count = NTOHS(cb.count);

	if(cb.count < 1 || cb.count > 123) {
		rs485.modbus_common_error_counters.illegal_data_value++;

		_modbus_report_exception(&rs485, rs485.modbus_rtu.request.rx_frame[1], (uint8_t)MODBUS_EC_ILLEGAL_DATA_VALUE);

		rs485.modbus_rtu.request.cb_invoke = false;

		return true;
	}

	memcpy(&cb.starting_address, &rs485.modbus_rtu.request.rx_frame[2], 2);

	// Fix endianness (BE->LE).
	cb.starting_address = NTOHS(cb.starting_address);

	cb.request_id = rs485.modbus_rtu.request.id;

	// Data to be handled.
	cb.stream_chunk_offset = 0;
	cb.stream_total_length = rs485.modbus_rtu.request.rx_frame[6] / 2;

	tfp_make_default_header(&cb.header,
	                        bootloader_get_uid(),
	                        sizeof(ModbusWriteMultipleRegistersRequestLowLevelCallback),
	                        FID_CALLBACK_MODBUS_WRITE_MULTIPLE_REGISTERS_REQUEST_LOW_LEVEL);

	if(rs485.modbus_rtu.request.rx_frame[6] <= sizeof(cb.stream_chunk_data)) {
		// Fits in one packet.
		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			for(uint16_t i = 0; i < rs485.modbus_rtu.request.rx_frame[6] / 2; i++) {
				uint16_t value = 0;

				memcpy(&value, &rs485.modbus_rtu.request.rx_frame[7] + (i * 2), sizeof(uint16_t));
				value = NTOHS(value);

				memcpy(&cb.stream_chunk_data[i], &value, sizeof(uint16_t));
			}

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusWriteMultipleRegistersRequestLowLevelCallback));

			rs485.modbus_rtu.request.cb_invoke = false;

			return true;
		}
		else {
			return false;
		}
	}
	else {
		// Need more than one packet to send the data.
		if(!rs485.modbus_rtu.request.stream_chunking->in_progress) {
			chunks = rs485.modbus_rtu.request.rx_frame[6] / sizeof(cb.stream_chunk_data);
			chunks = ((rs485.modbus_rtu.request.rx_frame[6] % sizeof(cb.stream_chunk_data)) > 0) ? chunks + 1 : chunks;

			rs485.modbus_rtu.request.stream_chunking->in_progress = true;
			rs485.modbus_rtu.request.stream_chunking->chunk_current = 0;
			rs485.modbus_rtu.request.stream_chunking->chunk_total = chunks;
			rs485.modbus_rtu.request.stream_chunking->stream_total = rs485.modbus_rtu.request.rx_frame[6] / 2;
		}

		return send_stream_chunks(FID_CALLBACK_MODBUS_WRITE_MULTIPLE_REGISTERS_REQUEST_LOW_LEVEL, &cb);
	}
}

bool handle_modbus_write_multiple_registers_response_callback(void) {
	// This callback is processed only in master mode.

	ModbusWriteMultipleRegistersResponseCallback cb;

	if((rs485.mode != MODE_MODBUS_MASTER_RTU) ||
	   (rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_MASTER_WAITING_RESPONSE) ||
	   (rs485.modbus_rtu.request.tx_frame[1] != MODBUS_FC_WRITE_MULTIPLE_REGISTERS) ||
	   !rs485.modbus_rtu.request.cb_invoke) {
	  return false;
	}

	cb.request_id = rs485.modbus_rtu.request.id;
	cb.exception_code = 0;

	tfp_make_default_header(&cb.header,
	                        bootloader_get_uid(),
	                        sizeof(ModbusWriteMultipleRegistersResponseCallback),
	                        FID_CALLBACK_MODBUS_WRITE_MULTIPLE_REGISTERS_RESPONSE);

	// Check if the request has timed out.
	if(rs485.modbus_rtu.request.master_request_timed_out) {
	   cb.exception_code = (int8_t)MODBUS_EC_TIMEOUT;
	   cb.starting_address = 0;
	   cb.count = 0;

		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			modbus_clear_request(&rs485);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusWriteMultipleRegistersResponseCallback));

			return true;
		}
		else {
			return false;
		}
	}

	// Check if the slave response is an exception.
	if(rs485.modbus_rtu.request.rx_frame[1] == rs485.modbus_rtu.request.tx_frame[1] + 0x80) {
		cb.exception_code = rs485.modbus_rtu.request.rx_frame[2];

		if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_FUNCTION) {
			rs485.modbus_common_error_counters.illegal_function++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_DATA_ADDRESS) {
			rs485.modbus_common_error_counters.illegal_data_address++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_ILLEGAL_DATA_VALUE) {
			rs485.modbus_common_error_counters.illegal_data_value++;
		}
		else if(rs485.modbus_rtu.request.rx_frame[2] == MODBUS_EC_SLAVE_DEVICE_FAILURE) {
			rs485.modbus_common_error_counters.slave_device_failure++;
		}

		if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
			modbus_clear_request(&rs485);

			bootloader_spitfp_send_ack_and_message(&bootloader_status,
			                                       (uint8_t*)&cb,
			                                       sizeof(ModbusWriteMultipleRegistersResponseCallback));

			return true;
		}
		else {
			return false;
		}
	}

	memcpy(&cb.starting_address, &rs485.modbus_rtu.request.rx_frame[2], 2);
	memcpy(&cb.count, &rs485.modbus_rtu.request.rx_frame[4], 2);

	cb.starting_address = NTOHS(cb.starting_address);
	cb.count = NTOHS(cb.count);

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
	   modbus_clear_request(&rs485);

	  bootloader_spitfp_send_ack_and_message(&bootloader_status,
	                                         (uint8_t*)&cb,
	                                         sizeof(ModbusWriteMultipleRegistersResponseCallback));

	  return true;
	}
	else {
		return false;
	}
}

#if 0
void handle_xxx_callback(void) {
	static bool is_buffered = false;
	static XxxCallback cb;

	if(!is_buffered) {
		tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(XxxCallback), FID_CALLBACK_XXX);
		// TODO: Implement Xxx callback handling

		return false;
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(XxxCallback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
	}

	return false;
}
#endif


void communication_tick(void) {
	communication_callback_tick();
}

void communication_init(void) {
	communication_callback_init();
}
