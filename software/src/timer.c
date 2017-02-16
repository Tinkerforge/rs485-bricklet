/* rs485-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * timer.c: Timer handling
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

#include "timer.h"
#include "rs485.h"

#include "xmc_ccu4.h"
#include "xmc_eru.h"

#include "bricklib2/hal/uartbb/uartbb.h"

bool timer_us_elapsed_since_last_timer_reset(const uint32_t us) {
	if((CCU40_CC41->TCST & CCU4_CC4_TCST_TRB_Msk) == 0) {
		return true;
	}

	return XMC_CCU4_SLICE_GetTimerValue(CCU40_CC41) >= us/10;
}

void timer_init(void) {
	XMC_CCU4_SLICE_COMPARE_CONFIG_t timer0_config = {
		.timer_mode          = XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
		.monoshot            = XMC_CCU4_SLICE_TIMER_REPEAT_MODE_REPEAT,
		.shadow_xfer_clear   = false,
		.dither_timer_period = false,
		.dither_duty_cycle   = false,
		.prescaler_mode      = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,
		.mcm_enable          = false,
		.prescaler_initval   = XMC_CCU4_SLICE_PRESCALER_2, // Use prescaler 2 to get mclk = fccu4
		.float_limit         = 0U,
		.dither_limit        = 0U,
		.passive_level       = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW,
		.timer_concatenation = false
	};

	XMC_CCU4_SLICE_COMPARE_CONFIG_t timer1_config = {
		.timer_mode          = XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
		.monoshot            = XMC_CCU4_SLICE_TIMER_REPEAT_MODE_SINGLE,
		.shadow_xfer_clear   = false,
		.dither_timer_period = false,
		.dither_duty_cycle   = false,
		.prescaler_mode      = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,
		.mcm_enable          = false,
		.prescaler_initval   = XMC_CCU4_SLICE_PRESCALER_2,
		.float_limit         = 0U,
		.dither_limit        = 0U,
		.passive_level       = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW,
		.timer_concatenation = true
	};

	// Initialize CCU4
    XMC_CCU4_Init(CCU40, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
    XMC_CCU4_StartPrescaler(CCU40);

    // Slice 0: Count from 0 to 4800 (100us)
    XMC_CCU4_EnableClock(CCU40, 0);
    XMC_CCU4_SLICE_CompareInit(CCU40_CC40, &timer0_config);
    XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC40, 480);
    XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC40, 0);
    XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_0 | XMC_CCU4_SHADOW_TRANSFER_PRESCALER_SLICE_0);

    // Slice 1: Concatenate with Slice 0, count for every 100us (10000 counts per seconds)
    XMC_CCU4_EnableClock(CCU40, 1);
    XMC_CCU4_SLICE_CompareInit(CCU40_CC41, &timer1_config);
    XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC41, 0xFFFF);
    XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC41, 0);

#if 0
    XMC_CCU4_SLICE_EVENT_CONFIG_t config;

    config.duration = XMC_CCU4_SLICE_EVENT_FILTER_5_CYCLES;
    config.edge = XMC_CCU4_SLICE_EVENT_EDGE_SENSITIVITY_DUAL_EDGE;
    config.level = XMC_CCU4_SLICE_EVENT_LEVEL_SENSITIVITY_ACTIVE_HIGH ; /* Not needed */
    config.mapped_input = XMC_CCU4_SLICE_INPUT_AA;

    XMC_CCU4_SLICE_ConfigureEvent(CCU40_CC41, XMC_CCU4_SLICE_EVENT_0, &config);
    XMC_CCU4_SLICE_StartConfig(CCU40_CC41, XMC_CCU4_SLICE_EVENT_0, XMC_CCU4_SLICE_START_MODE_TIMER_START_CLEAR);
    XMC_CCU4_SLICE_EnableEvent(CCU40_CC41, XMC_CCU4_SLICE_IRQ_ID_EVENT0);
    XMC_CCU4_SLICE_SetInterruptNode(CCU40_CC41, XMC_CCU4_SLICE_IRQ_ID_EVENT0, XMC_CCU4_SLICE_SR_ID_0);
    NVIC_SetPriority(/*CCU40_1_IRQn*/ 22, 2);
    NVIC_EnableIRQ(22);
#endif

    XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_1 | XMC_CCU4_SHADOW_TRANSFER_PRESCALER_SLICE_1);

    // Start
    XMC_CCU4_SLICE_StartTimer(CCU40_CC40);
    XMC_CCU4_SLICE_StartTimer(CCU40_CC41);
}
