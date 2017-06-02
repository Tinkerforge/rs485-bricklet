function octave_example_loopback()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your RS485 Bricklet

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    rs485 = javaObject("com.tinkerforge.BrickletRS485", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register read callback to function cb_read
    rs232.addReadCallback(@cb_read);

    % Enable read callback
    rs485.enableReadCallback();

    % Write "test" string
    buffer_string = "test";
    buffer_char = javaArray("java.lang.String", length(buffer_string));

    for i = 1:length(buffer_string)
      buffer_char(i) = substr(buffer_string, i, 1);
    end

    rs485.write(buffer_char);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for read callback
function cb_read(e)
    fprintf("Message (Length: %d): \"%s\"\n", e.message.length, e.message);
end
