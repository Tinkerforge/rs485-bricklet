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
#define FID_WRITE_LOW_LEVEL 1
#define FID_READ_LOW_LEVEL 2
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
#define FID_MODBUS_SLAVE_REPORT_EXCEPTION 24
#define FID_MODBUS_SLAVE_ANSWER_READ_COILS_REQUEST_LOW_LEVEL 25
#define FID_MODBUS_MASTER_READ_COILS 26
#define FID_MODBUS_SLAVE_ANSWER_READ_HOLDING_REGISTERS_REQUEST_LOW_LEVEL 27
#define FID_MODBUS_MASTER_READ_HOLDING_REGISTERS 28
#define FID_MODBUS_SLAVE_ANSWER_WRITE_SINGLE_COIL_REQUEST 29
#define FID_MODBUS_MASTER_WRITE_SINGLE_COIL 30
#define FID_MODBUS_SLAVE_ANSWER_WRITE_SINGLE_REGISTER_REQUEST 31
#define FID_MODBUS_MASTER_WRITE_SINGLE_REGISTER 32
#define FID_MODBUS_SLAVE_ANSWER_WRITE_MULTIPLE_COILS_REQUEST 33
#define FID_MODBUS_MASTER_WRITE_MULTIPLE_COILS_LOW_LEVEL 34
#define FID_MODBUS_SLAVE_ANSWER_WRITE_MULTIPLE_REGISTERS_REQUEST 35
#define FID_MODBUS_MASTER_WRITE_MULTIPLE_REGISTERS_LOW_LEVEL 36
#define FID_MODBUS_SLAVE_ANSWER_READ_DISCRETE_INPUTS_REQUEST_LOW_LEVEL 37
#define FID_MODBUS_MASTER_READ_DISCRETE_INPUTS 38
#define FID_MODBUS_SLAVE_ANSWER_READ_INPUT_REGISTERS_REQUEST_LOW_LEVEL 39
#define FID_MODBUS_MASTER_READ_INPUT_REGISTERS 40

// Callbacks.
#define FID_CALLBACK_READ_LOW_LEVEL 41
#define FID_CALLBACK_ERROR_COUNT 42

// Modbus specific.
#define FID_CALLBACK_MODBUS_SLAVE_READ_COILS_REQUEST 43
#define FID_CALLBACK_MODBUS_MASTER_READ_COILS_RESPONSE_LOW_LEVEL 44
#define FID_CALLBACK_MODBUS_SLAVE_READ_HOLDING_REGISTERS_REQUEST 45
#define FID_CALLBACK_MODBUS_MASTER_READ_HOLDING_REGISTERS_RESPONSE_LOW_LEVEL 46
#define FID_CALLBACK_MODBUS_SLAVE_WRITE_SINGLE_COIL_REQUEST 47
#define FID_CALLBACK_MODBUS_MASTER_WRITE_SINGLE_COIL_RESPONSE 48
#define FID_CALLBACK_MODBUS_SLAVE_WRITE_SINGLE_REGISTER_REQUEST 49
#define FID_CALLBACK_MODBUS_MASTER_WRITE_SINGLE_REGISTER_RESPONSE 50
#define FID_CALLBACK_MODBUS_SLAVE_WRITE_MULTIPLE_COILS_REQUEST_LOW_LEVEL 51
#define FID_CALLBACK_MODBUS_MASTER_WRITE_MULTIPLE_COILS_RESPONSE 52
#define FID_CALLBACK_MODBUS_SLAVE_WRITE_MULTIPLE_REGISTERS_REQUEST_LOW_LEVEL 53
#define FID_CALLBACK_MODBUS_MASTER_WRITE_MULTIPLE_REGISTERS_RESPONSE 54
#define FID_CALLBACK_MODBUS_SLAVE_READ_DISCRETE_INPUTS_REQUEST 55
#define FID_CALLBACK_MODBUS_MASTER_READ_DISCRETE_INPUTS_RESPONSE_LOW_LEVEL 56
#define FID_CALLBACK_MODBUS_SLAVE_READ_INPUT_REGISTERS_REQUEST 57
#define FID_CALLBACK_MODBUS_MASTER_READ_INPUT_REGISTERS_RESPONSE_LOW_LEVEL 58

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
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint8_t stream_chunk_data[59];
} __attribute__((__packed__)) WriteLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t stream_chunk_written;
} __attribute__((__packed__)) WriteLowLevel_Response;

typedef struct {
	TFPMessageHeader header;
	uint16_t length;
} __attribute__((__packed__)) ReadLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	char stream_chunk_data[60];
} __attribute__((__packed__)) ReadLowLevel_Response;

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
} __attribute__((__packed__)) IsReadCallbackEnabled_Response;

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
} __attribute__((__packed__)) GetRS485Configuration_Response;

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
} __attribute__((__packed__)) GetModbusConfiguration_Response;

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
} __attribute__((__packed__)) GetMode_Response;

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
} __attribute__((__packed__)) GetCommunicationLEDConfig_Response;

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
} __attribute__((__packed__)) GetErrorLEDConfig_Response;

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
} __attribute__((__packed__)) GetBufferConfig_Response;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetBufferStatus;

typedef struct {
	TFPMessageHeader header;
	uint16_t send_buffer_used;
	uint16_t receive_buffer_used;
} __attribute__((__packed__)) GetBufferStatus_Response;

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
} __attribute__((__packed__)) IsErrorCountCallbackEnabled_Response;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetErrorCount;

typedef struct {
	TFPMessageHeader header;
	uint32_t overrun_error_count;
	uint32_t parity_error_count;
} __attribute__((__packed__)) GetErrorCount_Response;

typedef struct {
	TFPMessageHeader header;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	char stream_chunk_data[60];
} __attribute__((__packed__)) ReadLowLevel_Callback;

typedef struct {
	TFPMessageHeader header;
	uint32_t overrun_error_count;
	uint32_t parity_error_count;
} __attribute__((__packed__)) ErrorCount_Callback;

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
} __attribute__((__packed__)) GetModbusCommonErrorCount_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
} __attribute__((__packed__)) ModbusSlaveReportException;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint8_t stream_chunk_data[59];
} __attribute__((__packed__)) ModbusSlaveAnswerReadCoilsRequestLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusMasterReadCoils;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusMasterReadCoils_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusSlaveReadCoilsRequest_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint8_t stream_chunk_data[58];
} __attribute__((__packed__)) ModbusMasterReadCoilsResponseLowLevel_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint16_t stream_chunk_data[29];
} __attribute__((__packed__)) ModbusSlaveAnswerReadHoldingRegistersRequestLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusMasterReadHoldingRegisters;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusMasterReadHoldingRegisters_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusSlaveReadHoldingRegistersRequest_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint16_t stream_chunk_data[29];
} __attribute__((__packed__)) ModbusMasterReadHoldingRegistersResponseLowLevel_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t coil_address;
	bool coil_value;
} __attribute__((__packed__)) ModbusMasterWriteSingleCoil;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusMasterWriteSingleCoil_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusSlaveAnswerWriteSingleCoilRequest;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t coil_address;
	bool coil_value;
} __attribute__((__packed__)) ModbusSlaveWriteSingleCoilRequest_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
} __attribute__((__packed__)) ModbusMasterWriteSingleCoilResponse_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t register_address;
	uint16_t register_value;
} __attribute__((__packed__)) ModbusMasterWriteSingleRegister;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusMasterWriteSingleRegister_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusSlaveAnswerWriteSingleRegisterRequest;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t register_address;
	uint16_t register_value;
} __attribute__((__packed__)) ModbusSlaveWriteSingleRegisterRequest_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
} __attribute__((__packed__)) ModbusMasterWriteSingleRegisterResponse_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusSlaveAnswerWriteMultipleCoilsRequest;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t starting_address;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint8_t stream_chunk_data[54];
} __attribute__((__packed__)) ModbusMasterWriteMultipleCoilsLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusMasterWriteMultipleCoilsLowLevel_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t starting_address;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint8_t stream_chunk_data[55];
} __attribute__((__packed__)) ModbusSlaveWriteMultipleCoilsRequestLowLevel_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
} __attribute__((__packed__)) ModbusMasterWriteMultipleCoilsResponse_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusSlaveAnswerWriteMultipleRegistersRequest;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t starting_address;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint16_t stream_chunk_data[27];
} __attribute__((__packed__)) ModbusMasterWriteMultipleRegistersLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusMasterWriteMultipleRegistersLowLevel_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t starting_address;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint16_t stream_chunk_data[27];
} __attribute__((__packed__)) ModbusSlaveWriteMultipleRegistersRequestLowLevel_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
} __attribute__((__packed__)) ModbusMasterWriteMultipleRegistersResponse_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint8_t stream_chunk_data[59];
} __attribute__((__packed__)) ModbusSlaveAnswerReadDiscreteInputsRequestLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusMasterReadDiscreteInputs;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusMasterReadDiscreteInputs_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusSlaveReadDiscreteInputsRequest_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint8_t stream_chunk_data[58];
} __attribute__((__packed__)) ModbusMasterReadDiscreteInputsResponseLowLevel_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint16_t stream_chunk_data[29];
} __attribute__((__packed__)) ModbusSlaveAnswerReadInputRegistersRequestLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t slave_address;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusMasterReadInputRegisters;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
} __attribute__((__packed__)) ModbusMasterReadInputRegisters_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	uint16_t starting_address;
	uint16_t count;
} __attribute__((__packed__)) ModbusSlaveReadInputRegistersRequest_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t request_id;
	int8_t exception_code;
	uint16_t stream_total_length;
	uint16_t stream_chunk_offset;
	uint16_t stream_chunk_data[29];
} __attribute__((__packed__)) ModbusMasterReadInputRegistersResponseLowLevel_Callback;

// Function prototypes
BootloaderHandleMessageResponse write_low_level(const WriteLowLevel *data, WriteLowLevel_Response *response);
BootloaderHandleMessageResponse read_low_level(const ReadLowLevel *data, ReadLowLevel_Response *response);
BootloaderHandleMessageResponse enable_read_callback(const EnableReadCallback *data);
BootloaderHandleMessageResponse disable_read_callback(const DisableReadCallback *data);
BootloaderHandleMessageResponse is_read_callback_enabled(const IsReadCallbackEnabled *data,
                                                         IsReadCallbackEnabled_Response *response);
BootloaderHandleMessageResponse set_rs485_configuration(const SetRS485Configuration *data);
BootloaderHandleMessageResponse get_rs485_configuration(const GetRS485Configuration *data,
                                                        GetRS485Configuration_Response *response);

// Modbus specific.
BootloaderHandleMessageResponse set_modbus_configuration(const SetModbusConfiguration *data);
BootloaderHandleMessageResponse get_modbus_configuration(const GetModbusConfiguration *data,
                                                         GetModbusConfiguration_Response *response);

BootloaderHandleMessageResponse set_mode(const SetMode *data);
BootloaderHandleMessageResponse get_mode(const GetMode *data, GetMode_Response *response);
BootloaderHandleMessageResponse set_communication_led_config(const SetCommunicationLEDConfig *data);
BootloaderHandleMessageResponse get_communication_led_config(const GetCommunicationLEDConfig *data,
                                                             GetCommunicationLEDConfig_Response *response);
BootloaderHandleMessageResponse set_error_led_config(const SetErrorLEDConfig *data);
BootloaderHandleMessageResponse get_error_led_config(const GetErrorLEDConfig *data,
                                                     GetErrorLEDConfig_Response *response);
BootloaderHandleMessageResponse set_buffer_config(const SetBufferConfig *data);
BootloaderHandleMessageResponse get_buffer_config(const GetBufferConfig *data,
                                                  GetBufferConfig_Response *response);
BootloaderHandleMessageResponse get_buffer_status(const GetBufferStatus *data,
                                                  GetBufferStatus_Response *response);
BootloaderHandleMessageResponse enable_error_count_callback(const EnableErrorCountCallback *data);
BootloaderHandleMessageResponse disable_error_count_callback(const DisableErrorCountCallback *data);
BootloaderHandleMessageResponse is_error_count_callback_enabled(const IsErrorCountCallbackEnabled *data,
                                                                IsErrorCountCallbackEnabled_Response *response);
BootloaderHandleMessageResponse get_error_count(const GetErrorCount *data,
                                                GetErrorCount_Response *response);

// Modbus specific.
BootloaderHandleMessageResponse
get_modbus_common_error_count(const GetModbusCommonErrorCount *data,
                              GetModbusCommonErrorCount_Response *response);

BootloaderHandleMessageResponse
modbus_slave_report_exception(const ModbusSlaveReportException *data);

BootloaderHandleMessageResponse
modbus_slave_answer_read_coils_request_low_level(const ModbusSlaveAnswerReadCoilsRequestLowLevel *data);

BootloaderHandleMessageResponse
modbus_master_read_coils(const ModbusMasterReadCoils *data,
                         ModbusMasterReadCoils_Response *response);

BootloaderHandleMessageResponse
modbus_slave_answer_read_holding_registers_request_low_level(const ModbusSlaveAnswerReadHoldingRegistersRequestLowLevel *data);

BootloaderHandleMessageResponse
modbus_master_read_holding_registers(const ModbusMasterReadHoldingRegisters *data,
                                     ModbusMasterReadHoldingRegisters_Response *response);

BootloaderHandleMessageResponse
modbus_slave_answer_write_single_coil_request(const ModbusSlaveAnswerWriteSingleCoilRequest *data);

BootloaderHandleMessageResponse
modbus_master_write_single_coil(const ModbusMasterWriteSingleCoil *data,
                                ModbusMasterWriteSingleCoil_Response *response);

BootloaderHandleMessageResponse
modbus_slave_answer_write_single_register_request(const ModbusSlaveAnswerWriteSingleRegisterRequest *data);

BootloaderHandleMessageResponse
modbus_master_write_single_register(const ModbusMasterWriteSingleRegister *data,
                                    ModbusMasterWriteSingleRegister_Response *response);

BootloaderHandleMessageResponse
modbus_slave_answer_write_multiple_coils_request(const ModbusSlaveAnswerWriteMultipleCoilsRequest *data);

BootloaderHandleMessageResponse
modbus_master_write_multiple_coils_low_level(const ModbusMasterWriteMultipleCoilsLowLevel *data,
                                             ModbusMasterWriteMultipleCoilsLowLevel_Response *response);

BootloaderHandleMessageResponse
modbus_slave_answer_write_multiple_registers_request(const ModbusSlaveAnswerWriteMultipleRegistersRequest *data);

BootloaderHandleMessageResponse
modbus_master_write_multiple_registers_low_level(const ModbusMasterWriteMultipleRegistersLowLevel *data,
                                                 ModbusMasterWriteMultipleRegistersLowLevel_Response *response);

BootloaderHandleMessageResponse
modbus_slave_answer_read_discrete_inputs_request_low_level(const ModbusSlaveAnswerReadDiscreteInputsRequestLowLevel *data);

BootloaderHandleMessageResponse
modbus_master_read_discrete_inputs(const ModbusMasterReadDiscreteInputs *data,
                                   ModbusMasterReadDiscreteInputs_Response *response);

BootloaderHandleMessageResponse
modbus_slave_answer_read_input_registers_request_low_level(const ModbusSlaveAnswerReadInputRegistersRequestLowLevel *data);

BootloaderHandleMessageResponse
modbus_master_read_input_registers(const ModbusMasterReadInputRegisters *data,
                                   ModbusMasterReadInputRegisters_Response *response);

// Callbacks
bool handle_read_low_level_callback(void);
bool handle_error_count_callback(void);

// Modbus specific.
bool handle_modbus_slave_read_coils_request_callback(void);
bool handle_modbus_master_read_coils_response_low_level_callback(void);
bool handle_modbus_slave_read_holding_registers_request_callback(void);
bool handle_modbus_master_read_holding_registers_response_low_level_callback(void);
bool handle_modbus_slave_write_single_coil_request_callback(void);
bool handle_modbus_master_write_single_coil_response_callback(void);
bool handle_modbus_slave_write_single_register_request_callback(void);
bool handle_modbus_master_write_single_register_response_callback(void);
bool handle_modbus_slave_write_multiple_coils_request_low_level_callback(void);
bool handle_modbus_master_write_multiple_coils_response_callback(void);
bool handle_modbus_slave_write_multiple_registers_request_low_level_callback(void);
bool handle_modbus_master_write_multiple_registers_response_callback(void);
bool handle_modbus_slave_read_discrete_inputs_request_callback(void);
bool handle_modbus_master_read_discrete_inputs_response_low_level_callback(void);
bool handle_modbus_slave_read_input_registers_request_callback(void);
bool handle_modbus_master_read_input_registers_response_low_level_callback(void);

#define CALLBACK_ERROR_COUNT_DEBOUNCE_MS 100
#define COMMUNICATION_CALLBACK_TICK_WAIT_MS 1
#define COMMUNICATION_CALLBACK_HANDLER_NUM 18
#define COMMUNICATION_CALLBACK_LIST_INIT \
	handle_read_low_level_callback, \
	handle_error_count_callback, \
	handle_modbus_slave_read_coils_request_callback, \
	handle_modbus_master_read_coils_response_low_level_callback, \
	handle_modbus_slave_read_holding_registers_request_callback, \
	handle_modbus_master_read_holding_registers_response_low_level_callback, \
	handle_modbus_slave_write_single_coil_request_callback, \
	handle_modbus_master_write_single_coil_response_callback, \
	handle_modbus_slave_write_single_register_request_callback, \
	handle_modbus_master_write_single_register_response_callback, \
	handle_modbus_slave_write_multiple_coils_request_low_level_callback, \
	handle_modbus_master_write_multiple_coils_response_callback, \
	handle_modbus_slave_write_multiple_registers_request_low_level_callback, \
	handle_modbus_master_write_multiple_registers_response_callback, \
	handle_modbus_slave_read_discrete_inputs_request_callback, \
	handle_modbus_master_read_discrete_inputs_response_low_level_callback, \
	handle_modbus_slave_read_input_registers_request_callback, \
	handle_modbus_master_read_input_registers_response_low_level_callback, \

#endif
