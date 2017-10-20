import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletRS485;
import com.tinkerforge.TinkerforgeException;

public class ExampleModbusSlave {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;

	// Change XYZ to the UID of your RS485 Bricklet
	private static final String UID = "XYZ";

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions
	//       you might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		// Note: Declare rs485 as final, so the listener can access it
		final BrickletRS485 rs485 = new BrickletRS485(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Set operating mode to Modbus RTU slave
		rs485.setMode(BrickletRS485.MODE_MODBUS_SLAVE_RTU);

		// Modbus specific configuration:
		// - slave address = 17
		// - master request timeout = 0ms (unused in slave mode)
		rs485.setModbusConfiguration(17, 0);

		// Add Modbus slave write single register request listener
		rs485.addModbusSlaveWriteSingleRegisterRequestListener(new BrickletRS485.ModbusSlaveWriteSingleRegisterRequestListener() {
			public void modbusSlaveWriteSingleRegisterRequest(int requestID,
			                                                  long registerAddress,
			                                                  int registerValue) {
				System.out.println("Request ID: " + requestID);
				System.out.println("Register Address: " + registerAddress);
				System.out.println("Register Value: " + registerValue);

				if (registerAddress != 42) {
					System.out.println("Error: Invalid register address");

					try {
						rs485.modbusSlaveReportException(requestID, BrickletRS485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
					} catch(TinkerforgeException e) {
					}
				} else {
					try {
						rs485.modbusSlaveAnswerWriteSingleRegisterRequest(requestID);
					} catch(TinkerforgeException e) {
					}
				}
			}
		});

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
