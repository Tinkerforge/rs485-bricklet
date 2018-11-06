use std::{error::Error, io, thread};
use tinkerforge::{ipconnection::IpConnection, rs485_bricklet::*};

// For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
// and configure the DIP switch on the Bricklet to full-duplex mode

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your RS485 Bricklet

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let rs485_bricklet = RS485Bricklet::new(UID, &ipcon); // Create device object

    ipcon.connect(HOST, PORT).recv()??; // Connect to brickd
                                        // Don't use device before ipcon is connected

    // Enable full-duplex mode
    rs485_bricklet.set_rs485_configuration(
        115200,
        RS485_BRICKLET_PARITY_NONE,
        RS485_BRICKLET_STOPBITS_1,
        RS485_BRICKLET_WORDLENGTH_8,
        RS485_BRICKLET_DUPLEX_FULL,
    );

    //Create listener for read events.
    let read_listener = rs485_bricklet.get_read_receiver();
    // Spawn thread to handle received events. This thread ends when the rs485_bricklet
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for event in read_listener {
            match event {
                Some((payload, _)) => {
                    let message: String = payload.iter().collect();
                    println!("Message (Length: {}) {}", message.len(), message);
                }
                None => println!("Stream was out of sync."),
            }
        }
    });

    // Enable read callback
    rs485_bricklet.enable_read_callback();

    // Write "test" string
    rs485_bricklet.write(&['t', 'e', 's', 't'])?;

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
