using System;
using Tinkerforge;

// For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
// and configure the DIP switch on the Bricklet to full-duplex mode

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your RS485 Bricklet

	// Callback function for read callback
	static void ReadCB(BrickletRS485 sender, char[] message)
	{
		// Assume that the message consists of ASCII characters and
		// convert it from an array of chars to a string
		Console.WriteLine("Message: \"" + new string(message) + "\"");
	}

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletRS485 rs485 = new BrickletRS485(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Register read callback to function ReadCB
		rs485.ReadCallback += ReadCB;

		// Enable read callback
		rs485.EnableReadCallback();

		// Write "test" string
		rs485.Write("test".ToCharArray());

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
