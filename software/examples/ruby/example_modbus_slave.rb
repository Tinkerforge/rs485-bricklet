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

# Set operating mode to Modbus RTU slave
rs485.set_mode BrickletRS485::MODE_MODBUS_SLAVE_RTU

# Modbus specific configuration:
# - slave address = 17
# - master request timeout = 0ms (unused in slave mode)
rs485.set_modbus_configuration 17, 0

# Register Modbus slave write single register request callback
rs485.register_callback(BrickletRS485::CALLBACK_MODBUS_SLAVE_WRITE_SINGLE_REGISTER_REQUEST) \
do |request_id, register_address, register_value|
  puts "Request ID: #{request_id}"
  puts "Register Address: #{register_address}"
  puts "Register Value: #{register_value}"

  if register_address != 42
    puts 'Error: Invalid register address'
    rs485.modbus_slave_report_exception request_id, BrickletRS485::EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS
  else
    rs485.modbus_slave_answer_write_single_register_request request_id
  end
end

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
