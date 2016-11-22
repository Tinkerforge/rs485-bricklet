/* rs485-bricklet
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * rs485.h: RS485 handling
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef RS485_H
#define RS485_H

#include "configs/config_rs485.h"

#include "bricklib2/utility/ringbuffer.h"

#include <stdint.h>

#define RS485_BUFFER_SIZE 1024*10

#define RS485_OVERSAMPLING 16
#define RS485_BAUDRATE_MIN 100
#define RS485_BAUDRATE_MAX (((48000000/1024)*1023)/RS485_OVERSAMPLING) // This is defined in xmclib

typedef enum {
	PARITY_NONE = 0,
	PARITY_ODD = 1,
	PARITY_EVEN = 2,
} RS485Parity;

typedef enum {
	STOPBITS_1 = 1,
	STOPBITS_2 = 2,
} RS485Stopbits;

typedef enum {
	WORDLENGTH_5 = 5,
	WORDLENGTH_6 = 6,
	WORDLENGTH_7 = 7,
	WORDLENGTH_8 = 8,
} RS485Wordlength;

typedef enum {
	DUPLEX_HALF = 0,
	DUPLEX_FULL = 1,
} RS485Duplex;

// TODO: Can we detect more errors? Framing error?
typedef enum {
	ERROR_OVERRUN = 1 << 0,
	ERROR_PARITY  = 1 << 1,
}RS485Error;

typedef enum {
	TODO
} RS485State;

typedef struct {
	Ringbuffer ringbuffer_tx;
	Ringbuffer ringbuffer_rx;
	uint8_t buffer[RS485_BUFFER_SIZE];
	uint16_t buffer_size_rx;

	bool read_callback_enabled;
	uint8_t error;

	uint32_t baudrate;
	RS485Parity parity;
	RS485Stopbits stopbits;
	RS485Wordlength wordlength;
	RS485Duplex duplex;

	RS485State state;
} RS485;


void rs485_init(RS485 *rs485);
void rs485_tick(RS485 *rs485);
void rs485_apply_configuration(RS485 *rs485);

#endif
