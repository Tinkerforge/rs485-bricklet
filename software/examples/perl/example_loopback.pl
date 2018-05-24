#!/usr/bin/perl

# For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
# and configure the DIP switch on the Bricklet to full-duplex mode

use strict;
use Tinkerforge::IPConnection;
use Tinkerforge::BrickletRS485;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your RS485 Bricklet

# Callback subroutine for read callback
sub cb_read
{
    my ($message) = @_;

    print "Message: \"" . join('', @{$message}) . "\"\n";
}

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $rs485 = Tinkerforge::BrickletRS485->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Enable full-duplex mode
$rs485->set_rs485_configuration(115200, $rs485->PARITY_NONE, $rs485->STOPBITS_1,
                                $rs485->WORDLENGTH_8, $rs485->DUPLEX_FULL);

# Register read callback to subroutine cb_read
$rs485->register_callback($rs485->CALLBACK_READ, 'cb_read');

# Enable read callback
$rs485->enable_read_callback();

# Write "test" string
my @message = split('', 'test');
$rs485->write(\@message);

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
