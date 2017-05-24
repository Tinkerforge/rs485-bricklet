var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change XYZ to the UID of your RS485 Bricklet

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var rs485 = new Tinkerforge.BrickletRS485(UID, ipcon); // Create device object

// For this example connect the RX+/- pins to TX+/- pins on the same bricklet
// and configure the Bricklet to be in full-duplex mode

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Register read callback
        rs485.on(Tinkerforge.BrickletRS485.CALLBACK_READ,
            function (message) {
                console.log('Message (Length: ' + message.length + '): ' + message.join(''));
            }
        );

        // Enable read callback
        rs485.enableReadCallback();

        // Write "test" string
        rs485.write('test'.split(''));
    }
);

console.log('Press key to exit');
process.stdin.on('data',

    function (data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
