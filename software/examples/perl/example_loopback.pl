#!/usr/bin/perl

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletRS485;
use threads::shared;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your RS485 Bricklet

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $rs485 = Tinkerforge::BrickletRS485->new(&UID, $ipcon); # Create device object

# For this example connect the RX+/- pins to TX+/- pins on the same bricklet
# and configure the Bricklet to be in full-duplex mode

# Callback function for read callback
sub cb_read
{
  my ($message) = @_;

  printf("Message (Length: %d): %s\n", scalar(@{$message}), join('', @{$message}));
}

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connecte

# Enable read callback
$rs485->enable_read_callback();

# Register read callback to function cb_read
$rs485->register_callback($rs485->CALLBACK_READ, 'cb_read');

# Write "test" string
$rs485->write(split(' ', 'test'));

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
