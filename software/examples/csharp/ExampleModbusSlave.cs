using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your RS485 Bricklet

	// Callback function for Modbus slave write single register request callback
	static void ModbusSlaveWriteSingleRegisterRequestCB(BrickletRS485 sender,
	                                                    byte requestID,
	                                                    long registerAddress,
	                                                    int registerValue)
	{
		Console.WriteLine("Request ID: " + requestID);
		Console.WriteLine("Register Address: " + registerAddress);
		Console.WriteLine("Register Value: " + registerValue);

		if (registerAddress != 42)
		{
			Console.WriteLine("Error: Invalid register address");
			sender.ModbusSlaveReportException(requestID, BrickletRS485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
		}
		else
		{
			sender.ModbusSlaveAnswerWriteSingleRegisterRequest(requestID);
		}
	}

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletRS485 rs485 = new BrickletRS485(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Set operating mode to Modbus RTU slave
		rs485.SetMode(BrickletRS485.MODE_MODBUS_SLAVE_RTU);

		// Modbus specific configuration:
		// - slave address = 17
		// - master request timeout = 0ms (unused in slave mode)
		rs485.SetModbusConfiguration(17, 0);

		// Register Modbus slave write single register request callback
		// to function ModbusSlaveWriteSingleRegisterRequestCB
		rs485.ModbusSlaveWriteSingleRegisterRequestCallback += ModbusSlaveWriteSingleRegisterRequestCB;

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
