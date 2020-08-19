// For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
// and configure the DIP switch on the Bricklet to full-duplex mode

#include <string.h>

#include "bindings/hal_common.h"
#include "bindings/bricklet_rs485.h"

#define UID "XYZ" // Change XYZ to the UID of your RS485 Bricklet

void check(int rc, const char* msg);

char buffer[61]; // + 1 for the null terminator.

// Callback function for read callback
void read_low_level_handler(TF_RS485 *device, uint16_t message_length, uint16_t message_chunk_offset, char message_chunk_data[60], void *user_data) {
	(void)device; (void)user_data; // avoid unused parameter warning

    bool last_chunk = message_chunk_offset + 60 > message_length;

	uint16_t to_copy = 60;
	if (last_chunk) {
		//This is the last chunk, only read the valid part of the message
		to_copy = message_length - message_chunk_offset;
	}

	memcpy(buffer, message_chunk_data, to_copy);
	buffer[to_copy] = '\0';

	if(message_chunk_offset == 0) {
		tf_hal_printf("Message: \"", buffer);
	}

    tf_hal_printf("%s", buffer);

    if(last_chunk) {
		tf_hal_printf("\"\n", buffer);
	}
}


TF_RS485 rs485;

void example_setup(TF_HalContext *hal) {
	// Create device object
	check(tf_rs485_create(&rs485, UID, hal), "create device object");


	// Enable full-duplex mode
	check(tf_rs485_set_rs485_configuration(&rs485, 115200, TF_RS485_PARITY_NONE,
	                                       TF_RS485_STOPBITS_1, TF_RS485_WORDLENGTH_8,
	                                       TF_RS485_DUPLEX_FULL), "call set_rs485_configuration");

	// Register read callback to function read_low_level_handler
	tf_rs485_register_read_low_level_callback(&rs485,
	                                          read_low_level_handler,
	                                          NULL);

	// Enable read callback
	check(tf_rs485_enable_read_callback(&rs485), "call enable_read_callback");

	// Write "test" string
	char write_buffer[] = {'t', 'e', 's', 't'};
	uint16_t written;
	tf_rs485_write(&rs485, write_buffer, sizeof(write_buffer), &written);
}

void example_loop(TF_HalContext *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
