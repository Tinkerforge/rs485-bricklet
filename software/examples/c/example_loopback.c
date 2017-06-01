#include <stdio.h>
#include <string.h>

#include "ip_connection.h"
#include "bricklet_rs485.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change XYZ to the UID of your RS485 Bricklet

// For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
// and configure the Bricklet to be in full-duplex mode

// Callback function for read callback
void cb_read(char *message, uint16_t message_length, void *user_data) {
	// Assume that the message consists of ASCII characters and
	// convert it from an array of chars to a NUL-terminated string
	char *buffer = strndup(message, message_length);

	printf("Message (Length: %d): \"%s\"\n", message_length, buffer);

	free(buffer);
}

int main(void) {
	// Create IP connection
	IPConnection ipcon;
	ipcon_create(&ipcon);

	// Create device object
	RS485 rs485;
	rs485_create(&rs485, UID, &ipcon);

	// Connect to brickd
	if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		fprintf(stderr, "Could not connect\n");
		return 1;
	}
	// Don't use device before ipcon is connected

	// Register read callback to function cb_read
	rs485_register_callback(&rs485,
	                        RS485_CALLBACK_READ,
	                        (void *)cb_read,
	                        NULL);

	// Enable read callback
	rs485_enable_read_callback(&rs485);

	// Write "test" string
	uint16_t written;
	char buffer[4] = "test";

	rs485_write(&rs485, buffer, sizeof(buffer), &written);

	printf("Press key to exit\n");
	getchar();
	rs485_destroy(&rs485);
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
	return 0;
}
