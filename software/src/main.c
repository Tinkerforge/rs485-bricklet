/* rs485-bricklet
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * main.c: Initialization for RS485 Bricklet
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

#include <stdio.h>
#include <stdbool.h>

#include "configs/config.h"

#include "xmc_gpio.h"

#include "bricklib2/bootloader/bootloader.h"
#include "communication.h"
#include "rs485.h"

RS485 rs485;

int main(void) {
	communication_init();
	rs485_init(&rs485);

	while(true) {
		rs485_tick(&rs485);
		communication_tick();
		bootloader_tick();
	}

}
