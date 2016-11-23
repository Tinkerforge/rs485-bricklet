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
#include "bricklib2/hal/uartbb/uartbb.h"
#include "communication.h"
#include "rs485.h"

RS485 rs485;

int main(void) {
	uartbb_init();
	uartbb_puts("Start RS485 Bricklet\n\r");
	uartbb_puti(SystemCoreClock);
	uartbb_putnl();

	uint32_t *unique_chip_id = (uint32_t*)0x10000FF0;

	for(uint8_t i = 0; i < 4; i++) {
		uartbb_puts("uid ");
		uartbb_puti(i);
		uartbb_puts(": ");
		uartbb_puti(unique_chip_id[i]);
		uartbb_putnl();
	}

	communication_init();
	rs485_init(&rs485);

	while(true) {
		rs485_tick(&rs485);
		communication_tick();
		bootloader_tick();
	}

}
