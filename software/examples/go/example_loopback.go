package main

import (
	"fmt"
	"github.com/Tinkerforge/go-api-bindings/ipconnection"
    "github.com/Tinkerforge/go-api-bindings/rs485_bricklet"
)

// For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
// and configure the DIP switch on the Bricklet to full-duplex mode

const ADDR string = "localhost:4223"
const UID string = "XYZ" // Change XYZ to the UID of your RS485 Bricklet.

func main() {
	ipcon := ipconnection.New()
    defer ipcon.Close()
	rs485, _ := rs485_bricklet.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
    defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	// Enable full-duplex mode
	rs485.SetRS485Configuration(115200, rs485_bricklet.ParityNone,
		rs485_bricklet.Stopbits1,
		rs485_bricklet.Wordlength8,
		rs485_bricklet.DuplexFull)

	rs485.RegisterReadCallback(func(message []rune) {
        fmt.Println(string(message))
	})

	// Enable read callback
	rs485.EnableReadCallback()

	fmt.Print("Press enter to exit.")
	fmt.Scanln()

	ipcon.Disconnect()
}
