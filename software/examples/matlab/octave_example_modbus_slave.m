function octave_example_modbus_slave()
    more off;
    global rs485;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your RS485 Bricklet

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    rs485 = javaObject("com.tinkerforge.BrickletRS485", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Set operating mode to Modbus RTU slave
    rs485.setMode(rs485.MODE_MODBUS_SLAVE_RTU);

    % Modbus specific configuration:
    % - slave address = 17
    % - master request timeout = 0ms (unused in slave mode)
    rs485.setModbusConfiguration(17, 0);

    % Register Modbus slave write single register request callback to
    % function cb_modbus_slave_write_single_register_request
    rs485.addModbusSlaveWriteSingleRegisterRequestCallback(@cb_modbus_slave_write_single_register_request);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for Modbus slave write single register request callback
function cb_modbus_slave_write_single_register_request(e)
    global rs485;

    fprintf("Request ID: %d\n", e.requestID);
    fprintf("Register Address: %d\n", java2int(e.registerAddress));
    fprintf("Register Value: %d\n", e.registerValue);

    if e.registerAddress ~= 42
        fprintf("Error: Invalid register address\n");
        rs485.modbusSlaveReportException(e.requestID, rs485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
    else
        rs485.modbusSlaveAnswerWriteSingleRegisterRequest(e.requestID);
    end
end

function int = java2int(value)
    if compare_versions(version(), "3.8", "<=")
        int = value.intValue();
    else
        int = value;
    end
end
