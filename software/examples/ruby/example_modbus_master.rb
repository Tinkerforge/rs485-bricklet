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

expected_request_id = 0

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Set operating mode to Modbus RTU master
rs485.set_mode BrickletRS485::MODE_MODBUS_MASTER_RTU

# Modbus specific configuration:
# - slave address = 1 (unused in master mode)
# - master request timeout = 1000ms
rs485.set_modbus_configuration 1, 1000

# Register Modbus master write single register response callback
rs485.register_callback(BrickletRS485::CALLBACK_MODBUS_MASTER_WRITE_SINGLE_REGISTER_RESPONSE) do |request_id,
                                                                                                  exception_code|
  puts "Request ID: #{request_id}"
  puts "Exception Code: #{exception_code}"
  if request_id != expected_request_id
    puts "Unexpected request ID"
  puts ''
end

# Write 65535 to register 42 of slave 17
expected_request_id = rs485.modbus_master_write_single_register 17, 42, 65535

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
