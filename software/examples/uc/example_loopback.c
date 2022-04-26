// This example is not self-contained.
// It requires usage of the example driver specific to your platform.
// See the HAL documentation.

#include "src/bindings/hal_common.h"
#include "src/bindings/bricklet_rs485.h"

// For this example connect the RX+/- pins to TX+/- pins on the same Bricklet
// and configure the DIP switch on the Bricklet to full-duplex mode

void check(int rc, const char *msg);
void example_setup(TF_HAL *hal);
void example_loop(TF_HAL *hal);

static char buffer[5] = {0}; // +1 for the null terminator

// Callback function for read callback
static void read_handler(TF_RS485 *device, char *message, uint16_t message_length,
                         void *user_data) {
	(void)device; (void)user_data; // avoid unused parameter warning

	message[message_length] = '\0';

	tf_hal_printf("Message: \"%s\"\n", message);
}

static TF_RS485 rs485;

void example_setup(TF_HAL *hal) {
	// Create device object
	check(tf_rs485_create(&rs485, NULL, hal), "create device object");

	// Enable full-duplex mode
	check(tf_rs485_set_rs485_configuration(&rs485, 115200, TF_RS485_PARITY_NONE,
	                                       TF_RS485_STOPBITS_1, TF_RS485_WORDLENGTH_8,
	                                       TF_RS485_DUPLEX_FULL), "call set_rs485_configuration");

	// Register read callback to function read_handler
	tf_rs485_register_read_callback(&rs485,
	                                read_handler,
	                                buffer,
	                                NULL);

	// Enable read callback
	check(tf_rs485_enable_read_callback(&rs485), "call enable_read_callback");

	// Write "test" string
	char write_buffer[] = {'t', 'e', 's', 't'};
	uint16_t written;
	tf_rs485_write(&rs485, write_buffer, sizeof(write_buffer), &written);
}

void example_loop(TF_HAL *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
