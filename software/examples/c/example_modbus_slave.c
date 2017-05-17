#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_rs485.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change XYZ to the UID of your RS232 Bricklet

RS485 rs485;
uint16_t single_register_value = 0;
uint32_t valid_reg_address_upto = 4;

// Callback function for write single register request callback
void cb_modbus_slave_write_single_register_request(uint8_t request_id,
                                                   uint32_t register_address,
                                                   uint16_t register_value,
                                                   void *user_data) {
    fprintf(stdout, "Request ID = %d\n", request_id);
    fprintf(stdout, "Register Address = %d\n", register_address);
    fprintf(stdout, "Register Value = %d\n", register_value);

    // Here we assume valid writable register addresses are 1, 2, 3 and 4
    if(register_address > valid_reg_address_upto) {
      fprintf(stdout, "Requested invalid register address\n");
      rs485_modbus_slave_report_exception(&rs485, request_id, RS485_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
    }
    else {
      fprintf(stdout, "Request OK\n");
      single_register_value = register_value;
      rs485_modbus_slave_answer_write_single_register_request(&rs485, request_id);
    }
}

int main(void) {
    // Create IP connection
    IPConnection ipcon;
    ipcon_create(&ipcon);

    // Create device object
    rs485_create(&rs485, UID, &ipcon);

    // Connect to brickd
    if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
        fprintf(stderr, "Could not connect\n");

        return 1;
    }
    // Don't use device before ipcon is connected

    // Configure the Bricklet
    rs485_set_rs485_configuration(&rs485,
                                  115200,
                                  RS485_PARITY_EVEN,
                                  RS485_STOPBITS_1,
                                  RS485_WORDLENGTH_8,
                                  RS485_DUPLEX_HALF);

    // Set operating mode of the Bricklet
    rs485_set_mode(&rs485, RS485_MODE_MODBUS_SLAVE_RTU);

    // Modbus specific configuration
    rs485_set_modbus_configuration(&rs485, 1, 1000);

    // Register write single register callback
    rs485_register_callback(&rs485,
                            RS485_CALLBACK_MODBUS_SLAVE_WRITE_SINGLE_REGISTER_REQUEST,
                            (void *)cb_modbus_slave_write_single_register_request,
                            NULL);

    fprintf(stdout, "Press key to exit\n");
    getchar();

    rs485_destroy(&rs485);
    ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally

    return 0;
}
