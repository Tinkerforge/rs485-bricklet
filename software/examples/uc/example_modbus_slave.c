#include "bindings/hal_common.h"
#include "bindings/bricklet_rs485.h"

#define UID "XYZ" // Change XYZ to the UID of your RS485 Bricklet

void check(int rc, const char* msg);

int received_request_id = -1;
bool register_address_valid = false;

// Callback function for Modbus slave write single register request callback
void modbus_slave_write_single_register_request_handler(TF_RS485 *device, uint8_t request_id,
                                                        uint32_t register_address,
                                                        uint16_t register_value,
                                                        void *user_data) {
	(void)device; (void)user_data; // avoid unused parameter warning

	tf_hal_printf("Request ID: %u\n", request_id);
	tf_hal_printf("Register Address: %u\n", register_address);
	tf_hal_printf("Register Value: %u\n", register_value);
	tf_hal_printf("\n");

	received_request_id = request_id;
	register_address_valid = register_address == 42;

	if (!register_address_valid) {
		tf_hal_printf("Error: Invalid register address\n");
	}
}

TF_RS485 rs485;

void example_setup(TF_HalContext *hal) {
	// Create device object
	check(tf_rs485_create(&rs485, UID, hal), "create device object");

	// Set operating mode to Modbus RTU slave
	check(tf_rs485_set_mode(&rs485, TF_RS485_MODE_MODBUS_SLAVE_RTU), "call set_mode");

	// Modbus specific configuration:
	// - slave address = 17
	// - master request timeout = 0ms (unused in slave mode)
	check(tf_rs485_set_modbus_configuration(&rs485, 17,
	                                        0), "call set_modbus_configuration");

	// Register Modbus slave write single register request callback to function
	// modbus_slave_write_single_register_request_handler
	tf_rs485_register_modbus_slave_write_single_register_request_callback(&rs485,
	                                                                      modbus_slave_write_single_register_request_handler,
	                                                                      NULL);
}

void example_loop(TF_HalContext *hal) {
	// Poll for callbacks
	// Polling with 0 will process one packet at most, so we can't miss a request.
	tf_hal_callback_tick(hal, 0);
    
	if(received_request_id == 0) {
		// We didn't receive a request.
		return;
	}

	if(!register_address_valid) {
		tf_rs485_modbus_slave_report_exception(&rs485, received_request_id, TF_RS485_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
	} else {
		tf_rs485_modbus_slave_answer_write_single_register_request(&rs485, received_request_id);
	}

	// Make sure we respond to the request only once.
	received_request_id = -1;
}
