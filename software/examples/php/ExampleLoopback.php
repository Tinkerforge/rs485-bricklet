<?php

// For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
// and configure the DIP switch on the Bricklet to full-duplex mode

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletRS485.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletRS485;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your RS485 Bricklet

// Callback function for read callback
function cb_read($message)
{
    echo "Message: \"" . implode('', $message) . "\"\n";
}

$ipcon = new IPConnection(); // Create IP connection
$rs485 = new BrickletRS485(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Enable full-duplex mode
$rs485->setRS485Configuration(115200, BrickletRS485::PARITY_NONE,
                              BrickletRS485::STOPBITS_1, BrickletRS485::WORDLENGTH_8,
                              BrickletRS485::DUPLEX_FULL);

// Register read callback to function cb_read
$rs485->registerCallback(BrickletRS485::CALLBACK_READ, 'cb_read');

// Enable read callback
$rs485->enableReadCallback();

# Write "test" string
$rs485->write(str_split('test'));

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
