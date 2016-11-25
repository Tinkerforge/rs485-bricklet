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

#include <stdint.h>
#include <stdbool.h>

#include "bricklib2/bootloader/bootloader.h"

// Default functions
BootloaderHandleMessageResponse handle_message(const void *data, void *response);
void communication_tick(void);
void communication_init(void);

// Function and callback IDs and structs
#define FID_WRITE 1
#define FID_READ 2
#define FID_ENABLE_READ_CALLBACK 3
#define FID_DISABLE_READ_CALLBACK 4
#define FID_IS_READ_CALLBACK_ENABLED 5
#define FID_SET_CONFIGURATION 6
#define FID_GET_CONFIGURATION 7
#define FID_SET_COMMUNICATION_LED_CONFIG 8
#define FID_GET_COMMUNICATION_LED_CONFIG 9
#define FID_SET_ERROR_LED_CONFIG 10
#define FID_GET_ERROR_LED_CONFIG 11
#define FID_SET_BUFFER_CONFIG 12
#define FID_GET_BUFFER_CONFIG 13
#define FID_GET_BUFFER_STATUS 14
#define FID_ENABLE_ERROR_COUNT_CALLBACK 15
#define FID_DISABLE_ERROR_COUNT_CALLBACK 16
#define FID_IS_ERROR_COUNT_CALLBACK_ENABLED 17
#define FID_GET_ERROR_COUNT 18

#define FID_CALLBACK_READ_CALLBACK 19
#define FID_CALLBACK_ERROR_COUNT_CALLBACK 20

// enums
typedef enum {
	ERROR_LED_CONFIG_OFF = 0,
	ERROR_LED_CONFIG_ON = 1,
	ERROR_LED_CONFIG_SHOW_ERROR = 2
} ErrorLEDConfig;

typedef enum {
	COMMUNICATION_LED_CONFIG_OFF = 0,
	COMMUNICATION_LED_CONFIG_ON = 1,
	COMMUNICATION_LED_CONFIG_SHOW_COMMUNICATION = 2
} CommunicationLEDConfig;

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
	uint32_t baudrate;
	uint8_t parity;
	uint8_t stopbits;
	uint8_t wordlength;
	uint8_t duplex;
} __attribute__((__packed__)) SetConfiguration;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint32_t baudrate;
	uint8_t parity;
	uint8_t stopbits;
	uint8_t wordlength;
	uint8_t duplex;
} __attribute__((__packed__)) GetConfigurationResponse;

typedef struct {
	TFPMessageHeader header;
	uint8_t config;
} __attribute__((__packed__)) SetCommunicationLEDConfig;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetCommunicationLEDConfig;

typedef struct {
	TFPMessageHeader header;
	uint8_t config;
} __attribute__((__packed__)) GetCommunicationLEDConfigResponse;

typedef struct {
	TFPMessageHeader header;
	uint8_t config;
} __attribute__((__packed__)) SetErrorLEDConfig;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetErrorLEDConfig;

typedef struct {
	TFPMessageHeader header;
	uint8_t config;
} __attribute__((__packed__)) GetErrorLEDConfigResponse;

typedef struct {
	TFPMessageHeader header;
	uint16_t send_buffer_size;
	uint16_t receive_buffer_size;
} __attribute__((__packed__)) SetBufferConfig;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetBufferConfig;

typedef struct {
	TFPMessageHeader header;
	uint16_t send_buffer_size;
	uint16_t receive_buffer_size;
} __attribute__((__packed__)) GetBufferConfigResponse;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetBufferStatus;

typedef struct {
	TFPMessageHeader header;
	uint16_t send_buffer_used;
	uint16_t receive_buffer_used;
} __attribute__((__packed__)) GetBufferStatusResponse;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) EnableErrorCountCallback;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) DisableErrorCountCallback;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) IsErrorCountCallbackEnabled;

typedef struct {
	TFPMessageHeader header;
	bool enabled;
} __attribute__((__packed__)) IsErrorCountCallbackEnabledResponse;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetErrorCount;

typedef struct {
	TFPMessageHeader header;
	uint32_t overrun_error_count;
	uint32_t parity_error_count;
} __attribute__((__packed__)) GetErrorCountResponse;

typedef struct {
	TFPMessageHeader header;
	char message[60];
	uint8_t length;
} __attribute__((__packed__)) ReadCallbackCallback;

typedef struct {
	TFPMessageHeader header;
	uint32_t overrun_error_count;
	uint32_t parity_error_count;
} __attribute__((__packed__)) ErrorCountCallbackCallback;

// Function prototypes
BootloaderHandleMessageResponse write(const Write *data, WriteResponse *response);
BootloaderHandleMessageResponse read(const Read *data, ReadResponse *response);
BootloaderHandleMessageResponse enable_read_callback(const EnableReadCallback *data);
BootloaderHandleMessageResponse disable_read_callback(const DisableReadCallback *data);
BootloaderHandleMessageResponse is_read_callback_enabled(const IsReadCallbackEnabled *data, IsReadCallbackEnabledResponse *response);
BootloaderHandleMessageResponse set_configuration(const SetConfiguration *data);
BootloaderHandleMessageResponse get_configuration(const GetConfiguration *data, GetConfigurationResponse *response);
BootloaderHandleMessageResponse set_communication_led_config(const SetCommunicationLEDConfig *data);
BootloaderHandleMessageResponse get_communication_led_config(const GetCommunicationLEDConfig *data, GetCommunicationLEDConfigResponse *response);
BootloaderHandleMessageResponse set_error_led_config(const SetErrorLEDConfig *data);
BootloaderHandleMessageResponse get_error_led_config(const GetErrorLEDConfig *data, GetErrorLEDConfigResponse *response);
BootloaderHandleMessageResponse set_buffer_config(const SetBufferConfig *data);
BootloaderHandleMessageResponse get_buffer_config(const GetBufferConfig *data, GetBufferConfigResponse *response);
BootloaderHandleMessageResponse get_buffer_status(const GetBufferStatus *data, GetBufferStatusResponse *response);
BootloaderHandleMessageResponse enable_error_count_callback(const EnableErrorCountCallback *data);
BootloaderHandleMessageResponse disable_error_count_callback(const DisableErrorCountCallback *data);
BootloaderHandleMessageResponse is_error_count_callback_enabled(const IsErrorCountCallbackEnabled *data, IsErrorCountCallbackEnabledResponse *response);
BootloaderHandleMessageResponse get_error_count(const GetErrorCount *data, GetErrorCountResponse *response);

// Callbacks
bool handle_read_callback_callback(void);
bool handle_error_count_callback_callback(void);

#define CALLBACK_ERROR_COUNT_DEBOUNCE_MS 100
#define COMMUNICATION_CALLBACK_TICK_WAIT_MS 1
#define COMMUNICATION_CALLBACK_HANDLER_NUM 2
#define COMMUNICATION_CALLBACK_LIST_INIT \
	{NULL, NULL, handle_read_callback_callback}, \
	{NULL, NULL, handle_error_count_callback_callback},


#endif
