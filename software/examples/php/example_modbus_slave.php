<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletRS485.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletRS485;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your RS485 Bricklet

// Callback function for read callback
function cb_write_single_register_request($request_id, $register_address, $register_value)
{
  echo "Request ID = " . $request_id . "\n";
  echo "Register Address = " . $register_address . "\n";
  echo "Register Value = " . $register_value . "\n";

  // Here we assume valid writable register address is 42
  if($register_address != 42)
  {
    echo "Requested invalid register address\n";
    $GLOBALS['rs485']->modbusSlaveReportException($request_id, BrickletRS485::EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
  }
  else
  {
    echo "Request OK\n";
    $GLOBALS['rs485']->modbusSlaveAnswerWriteSingleRegisterRequest($request_id);
  }
}

$ipcon = new IPConnection(); // Create IP connection
$rs485 = new BrickletRS485(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Set operating mode of the Bricklet
$rs485->setMode(BrickletRS485::MODE_MODBUS_SLAVE_RTU);

/*
 * Modbus specific configuration
 *
 * Slave mode address = 1 (Used only in slave mode)
 * Master mode request timeout = 1000ms (Used only in master mode)
 */
$rs485->setModbusConfiguration(1, 1000);

// Register write single register response callback
$rs485->registerCallback(BrickletRS485::CALLBACK_MODBUS_SLAVE_WRITE_SINGLE_REGISTER_REQUEST,
                         'cb_write_single_register_request');

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
