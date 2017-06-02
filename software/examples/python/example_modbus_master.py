#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your RS485 Bricklet

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_rs485 import BrickletRS485

expected_request_id = None

# Callback function for write single register response callback
def cb_modbus_master_write_single_register_response(request_id, exception_code):
    if request_id != expected_request_id:
        print('Unexpected request ID')

        return

    print('Request ID = ' + str(request_id))
    print('Exception Code = ' + str(exception_code))

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    rs485 = BrickletRS485(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Set operating mode
    rs485.set_mode(rs485.MODE_MODBUS_MASTER_RTU)

    # Modbus specific configuration
    #
    # Slave address = 1 (Unused in master mode)
    # Request timeout = 1000ms (Unused in slave mode)
    rs485.set_modbus_configuration(1, 1000)

    # Register write single register response callback
    rs485.register_callback(rs485.CALLBACK_MODBUS_MASTER_WRITE_SINGLE_REGISTER_RESPONSE,
                            cb_modbus_master_write_single_register_response)

    # Request single register write
    expected_request_id = rs485.modbus_master_write_single_register(1, 42, 65535)

    raw_input("Press key to exit\n") # Use input() in Python 3
    ipcon.disconnect()
