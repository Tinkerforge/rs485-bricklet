program ExampleLoopback;

{$ifdef MSWINDOWS}{$apptype CONSOLE}{$endif}
{$ifdef FPC}{$mode OBJFPC}{$H+}{$endif}

uses
  SysUtils, IPConnection, BrickletRS485;

  {
    For this example connect the RX+/- pins to TX+/- pins on the same bricklet
    and configure the Bricklet to be in full-duplex mode
  }

type
  TExample = class
  private
    ipcon: TIPConnection;
    rs485: TBrickletRS485;
  public
    procedure ReadCB(sender: TBrickletRS485; const message: array of byte);
    procedure Execute;
  end;

const
  HOST = 'localhost';
  PORT = 4223;
  UID = 'XYZ'; { Change XYZ to the UID of your RS485 Bricklet }

var
  e: TExample;
  bufferString: string;
  bufferBytes: array of byte;

{ Callback function for read callback }
procedure TExample.ReadCB(sender: TBrickletRS485; const message: array of byte);
begin
  WriteLn(Format('Message (Length: %d): "%s"', [length(message), StrPas(message)]);
end;

procedure TExample.Execute;
begin
  { Create IP connection }
  ipcon := TIPConnection.Create;

  { Create device object }
  rs485 := TBrickletRS485.Create(UID, ipcon);

  { Connect to brickd }
  ipcon.Connect(HOST, PORT);
  { Don't use device before ipcon is connected }

  { Enable read callback }
  rs485.EnableReadCallback;

  { Register read callback to function ReadCB }
  rs485.OnRead := {$ifdef FPC}@{$endif}ReadCB;

  { Write "test" string}
  bufferString := "test";
  Move(bufferString[1], bufferBytes^, Length(bufferString));
  rs485.Write(bufferBytes);

  WriteLn('Press key to exit');
  ReadLn;
  ipcon.Destroy; { Calls ipcon.Disconnect internally }
end;

begin
  e := TExample.Create;
  e.Execute;
  e.Destroy;
end.
