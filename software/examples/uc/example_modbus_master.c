#include "bindings/hal_common.h"
#include "bindings/bricklet_rs485.h"

#define UID "XYZ" // Change XYZ to the UID of your RS485 Bricklet

void example_setup(TF_HalContext *hal);
void example_loop(TF_HalContext *hal);

void check(int rc, const char* msg);

static void log_exception(int8_t exception_code) {
	if(exception_code == TF_RS485_EXCEPTION_CODE_TIMEOUT) {
		tf_hal_printf("Exception Code: Timeout\n");
	} else if(exception_code == TF_RS485_EXCEPTION_CODE_SUCCESS) {
		tf_hal_printf("Exception Code: Success\n");
	} else if(exception_code == TF_RS485_EXCEPTION_CODE_ILLEGAL_FUNCTION) {
		tf_hal_printf("Exception Code: Illegal Function\n");
	} else if(exception_code == TF_RS485_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS) {
		tf_hal_printf("Exception Code: Illegal Data Address\n");
	} else if(exception_code == TF_RS485_EXCEPTION_CODE_ILLEGAL_DATA_VALUE) {
		tf_hal_printf("Exception Code: Illegal Data Value\n");
	} else if(exception_code == TF_RS485_EXCEPTION_CODE_SLAVE_DEVICE_FAILURE) {
		tf_hal_printf("Exception Code: Slave Device Failure\n");
	} else if(exception_code == TF_RS485_EXCEPTION_CODE_ACKNOWLEDGE) {
		tf_hal_printf("Exception Code: Acknowledge\n");
	} else if(exception_code == TF_RS485_EXCEPTION_CODE_SLAVE_DEVICE_BUSY) {
		tf_hal_printf("Exception Code: Slave Device Busy\n");
	} else if(exception_code == TF_RS485_EXCEPTION_CODE_MEMORY_PARITY_ERROR) {
		tf_hal_printf("Exception Code: Memory Parity Error\n");
	} else if(exception_code == TF_RS485_EXCEPTION_CODE_GATEWAY_PATH_UNAVAILABLE) {
		tf_hal_printf("Exception Code: Gateway Path Unavailable\n");
	} else if(exception_code == TF_RS485_EXCEPTION_CODE_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND) {
		tf_hal_printf("Exception Code: Gateway Target Device Failed To Respond\n");
	}
}

// Callback function for Modbus master write single register response callback
static void modbus_master_write_single_register_response_handler(TF_RS485 *device, uint8_t request_id,
                                                          int8_t exception_code,
                                                          void *user_data) {
	(void)device; // avoid unused parameter warning

	uint8_t expected_request_id = *(uint8_t *)user_data;

	tf_hal_printf("Request ID: %u\n", request_id);

	if(exception_code != TF_RS485_EXCEPTION_CODE_SUCCESS) {
		log_exception(exception_code);
	}

	if (request_id != expected_request_id) {
		tf_hal_printf("Error: Unexpected request ID\n");
	}

	tf_hal_printf("\n");
}

static TF_RS485 rs485;
static uint8_t expected_request_id = 0;

void example_setup(TF_HalContext *hal) {
	// Create device object
	check(tf_rs485_create(&rs485, UID, hal), "create device object");

	// Set operating mode to Modbus RTU master
	check(tf_rs485_set_mode(&rs485, TF_RS485_MODE_MODBUS_MASTER_RTU), "call set_mode");

	// Modbus specific configuration:
	// - slave address = 1 (unused in master mode)
	// - master request timeout = 1000ms
	check(tf_rs485_set_modbus_configuration(&rs485, 1,
	                                        1000), "call set_modbus_configuration");

	// Register Modbus master write single register response callback to function
	// modbus_master_write_single_register_response_handler
	tf_rs485_register_modbus_master_write_single_register_response_callback(&rs485,
	                                                                        modbus_master_write_single_register_response_handler,
	                                                                        &expected_request_id);

	// Write 65535 to register 42 of slave 17
	check(tf_rs485_modbus_master_write_single_register(&rs485, 17, 42,
	                                                   65535, &expected_request_id), "call modbus_master_write_single_register");
}

void example_loop(TF_HalContext *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
