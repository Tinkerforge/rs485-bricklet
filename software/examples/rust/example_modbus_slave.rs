use std::{error::Error, io, thread};
use tinkerforge::{ip_connection::IpConnection, rs485_bricklet::*};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your RS485 Bricklet

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let rs485 = Rs485Bricklet::new(UID, &ipcon); // Create device object

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd
                                          // Don't use device before ipcon is connected

    // Set operating mode to Modbus RTU slave
    rs485.set_mode(RS485_BRICKLET_MODE_MODBUS_SLAVE_RTU);

    // Modbus specific configuration:
    // - slave address = 17
    // - master request timeout = 0ms (unused in slave mode)
    rs485.set_modbus_configuration(17, 0);

    let modbus_slave_write_single_register_request_receiver = rs485.get_modbus_slave_write_single_register_request_callback_receiver();

    // Spawn thread to handle received callback messages.
    // This thread ends when the `rs485` object
    // is dropped, so there is no need for manual cleanup.
    let rs485_copy = rs485.clone(); //Device objects don't implement Sync, so they can't be shared between threads (by reference). So clone the device and move the copy.
    thread::spawn(move || {
        for request in modbus_slave_write_single_register_request_receiver {
            println!("Request ID: {}", request.request_id);
            println!("Register Address: {}", request.register_address);
            println!("Register Value: {}", request.register_value);
            if request.register_address != 42 {
                println!("Error: Invalid register address");
                rs485_copy.modbus_slave_report_exception(request.request_id, RS485_BRICKLET_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
            } else {
                rs485_copy.modbus_slave_answer_write_single_register_request(request.request_id);
            }
        }
    });

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
