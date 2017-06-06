function octave_example_modbus_master()
    more off;
    global expected_request_id;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your RS485 Bricklet

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    rs485 = javaObject("com.tinkerforge.BrickletRS485", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Set operating mode to Modbus RTU master
    rs485.setMode(rs485.MODE_MODBUS_MASTER_RTU);

    % Modbus specific configuration:
    % - slave address = 1 (unused in master mode)
    % - master request timeout = 1000ms
    rs485.setModbusConfiguration(1, 1000);

    % Register Modbus master write single register response callback to
    % function cb_modbus_master_write_single_register_response
    rs485.addModbusMasterWriteSingleRegisterResponseCallback(@cb_modbus_master_write_single_register_response);

    % Write 65535 to register 42 of slave 17
    expected_request_id = rs485.modbusMasterWriteSingleRegister(17, 42, 65535);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for Modbus master write single register response callback
function cb_modbus_master_write_single_register_response(e)
    global expected_request_id;

    fprintf("Request ID: %d\n", e.requestID);
    fprintf("Exception Code: %d\n", e.exceptionCode);

    if e.requestID ~= expected_request_id
        fprintf("Error: Unexpected request ID\n");
    end
end
