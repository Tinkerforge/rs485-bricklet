use std::{error::Error, io, sync::mpsc::channel, thread};
use tinkerforge::{ip_connection::IpConnection, rs485_bricklet::*};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your RS485 Bricklet

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let rs485 = Rs485Bricklet::new(UID, &ipcon); // Create device object

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd
                                          // Don't use device before ipcon is connected

    // Set operating mode to Modbus RTU master
    rs485.set_mode(RS485_BRICKLET_MODE_MODBUS_MASTER_RTU);

    // Modbus specific configuration:
    // - slave address = 1 (unused in master mode)
    // - master request timeout = 1000ms
    rs485.set_modbus_configuration(1, 1000);

    let modbus_master_write_single_register_response_receiver =
        rs485.get_modbus_master_write_single_register_response_callback_receiver();

    //Create channel to send expected request id to the event handling thread.
    let (tx, rx) = channel();

    // Spawn thread to handle received events.
    // This thread ends when the `rs485` object
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for response in modbus_master_write_single_register_response_receiver {
            println!("Request ID: {}", response.request_id);
            println!("Exception Code: {}", response.exception_code);
            let expected_request_id = rx.recv().unwrap();
            if response.request_id != expected_request_id {
                println!("Error: Unexpected request ID");
            }
        }
    });

    // Write 65535 to register 42 of slave 17
    let expected_request_id = rs485
        .modbus_master_write_single_register(17, 42, 65535)
        .recv()?;
    tx.send(expected_request_id)?;

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
