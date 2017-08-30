/* rs485-bricklet
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config.h: All configurations for RS485 Bricklet
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

#ifndef CONFIG_GENERAL_H
#define CONFIG_GENERAL_H

#include "xmc_device.h"

#define CRC16_USE_MODBUS

#define STARTUP_SYSTEM_INIT_ALREADY_DONE

#define UARTBB_TX_PIN  P4_6

#define FIRMWARE_VERSION_MAJOR 2
#define FIRMWARE_VERSION_MINOR 1
#define FIRMWARE_VERSION_REVISION 0

#define SYSTEM_TIMER_FREQUENCY 1000 // Use 1 kHz system timer

#include "config_custom_bootloader.h"
#include "config_rs485.h"

#endif
