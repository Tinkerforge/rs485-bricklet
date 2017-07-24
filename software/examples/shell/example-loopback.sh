#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

# For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
# and configure the DIP switch on the Bricklet to full-duplex mode

uid=XYZ # Change XYZ to the UID of your RS485 Bricklet

# Enable full-duplex mode
tinkerforge call rs485-bricklet $uid set-rs485-configuration 115200 parity-none stopbits-1 wordlength-8 duplex-full

# Handle incoming read callbacks
tinkerforge dispatch rs485-bricklet $uid read &

# Enable read callback
tinkerforge call rs485-bricklet $uid enable-read-callback

# Write "test" string
tinkerforge call rs485-bricklet $uid write t,e,s,t

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
