import java.util.Arrays;
import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletRS485;
import com.tinkerforge.TimeoutException;
import com.tinkerforge.NotConnectedException;

public class ExampleModbusSlave {
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

		// Set operating mode
		rs485.setMode(BrickletRS485.MODE_MODBUS_SLAVE_RTU);

		/*
		 * Modbus specific configuration
		 *
		 * Slave mode address = 1 (Unused in master mode)
		 * Master mode request timeout = 1000ms (Unused in slave mode)
		 */
		rs485.setModbusConfiguration((short)1, 1000);

		// Add Modbus slave write single register request listener
		rs485.addModbusSlaveWriteSingleRegisterRequestListener(new BrickletRS485.ModbusSlaveWriteSingleRegisterRequestListener() {
			public void modbusSlaveWriteSingleRegisterRequest(int requestID, long registerAddress, int registerValue) {
				try {
					System.out.println("Request ID: " + requestID);
					System.out.println("Register Address: " + registerAddress);
					System.out.println("Register Value: " + registerValue);

					// Here we assume valid writable register address is 42
					if(registerAddress != 42) {
						System.out.println("Requested invalid register address");
						rs485.modbusSlaveReportException(requestID, rs485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
					} else {
						System.out.println("Request OK");
						rs485.modbusSlaveAnswerWriteSingleRegisterRequest(requestID);
					}
				}
				catch(Exception e) {
				}
			}
		});

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
