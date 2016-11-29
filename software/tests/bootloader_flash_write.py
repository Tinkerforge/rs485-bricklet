#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "PrEFT"
UID_MASTER = "6qzRzc"

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_rs485 import BrickletRS485
from tinkerforge.brick_master import BrickMaster
import time

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    rs485 = BrickletRS485(UID, ipcon) # Create device object
    master = BrickMaster(UID_MASTER, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    master.set_spitfp_baudrate('c', 2000000)

    fw = open('/home/olaf/tf/rs485-bricklet/software/build/rs485-bricklet-with-checksum.bin', 'rb').read()

    print "firmware length:", len(fw)
    print "bootloader mode:", rs485.get_bootloader_mode()

    print "set bootloader mode 0:", rs485.set_bootloader_mode(0)
    time.sleep(1)
    print "bootloader mode:", rs485.get_bootloader_mode()
    for i in range(len(fw)/64):
        from_ptr = i*64
        to_ptr = (i+1)*64
        print "write ptr:", from_ptr, to_ptr
        rs485.set_write_firmware_pointer(from_ptr)

        data = map(ord, fw[from_ptr:to_ptr])
        print "write firmware:", rs485.write_firmware(data)
#        time.sleep(1)

    print "set bootloader mode 1:", rs485.set_bootloader_mode(1)
    time.sleep(1)
    print "bootloader mode:", rs485.get_bootloader_mode()

    ipcon.disconnect()
