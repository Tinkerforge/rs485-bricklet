#!/usr/bin/perl

# For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
# and configure the DIP switch on the Bricklet to full-duplex mode

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletRS485;
use threads::shared;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your RS485 Bricklet

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $rs485 = Tinkerforge::BrickletRS485->new(&UID, $ipcon); # Create device object

# Callback subroutine for read callback
sub cb_read
{
    my ($message) = @_;

    print "Message: \"" . join('', @{$message}) . "\"\n";
}

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Register read callback to subroutine cb_read
$rs485->register_callback($rs485->CALLBACK_READ, 'cb_read');

# Enable read callback
$rs485->enable_read_callback();

# Write "test" string
$rs485->write(split('', 'test'));

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
