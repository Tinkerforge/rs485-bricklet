#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your RS485 Bricklet

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_rs485 import BrickletRS485

# Callback function for write single register request callback
def cb_modbus_slave_write_single_register_request(request_id, register_address, register_value):
    print('Request ID = ' + str(request_id))
    print('Register Address = ' + str(register_address))
    print('Register Value = ' + str(register_value))

    # Here we assume valid writable register address is 42
    if register_address != 42:
        print('Requested invalid register address')
        rs485.modbus_slave_report_exception(request_id, BrickletRS485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS)
    else:
        print('Request OK')
        rs485.modbus_slave_answer_write_single_register_request(request_id)

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    rs485 = BrickletRS485(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Set operating mode
    rs485.set_mode(rs485.MODE_MODBUS_SLAVE_RTU)

    # Modbus specific configuration
    #
    # Slave address = 1 (Unused in master mode)
    # Request timeout = 1000ms (Unused in slave mode)
    rs485.set_modbus_configuration(1, 1000)

    # Register write single register request callback
    rs485.register_callback(rs485.CALLBACK_MODBUS_SLAVE_WRITE_SINGLE_REGISTER_REQUEST,
                            cb_modbus_slave_write_single_register_request)

    raw_input("Press key to exit\n") # Use input() in Python 3
    ipcon.disconnect()
