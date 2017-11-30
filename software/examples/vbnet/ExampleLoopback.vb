Imports System
Imports Tinkerforge

' For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
' and configure the DIP switch on the Bricklet to full-duplex mode

Module ExampleLoopback
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your RS485 Bricklet

    ' Callback subroutine for read callback
    Sub ReadCB(ByVal sender As BrickletRS485, ByVal message As Char())
        ' Assume that the message consists of ASCII characters and
        ' convert it from an array of chars to a string
        Console.WriteLine("Message: ""{0}""", New String(message))
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim rs485 As New BrickletRS485(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Enable full-duplex mode
        rs485.SetRS485Configuration(115200, BrickletRS485.PARITY_NONE, _
                                    BrickletRS485.STOPBITS_1, _
                                    BrickletRS485.WORDLENGTH_8, _
                                    BrickletRS485.DUPLEX_FULL)

        ' Register read callback to subroutine ReadCB
        AddHandler rs485.ReadCallback, AddressOf ReadCB

        ' Enable read callback
        rs485.EnableReadCallback()

        ' Write "test" string
        rs485.write("test".ToCharArray())

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
