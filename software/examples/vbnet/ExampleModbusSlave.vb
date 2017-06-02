Imports System
Imports Tinkerforge

Module ExampleModbusSlave
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your RS485 Bricklet

    ' Callback function for read callback
    sub WriteSingleRegisterRequestCB(ByVal sender As BrickletRS485,
        ByVal requestID As Byte,
        ByVal registersAddress As Long,
        ByVal registersValue As Integer)
        Console.WriteLine("Request ID = {0}", requestID)
        Console.WriteLine("Register Address = {0}", requestID)
        Console.WriteLine("Register Value = {0}", requestID)

        ' Here we assume valid writable register address is 42
        if(registersAddress <> 42) Then
            Console.WriteLine("Requested invalid register address")
            sender.modbusSlaveReportException(requestID, BrickletRS485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS)
        Else
            Console.WriteLine("Request OK")
            sender.modbusSlaveAnswerWriteSingleRegisterRequest(requestID)
        End If
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim rs485 As New BrickletRS485(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Set operating mode
        rs485.SetMode(BrickletRS485.MODE_MODBUS_SLAVE_RTU)

        ' Modbus specific configuration
        '
        ' Slave mode address = 1 (Used only in slave mode)
        ' Master mode request timeout = 1000ms (Used only in master mode)
        rs485.SetModbusConfiguration(1, 1000)

        ' Register write single register response callback
        AddHandler rs485.ModbusSlaveWriteSingleRegisterRequestCallback,
        AddressOf WriteSingleRegisterRequestCB


        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
