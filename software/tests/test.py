#!/usr/bin/env python3

import serial
import time

CONFIG_BAUDRATE = 1000000 #115200 # 19200
CONFIG_TTY      = '/dev/ttyUSB1'

def main(baudrate, tty):
    with serial.Serial() as s:
        s.baudrate = baudrate
        s.port = tty
        s.parity = serial.PARITY_NONE
        s.timeout = 0.01
        s.open()

        i = 0
        while True:
#        for _ in range(50000):
            i += 1
#            r = s.read(100)
#            print("read: " + str(r))

            w = bytes(str(i) + '\n', "utf-8")
            s.write(w)
            time.sleep(0.01)

if __name__ == '__main__':
    main(CONFIG_BAUDRATE, CONFIG_TTY)
