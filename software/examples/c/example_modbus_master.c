#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_rs485.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change XYZ to the UID of your RS232 Bricklet

RS485 rs485;
int function_return = 0;
uint8_t ret_request_id = 0;
uint32_t valid_reg_address = 1;
uint32_t invalid_reg_address = 8;
uint16_t single_reg_value = 65535;

// Callback function for write single register response callback
void cb_modbus_master_write_single_register_response(uint8_t request_id,
                                                     int8_t exception_code,
                                                     void *user_data) {
    if(request_id != ret_request_id) {
      return;
    }

    switch(exception_code) {
      case RS485_EXCEPTION_CODE_TIMEOUT:
        fprintf(stdout, "Request timed out\n");

        break;

      case RS485_EXCEPTION_CODE_SUCCESS:
        fprintf(stdout, "Request successful\n");

        break;

      case RS485_EXCEPTION_CODE_ILLEGAL_FUNCTION:
        fprintf(stdout, "Illegal function\n");

        break;

      case RS485_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS:
        fprintf(stdout, "Illegal data address\n");

        // Second request with valid register address
        function_return = rs485_modbus_master_write_single_register(&rs485,
                                                                    1,
                                                                    valid_reg_address,
                                                                    single_reg_value,
                                                                    &ret_request_id);

        if(function_return == 0) {
          fprintf(stdout, "Modbus write single register request ID = %d\n", ret_request_id);
        }
        else {
          fprintf(stdout, "Modbus write single register request failed\n");
        }

        break;

      case RS485_EXCEPTION_CODE_ILLEGAL_DATA_VALUE:
        fprintf(stdout, "Illegal data value\n");

        break;

      case RS485_EXCEPTION_CODE_SLAVE_DEVICE_FAILURE:
        fprintf(stdout, "Slave device failure\n");

        break;

      case RS485_EXCEPTION_CODE_ACKNOWLEDGE:
        fprintf(stdout, "Acknowledge\n");

        break;

      case RS485_EXCEPTION_CODE_SLAVE_DEVICE_BUSY:
        fprintf(stdout, "Device busy\n");

        break;

      case RS485_EXCEPTION_CODE_MEMORY_PARITY_ERROR:
        fprintf(stdout, "Memory parity error\n");

        break;

      case RS485_EXCEPTION_CODE_GATEWAY_PATH_UNAVAILABLE:
        fprintf(stdout, "gateway path unavailable\n");

        break;

      case RS485_EXCEPTION_CODE_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND:
        fprintf(stdout, "Gateway target device failed to respond\n");

        break;

      default:
        fprintf(stdout, "Unknown exception code\n");
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
    rs485_set_mode(&rs485, RS485_MODE_MODBUS_MASTER_RTU);

    // Modbus specific configuration
    rs485_set_modbus_configuration(&rs485, 1, 1000);

    // Register write single register callback
    rs485_register_callback(&rs485,
                            RS485_CALLBACK_MODBUS_MASTER_WRITE_SINGLE_REGISTER_RESPONSE,
                            (void *)cb_modbus_master_write_single_register_response,
                            NULL);

    // First request with invalid register address
    function_return = rs485_modbus_master_write_single_register(&rs485,
                                                                1,
                                                                invalid_reg_address,
                                                                single_reg_value,
                                                                &ret_request_id);

    if(function_return == 0) {
      fprintf(stdout, "Modbus write single register request ID = %d\n", ret_request_id);
    }
    else {
      fprintf(stdout, "Modbus write single register request failed\n");
    }

    fprintf(stdout, "Press key to exit\n");
    getchar();

    rs485_destroy(&rs485);
    ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally

    return 0;
}
