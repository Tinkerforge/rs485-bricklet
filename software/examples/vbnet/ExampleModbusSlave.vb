Imports System
Imports Tinkerforge

Module ExampleModbusSlave
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your RS485 Bricklet

    ' Callback subroutine for Modbus slave write single register request callback
    Sub ModbusSlaveWriteSingleRegisterRequestCB(ByVal sender As BrickletRS485, _
                                                ByVal requestID As Byte, _
                                                ByVal registerAddress As Long, _
                                                ByVal registerValue As Integer)
        Console.WriteLine("Request ID: " + requestID.ToString())
        Console.WriteLine("Register Address: " + registerAddress.ToString())
        Console.WriteLine("Register Value: " + registerValue.ToString())

        If registersAddress <> 42 Then
            Console.WriteLine("Error: Invalid register address")
            sender.modbusSlaveReportException(requestID, BrickletRS485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS)
        Else
            sender.modbusSlaveAnswerWriteSingleRegisterRequest(requestID)
        End If
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim rs485 As New BrickletRS485(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Set operating mode to Modbus RTU slave
        rs485.SetMode(BrickletRS485.MODE_MODBUS_SLAVE_RTU)

        ' Modbus specific configuration:
        ' - slave address = 17
        ' - master request timeout = 0ms (unused in slave mode)
        rs485.SetModbusConfiguration(17, 0)

        ' Register Modbus slave write single register request callback to
        ' subroutine ModbusSlaveWriteSingleRegisterRequestCB
        AddHandler rs485.ModbusSlaveWriteSingleRegisterRequestCallback, _
                   AddressOf ModbusSlaveWriteSingleRegisterRequestCB

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
