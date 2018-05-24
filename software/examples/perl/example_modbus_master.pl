#!/usr/bin/perl

use strict;
use Tinkerforge::IPConnection;
use Tinkerforge::BrickletRS485;
use threads::shared;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your RS485 Bricklet

our $expected_request_id :shared = 0;

# Callback subroutine for Modbus master write single register response callback
sub cb_modbus_master_write_single_register_response
{
    my ($request_id, $exception_code) = @_;

    print "Request ID: $request_id\n";
    print "Exception Code: $exception_code\n";

    if ($request_id != $expected_request_id)
    {
        print "Error: Unexpected request ID\n";
    }
}

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $rs485 = Tinkerforge::BrickletRS485->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Set operating mode to Modbus RTU master
$rs485->set_mode($rs485->MODE_MODBUS_MASTER_RTU);

# Modbus specific configuration:
# - slave address = 1 (unused in master mode)
# - master request timeout = 1000ms
$rs485->set_modbus_configuration(1, 1000);

# Register Modbus master write single register response callback to subroutine
# cb_modbus_master_write_single_register_response
$rs485->register_callback($rs485->CALLBACK_MODBUS_MASTER_WRITE_SINGLE_REGISTER_RESPONSE,
                          'cb_modbus_master_write_single_register_response');

# Write 65535 to register 42 of slave 17
$expected_request_id = $rs485->modbus_master_write_single_register(17, 42, 65535);

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
