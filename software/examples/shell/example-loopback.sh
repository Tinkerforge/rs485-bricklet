#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

# For this example connect the RX+/- pins to TX+/- pins on the same bricklet
# and configure the Bricklet to be in full-duplex mode

uid=MST # Change XYZ to the UID of your RS485 Bricklet

# Handle incoming read callback
tinkerforge dispatch rs485-bricklet $uid read &

# Enable read callback
tinkerforge call rs485-bricklet $uid enable-read-callback

# Write "test" string
tinkerforge call rs485-bricklet $uid write t,e,s,t,.. 4

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
