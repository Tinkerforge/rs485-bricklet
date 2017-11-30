#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your RS485 Bricklet

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_rs485 import BrickletRS485

expected_request_id = None

# Callback function for Modbus master write single register response callback
def cb_modbus_master_write_single_register_response(request_id, exception_code):
    print("Request ID: " + str(request_id))
    print("Exception Code: " + str(exception_code))

    if request_id != expected_request_id:
        print("Error: Unexpected request ID")

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    rs485 = BrickletRS485(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Set operating mode to Modbus RTU master
    rs485.set_mode(rs485.MODE_MODBUS_MASTER_RTU)

    # Modbus specific configuration:
    # - slave address = 1 (unused in master mode)
    # - master request timeout = 1000ms
    rs485.set_modbus_configuration(1, 1000)

    # Register Modbus master write single register response callback to function
    # cb_modbus_master_write_single_register_response
    rs485.register_callback(rs485.CALLBACK_MODBUS_MASTER_WRITE_SINGLE_REGISTER_RESPONSE,
                            cb_modbus_master_write_single_register_response)

    # Write 65535 to register 42 of slave 17
    expected_request_id = rs485.modbus_master_write_single_register(17, 42, 65535)

    raw_input("Press key to exit\n") # Use input() in Python 3
    ipcon.disconnect()
