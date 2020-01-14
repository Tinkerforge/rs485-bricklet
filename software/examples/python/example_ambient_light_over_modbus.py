#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID_RS485 = "XYZ" # Change XYZ to the UID of your RS485 Bricklet
UID_AMBIENT_LIGHT_V3 = "ABC" # Change ABC to the UID of your Ambient Light Bricklet 3.0

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_rs485 import BrickletRS485
from tinkerforge.bricklet_ambient_light_v3 import BrickletAmbientLightV3

# Callback function for Modbus slave read input registers request callback to expose the
# illuminance value of an Ambient Light Bricklet 3.0 at input register address 42
def cb_modbus_slave_read_input_registers_request(al, request_id, starting_address, count):
    print("Request ID: " + str(request_id))
    print("Starting Address: " + str(starting_address))
    print("Count: " + str(count))

    if starting_address != 42:
        print("Error: Invalid starting address")
        rs485.modbus_slave_report_exception(request_id, BrickletRS485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS)
    elif count != 1:
        print("Error: Invalid count")
        rs485.modbus_slave_report_exception(request_id, BrickletRS485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS)
    else:
        # Get current Illuminance
        illuminance = al.get_illuminance()
        print("Illuminance: " + str(illuminance/100.0) + " lx")

        rs485.modbus_slave_answer_read_input_registers_request(request_id, [illuminance//100])

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    rs485 = BrickletRS485(UID_RS485, ipcon) # Create device object
    al = BrickletAmbientLightV3(UID_AMBIENT_LIGHT_V3, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Set operating mode to Modbus RTU slave
    rs485.set_mode(rs485.MODE_MODBUS_SLAVE_RTU)

    # Modbus specific configuration:
    # - slave address = 17
    # - master request timeout = 0ms (unused in slave mode)
    rs485.set_modbus_configuration(17, 0)

    # Register Modbus slave write single register request callback to function
    # cb_modbus_slave_read_input_registers_request
    rs485.register_callback(rs485.CALLBACK_MODBUS_SLAVE_READ_INPUT_REGISTERS_REQUEST,
                            lambda *args: cb_modbus_slave_read_input_registers_request(al, *args))

    input("Press key to exit\n") # Use raw_input() in Python 2
    ipcon.disconnect()
