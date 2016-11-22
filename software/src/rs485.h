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

typedef enum {
	TODO
} RS485State;

typedef struct {
	Ringbuffer ringbuffer_tx;
	Ringbuffer ringbuffer_rx;
	uint8_t buffer[RS485_BUFFER_SIZE];
	uint16_t buffer_size_rx;

	RS485State state;
} RS485;


void rs485_init(RS485 *rs485);
void rs485_tick(RS485 *rs485);
void rs485_apply_configuration(RS485 *rs485);

#endif
