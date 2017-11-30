#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your RS485 Bricklet

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_rs485 import BrickletRS485

# Callback function for Modbus slave write single register request callback
def cb_modbus_slave_write_single_register_request(request_id, register_address,
                                                  register_value):
    print("Request ID: " + str(request_id))
    print("Register Address: " + str(register_address))
    print("Register Value: " + str(register_value))

    if register_address != 42:
        print("Error: Invalid register address")
        rs485.modbus_slave_report_exception(request_id, BrickletRS485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS)
    else:
        rs485.modbus_slave_answer_write_single_register_request(request_id)

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    rs485 = BrickletRS485(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Set operating mode to Modbus RTU slave
    rs485.set_mode(rs485.MODE_MODBUS_SLAVE_RTU)

    # Modbus specific configuration:
    # - slave address = 17
    # - master request timeout = 0ms (unused in slave mode)
    rs485.set_modbus_configuration(17, 0)

    # Register Modbus slave write single register request callback to function
    # cb_modbus_slave_write_single_register_request
    rs485.register_callback(rs485.CALLBACK_MODBUS_SLAVE_WRITE_SINGLE_REGISTER_REQUEST,
                            cb_modbus_slave_write_single_register_request)

    raw_input("Press key to exit\n") # Use input() in Python 3
    ipcon.disconnect()
