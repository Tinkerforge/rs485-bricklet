/* rs485-bricklet
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.h: TFP protocol message handling
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

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "bricklib2/bootloader/bootloader.h"

BootloaderHandleMessageResponse handle_message(const void *data, void *response);


#define FID_WRITE 1
#define FID_READ 2
#define FID_ENABLE_READ_CALLBACK 3
#define FID_DISABLE_READ_CALLBACK 4
#define FID_IS_READ_CALLBACK_ENABLED 5
#define FID_SET_CONFIGURATION 6
#define FID_GET_CONFIGURATION 7
#define FID_SET_BREAK_CONDITION 10

#define FID_CALLBACK_READ_CALLBACK 8
#define FID_CALLBACK_ERROR_CALLBACK 9


typedef struct {
	TFPMessageHeader header;
	char message[60];
	uint8_t length;
} __attribute__((__packed__)) Write;

typedef struct {
	TFPMessageHeader header;
	uint8_t written;
} __attribute__((__packed__)) WriteResponse;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) Read;

typedef struct {
	TFPMessageHeader header;
	char message[60];
	uint8_t length;
} __attribute__((__packed__)) ReadResponse;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) EnableReadCallback;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) DisableReadCallback;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) IsReadCallbackEnabled;

typedef struct {
	TFPMessageHeader header;
	bool enabled;
} __attribute__((__packed__)) IsReadCallbackEnabledResponse;

typedef struct {
	TFPMessageHeader header;
	uint8_t baudrate;
	uint8_t parity;
	uint8_t stopbits;
	uint8_t wordlength;
	uint8_t hardware_flowcontrol;
	uint8_t software_flowcontrol;
} __attribute__((__packed__)) SetConfiguration;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint8_t baudrate;
	uint8_t parity;
	uint8_t stopbits;
	uint8_t wordlength;
	uint8_t hardware_flowcontrol;
	uint8_t software_flowcontrol;
} __attribute__((__packed__)) GetConfigurationResponse;

typedef struct {
	TFPMessageHeader header;
	char message[60];
	uint8_t length;
} __attribute__((__packed__)) ReadCallbackCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t error;
} __attribute__((__packed__)) ErrorCallbackCallback;

typedef struct {
	TFPMessageHeader header;
	uint16_t break_time;
} __attribute__((__packed__)) SetBreakCondition;



BootloaderHandleMessageResponse write(const Write *data, WriteResponse *response);
BootloaderHandleMessageResponse read(const Read *data, ReadResponse *response);
BootloaderHandleMessageResponse enable_read_callback(const EnableReadCallback *data);
BootloaderHandleMessageResponse disable_read_callback(const DisableReadCallback *data);
BootloaderHandleMessageResponse is_read_callback_enabled(const IsReadCallbackEnabled *data, IsReadCallbackEnabledResponse *response);
BootloaderHandleMessageResponse set_configuration(const SetConfiguration *data);
BootloaderHandleMessageResponse get_configuration(const GetConfiguration *data, GetConfigurationResponse *response);
BootloaderHandleMessageResponse set_break_condition(const SetBreakCondition *data);


#endif
