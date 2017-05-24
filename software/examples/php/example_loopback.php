<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletRS485.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletRS485;

// For this example connect the RX+/- pins to TX+/- pins on the same bricklet
// and configure the Bricklet to be in full-duplex mode

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your RS485 Bricklet

// Callback function for read callback
function cb_read($message)
{
  printf("Message (Length: %g): %s\n", count($message), implode('', $message));
}

$ipcon = new IPConnection(); // Create IP connection
$rs485 = new BrickletRS485(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Register read callback to function cb_read
$rs485->registerCallback(BrickletRS485::CALLBACK_READ, 'cb_read');

// Enable read callback
$rs485->enableReadCallback();

# Write "test" string
$rs485->write(explode('', 'test'));

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
