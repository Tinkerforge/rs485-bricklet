#!/usr/bin/env python
# -*- coding: utf-8 -*-

# For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
# and configure the DIP switch on the Bricklet to full-duplex mode

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your RS485 Bricklet

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_rs485 import BrickletRS485

# Callback function for read callback
def cb_read(message):
    print('Message: "' + ''.join(message) + '"')

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    rs485 = BrickletRS485(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Enable full-duplex mode
    rs485.set_rs485_configuration(115200, rs485.PARITY_NONE, rs485.STOPBITS_1,
                                  rs485.WORDLENGTH_8, rs485.DUPLEX_FULL)

    # Register read callback to function cb_read
    rs485.register_callback(rs485.CALLBACK_READ, cb_read)

    # Enable read callback
    rs485.enable_read_callback()

    # Write "test" string
    rs485.write(list('test'))

    raw_input("Press key to exit\n") # Use input() in Python 3
    ipcon.disconnect()
