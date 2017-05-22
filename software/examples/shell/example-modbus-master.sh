#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your RS232 Bricklet

export _tmp=$(mktemp)

# Register write single register response callback
tinkerforge dispatch rs485-bricklet $uid modbus-master-write-single-register-response \
  --execute "export _expected-request-id=\$(cat \$_tmp)

             if [ {request-id} != \"\$_expected-request-id\" ]; then
               echo 'Unexpected request ID'
             else
               echo 'Request ID = {request-id}'
               echo 'Exception Code = {exception-code}'
             fi

             rm -rf $_tmp" &

# Set operating mode of the Bricklet
tinkerforge call rs485-bricklet $uid set-mode mode-modbus-master-rtu

# Modbus specific configuration
#
# Slave address = 1 (Unused in master mode)
# Request timeout = 1000ms (Unused in slave mode)
tinkerforge call rs485-bricklet $uid set-modbus-configuration 1 1000

# Request single register write
tinkerforge call rs485-bricklet $uid modbus-master-write-single-register 1 42 65535 \
  --execute "echo {request-id} > ${_tmp}"

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
