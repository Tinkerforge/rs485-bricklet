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

    print('Baudrate before: ' + str(master.get_spitfp_baudrate('c')))
    master.set_spitfp_baudrate('c', 2000000)
    print('Baudrate after: ' + str(master.get_spitfp_baudrate('c')))

if __name__ == '__main__':
    main(HOST, PORT, UID_RS485, UID_MASTER)
