function matlab_example_modbus_master()
    global expected_request_id;

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

    % Set operating mode of the Bricklet
    rs485.setMode(com.tinkerforge.BrickletRS485.MODE_MODBUS_MASTER_RTU);

    % Modbus specific configuration
    %
    % Slave address = 1 (Unused in master mode)
    % Request timeout = 1000ms (Unused in slave mode)
    rs485.setModbusConfiguration(1, 1000);

    % Register write single register response callback
    set(rs485, 'ModbusMasterWriteSingleRegisterResponseCallback', @(h, e) cb_modbus_master_write_single_register_response(e));

    % Request single register write
    rs485.modbusMasterWriteSingleRegister(1, 42, 65535);

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end

% Callback function for Modbus master write single register response callback
function cb_modbus_master_write_single_register_response(e)
    global expected_request_id;

    if e.requestID ~= expected_request_id
        fprintf('Unexpected request ID');

        return;
    end

    fprintf('Request ID = %g\n', e.requestID);
    fprintf('Exception Code = %g\n', e.exceptionCode);
end
