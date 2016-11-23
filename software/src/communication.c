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

#include "bricklib2/hal/system_timer/system_timer.h"
#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/utility/ringbuffer.h"
#include "bricklib2/utility/util_definitions.h"
#include "bricklib2/bootloader/bootloader.h"

#include "xmc_usic.h"
#include "xmc_uart.h"

#define MESSAGE_LENGTH 60

extern RS485 rs485;

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_WRITE: return write(message, response);
		case FID_READ: return read(message, response);
		case FID_ENABLE_READ_CALLBACK: return enable_read_callback(message);
		case FID_DISABLE_READ_CALLBACK: return disable_read_callback(message);
		case FID_IS_READ_CALLBACK_ENABLED: return is_read_callback_enabled(message, response);
		case FID_SET_CONFIGURATION: return set_configuration(message);
		case FID_GET_CONFIGURATION: return get_configuration(message, response);
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

	rs485.baudrate   = BETWEEN(RS485_BAUDRATE_MIN, data->baudrate, RS485_BAUDRATE_MAX);
	rs485.parity     = data->parity;
	rs485.stopbits   = data->stopbits;
	rs485.wordlength = data->wordlength;
	rs485.duplex     = data->duplex;

	rs485_apply_configuration(&rs485);

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_configuration(const GetConfiguration *data, GetConfigurationResponse *response) {
	response->header.length = sizeof(GetConfigurationResponse);
	response->baudrate      = rs485.baudrate;
	response->parity        = rs485.parity;
	response->stopbits      = rs485.stopbits;
	response->wordlength    = rs485.wordlength;
	response->duplex        = rs485.duplex;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

bool handle_read_callback_callback(void) {
	static bool is_buffered = false;
	static ReadCallbackCallback cb;
	if(!rs485.read_callback_enabled) {
		return false;
	}

	if(!is_buffered) {
		if(ringbuffer_get_used(&rs485.ringbuffer_rx) > 0) {
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

bool handle_error_callback_callback(void) {
	static bool is_buffered = false;
	static ErrorCallbackCallback cb;

	if(!is_buffered) {
		if(rs485.error != 0) {
			tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(ErrorCallbackCallback), FID_CALLBACK_ERROR_CALLBACK);
			cb.error = rs485.error;
			rs485.error = 0;

			is_buffered = true;
		} else {
			is_buffered = false;
			return false;
		}
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(ErrorCallbackCallback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
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
