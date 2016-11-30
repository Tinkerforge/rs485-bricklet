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

def cb_enumerate(uid, connected_uid, position, hardware_version, firmware_version,
                 device_identifier, enumeration_type):
    
    global UID
    if device_identifier == 277:
        UID = uid
    else:
        return

    print("UID:               " + uid)
    print("Enumeration Type:  " + str(enumeration_type))

    if enumeration_type == IPConnection.ENUMERATION_TYPE_DISCONNECTED:
        print("")
        return

    print("Connected UID:     " + connected_uid)
    print("Position:          " + position)
    print("Hardware Version:  " + str(hardware_version))
    print("Firmware Version:  " + str(firmware_version))
    print("Device Identifier: " + str(device_identifier))
    print("")


if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    master = BrickMaster(UID_MASTER, ipcon) # Create device object
    ipcon.register_callback(IPConnection.CALLBACK_ENUMERATE, cb_enumerate)

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    ipcon.enumerate()
    time.sleep(0.5)
    rs485 = BrickletRS485(UID, ipcon) # Create device object

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
