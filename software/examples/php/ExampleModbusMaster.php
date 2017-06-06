<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletRS485.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletRS485;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your RS485 Bricklet

$expected_request_id = 0;

// Callback function for Modbus master write single register response callback
function cb_modbusMasterWriteSingleRegisterResponse($request_id, $exception_code)
{
    echo "Request ID: $request_id\n";
    echo "Exception Code: $exception_code\n";

    if ($request_id != $GLOBALS['expected_request_id'])
    {
        echo "Error: Unexpected request ID";
    }
}

$ipcon = new IPConnection(); // Create IP connection
$rs485 = new BrickletRS485(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Set operating mode to Modbus RTU master
$rs485->setMode(BrickletRS485::MODE_MODBUS_MASTER_RTU);

// Modbus specific configuration:
// - slave address = 1 (unused in master mode)
// - master request timeout = 1000ms
$rs485->setModbusConfiguration(1, 1000);

// Register Modbus master write single register response callback to
// function cb_modbusMasterWriteSingleRegisterResponse
$rs485->registerCallback(BrickletRS485::CALLBACK_MODBUS_MASTER_WRITE_SINGLE_REGISTER_RESPONSE,
                         'cb_modbusMasterWriteSingleRegisterResponse');

// Write 65535 to register 42 of slave 17
$expected_request_id = $rs485->modbusMasterWriteSingleRegister(17, 42, 65535);

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
