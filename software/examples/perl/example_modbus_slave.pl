#!/usr/bin/perl

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletRS485;
use threads::shared;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your RS485 Bricklet

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $rs485 :shared = Tinkerforge::BrickletRS485->new(&UID, $ipcon); # Create device object

# Callback subroutine for write single register request callback
sub cb_modbus_slave_write_single_register_request
{
  my ($request_id, $register_address, $register_value) = @_;

  print "Request ID = " . $request_id . "\n";
  print "Register Address = " . $register_address . "\n";
  print "Register Value = " . $register_value . "\n";

  # Here we assume valid writable register address is 42
  if($register_address != 42)
  {
      print "Requested invalid register address\n";
      $rs485->modbus_slave_report_exception($request_id, $rs485->EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
  }
  else
  {
      print "Request OK";
      $rs485->modbus_slave_answer_write_single_register_request($request_id);
  }
}

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Set operating mode
$rs485->set_mode($rs485->MODE_MODBUS_SLAVE_RTU);

# Modbus specific configuration
#
# Slave address = 1 (Unused in master mode)
# Request timeout = 1000ms (Unused in slave mode)
$rs485->set_modbus_configuration(1, 1000);

# Register write single register request callback
$rs485->register_callback($rs485->CALLBACK_MODBUS_SLAVE_WRITE_SINGLE_REGISTER_REQUEST,
                          'cb_modbus_slave_write_single_register_request');

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
