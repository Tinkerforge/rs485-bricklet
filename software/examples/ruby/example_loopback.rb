#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_rs485'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change XYZ to the UID of your RS485 Bricklet

ipcon = IPConnection.new # Create IP connection
rs485 = BrickletRS485.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Register read callback
rs485.register_callback(BrickletRS485::CALLBACK_READ) \
  do |message|
    puts "Message (Length: #{message.length}): #{message.join('')}"
end

# Enable read callback
rs485.enable_read_callback()

# Write "test" string
rs485.write('test'.split(''))

puts "Press key to exit"

$stdin.gets
ipcon.disconnect
