Imports System
Imports Tinkerforge

Module ExampleModbusMaster
  Const HOST As String = "localhost"
  Const PORT As Integer = 4223
  Const UID As String = "XYZ" ' Change XYZ to the UID of your RS485 Bricklet

  Dim expectedRequestID as Byte = 0

  ' Callback function for read callback
  sub WriteSingleRegisterResponseCB(ByVal sender As BrickletRS485,
                                    ByVal requestID As Byte,
                                    ByVal exceptionCode As Short)
    if(requestID <> expectedRequestID) Then
      Console.WriteLine("Unexpected request ID")

      return
    End If

    Console.WriteLine("Request ID = {0}", requestID)
    Console.WriteLine("Exception Code = {0}", exceptionCode)
  End Sub

  Sub Main()
    Dim ipcon As New IPConnection() ' Create IP connection
    Dim rs485 As New BrickletRS485(UID, ipcon) ' Create device object

    ipcon.Connect(HOST, PORT) ' Connect to brickd
    ' Don't use device before ipcon is connected

    ' Set operating mode of the Bricklet
    rs485.SetMode(BrickletRS485.MODE_MODBUS_MASTER_RTU)

    ' Modbus specific configuration
    '
    ' Slave mode address = 1 (Used only in slave mode)
    ' Master mode request timeout = 1000ms (Used only in master mode)
    rs485.SetModbusConfiguration(1, 1000)

    ' Register write single register response callback
    AddHandler rs485.ModbusMasterWriteSingleRegisterResponseCallback,
      AddressOf WriteSingleRegisterResponseCB

    expectedRequestID = rs485.ModbusMasterWriteSingleregister(1, 42, 65535)

    Console.WriteLine("Press enter to exit")

    Console.ReadLine()
    ipcon.Disconnect()
  End Sub
End Module
