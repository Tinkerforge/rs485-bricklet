/* rs485-bricklet
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 * Copyright (C) 2017 Ishraq Ibne Ashraf <ishraq@tinkerforge.com>
 *
 * rs485.c: RS485 handling
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

#include "rs485.h"
#include "modbus.h"

#include "communication.h"

#include "configs/config.h"
#include "bricklib2/hal/system_timer/system_timer.h"
#include "bricklib2/utility/ringbuffer.h"
#include "timer.h"

#include "xmc_uart.h"
#include "xmc_scu.h"

extern RS485 rs485;

#define rs485_rx_irq_handler  IRQ_Hdlr_11
#define rs485_tx_irq_handler  IRQ_Hdlr_12
#define rs485_tff_irq_handler IRQ_Hdlr_13
#define rs485_rxa_irq_handler IRQ_Hdlr_14

#define RS485_HALF_DUPLEX_RX_ENABLE() \
	(RS485_NRXE_AND_TXE_PORT->OMR = ((0x10000 << RS485_TXE_PIN_NUM) | (0x10000 << RS485_NRXE_PIN_NUM)))

// Use duplex_shift_setting to not turn rx off in full-duplex
#define RS485_HALF_DUPLEX_TX_ENABLE() \
	(RS485_NRXE_AND_TXE_PORT->OMR = ((0x1 << RS485_TXE_PIN_NUM) | (rs485.duplex_shift_setting << RS485_NRXE_PIN_NUM)))

void __attribute__((optimize("-O3"))) rs485_tff_irq_handler(void) {
	if((RS485_USIC->PSR_ASCMode & (XMC_UART_CH_STATUS_FLAG_TRANSMITTER_FRAME_FINISHED | XMC_UART_CH_STATUS_FLAG_TRANSFER_STATUS_BUSY)) ==
		 XMC_UART_CH_STATUS_FLAG_TRANSMITTER_FRAME_FINISHED) {
			 RS485_HALF_DUPLEX_RX_ENABLE();
	}
}

void __attribute__((optimize("-O3"))) __attribute__ ((section (".ram_code"))) rs485_rx_irq_handler(void) {
	while(!XMC_USIC_CH_RXFIFO_IsEmpty(RS485_USIC)) {
		/*
		 * Instead of ringbuffer_add() we add the byte to the buffer
		 * by hand.
		 *
		 * We need to save the low watermark calculation overhead.
		 */

		uint16_t new_end = rs485.ringbuffer_rx.end + 1;

		if(new_end >= rs485.ringbuffer_rx.size) {
			new_end = 0;
		}

		if(new_end == rs485.ringbuffer_rx.start) {
			rs485.error_count_overrun++;

			if(rs485.red_led_state.config == LED_FLICKER_CONFIG_EXTERNAL) {
				XMC_GPIO_SetOutputLow(RS485_LED_RED_PIN);
			}

			// In the case of an overrun we read the byte and throw it away.
			volatile uint8_t __attribute__((unused)) _  = RS485_USIC->OUTR;
		}
		else {
			rs485.ringbuffer_rx.buffer[rs485.ringbuffer_rx.end] = RS485_USIC->OUTR;
			rs485.ringbuffer_rx.end = new_end;
		}
	}

	TIMER_RESET();
}


void __attribute__((optimize("-O3"))) rs485_rxa_irq_handler(void) {
	/*
	 * We get alternate rx interrupt if there is a parity error. In this case we
	 * still read the byte and give it to the user.
	 */
	rs485.error_count_parity++;

	if(rs485.red_led_state.config == LED_FLICKER_CONFIG_EXTERNAL) {
		XMC_GPIO_SetOutputLow(RS485_LED_RED_PIN);
	}

	rs485_rx_irq_handler();
}

void __attribute__((optimize("-O3"))) __attribute__ ((section (".ram_code"))) rs485_tx_irq_handler(void) {
	uint8_t data;

	RS485_HALF_DUPLEX_TX_ENABLE();

	while(!XMC_USIC_CH_TXFIFO_IsFull(RS485_USIC)) {
		// TX FIFO is not full, more data can be loaded on the FIFO from the ring buffer.

		if(!ringbuffer_get(&rs485.ringbuffer_tx, &data)) {
			// No more data to TX from ringbuffer, disable TX interrupt.
			XMC_USIC_CH_TXFIFO_DisableEvent(RS485_USIC, XMC_USIC_CH_TXFIFO_EVENT_CONF_STANDARD);

			rs485.modbus_rtu.tx_done = true;

			return;
		}

		RS485_USIC->IN[0] = data;
	}
}

void rs485_init_hardware(RS485 *rs485) {
	// TX pin configuration
	const XMC_GPIO_CONFIG_t tx_pin_config = {
		.mode             = RS485_TX_PIN_AF,
		.output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH
	};

	// RX pin configuration
	const XMC_GPIO_CONFIG_t rx_pin_config = {
		.mode             = XMC_GPIO_MODE_INPUT_PULL_UP,
		.input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_STANDARD
	};

	// TXE (tx enable) pin configuration
	const XMC_GPIO_CONFIG_t txe_pin_config = {
		.mode             = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
		.output_level     = XMC_GPIO_OUTPUT_LEVEL_LOW
	};

	// NRXE (not rx enable) pin configuration
	const XMC_GPIO_CONFIG_t nrxe_pin_config = {
		.mode             = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
		.output_level     = XMC_GPIO_OUTPUT_LEVEL_LOW
	};

	// Configure  pins
	XMC_GPIO_Init(RS485_TX_PIN, &tx_pin_config);
	XMC_GPIO_Init(RS485_RX_PIN, &rx_pin_config);
	XMC_GPIO_Init(RS485_TXE_PIN, &txe_pin_config);
	XMC_GPIO_Init(RS485_NRXE_PIN, &nrxe_pin_config);

	// Initialize USIC channel in UART master mode
	// USIC channel configuration
	XMC_UART_CH_CONFIG_t config;
	config.oversampling = RS485_OVERSAMPLING;
	config.frame_length = 8; //rs485->wordlength + (rs485->parity == PARITY_NONE ? 0 : 1); // TODO: Should this be wordlength?
	config.baudrate     = rs485->baudrate;
	config.stop_bits    = rs485->stopbits;
	config.data_bits    = rs485->wordlength;
	switch(rs485->parity) {
		case PARITY_NONE: config.parity_mode = XMC_USIC_CH_PARITY_MODE_NONE; break;
		case PARITY_EVEN: config.parity_mode = XMC_USIC_CH_PARITY_MODE_EVEN; break;
		case PARITY_ODD:  config.parity_mode = XMC_USIC_CH_PARITY_MODE_ODD;  break;
	}
	XMC_UART_CH_Init(RS485_USIC, &config);

	// Set input source path
	XMC_UART_CH_SetInputSource(RS485_USIC, RS485_RX_INPUT, RS485_RX_SOURCE);

	// Configure transmit FIFO
	XMC_USIC_CH_TXFIFO_Configure(RS485_USIC, 32, XMC_USIC_CH_FIFO_SIZE_32WORDS, 16);

	// Configure receive FIFO
	XMC_USIC_CH_RXFIFO_Configure(RS485_USIC, 0, XMC_USIC_CH_FIFO_SIZE_32WORDS, 0);

	// UART protocol events
	XMC_USIC_CH_SetInterruptNodePointer(RS485_USIC, XMC_USIC_CH_INTERRUPT_NODE_POINTER_PROTOCOL, RS485_SERVICE_REQUEST_TFF);
	XMC_USIC_CH_SetInterruptNodePointer(RS485_USIC, XMC_USIC_CH_INTERRUPT_NODE_POINTER_ALTERNATE_RECEIVE, RS485_SERVICE_REQUEST_RXA);
	XMC_UART_CH_EnableEvent(RS485_USIC, XMC_UART_CH_EVENT_FRAME_FINISHED);

	// Set service request for tx FIFO transmit interrupt
	XMC_USIC_CH_TXFIFO_SetInterruptNodePointer(RS485_USIC, XMC_USIC_CH_TXFIFO_INTERRUPT_NODE_POINTER_STANDARD, RS485_SERVICE_REQUEST_TX);

	// Set service request for rx FIFO receive interrupt
	XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(RS485_USIC, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_STANDARD, RS485_SERVICE_REQUEST_RX);
	XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(RS485_USIC, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_ALTERNATE, RS485_SERVICE_REQUEST_RX);

	// Set priority and enable NVIC node for transfer frame finished interrupt
	NVIC_SetPriority((IRQn_Type)RS485_IRQ_TFF, RS485_IRQ_TFF_PRIORITY);
	XMC_SCU_SetInterruptControl(RS485_IRQ_TFF, RS485_IRQCTRL_TFF);
	NVIC_EnableIRQ((IRQn_Type)RS485_IRQ_TFF);

	// Set priority and enable NVIC node for transmit interrupt
	NVIC_SetPriority((IRQn_Type)RS485_IRQ_TX, RS485_IRQ_TX_PRIORITY);
	XMC_SCU_SetInterruptControl(RS485_IRQ_TX, RS485_IRQCTRL_TX);
	NVIC_EnableIRQ((IRQn_Type)RS485_IRQ_TX);

	// Set priority and enable NVIC node for receive interrupt
	NVIC_SetPriority((IRQn_Type)RS485_IRQ_RX, RS485_IRQ_RX_PRIORITY);
	XMC_SCU_SetInterruptControl(RS485_IRQ_RX, RS485_IRQCTRL_RX);
	NVIC_EnableIRQ((IRQn_Type)RS485_IRQ_RX);

	// Set priority and enable NVIC node for receive interrupt
	NVIC_SetPriority((IRQn_Type)RS485_IRQ_RXA, RS485_IRQ_RXA_PRIORITY);
	XMC_SCU_SetInterruptControl(RS485_IRQ_RXA, RS485_IRQCTRL_RXA);
	NVIC_EnableIRQ((IRQn_Type)RS485_IRQ_RXA);

	// Start UART
	XMC_UART_CH_Start(RS485_USIC);

	XMC_USIC_CH_EnableEvent(RS485_USIC, XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);
	XMC_USIC_CH_RXFIFO_EnableEvent(RS485_USIC, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);
}

void rs485_init_buffer(RS485 *rs485) {
	// Disable interrupts so we can't accidentally
	// receive ringbuffer_adds in between a re-init
	NVIC_DisableIRQ((IRQn_Type)RS485_IRQ_TFF);
	NVIC_DisableIRQ((IRQn_Type)RS485_IRQ_TX);
	NVIC_DisableIRQ((IRQn_Type)RS485_IRQ_RX);
	NVIC_DisableIRQ((IRQn_Type)RS485_IRQ_RXA);
	__DSB();
	__ISB();

	// Initialize rs485 buffer
	memset(rs485->buffer, 0, RS485_BUFFER_SIZE);

	// rx buffer is at buffer[0:buffer_size_rx]
	ringbuffer_init(&rs485->ringbuffer_rx, rs485->buffer_size_rx, &rs485->buffer[0]);
	// tx buffer is at buffer[buffer_size_rx:RS485_BUFFER_SIZE]
	ringbuffer_init(&rs485->ringbuffer_tx, RS485_BUFFER_SIZE-rs485->buffer_size_rx, &rs485->buffer[rs485->buffer_size_rx]);

	NVIC_EnableIRQ((IRQn_Type)RS485_IRQ_TFF);
	NVIC_EnableIRQ((IRQn_Type)RS485_IRQ_TX);
	NVIC_EnableIRQ((IRQn_Type)RS485_IRQ_RX);
	NVIC_EnableIRQ((IRQn_Type)RS485_IRQ_RXA);
}

void rs485_init(RS485 *rs485) {
	// Default config is RS485 mode, 115200 baud, 8N1, half-duplex.
	rs485->mode                         = MODE_RS485;
	rs485->baudrate                     = 115200;
	rs485->parity                       = PARITY_NONE;
	rs485->wordlength                   = WORDLENGTH_8;
	rs485->stopbits                     = STOPBITS_1;
	rs485->duplex                       = DUPLEX_HALF;
	rs485->duplex_shift_setting         = 0x1;
	rs485->error_count_overrun          = 0;
	rs485->error_count_parity           = 0;
	rs485->error_count_parity           = 0;
	rs485->read_callback_enabled        = false;
	rs485->error_count_callback_enabled = false;

	// Modbus specific.
	rs485->modbus_slave_address          = MODBUS_DEFAULT_SLAVE_ADDRESS;
	rs485->modbus_master_request_timeout = MODBUS_DEFAULT_MASTER_REQUEST_TIMEOUT;

	// By default we use a 50/50 segmentation of rx/tx buffer
	rs485->buffer_size_rx = RS485_BUFFER_SIZE/2;

	rs485->yellow_led_state.config  = COMMUNICATION_LED_CONFIG_SHOW_COMMUNICATION;
	rs485->yellow_led_state.counter = 0;
	rs485->yellow_led_state.start   = 0;
	rs485->red_led_state.config     = LED_FLICKER_CONFIG_EXTERNAL;
	rs485->red_led_state.counter    = 0;
	rs485->red_led_state.start      = 0;

	// LED configuration
	XMC_GPIO_CONFIG_t led_pin_config = {
		.mode             = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
		.output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH
	};

	XMC_GPIO_Init(RS485_LED_RED_PIN, &led_pin_config);

	led_pin_config.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW; // Default on for yellow LED
	XMC_GPIO_Init(RS485_LED_YELLOW_PIN, &led_pin_config);

	rs485_init_buffer(rs485);
	rs485_init_hardware(rs485);

	if(rs485->mode == MODE_MODBUS_SLAVE_RTU || rs485->mode == MODE_MODBUS_MASTER_RTU) {
		modbus_init(rs485);
	}
}

void rs485_apply_configuration(RS485 *rs485) {
	// We start by turning off all events.
	XMC_USIC_CH_RXFIFO_DisableEvent(RS485_USIC, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);
	XMC_UART_CH_DisableEvent(RS485_USIC, XMC_UART_CH_EVENT_FRAME_FINISHED);
	XMC_USIC_CH_DisableEvent(RS485_USIC, XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE);

	// Then turn off the USIC, but wait until the tx buffer is empty.
	while(XMC_UART_CH_Stop(RS485_USIC) != XMC_UART_CH_STATUS_OK);

	if(rs485->duplex == DUPLEX_HALF) {
		rs485->duplex_shift_setting = 0x1;
	}
	else {
		rs485->duplex_shift_setting = 0x10000;
	}

	rs485_init_buffer(rs485);
	// Then we can safely reconfigure the hardware.
	rs485_init_hardware(rs485);

	if(rs485->mode == MODE_MODBUS_SLAVE_RTU || rs485->mode == MODE_MODBUS_MASTER_RTU) {
		modbus_init(rs485);
	}
}

void rs485_tick(RS485 *rs485) {
	static uint32_t last_rx_count = 0;
	static uint32_t last_tx_count = 0;

	if(rs485->mode == MODE_MODBUS_SLAVE_RTU || rs485->mode == MODE_MODBUS_MASTER_RTU) {
		modbus_update_rtu_wire_state_machine(rs485);
	}

	/*
	 * To get a nice communication LED flickering we increase the flicker counter
	 * every time something in the rx or tx buffer changes.
	 */
	uint32_t rx_count = ringbuffer_get_used(&rs485->ringbuffer_rx);
	uint32_t tx_count = ringbuffer_get_used(&rs485->ringbuffer_tx);

	if(rx_count != last_rx_count) {
		last_rx_count = rx_count;
		led_flicker_increase_counter(&rs485->yellow_led_state);
	}

	if(tx_count != last_tx_count) {
		last_tx_count = tx_count;
		led_flicker_increase_counter(&rs485->yellow_led_state);
	}

	led_flicker_tick(&rs485->yellow_led_state, system_timer_get_ms(), RS485_LED_YELLOW_PIN);
	led_flicker_tick(&rs485->red_led_state,    system_timer_get_ms(), RS485_LED_RED_PIN);
}
