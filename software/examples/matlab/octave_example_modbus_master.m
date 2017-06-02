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

    % Set operating mode
    rs485.setMode(rs485.MODE_MODBUS_MASTER_RTU);

    % Modbus specific configuration
    %
    % Slave address = 1 (Unused in master mode)
    % Request timeout = 1000ms (Unused in slave mode)
    rs485.setModbusConfiguration(1, 1000);

    % Register write single register response callback
    rs485.addModbusMasterWriteSingleRegisterResponseCallback(@cb_modbus_master_write_single_register_response);

    % Request single register write
    expected_request_id = rs485.modbusMasterWriteSingleRegister(1, 42, 65535);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for write single register response callback
function cb_modbus_master_write_single_register_response(e)
    global expected_request_id;

    if e.requestID ~= expected_request_id
        fprintf("Unexpected request ID\n");

        return;
    end

    fprintf("Request ID = %d\n", e.requestID);
    fprintf("Exception Code = %d\n", e.exceptionCode);
end
