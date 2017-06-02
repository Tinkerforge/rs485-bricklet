function matlab_example_modbus_slave()
    global rs485;

    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletRS485;
    import java.lang.String;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your RS485 Bricklet

    ipcon = IPConnection(); % Create IP connection
    rs485 = handle(BrickletRS485(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Set operating mode
    rs485.setMode(BrickletRS485.MODE_MODBUS_SLAVE_RTU);

    % Modbus specific configuration
    %
    % Slave address = 1 (Unused in master mode)
    % Request timeout = 1000ms (Unused in slave mode)
    rs485.setModbusConfiguration(1, 1000);

    % Register write single register request callback
    set(rs485, 'ModbusSlaveWriteSingleRegisterRequestCallback', @(h, e) cb_modbus_slave_write_single_register_request(e));

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end

% Callback function for Modbus master write single request callback
function cb_modbus_slave_write_single_register_request(e)
  global rs485;

  fprintf('Request ID = %g\n', e.requestID);
  fprintf('Register Address = %g\n', e.registerAddress);
  fprintf('Register Value = %g\n', e.registerValue);

  % Here we assume valid writable register address is 42
  if e.registerAddress ~= 42
      fprintf('Requested invalid register address\n');
      rs485.modbusSlaveReportException(e.requestID, com.tinkerforge.BrickletRS485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
  else
      fprintf('Request OK\n');
      rs485.modbusSlaveAnswerWriteSingleRegisterRequest(e.requestID);
  end
end
