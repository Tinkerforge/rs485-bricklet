import java.util.Arrays;
import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletRS485;
import com.tinkerforge.TimeoutException;
import com.tinkerforge.NotConnectedException;

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

		// Set operating mode
		rs485.setMode(BrickletRS485.MODE_MODBUS_MASTER_RTU);

		/*
		* Modbus specific configuration
		*
		* Slave mode address = 1 (Unused in master mode)
		* Master mode request timeout = 1000ms (Unused in slave mode)
		*/
		rs485.setModbusConfiguration((short)1, 1000);

		// Add Modbus master write single register response listener
		rs485.addModbusMasterWriteSingleRegisterResponseListener(new BrickletRS485.ModbusMasterWriteSingleRegisterResponseListener() {
			public void modbusMasterWriteSingleRegisterResponse(int requestID, int exceptionCode) {
				if(requestID != expectedRequestID) {
				System.out.println("Unexpected request ID");

				return;
				}

				System.out.println("Request ID: " + requestID);
				System.out.println("Exception Code: " + exceptionCode);
			}
		});

		// Request single register write
		expectedRequestID = rs485.modbusMasterWriteSingleRegister(1, 42, 65535);

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
