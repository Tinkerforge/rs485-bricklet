/* rs485-bricklet
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config_rs485.h: RS485 specific configurations
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

#ifndef CONFIG_RS485_H
#define CONFIG_RS485_H

#include "xmc_common.h"

#define RS485_LED_RED_PIN         P4_4
#define RS485_LED_YELLOW_PIN      P0_15

#define RS485_USIC_CHANNEL        USIC1_CH1
#define RS485_USIC                XMC_UART1_CH1

#define RS485_RX_PIN              P2_13
#define RS485_RX_INPUT            XMC_USIC_CH_INPUT_DX0
#define RS485_RX_SOURCE           0b011 // DX1D

#define RS485_TX_PIN              P2_12
#define RS485_TX_PIN_AF           (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT7 | P2_12_AF_U1C1_DOUT0)

#define RS485_NRXE_PIN            P2_11
#define RS485_TXE_PIN             P2_10

#define RS485_NRXE_AND_TXE_PORT   XMC_GPIO_PORT2
#define RS485_NRXE_PIN_NUM        11
#define RS485_TXE_PIN_NUM         10

#define RS485_SERVICE_REQUEST_RX  2  // receive
#define RS485_SERVICE_REQUEST_TX  3  // transfer
#define RS485_SERVICE_REQUEST_TFF 4  // transfer frame finished

// Make sure that the priorities are as follows
// TX > TFF > RX
// TFF should never displace TX. TFF is unnecessary if we are in TX IRQ
// RX should never displace TFF. If it does we can have a race condition between
// receiving new data and enabling RXE again)
#define RS485_IRQ_RX              11
#define RS485_IRQ_RX_PRIORITY     2
#define RS485_IRQCTRL_RX          XMC_SCU_IRQCTRL_USIC1_SR2_IRQ11

#define RS485_IRQ_TX              12
#define RS485_IRQ_TX_PRIORITY     0
#define RS485_IRQCTRL_TX          XMC_SCU_IRQCTRL_USIC1_SR3_IRQ12

#define RS485_IRQ_TFF             13
#define RS485_IRQ_TFF_PRIORITY    1
#define RS485_IRQCTRL_TFF         XMC_SCU_IRQCTRL_USIC1_SR4_IRQ13

#endif
