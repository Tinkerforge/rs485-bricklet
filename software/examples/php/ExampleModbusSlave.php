<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletRS485.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletRS485;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your RS485 Bricklet

// Callback function for Modbus slave write single register request callback
function cb_modbusSlaveWriteSingleRegisterRequest($request_id, $register_address,
                                                  $register_value, $user_data)
{
    $rs485 = $user_data;

    echo "Request ID: $request_id\n";
    echo "Register Address: $register_address\n";
    echo "Register Value: $register_value\n";

    if ($register_address != 42) {
        echo "Requested invalid register address\n";
        $rs485->modbusSlaveReportException($request_id, BrickletRS485::EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
    } else {
        $rs485->modbusSlaveAnswerWriteSingleRegisterRequest($request_id);
    }
}

$ipcon = new IPConnection(); // Create IP connection
$rs485 = new BrickletRS485(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Set operating mode to Modbus RTU slave
$rs485->setMode(BrickletRS485::MODE_MODBUS_SLAVE_RTU);

// Modbus specific configuration:
// - slave address = 17
// - master request timeout = 0ms (unused in slave mode)
$rs485->setModbusConfiguration(17, 0);

// Register Modbus slave write single register request callback to function
// cb_modbusSlaveWriteSingleRegisterRequest
$rs485->registerCallback(BrickletRS485::CALLBACK_MODBUS_SLAVE_WRITE_SINGLE_REGISTER_REQUEST,
                         'cb_modbusSlaveWriteSingleRegisterRequest', $rs485);

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
