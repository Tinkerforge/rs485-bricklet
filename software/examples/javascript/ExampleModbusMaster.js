var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change XYZ to the UID of your RS485 Bricklet

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var rs485 = new Tinkerforge.BrickletRS485(UID, ipcon); // Create device object

var expectedRequestID = 0;

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Set operating mode to Modbus RTU master
        rs485.setMode(Tinkerforge.BrickletRS485.MODE_MODBUS_MASTER_RTU);

        // Modbus specific configuration:
        // - slave address = 1 (unused in master mode)
        // - master request timeout = 1000ms
        rs485.setModbusConfiguration(1, 1000);

        // Write 65535 to register 42 of slave 17
        rs485.modbusMasterWriteSingleRegister(17, 42, 65535,
            function (requestID) {
                expectedRequestID = requestID;
            }
        );
    }
);

// Register Modbus master write single register response callback
rs485.on(Tinkerforge.BrickletRS485.CALLBACK_MODBUS_MASTER_WRITE_SINGLE_REGISTER_RESPONSE,
    // Callback function for Modbus master write single register response callback
    function (requestID, exceptionCode) {
        console.log('Request ID: ' + requestID);
        console.log('Exception Code: ' + exceptionCode);

        if (requestID != expectedRequestID) {
            console.log('Error: Unexpected request ID');
        }
    }
);

console.log('Press key to exit');
process.stdin.on('data',
    function (data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
