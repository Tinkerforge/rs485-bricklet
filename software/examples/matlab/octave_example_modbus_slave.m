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

    % Set operating mode
    rs485.setMode(rs485.MODE_MODBUS_SLAVE_RTU);

    % Modbus specific configuration
    %
    % Slave address = 1 (Unused in master mode)
    % Request timeout = 1000ms (Unused in slave mode)
    rs485.setModbusConfiguration(1, 1000);

    % Register write single register request callback
    rs485.addModbusSlaveWriteSingleRegisterRequestCallback(@cb_modbus_slave_write_single_register_request);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for write single register request callback
function cb_modbus_slave_write_single_register_request(e)
    global rs485;

    fprintf("Request ID = %d\n", e.requestID);
    fprintf("Register Address = %d\n", e.registerAddress);
    fprintf("Register Value = %d\n", e.registerValue);

    % Here we assume valid writable register address is 42
    if e.registerAddress ~= 42
        fprintf("Requested invalid register address\n");
        rs485.modbusSlaveReportException(e.requestID, rs485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
    else
        fprintf("Request OK\n");
        rs485.modbusSlaveAnswerWriteSingleRegisterRequest(e.requestID);
    end
end
