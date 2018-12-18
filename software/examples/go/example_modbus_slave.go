package main

import (
	"fmt"
	"github.com/tinkerforge/go-api-bindings/ipconnection"
    "github.com/tinkerforge/go-api-bindings/rs485_bricklet"
)

const ADDR string = "localhost:4223"
const UID string = "XYZ" // Change XYZ to the UID of your RS485 Bricklet.

func main() {
	ipcon := ipconnection.New()
    defer ipcon.Close()
	rs485, _ := rs485_bricklet.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
    defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	// Set operating mode to Modbus RTU slave
	rs485.SetMode(rs485_bricklet.ModeModbusSlaveRTU)

	// Modbus specific configuration:
	// - slave address = 17
	// - master request timeout = 0ms (unused in slave mode)
	rs485.SetModbusConfiguration(17, 0)

	rs485.RegisterModbusSlaveWriteSingleRegisterRequestCallback(func(requestID uint8, registerAddress uint32, registerValue uint16) {
		fmt.Println("Request ID: ", requestID)
		fmt.Println("Register Address: ", registerAddress)
		fmt.Println("Register Value: ", registerValue)
		if registerAddress != 42 {
            fmt.Println("Error: Invalid register address");
            rs485.ModbusSlaveReportException(requestID, rs485_bricklet.ExceptionCodeIllegalDataAddress);
        } else {
            rs485.ModbusSlaveAnswerWriteSingleRegisterRequest(requestID);
        }
	})

	fmt.Print("Press enter to exit.")
	fmt.Scanln()

	ipcon.Disconnect()
}
