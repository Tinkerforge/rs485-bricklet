#!/usr/bin/env python3

import serial
import time

HOST = "localhost"
PORT = 4223
UID = "PrEFT"

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_rs485 import BrickletRS485
import time


def main(host, port, uid):
    ipcon = IPConnection()
    rs485 = BrickletRS485(uid, ipcon)
    ipcon.connect(host, port)

#    rs485.set_buffer_config(5*1024, 5*1024)
    print(rs485.get_buffer_status())


if __name__ == '__main__':
    main(HOST, PORT, UID)
