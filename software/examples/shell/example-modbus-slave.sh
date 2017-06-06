#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this


uid=XYZ # Change XYZ to the UID of your RS485 Bricklet
export _tmp=$(mktemp)
echo 0 > ${_tmp}

# Register write single register request callback
tinkerforge dispatch rs485-bricklet $uid modbus-slave-write-single-register-request \
  --execute "echo 'Request ID = {request-id}'
             echo 'Register Address = {register-address}'
             echo 'Register Value = {register-value}'

             # Here we assume valid writable register address is 42
             if [ {register-address} != 42 ]; then
               echo 'Requested invalid register address'
               tinkerforge call rs485-bricklet $uid modbus-slave-report-exception {request-id} exception-code-illegal-data-address
             else
               echo 'Request OK'
               tinkerforge call rs485-bricklet $uid modbus-slave-answer-write-single-register-request {request-id}
             fi" &

# Set operating mode to Modbus RTU slave
tinkerforge call rs485-bricklet $uid set-mode mode-modbus-slave-rtu

# Modbus specific configuration:
# - slave address = 17
# - master request timeout = 0ms (unused in slave mode)
tinkerforge call rs485-bricklet $uid set-modbus-configuration 17 0

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
