import java.util.Arrays;
import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletRS485;
import com.tinkerforge.TimeoutException;
import com.tinkerforge.NotConnectedException;

// For this example connect the RX+/- pins to TX+/- pins on the same bricklet
// and configure the Bricklet to be in full-duplex mode

public class ExampleLoopback {
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

    // Add read listener
    rs485.addReadListener(new BrickletRS485.ReadListener() {
      public void read(char[] message) {
        System.out.println(String.format("Message (Length: %d): \"%s\"", message.length, String.valueOf(message)));
      }
    });

    // Enable read callback
    rs485.enableReadCallback();

    // Write "test" string
    String buffer = "test";

    // Set operating mode of the Bricklet
    rs485.write(buffer.split(''), buffer.split('').length);

    System.out.println("Press key to exit");

    System.in.read();
    ipcon.disconnect();
  }
}
