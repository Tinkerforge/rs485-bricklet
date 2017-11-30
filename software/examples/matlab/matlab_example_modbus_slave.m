function matlab_example_modbus_slave()
    global rs485;

    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletRS485;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your RS485 Bricklet

    ipcon = IPConnection(); % Create IP connection
    rs485 = handle(BrickletRS485(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Set operating mode to Modbus RTU slave
    rs485.setMode(BrickletRS485.MODE_MODBUS_SLAVE_RTU);

    % Modbus specific configuration:
    % - slave address = 17
    % - master request timeout = 0ms (unused in slave mode)
    rs485.setModbusConfiguration(17, 0);

    % Register Modbus slave write single register request callback to function
    % cb_modbus_slave_write_single_register_request
    set(rs485, 'ModbusSlaveWriteSingleRegisterRequestCallback',
        @(h, e) cb_modbus_slave_write_single_register_request(e));

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end

% Callback function for Modbus slave write single register request callback
function cb_modbus_slave_write_single_register_request(e)
    global rs485;

    fprintf('Request ID: %i\n', e.requestID);
    fprintf('Register Address: %i\n', e.registerAddress);
    fprintf('Register Value: %i\n', e.registerValue);

    if e.registerAddress ~= 42
        fprintf('Error: Invalid register address\n');
        rs485.modbusSlaveReportException(e.requestID, com.tinkerforge.BrickletRS485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
    else
        rs485.modbusSlaveAnswerWriteSingleRegisterRequest(e.requestID);
    end
end
