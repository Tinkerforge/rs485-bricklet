var Tinkerforge = require('tinkerforge');

// For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
// and configure the DIP switch on the Bricklet to full-duplex mode

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change XYZ to the UID of your RS485 Bricklet

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var rs485 = new Tinkerforge.BrickletRS485(UID, ipcon); // Create device object

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Enable read callback
        rs485.enableReadCallback();

        // Write "test" string
        rs485.write('test'.split(''));
    }
);

// Register read callback
rs485.on(Tinkerforge.BrickletRS485.CALLBACK_READ,
    // Callback function for read callback
    function (message) {
        // Assume that the message consists of ASCII characters and
        // convert it from an array of chars to a string
        console.log('Message: "' + message.join('') + '"');
    }
);

console.log('Press key to exit');
process.stdin.on('data',
    function (data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
