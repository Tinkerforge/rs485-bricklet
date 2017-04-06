/* rs485-bricklet
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 * Copyright (C) 2017 Ishraq Ibne Ashraf <ishraq@tinkerforge.com>
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
#define FID_SET_RS485_CONFIGURATION 6
#define FID_GET_RS485_CONFIGURATION 7

// Modbus specific.
#define FID_SET_MODBUS_CONFIGURATION 8
#define FID_GET_MODBUS_CONFIGURATION 9
#define FID_SET_MODE 10
#define FID_GET_MODE 11

#define FID_SET_COMMUNICATION_LED_CONFIG 12
#define FID_GET_COMMUNICATION_LED_CONFIG 13
#define FID_SET_ERROR_LED_CONFIG 14
#define FID_GET_ERROR_LED_CONFIG 15
#define FID_SET_BUFFER_CONFIG 16
#define FID_GET_BUFFER_CONFIG 17
#define FID_GET_BUFFER_STATUS 18
#define FID_ENABLE_ERROR_COUNT_CALLBACK 19
#define FID_DISABLE_ERROR_COUNT_CALLBACK 20
#define FID_IS_ERROR_COUNT_CALLBACK_ENABLED 21
#define FID_GET_ERROR_COUNT 22

// Modbus specific.
#define FID_GET_MODBUS_COMMON_ERROR_COUNT 23
#define FID_MODBUS_REPORT_EXCEPTION 24
#define FID_MODBUS_ANSWER_READ_COILS_REQUEST_LOW_LEVEL 25
#define FID_MODBUS_READ_COILS 26
#define FID_MODBUS_ANSWER_READ_HOLDING_REGISTERS_REQUEST_LOW_LEVEL 27
#define FID_MODBUS_READ_HOLDING_REGISTERS 28
#define FID_MODBUS_ANSWER_WRITE_SINGLE_COIL_REQUEST 29
#define FID_MODBUS_WRITE_SINGLE_COIL 30
#define FID_MODBUS_ANSWER_WRITE_SINGLE_REGISTER_REQUEST 31
#define FID_MODBUS_WRITE_SINGLE_REGISTER 32
#define FID_MODBUS_ANSWER_WRITE_MULTIPLE_COILS_REQUEST 33
#define FID_MODBUS_WRITE_MULTIPLE_COILS_LOW_LEVEL 34
#define FID_MODBUS_ANSWER_WRITE_MULTIPLE_REGISTERS_REQUEST 35
#define FID_MODBUS_WRITE_MULTIPLE_REGISTERS_LOW_LEVEL 36
#define FID_MODBUS_ANSWER_READ_DISCRETE_INPUTS_REQUEST_LOW_LEVEL 37
#define FID_MODBUS_READ_DISCRETE_INPUTS 38
#define FID_MODBUS_ANSWER_READ_INPUT_REGISTERS_REQUEST_LOW_LEVEL 39
#define FID_MODBUS_READ_INPUT_REGISTERS 40

// Callbacks.
#define FID_CALLBACK_READ_CALLBACK 41
#define FID_CALLBACK_ERROR_COUNT 42

// Modbus specific.
#define FID_CALLBACK_MODBUS_READ_COILS_REQUEST 43
#define FID_CALLBACK_MODBUS_READ_COILS_RESPONSE_LOW_LEVEL 44
#define FID_CALLBACK_MODBUS_READ_HOLDING_REGISTERS_REQUEST 45
#define FID_CALLBACK_MODBUS_READ_HOLDING_REGISTERS_RESPONSE_LOW_LEVEL 46
#define FID_CALLBACK_MODBUS_WRITE_SINGLE_COIL_REQUEST 47
#define FID_CALLBACK_MODBUS_WRITE_SINGLE_COIL_RESPONSE 48
#define FID_CALLBACK_MODBUS_WRITE_SINGLE_REGISTER_REQUEST 49
#define FID_CALLBACK_MODBUS_WRITE_SINGLE_REGISTER_RESPONSE 50
#define FID_CALLBACK_MODBUS_WRITE_MULTIPLE_COILS_REQUEST_LOW_LEVEL 51
#define FID_CALLBACK_MODBUS_WRITE_MULTIPLE_COILS_RESPONSE 52
#define FID_CALLBACK_MODBUS_WRITE_MULTIPLE_REGISTERS_REQUEST_LOW_LEVEL 53
#define FID_CALLBACK_MODBUS_WRITE_MULTIPLE_REGISTERS_RESPONSE 54
#define FID_CALLBACK_MODBUS_READ_DISCRETE_INPUTS_REQUEST 55
#define FID_CALLBACK_MODBUS_READ_DISCRETE_INPUTS_RESPONSE_LOW_LEVEL 56
#define FID_CALLBACK_MODBUS_READ_INPUT_REGISTERS_REQUEST 57
#define FID_CALLBACK_MODBUS_READ_INPUT_REGISTERS_RESPONSE_LOW_LEVEL 58

// enums
typedef enum {
	ERROR_LED_CONFIG_OFF = 0,
	ERROR_LED_CONFIG_ON = 1,
	ERROR_LED_CONFIG_SHOW_HEARTBEAT = 2,
	ERROR_LED_CONFIG_SHOW_ERROR = 3
} ErrorLEDConfig;

typedef enum {
	COMMUNICATION_LED_CONFIG_OFF = 0,
	COMMUNICATION_LED_CONFIG_ON = 1,
	COMMUNICATION_LED_CONFIG_SHOW_HEARTBEAT = 2,
	COMMUNICATION_LED_CONFIG_SHOW_COMMUNICATION = 3
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
} __attribute__((__packed__)) SetRS485Configuration;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetRS485Configuration;

typedef struct {
	TFPMessageHeader header;
	uint32_t baudrate;
	uint8_t parity;
	uint8_t stopbits;
	uint8_t wordlength;
	uint8_t duplex;
} __attribute__((__packed__)) GetRS485ConfigurationResponse;

// Modbus specific.
typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint32_t master_request_timeout;
} __attribute__((__packed__)) SetModbusConfiguration;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetModbusConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint32_t master_request_timeout;
} __attribute__((__packed__)) GetModbusConfigurationResponse;

typedef struct {
	TFPMessageHeader header;
	uint8_t mode;
} __attribute__((__packed__)) SetMode;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetMode;

typedef struct {
	TFPMessageHeader header;
	uint8_t mode;
} __attribute__((__packed__)) GetModeResponse;

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
} __attribute__((__packed__)) ErrorCountCallback;

// Modbus specific.
typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetModbusCommonErrorCount;

typedef struct {
	TFPMessageHeader header;
	uint32_t timeout_error_count;
	uint32_t checksum_error_count;
	uint32_t frame_too_big_error_count;
	uint32_t illegal_function_error_count;
	uint32_t illegal_data_address_error_count;
	uint32_t illegal_data_value_error_count;
	uint32_t slave_device_failure_error_count;
} __attribute__((__packed__)) GetModbusCommonErrorCountResponse;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint8_t exception_code;
} __attribute__((__packed__)) ModbusReportException;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint8_t stream_chunk_data[59];
} __attribute__((__packed__)) ModbusAnswerReadCoilsRequestLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusReadCoils;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusReadCoilsResponse;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusReadCoilsRequestCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint8_t stream_chunk_data[58];
} __attribute__((__packed__)) ModbusReadCoilsResponseLowLevelCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint16_t stream_chunk_data[29];
} __attribute__((__packed__)) ModbusAnswerReadHoldingRegistersRequestLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusReadHoldingRegisters;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusReadHoldingRegistersResponse;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusReadHoldingRegistersRequestCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint16_t stream_chunk_data[29];
} __attribute__((__packed__)) ModbusReadHoldingRegistersResponseLowLevelCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t coil_address;
	uint16_t coil_value;
} __attribute__((__packed__)) ModbusWriteSingleCoil;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusWriteSingleCoilResponse;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
  uint16_t coil_address;
	uint16_t coil_value;
} __attribute__((__packed__)) ModbusAnswerWriteSingleCoilRequest;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t coil_address;
	uint16_t coil_value;
} __attribute__((__packed__)) ModbusWriteSingleCoilRequestCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
  int8_t exception_code;
  uint16_t coil_address;
	uint16_t coil_value;
} __attribute__((__packed__)) ModbusWriteSingleCoilResponseCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t register_address;
	uint16_t register_value;
} __attribute__((__packed__)) ModbusWriteSingleRegister;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusWriteSingleRegisterResponse;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
  uint16_t register_address;
	uint16_t register_value;
} __attribute__((__packed__)) ModbusAnswerWriteSingleRegisterRequest;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t register_address;
	uint16_t register_value;
} __attribute__((__packed__)) ModbusWriteSingleRegisterRequestCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
  uint16_t register_address;
	uint16_t register_value;
} __attribute__((__packed__)) ModbusWriteSingleRegisterResponseCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusAnswerWriteMultipleCoilsRequest;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t starting_address;
	uint16_t count;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint8_t stream_chunk_data[54];
} __attribute__((__packed__)) ModbusWriteMultipleCoilsLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusWriteMultipleCoilsLowLevelResponse;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t starting_address;
	uint16_t count;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint8_t stream_chunk_data[55];
} __attribute__((__packed__)) ModbusWriteMultipleCoilsRequestLowLevelCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusWriteMultipleCoilsResponseCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusAnswerWriteMultipleRegistersRequest;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t starting_address;
	uint16_t count;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint16_t stream_chunk_data[27];
} __attribute__((__packed__)) ModbusWriteMultipleRegistersLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusWriteMultipleRegistersLowLevelResponse;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t starting_address;
	uint16_t count;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint16_t stream_chunk_data[27];
} __attribute__((__packed__)) ModbusWriteMultipleRegistersRequestLowLevelCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusWriteMultipleRegistersResponseCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint8_t stream_chunk_data[59];
} __attribute__((__packed__)) ModbusAnswerReadDiscreteInputsRequestLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusReadDiscreteInputs;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusReadDiscreteInputsResponse;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusReadDiscreteInputsRequestCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint8_t stream_chunk_data[58];
} __attribute__((__packed__)) ModbusReadDiscreteInputsResponseLowLevelCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint16_t stream_chunk_data[29];
} __attribute__((__packed__)) ModbusAnswerReadInputRegistersRequestLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusReadInputRegisters;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusReadInputRegistersResponse;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusReadInputRegistersRequestCallback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint16_t stream_chunk_data[29];
} __attribute__((__packed__)) ModbusReadInputRegistersResponseLowLevelCallback;

// Function prototypes
BootloaderHandleMessageResponse write(const Write *data, WriteResponse *response);
BootloaderHandleMessageResponse read(const Read *data, ReadResponse *response);
BootloaderHandleMessageResponse enable_read_callback(const EnableReadCallback *data);
BootloaderHandleMessageResponse disable_read_callback(const DisableReadCallback *data);
BootloaderHandleMessageResponse is_read_callback_enabled(const IsReadCallbackEnabled *data,
                                                         IsReadCallbackEnabledResponse *response);
BootloaderHandleMessageResponse set_rs485_configuration(const SetRS485Configuration *data);
BootloaderHandleMessageResponse get_rs485_configuration(const GetRS485Configuration *data,
                                                        GetRS485ConfigurationResponse *response);

// Modbus specific.
BootloaderHandleMessageResponse set_modbus_configuration(const SetModbusConfiguration *data);
BootloaderHandleMessageResponse get_modbus_configuration(const GetModbusConfiguration *data,
                                                         GetModbusConfigurationResponse *response);

BootloaderHandleMessageResponse set_mode(const SetMode *data);
BootloaderHandleMessageResponse get_mode(const GetMode *data, GetModeResponse *response);
BootloaderHandleMessageResponse set_communication_led_config(const SetCommunicationLEDConfig *data);
BootloaderHandleMessageResponse get_communication_led_config(const GetCommunicationLEDConfig *data,
                                                             GetCommunicationLEDConfigResponse *response);
BootloaderHandleMessageResponse set_error_led_config(const SetErrorLEDConfig *data);
BootloaderHandleMessageResponse get_error_led_config(const GetErrorLEDConfig *data,
                                                     GetErrorLEDConfigResponse *response);
BootloaderHandleMessageResponse set_buffer_config(const SetBufferConfig *data);
BootloaderHandleMessageResponse get_buffer_config(const GetBufferConfig *data,
                                                  GetBufferConfigResponse *response);
BootloaderHandleMessageResponse get_buffer_status(const GetBufferStatus *data,
                                                  GetBufferStatusResponse *response);
BootloaderHandleMessageResponse enable_error_count_callback(const EnableErrorCountCallback *data);
BootloaderHandleMessageResponse disable_error_count_callback(const DisableErrorCountCallback *data);
BootloaderHandleMessageResponse is_error_count_callback_enabled(const IsErrorCountCallbackEnabled *data,
                                                                IsErrorCountCallbackEnabledResponse *response);
BootloaderHandleMessageResponse get_error_count(const GetErrorCount *data,
                                                GetErrorCountResponse *response);

// Modbus specific.
BootloaderHandleMessageResponse modbus_report_exception(const ModbusReportException *data);
BootloaderHandleMessageResponse get_modbus_common_error_count(const GetModbusCommonErrorCount *data,
                                                              GetModbusCommonErrorCountResponse *response);
BootloaderHandleMessageResponse modbus_answer_read_coils_request_low_level(const ModbusAnswerReadCoilsRequestLowLevel *data);
BootloaderHandleMessageResponse modbus_read_coils(const ModbusReadCoils *data,
                                                  ModbusReadCoilsResponse *response);
BootloaderHandleMessageResponse modbus_answer_read_holding_registers_request_low_level(const ModbusAnswerReadHoldingRegistersRequestLowLevel *data);
BootloaderHandleMessageResponse modbus_read_holding_registers(const ModbusReadHoldingRegisters *data,
                                                              ModbusReadHoldingRegistersResponse *response);
BootloaderHandleMessageResponse modbus_answer_write_single_coil_request(const ModbusAnswerWriteSingleCoilRequest *data);
BootloaderHandleMessageResponse modbus_write_single_coil(const ModbusWriteSingleCoil *data,
                                                         ModbusWriteSingleCoilResponse *response);
BootloaderHandleMessageResponse modbus_answer_write_single_register_request(const ModbusAnswerWriteSingleRegisterRequest *data);
BootloaderHandleMessageResponse modbus_write_single_register(const ModbusWriteSingleRegister *data,
                                                             ModbusWriteSingleRegisterResponse *response);
BootloaderHandleMessageResponse modbus_answer_write_multiple_coils_request(const ModbusAnswerWriteMultipleCoilsRequest *data);
BootloaderHandleMessageResponse modbus_write_multiple_coils_low_level(const ModbusWriteMultipleCoilsLowLevel *data,
                                                                      ModbusWriteMultipleCoilsLowLevelResponse *response);
BootloaderHandleMessageResponse modbus_answer_write_multiple_registers_request(const ModbusAnswerWriteMultipleRegistersRequest *data);
BootloaderHandleMessageResponse modbus_write_multiple_registers_low_level(const ModbusWriteMultipleRegistersLowLevel *data,
                                                                          ModbusWriteMultipleRegistersLowLevelResponse *response);
BootloaderHandleMessageResponse modbus_answer_read_discrete_inputs_request_low_level(const ModbusAnswerReadDiscreteInputsRequestLowLevel *data);
BootloaderHandleMessageResponse modbus_read_discrete_inputs(const ModbusReadDiscreteInputs *data,
                                                            ModbusReadDiscreteInputsResponse *response);
BootloaderHandleMessageResponse modbus_answer_read_input_registers_request_low_level(const ModbusAnswerReadInputRegistersRequestLowLevel *data);
BootloaderHandleMessageResponse modbus_read_input_registers(const ModbusReadInputRegisters *data,
                                                            ModbusReadInputRegistersResponse *response);

// Callbacks
bool handle_read_callback_callback(void);
bool handle_error_count_callback(void);

// Modbus specific.
bool handle_modbus_read_coils_request_callback(void);
bool handle_modbus_read_coils_response_low_level_callback(void);
bool handle_modbus_read_holding_registers_request_callback(void);
bool handle_modbus_read_holding_registers_response_low_level_callback(void);
bool handle_modbus_write_single_coil_request_callback(void);
bool handle_modbus_write_single_coil_response_callback(void);
bool handle_modbus_write_single_register_request_callback(void);
bool handle_modbus_write_single_register_response_callback(void);
bool handle_modbus_write_multiple_coils_request_low_level_callback(void);
bool handle_modbus_write_multiple_coils_response_callback(void);
bool handle_modbus_write_multiple_registers_request_low_level_callback(void);
bool handle_modbus_write_multiple_registers_response_callback(void);
bool handle_modbus_read_discrete_inputs_request_callback(void);
bool handle_modbus_read_discrete_inputs_response_low_level_callback(void);
bool handle_modbus_read_input_registers_request_callback(void);
bool handle_modbus_read_input_registers_response_low_level_callback(void);

#define CALLBACK_ERROR_COUNT_DEBOUNCE_MS 100
#define COMMUNICATION_CALLBACK_TICK_WAIT_MS 1
#define COMMUNICATION_CALLBACK_HANDLER_NUM 18
#define COMMUNICATION_CALLBACK_LIST_INIT \
	handle_read_callback_callback, \
	handle_error_count_callback, \
	handle_modbus_read_coils_request_callback, \
	handle_modbus_read_coils_response_low_level_callback, \
	handle_modbus_read_holding_registers_request_callback, \
	handle_modbus_read_holding_registers_response_low_level_callback, \
	handle_modbus_write_single_coil_request_callback, \
	handle_modbus_write_single_coil_response_callback, \
	handle_modbus_write_single_register_request_callback, \
	handle_modbus_write_single_register_response_callback, \
	handle_modbus_write_multiple_coils_request_low_level_callback, \
	handle_modbus_write_multiple_coils_response_callback, \
	handle_modbus_write_multiple_registers_request_low_level_callback, \
	handle_modbus_write_multiple_registers_response_callback, \
	handle_modbus_read_discrete_inputs_request_callback, \
	handle_modbus_read_discrete_inputs_response_low_level_callback, \
	handle_modbus_read_input_registers_request_callback, \
	handle_modbus_read_input_registers_response_low_level_callback, \

#endif
