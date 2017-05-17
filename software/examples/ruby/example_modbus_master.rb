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

# Modbus specific configuration
#
# Slave address = 1 (Unused in master mode)
# Request timeout = 1000ms (Unused in slave mode)
rs485.set_modbus_configuration 1, 1000

rs485.set_mode BrickletRS485::MODE_MODBUS_MASTER_RTU

# Register write single register response callback
rs485.register_callback(BrickletRS485::CALLBACK_MODBUS_MASTER_WRITE_SINGLE_REGISTER_RESPONSE) \
  do |request_id, exception_code|
    if request_id != expected_request_id
      puts "Unexpected request ID"

      return
    end

    puts "Request ID = #{request_id}"
    puts "Exception Code = #{exception_code}"
end

# Request single register write
expected_request_id = rs485.modbus_master_write_single_register 1, 42+1, 65535

puts "Press key to exit"

$stdin.gets
ipcon.disconnect
