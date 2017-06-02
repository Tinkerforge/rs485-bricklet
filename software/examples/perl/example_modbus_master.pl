#!/usr/bin/perl

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletRS485;
use threads::shared;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your RS485 Bricklet

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $rs485 = Tinkerforge::BrickletRS485->new(&UID, $ipcon); # Create device object

my $expected_request_id :shared = 0;

# Callback subroutine for write single register response callback
sub cb_modbus_master_write_single_register_response
{
  my ($request_id, $exception_code) = @_;

  if($request_id != $expected_request_id)
  {
    print "Unexpected request ID\n";

    return;
  }

  print "Request ID = " . $request_id . "\n";
  print "Exception Code = " . $exception_code . "\n";
}

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Set operating mode
$rs485->set_mode($rs485->MODE_MODBUS_MASTER_RTU);

# Modbus specific configuration
#
# Slave address = 1 (Unused in master mode)
# Request timeout = 1000ms (Unused in slave mode)
$rs485->set_modbus_configuration(1, 1000);

# Register write single register response callback
$rs485->register_callback($rs485->CALLBACK_MODBUS_MASTER_WRITE_SINGLE_REGISTER_RESPONSE,
                          'cb_modbus_master_write_single_register_response');

# Request single register write
$expected_request_id = $rs485->modbus_master_write_single_register(1, 42, 65535);

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
