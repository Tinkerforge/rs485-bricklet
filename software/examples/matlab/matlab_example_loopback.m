function matlab_example_loopback()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletRS485;
    import java.lang.String;

    % For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
    % and configure the DIP switch on the Bricklet to full-duplex mode

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your RS485 Bricklet

    ipcon = IPConnection(); % Create IP connection
    rs485 = handle(BrickletRS485(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register read callback to function cb_read
    set(rs485, 'ReadCallback', @(h, e) cb_read(e));

    % Enable read callback
    rs485.enableReadCallback();

    % Write "test" string
    rs485.write(String('test').toCharArray());

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end

% Callback function for read callback
function cb_read(e)
    fprintf('Message: "%s"\n', e.message);
end
