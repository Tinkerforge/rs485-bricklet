package main

import (
	"fmt"
	"github.com/Tinkerforge/go-api-bindings/ipconnection"
    "github.com/Tinkerforge/go-api-bindings/rs485_bricklet"
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

	// Set operating mode to Modbus RTU master
	rs485.SetMode(rs485_bricklet.ModeModbusMasterRTU)

	// Modbus specific configuration:
	// - slave address = 1 (unused in master mode)
	// - master request timeout = 1000ms
	rs485.SetModbusConfiguration(1, 1000)

    expectedRequestID := uint8(0)
    
	rs485.RegisterModbusMasterWriteSingleRegisterResponseCallback(func(requestID uint8, exceptionCode rs485_bricklet.ExceptionCode) {
		fmt.Printf("Request ID: %d", requestID);
        if exceptionCode == rs485_bricklet.ExceptionCodeTimeout {
			fmt.Println("Exception Code: Timeout")
		} else if exceptionCode == rs485_bricklet.ExceptionCodeSuccess {
			fmt.Println("Exception Code: Success")
		} else if exceptionCode == rs485_bricklet.ExceptionCodeIllegalFunction {
			fmt.Println("Exception Code: Illegal Function")
		} else if exceptionCode == rs485_bricklet.ExceptionCodeIllegalDataAddress {
			fmt.Println("Exception Code: Illegal Data Address")
		} else if exceptionCode == rs485_bricklet.ExceptionCodeIllegalDataValue {
			fmt.Println("Exception Code: Illegal Data Value")
		} else if exceptionCode == rs485_bricklet.ExceptionCodeSlaveDeviceFailure {
			fmt.Println("Exception Code: Slave Device Failure")
		} else if exceptionCode == rs485_bricklet.ExceptionCodeAcknowledge {
			fmt.Println("Exception Code: Acknowledge")
		} else if exceptionCode == rs485_bricklet.ExceptionCodeSlaveDeviceBusy {
			fmt.Println("Exception Code: Slave Device Busy")
		} else if exceptionCode == rs485_bricklet.ExceptionCodeMemoryParityError {
			fmt.Println("Exception Code: Memory Parity Error")
		} else if exceptionCode == rs485_bricklet.ExceptionCodeGatewayPathUnavailable {
			fmt.Println("Exception Code: Gateway Path Unavailable")
		} else if exceptionCode == rs485_bricklet.ExceptionCodeGatewayTargetDeviceFailedToRespond {
			fmt.Println("Exception Code: Gateway Target Device Failed To Respond")
		}
        if requestID != expectedRequestID {
            fmt.Println("Error: Unexpected request ID");
        }
	})

	// Write 65535 to register 42 of slave 17
	expectedRequestID, _ = rs485.ModbusMasterWriteSingleRegister(17, 42, 65535)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()

	ipcon.Disconnect()
}
