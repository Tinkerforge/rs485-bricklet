import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletRS485;

public class ExampleModbusMaster {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;

	// Change XYZ to the UID of your RS485 Bricklet
	private static final String UID = "XYZ";

	private static int expectedRequestID = 0;

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions
	//       you might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletRS485 rs485 = new BrickletRS485(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Set operating mode to Modbus RTU master
		rs485.setMode(BrickletRS485.MODE_MODBUS_MASTER_RTU);

		// Modbus specific configuration:
		// - slave address = 1 (unused in master mode)
		// - master request timeout = 1000ms
		rs485.setModbusConfiguration(1, 1000);

		// Add Modbus master write single register response listener
		rs485.addModbusMasterWriteSingleRegisterResponseListener(new BrickletRS485.ModbusMasterWriteSingleRegisterResponseListener() {
			public void modbusMasterWriteSingleRegisterResponse(int requestID,
			                                                    int exceptionCode) {
				System.out.println("Request ID: " + requestID);
				System.out.println("Exception Code: " + exceptionCode);

				if (requestID != expectedRequestID) {
					System.out.println("Error: Unexpected request ID");
				}
			}
		});

		// Write 65535 to register 42 of slave 17
		expectedRequestID = rs485.modbusMasterWriteSingleRegister(17, 42, 65535);

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
