<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletRS485.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletRS485;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your RS485 Bricklet

$expected_request_id = 0;

// Callback function for read callback
function cb_write_single_register_response($request_id, $exception_code)
{
  if($request_id != $GLOBALS['expected_request_id'])
  {
    print('Unexpected request ID');

    return;
  }

  echo("Request ID = " . $request_id . "\n");
  echo("Exception Code = " . $exception_code . "\n");
}

$ipcon = new IPConnection(); // Create IP connection
$rs485 = new BrickletRS485(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Set operating mode
$rs485->setMode(BrickletRS485::MODE_MODBUS_MASTER_RTU);

/*
 * Modbus specific configuration
 *
 * Slave mode address = 1 (Used only in slave mode)
 * Master mode request timeout = 1000ms (Used only in master mode)
 */
$rs485->setModbusConfiguration(1, 1000);

// Register write single register response callback
$rs485->registerCallback(BrickletRS485::CALLBACK_MODBUS_MASTER_WRITE_SINGLE_REGISTER_RESPONSE,
                         'cb_write_single_register_response');

$expected_request_id = $rs485->modbusMasterWriteSingleRegister(1, 42, 65535);

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
