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
        // Set operating mode
        rs485.setMode(Tinkerforge.BrickletRS485.MODE_MODBUS_SLAVE_RTU);

        /*
         * Modbus specific configuration
         *
         * Slave address = 1 (Unused in master mode)
         * Request timeout = 1000ms (Unused in slave mode)
         * rs485.set_modbus_configuration(1, 1000)
         */
        rs485.setModbusConfiguration(1, 1000);

        // Register write single register request callback
        rs485.on(Tinkerforge.BrickletRS485.CALLBACK_MODBUS_SLAVE_WRITE_SINGLE_REGISTER_REQUEST,
            function (requestID, registerAddress, registerValue) {
                console.log('Request ID = ' + requestID);
                console.log('Register Address = ' + registerAddress);
                console.log('Register Value = ' + registerValue);

                // Here we assume valid writable register address is 42
                if (registerAddress != 42) {
                    console.log('Requested invalid register address');
                    rs485.modbusSlaveReportException(requestID, Tinkerforge.BrickletRS485.EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
                }
                else {
                    console.log('Request OK');
                    rs485.modbusSlaveAnswerWriteSingleRegisterRequest(requestID);
                }
            }
        );
    }
);

console.log('Press key to exit');
process.stdin.on('data',
    function (data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
