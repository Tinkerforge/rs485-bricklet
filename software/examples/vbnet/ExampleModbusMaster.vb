Imports System
Imports Tinkerforge

Module ExampleModbusMaster
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your RS485 Bricklet

    Dim expectedRequestID As Byte = 0

    ' Callback subroutine for Modbus master write single register response callback
    Sub ModbusMasterWriteSingleRegisterResponseCB(ByVal sender As BrickletRS485, _
                                                  ByVal requestID As Byte, _
                                                  ByVal exceptionCode As Short)
        Console.WriteLine("Request ID: " + requestID.ToString())
        Console.WriteLine("Exception Code: " + exceptionCode.ToString())

        If requestID <> expectedRequestID Then
            Console.WriteLine("Error: Unexpected request ID")
        End If
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim rs485 As New BrickletRS485(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Set operating mode to Modbus RTU master
        rs485.SetMode(BrickletRS485.MODE_MODBUS_MASTER_RTU)

        ' Modbus specific configuration:
        ' - slave address = 1 (unused in master mode)
        ' - master request timeout = 1000ms
        rs485.SetModbusConfiguration(1, 1000)

        ' Register Modbus master write single register response callback to
        ' subroutine ModbusMasterWriteSingleRegisterResponseCB
        AddHandler rs485.ModbusMasterWriteSingleRegisterResponseCallback, _
                   AddressOf ModbusMasterWriteSingleRegisterResponseCB

        ' Write 65535 to register 42 of slave 17
        expectedRequestID = rs485.ModbusMasterWriteSingleRegister(17, 42, 65535)

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
