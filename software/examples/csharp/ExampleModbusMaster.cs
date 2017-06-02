using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your RS485 Bricklet

	private static byte expectedRequestID = 0;

	// Callback function for write single register response callback
	static void WriteSingleRegisterResponseCB(BrickletRS485 sender,
	                                          byte requestID,
	                                          short exceptionCode)
	{
		if(requestID != expectedRequestID)
		{
			Console.WriteLine("Unexpected request ID");

			return;
		}

		Console.WriteLine("Request ID: " + requestID);
		Console.WriteLine("Exception Code: " + exceptionCode);
	}

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletRS485 rs485 = new BrickletRS485(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Set operating mode
		rs485.SetMode(BrickletRS485.MODE_MODBUS_MASTER_RTU);

		/*
		 * Modbus specific configuration
		 *
		 * Slave mode address = 1 (Unused in master mode)
		 * Master mode request timeout = 1000ms (Unused in slave mode)
		 */
		rs485.SetModbusConfiguration(1, 1000);

		// Register write single register response callback
		rs485.ModbusMasterWriteSingleRegisterResponseCallback += WriteSingleRegisterResponseCB;

		// Request single register write
		expectedRequestID = rs485.ModbusMasterWriteSingleRegister(1, 42, 65535);

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
