#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_rs485.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change XYZ to the UID of your RS232 Bricklet

RS485 rs485;
uint8_t expected_request_id = 0;

// Callback function for write single register response callback
void cb_modbus_master_write_single_register_response(uint8_t request_id,
                                                     int8_t exception_code,
                                                     void *user_data) {
    if(request_id != expected_request_id) {
      printf("Unexpected request ID\n");

      return;
    }

    printf("Request ID = %d\n", request_id);
    printf("Exception Code = %d\n", exception_code);
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

    // Set operating mode of the Bricklet
    rs485_set_mode(&rs485, RS485_MODE_MODBUS_MASTER_RTU);

    /*
     * Modbus specific configuration
     *
     * Slave mode address = 1 (Unused in master mode)
     * Master mode request timeout = 1000ms (Unused in slave mode)
     */
    rs485_set_modbus_configuration(&rs485, 1, 1000);

    // Register write single register response callback
    rs485_register_callback(&rs485,
                            RS485_CALLBACK_MODBUS_MASTER_WRITE_SINGLE_REGISTER_RESPONSE,
                            (void *)cb_modbus_master_write_single_register_response,
                            NULL);

    // Request single register write
    rs485_modbus_master_write_single_register(&rs485, 1, 42, 65535, &expected_request_id);

    fprintf(stdout, "Press key to exit\n");
    getchar();

    rs485_destroy(&rs485);
    ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally

    return 0;
}
