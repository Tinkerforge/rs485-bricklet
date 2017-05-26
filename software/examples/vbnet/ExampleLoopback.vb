Imports System
Imports Tinkerforge

Module ExampleLoopback
  Const HOST As String = "localhost"
  Const PORT As Integer = 4223
  Const UID As String = "XYZ" ' Change XYZ to the UID of your RS485 Bricklet

  ' Callback function for read callback
  sub ReadCB(ByVal sender As BrickletRS485,
             ByVal message As Char())
    Dim messageString As String = New String(message)
    Console.WriteLine("Message (Length: {0}): {1}", message.Length, messageString)
  End Sub

  Sub Main()
    Dim ipcon As New IPConnection() ' Create IP connection
    Dim rs485 As New BrickletRS485(UID, ipcon) ' Create device object

    ipcon.Connect(HOST, PORT) ' Connect to brickd
    ' Don't use device before ipcon is connected

    ' Register read callback
    AddHandler rs485.ReadCallback, AddressOf ReadCB

    ' Enable read callback
    rs485.EnableReadCallback()

    ' Write "test" string
    rs485.write("test".ToCharArray())

    Console.WriteLine("Press enter to exit")

    Console.ReadLine()
    ipcon.Disconnect()
  End Sub
End Module
