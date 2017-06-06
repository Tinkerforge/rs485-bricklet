using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your RS485 Bricklet

	private static byte expectedRequestID = 0;

	// Callback function for Modbus master write single register response callback
	static void ModbusMasterWriteSingleRegisterResponseCB(BrickletRS485 sender,
	                                                      byte requestID,
	                                                      short exceptionCode)
	{
		Console.WriteLine("Request ID: " + requestID);
		Console.WriteLine("Exception Code: " + exceptionCode);

		if (requestID != expectedRequestID)
		{
			Console.WriteLine("Error: Unexpected request ID");
		}
	}

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletRS485 rs485 = new BrickletRS485(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Set operating mode to Modbus RTU master
		rs485.SetMode(BrickletRS485.MODE_MODBUS_MASTER_RTU);

		// Modbus specific configuration:
		// - slave address = 1 (unused in master mode)
		// - master request timeout = 1000ms
		rs485.SetModbusConfiguration(1, 1000);

		// Register Modbus master write single register response callback
		// to function ModbusMasterWriteSingleRegisterResponseCB
		rs485.ModbusMasterWriteSingleRegisterResponseCallback += ModbusMasterWriteSingleRegisterResponseCB;

		// Write 65535 to register 42 of slave 17
		expectedRequestID = rs485.ModbusMasterWriteSingleRegister(17, 42, 65535);

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
