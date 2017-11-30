#!/usr/bin/perl

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletRS485;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your RS485 Bricklet

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
our $rs485 = Tinkerforge::BrickletRS485->new(&UID, $ipcon); # Create device object

# Callback subroutine for Modbus slave write single register request callback
sub cb_modbus_slave_write_single_register_request
{
    my ($request_id, $register_address, $register_value) = @_;

    print "Request ID: $request_id\n";
    print "Register Address: $register_address\n";
    print "Register Value: $register_value\n";

    if($register_address != 42)
    {
        print "Error: Invalid register address\n";
        $rs485->modbus_slave_report_exception($request_id, $rs485->EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
    }
    else
    {
        $rs485->modbus_slave_answer_write_single_register_request($request_id);
    }
}

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Set operating mode to Modbus RTU slave
$rs485->set_mode($rs485->MODE_MODBUS_SLAVE_RTU);

# Modbus specific configuration:
# - slave address = 17
# - master request timeout = 0ms (unused in slave mode)
$rs485->set_modbus_configuration(17, 0);

# Register Modbus slave write single register request callback to subroutine
# cb_modbus_slave_write_single_register_request
$rs485->register_callback($rs485->CALLBACK_MODBUS_SLAVE_WRITE_SINGLE_REGISTER_REQUEST,
                          'cb_modbus_slave_write_single_register_request');

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
