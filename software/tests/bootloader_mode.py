#!/usr/bin/env python3

import serial
import time

HOST = "localhost"
PORT = 4223
UID_RS485 = "PrEFT"
UID_MASTER = "6qzRzc"

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_rs485 import BrickletRS485
from tinkerforge.brick_master import BrickMaster
import time


def main(host, port, uid_rs485, uid_master):
    ipcon = IPConnection()
    rs485 = BrickletRS485(uid_rs485, ipcon)
    master = BrickMaster(uid_master, ipcon)
    ipcon.connect(host, port)

    print("get:   " + str(rs485.get_bootloader_mode()))
    print("set 1: " + str(rs485.set_bootloader_mode(1)))

if __name__ == '__main__':
    main(HOST, PORT, UID_RS485, UID_MASTER)
