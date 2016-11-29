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

    t = time.time()
    for _ in range(10000):
        rs485.write("1"*60, 60)

    print(time.time() - t)
#    while True:
#        print('Temperature: ' + str(rs485.get_chip_temperature()))

if __name__ == '__main__':
    main(HOST, PORT, UID)
