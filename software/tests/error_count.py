#!/usr/bin/env python3

import serial
import time

HOST = "localhost"
PORT = 4223
UID_RS4851 = "2vDtFK"
UID_RS4852 = "2A4L88"
UID_RS4853 = "UL5Hc"
UID_MASTER = "6krzfH"

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_rs485 import BrickletRS485
from tinkerforge.brick_master import BrickMaster
import time


def main(host, port, uid1, uid2, uid3, uid_master):
    ipcon = IPConnection()
    rs4851 = BrickletRS485(uid1, ipcon)
    rs4852 = BrickletRS485(uid2, ipcon)
    rs4853 = BrickletRS485(uid3, ipcon)
    master = BrickMaster(uid_master, ipcon)
    ipcon.connect(host, port)

    while True:
        print('RS485-1: ' + str(rs4851.get_spitfp_error_count()))
        print('RS485-2: ' + str(rs4852.get_spitfp_error_count()))
        print('RS485-3: ' + str(rs4853.get_spitfp_error_count()))
        print('Master-a: ' + str(master.get_spitfp_error_count('a')))
        print('Master-b: ' + str(master.get_spitfp_error_count('b')))
        print('Master-c: ' + str(master.get_spitfp_error_count('c')))
        time.sleep(1)

if __name__ == '__main__':
    main(HOST, PORT, UID_RS4851, UID_RS4852, UID_RS4853, UID_MASTER)
