using System;
using Tinkerforge;

class ExampleModbusSlave
{
  private static string HOST = "localhost";
  private static int PORT = 4223;
  private static string UID = "XYZ"; // Change XYZ to the UID of your RS485 Bricklet

  // Callback function for write single register request callback
  static void WriteSingleRegisterRequestCB(BrickletRS485 sender,
                                           byte requestID,
                                           long registerAddress,
                                           int registerValue)
  {
    Console.WriteLine("Request ID: " + requestID);
    Console.WriteLine("Register Address: " + registerAddress);
    Console.WriteLine("Register Value: " + registerValue);

    // Here we assume valid writable register address is 42
    if(registerAddress != 42)
    {
      Console.WriteLine("Requested invalid register address");
      sender.ModbusSlaveReportException(requestID, BrickletRS485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
    }
    else
    {
      Console.WriteLine("Request OK");
      sender.ModbusSlaveAnswerWriteSingleRegisterRequest(requestID);
    }
  }

  static void Main()
  {
    IPConnection ipcon = new IPConnection(); // Create IP connection
    BrickletRS485 rs485 = new BrickletRS485(UID, ipcon); // Create device object

    ipcon.Connect(HOST, PORT); // Connect to brickd
    // Don't use device before ipcon is connected

    // Set operating mode of the Bricklet
    rs485.SetMode(BrickletRS485.MODE_MODBUS_SLAVE_RTU);

    /*
     * Modbus specific configuration
     *
     * Slave mode address = 1 (Unused in slave mode)
     * Master mode request timeout = 1000ms (Unused in master mode)
     */
    rs485.SetModbusConfiguration(1, 1000);

    // Register write single register request callback
    rs485.ModbusSlaveWriteSingleRegisterRequestCallback += WriteSingleRegisterRequestCB;

    Console.WriteLine("Press enter to exit");
    Console.ReadLine();
    ipcon.Disconnect();
  }
}
