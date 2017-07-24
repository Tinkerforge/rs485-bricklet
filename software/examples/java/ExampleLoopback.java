import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletRS485;

// For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
// and configure the DIP switch on the Bricklet to full-duplex mode

public class ExampleLoopback {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;

	// Change XYZ to the UID of your RS485 Bricklet
	private static final String UID = "XYZ";

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions
	//       you might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletRS485 rs485 = new BrickletRS485(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Enable full-duplex mode
		rs485.setRS485Configuration(115200, BrickletRS485.PARITY_NONE,
		                            BrickletRS485.STOPBITS_1, BrickletRS485.WORDLENGTH_8,
		                            BrickletRS485.DUPLEX_FULL);

		// Add read listener
		rs485.addReadListener(new BrickletRS485.ReadListener() {
			public void read(char[] message) {
				// Assume that the message consists of ASCII characters and
				// convert it from an array of chars to a string
				System.out.println("Message: \"" + String.valueOf(message) + "\"");
			}
		});

		// Enable read callback
		rs485.enableReadCallback();

		// Write "test" string
		rs485.write("test".toCharArray());

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
