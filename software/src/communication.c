/* rs485-bricklet
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
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

static bool modbus_slave_check_current_request(const uint8_t request_id) {
	if(rs485.mode != MODE_MODBUS_SLAVE ||
		 rs485.modbus_rtu.request.state != MODBUS_REQUEST_PROCESS_STATE_SLAVE_PROCESSING_REQUEST ||
		 request_id != rs485.modbus_rtu.request.id) {
			 return false;
	}

	return true;
}

static void modbus_store_tx_frame_data_bytes(const uint8_t *data, const uint8_t length) {
	for(uint16_t i = 0; i < length; i++) {
		ringbuffer_add(&rs485.ringbuffer_tx, data[i]);
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

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_WRITE: return write(message, response);
		case FID_READ: return read(message, response);
		case FID_ENABLE_READ_CALLBACK: return enable_read_callback(message);
		case FID_DISABLE_READ_CALLBACK: return disable_read_callback(message);
		case FID_IS_READ_CALLBACK_ENABLED: return is_read_callback_enabled(message, response);

		/*
		 * FIXME: Split set/get configuration function to set/get RS485 configuration,
		 * set/get Modbus configuration and set/get mode functions.
		 */
		case FID_SET_CONFIGURATION: return set_configuration(message);
		case FID_GET_CONFIGURATION: return get_configuration(message, response);

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
		case FID_ANSWER_MODBUS_READ_COILS_REQUEST_LOW_LEVEL: return answer_modbus_read_coils_request_low_level(message);

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

BootloaderHandleMessageResponse set_configuration(const SetConfiguration *data) {
	if((data->parity >= 3) ||
	   (data->stopbits != 1 && data->stopbits != 2) ||
	   (data->wordlength < 5 || data->wordlength > 8)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	rs485.mode                          = data->mode;
	rs485.baudrate                      = BETWEEN(RS485_BAUDRATE_MIN, data->baudrate, RS485_BAUDRATE_MAX);
	rs485.parity                        = data->parity;
	rs485.stopbits                      = data->stopbits;
	rs485.wordlength                    = data->wordlength;
	rs485.duplex                        = data->duplex;

	// Modbus specific.
	rs485.modbus_slave_address          = data->modbus_slave_address;
	rs485.modbus_master_request_timeout = data->modbus_master_request_timeout;

	rs485_apply_configuration(&rs485);

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_configuration(const GetConfiguration *data, GetConfigurationResponse *response) {
	response->header.length                 = sizeof(GetConfigurationResponse);
	response->mode                          = rs485.mode;
	response->baudrate                      = rs485.baudrate;
	response->parity                        = rs485.parity;
	response->stopbits                      = rs485.stopbits;
	response->wordlength                    = rs485.wordlength;
	response->duplex                        = rs485.duplex;

	// Modbus specific.
	response->modbus_slave_address          = rs485.modbus_slave_address;
	response->modbus_master_request_timeout = rs485.modbus_master_request_timeout;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
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

	// The error config and flicker config are not the same, we save the "show error" option as "external"
	if(data->config == ERROR_LED_CONFIG_SHOW_ERROR) {
		rs485.red_led_state.config = LED_FLICKER_CONFIG_EXTERNAL;
	} else {
		rs485.red_led_state.config = data->config;
	}

	// Set LED according to value
	if(rs485.red_led_state.config == ERROR_LED_CONFIG_OFF || rs485.red_led_state.config == LED_FLICKER_CONFIG_EXTERNAL) {
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
	} else {
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
BootloaderHandleMessageResponse answer_modbus_read_coils_request_low_level(const AnswerModbusReadCoilsRequestLowLevel *message) {
	uint16_t count = 0;
	uint16_t expected_bytes = 0;

	if(!modbus_slave_check_current_request(message->request_id)){
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(rs485.modbus_rtu.request.rx_frame[1] != MODBUS_FC_READ_COILS) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	// Total length exceeds maximum Modbus frame size.
	if(message->stream_total_length > RS485_MODBUS_RTU_FRAME_SIZE_MAX) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	memcpy(&count, &rs485.modbus_rtu.request.rx_frame[4], 2);
	// Fix endianness (BE->LE).
	count = (count >> 8) | (count << 8);

	expected_bytes = count / 8;

	if((count % 8) != 0) {
		expected_bytes ++;
	}

	if(message->stream_total_length != expected_bytes) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	// The first chunk.
	if(message->stream_chunk_offset == 0) {
		/*
		 * Return value of ringbuffer_add() is not checked as in Modbus mode
		 * a buffer overflow can't happen.
		 */
		ringbuffer_add(&rs485.ringbuffer_tx, rs485.modbus_slave_address);
		ringbuffer_add(&rs485.ringbuffer_tx, MODBUS_FC_READ_COILS);
		ringbuffer_add(&rs485.ringbuffer_tx, expected_bytes);

		if(message->stream_total_length <= 59) {
			// All data fits in the first chunk there will not be other chunks.
			modbus_store_tx_frame_data_bytes(message->stream_chunk_data, message->stream_total_length);

			// Calculate checksum and put it at the end of the TX buffer.
			modbus_add_tx_frame_checksum();

			/*
			 * The frame is in the TX buffer now. Enable TX callback which is called
			 * when the TX FIFO usage falls below a certain limit.
			 */
			modbus_start_tx_from_buffer(&rs485);

			return HANDLE_MESSAGE_RESPONSE_EMPTY;
		}
	}

	if((message->stream_chunk_offset + sizeof(message->stream_chunk_data)) <= message->stream_total_length) {
		modbus_store_tx_frame_data_bytes(message->stream_chunk_data, sizeof(message->stream_chunk_data));

		if((message->stream_chunk_offset + sizeof(message->stream_chunk_data)) == message->stream_total_length) {
			// We have the frame in the TX buffer.

			// Calculate checksum and put it at the end of the TX buffer.
			modbus_add_tx_frame_checksum();

			/*
			 * Enable TX callback which is called when the TX FIFO usage falls below
			 * a certain limit.
			 */
			modbus_start_tx_from_buffer(&rs485);
		}
	}
	else {
		modbus_store_tx_frame_data_bytes(message->stream_chunk_data,
																		 (message->stream_chunk_offset + sizeof(message->stream_chunk_data)) - message->stream_total_length);

		// We have the frame in the TX buffer.

		// Calculate checksum and put it at the end of the TX buffer.
		modbus_add_tx_frame_checksum();

		/*
		 * Enable TX callback which is called when the TX FIFO usage falls below
		 * a certain limit.
		 */
		modbus_start_tx_from_buffer(&rs485);
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse report_modbus_exception(const ReportModbusException *data) {
	if(!modbus_slave_check_current_request(data->request_id)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	modbus_report_exception(&rs485,
													rs485.modbus_rtu.request.rx_frame[1],
													data->exception_code);

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

bool handle_read_callback_callback(void) {
	static bool is_buffered = false;
	static ReadCallbackCallback cb;

	static uint32_t last_used = 0;
	static uint32_t last_time = 0;

	if(rs485.mode == MODE_MODBUS_MASTER ||
		 rs485.mode == MODE_MODBUS_SLAVE) {
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
		} else {
			is_buffered = false;
			return false;
		}
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(ReadCallbackCallback));
		is_buffered = false;
		return true;
	} else {
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
		} else {
			is_buffered = false;
			return false;
		}
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(ErrorCountCallback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
	}

	return false;
}

// Modbus specific.
bool handle_modbus_read_coils_request_callback(void) {
	static ModbusReadCoilsRequestCallback cb;

	if(rs485.mode == MODE_MODBUS_SLAVE &&
		 rs485.modbus_rtu.request.state == MODBUS_REQUEST_PROCESS_STATE_SLAVE_PROCESSING_REQUEST &&
		 rs485.modbus_rtu.request.rx_frame[1] == MODBUS_FC_READ_COILS &&
	   rs485.modbus_rtu.request.cb_invoke) {
			 	// Read data from rx_frame form a TFP callback packet and send.
				if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
					tfp_make_default_header(&cb.header,
																	bootloader_get_uid(),
																	sizeof(ModbusReadCoilsRequestCallback),
																	FID_CALLBACK_MODBUS_READ_COILS_REQUEST);

					cb.request_id = rs485.modbus_rtu.request.id;

					memcpy(&cb.count, &rs485.modbus_rtu.request.rx_frame[4], 2);
					memcpy(&cb.starting_address, &rs485.modbus_rtu.request.rx_frame[2], 2);

					// Fix endianness (BE->LE).
					cb.count = (cb.count >> 8) | (cb.count << 8);
					cb.starting_address = (cb.starting_address >> 8) | (cb.starting_address << 8);

					bootloader_spitfp_send_ack_and_message(&bootloader_status,
																								 (uint8_t*)&cb,
																								 sizeof(ModbusReadCoilsRequestCallback));

					rs485.modbus_rtu.request.cb_invoke = false;

					return true;
				}
				else {
					return false;
				}
	}

	return false;
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
