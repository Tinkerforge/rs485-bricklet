program ExampleModbusSlave;

{$ifdef MSWINDOWS}{$apptype CONSOLE}{$endif}
{$ifdef FPC}{$mode OBJFPC}{$H+}{$endif}

uses
  SysUtils, IPConnection, BrickletRS485;

type
  TExampleModbusSlave = class
  private
    ipcon: TIPConnection;
    rs485: TBrickletRS485;
  public
    procedure ModbusSlaveWriteSingleRegisterRequestCB(sender: TBrickletRS485;
                                                      const requestID: byte;
                                                      const registerAddress: longword;
                                                      const registerValue: word);
    procedure Execute;
  end;

const
  HOST = 'localhost';
  PORT = 4223;
  UID = 'XYZ'; { Change XYZ to the UID of your RS485 Bricklet }

var
  e: TExampleModbusSlave;

{ Callback function for write single register request callback }
procedure TExampleModbusSlave.ModbusSlaveWriteSingleRegisterRequestCB(sender: TBrickletRS485;
                                                                      const requestID: byte;
                                                                      const registerAddress: longword;
                                                                      const registerValue: word);
begin
  WriteLn(Format('Request ID = %d', [requestID]));
  WriteLn(Format('Register Address = %d', [registerAddress]));
  WriteLn(Format('Register Value = %d', [registerValue]));

  { Here we assume valid writable register address is 42 }
  if (registerAddress <> 42) then
    begin
      WriteLn('Requested invalid register address');
      sender.ModbusSlaveReportException(requestID, BRICKLET_RS485_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
    end
  else
    begin
      WriteLn('Request OK');
      sender.ModbusSlaveAnswerWriteSingleRegisterRequest(requestID);
    end
end;

procedure TExampleModbusSlave.Execute;
begin
  { Create IP connection }
  ipcon := TIPConnection.Create;

  { Create device object }
  rs485 := TBrickletRS485.Create(UID, ipcon);

  { Connect to brickd }
  ipcon.Connect(HOST, PORT);
  { Don't use device before ipcon is connected }

  { Set operating mode of the Bricklet }
  rs485.SetMode(BRICKLET_RS485_MODE_MODBUS_SLAVE_RTU);

  {
    Modbus specific configuration

    Slave address = 1 (Unused in master mode)
    Request timeout = 1000ms (Unused in slave mode)
  }
  rs485.SetModbusConfiguration(1, 1000);

  { Register write single register request callback }
  rs485.OnModbusSlaveWriteSingleRegisterRequest := {$ifdef FPC}@{$endif}ModbusSlaveWriteSingleRegisterRequestCB;

  WriteLn('Press key to exit');
  ReadLn;
  ipcon.Destroy; { Calls ipcon.Disconnect internally }
end;

begin
  e := TExampleModbusSlave.Create;
  e.Execute;
  e.Destroy;
end.
