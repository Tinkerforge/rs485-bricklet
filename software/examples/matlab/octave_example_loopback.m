function octave_example_loopback()
    more off;

    % For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
    % and configure the DIP switch on the Bricklet to full-duplex mode

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your RS485 Bricklet

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    rs485 = javaObject("com.tinkerforge.BrickletRS485", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register read callback to function cb_read
    rs485.addReadCallback(@cb_read);

    % Enable read callback
    rs485.enableReadCallback();

    % Write "test" string
    rs485.write(string2chars("test"));

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for read callback
function cb_read(e)
    fprintf("Message: \"%s\"\n", chars2string(e.message));
end

% Convert string to array of chars as needed by write
function chars = string2chars(string)
    chars = javaArray("java.lang.String", length(string));

    for i = 1:length(string)
        chars(i) = substr(string, i, 1);
    end
end

% Assume that the message consists of ASCII characters and
% convert it from an array of chars to a string
function string = chars2string(chars)
    string = "";

    for i = 1:length(chars)
        string = strcat(string, chars(i));
    end
end
