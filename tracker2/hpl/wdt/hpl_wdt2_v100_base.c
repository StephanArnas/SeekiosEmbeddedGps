/**
 * \file
 *
 * \brief SAM Watchdog
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMIT ED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#include <hri_wdt2_v100.h>
#include <hpl_wdt2_v100_config.h>
#include <hpl_wdt2_v100_base.h>
#include <hpl_wdt.h>
#include <utils_assert.h>

/**
 * \brief Macro is used to indicate the actual clock cycles of PER which
 * SAMD21/SAMR21 supports
 */
/** 8 clock cycles */
#define WDT_CLK_8CYCLE     8
/** 16 clock cycles */
#define WDT_CLK_16CYCLE    16
/** 32 clock cycles */
#define WDT_CLK_32CYCLE    32
/** 64 clock cycles */
#define WDT_CLK_64CYCLE    64
/** 128 clock cycles */
#define WDT_CLK_128CYCLE   128
/** 256 clock cycles */
#define WDT_CLK_256CYCLE   256
/** 512 clock cycles */
#define WDT_CLK_512CYCLE   512
/** 1024 clock cycles */
#define WDT_CLK_1024CYCLE  1024
/** 20488 clock cycles */
#define WDT_CLK_2048CYCLE  2048
/** 4096 clock cycles */
#define WDT_CLK_4096CYCLE  4096
/** 8192 clock cycles */
#define WDT_CLK_8192CYCLE  8192
/** 16384 clock cycles */
#define WDT_CLK_16384CYCLE 16384

/**
 * \brief Macro is used to indicate the rate of second/millisecond
 */
#define WDT_PERIOD_RATE 1000

/**
 * \brief WDT initialization function
 */
int32_t _wdt_init(struct wdt_dev *const dev)
{
	/* Sanity check arguments */
	ASSERT(dev && dev->hw);

	if(hri_wdt_get_CTRLA_ALWAYSON_bit(dev->hw) ||
			hri_wdt_get_CTRLA_ENABLE_bit(dev->hw)) {
		return ERR_DENIED;
	} else {
		if (CONF_WDT_WINDOW_EN) {
			hri_wdt_write_CONFIG_WINDOW_bf(dev->hw, CONF_WDT_WINDOW);
		} else {
			hri_wdt_clear_CTRLA_WEN_bit(dev->hw);
		}

		hri_wdt_write_CONFIG_PER_bf(dev->hw, CONF_WDT_PER);
	}

	return ERR_NONE;
}
/**
 * \brief De-initialize WDT
 */
int32_t _wdt_deinit(struct wdt_dev *const dev)
{
	/* Sanity check arguments */
	ASSERT(dev && dev->hw);

	if(hri_wdt_get_CTRLA_ALWAYSON_bit(dev->hw)) {
		return ERR_DENIED;
	} else {
		hri_wdt_clear_CTRLA_ENABLE_bit(dev->hw);
	}

	return ERR_NONE;
}

/**
 * \brief set timeout period for WDT instance
 */
int32_t _wdt_set_timeout_period(struct wdt_dev *const dev,
		const uint32_t clk_rate,
		const uint16_t timeout_period)
{
	uint64_t tmp;
	uint32_t period_cycles;
	enum wdt_period_reg timeout_period_reg;
	/* Sanity check arguments */
	ASSERT(dev && dev->hw);

	if(hri_wdt_get_CTRLA_ALWAYSON_bit(dev->hw) ||
			hri_wdt_get_CTRLA_ENABLE_bit(dev->hw)) {
		return ERR_DENIED;
	} else {
		/* calc the period cycles corresponding to timeout period */
		tmp = (uint64_t)timeout_period * clk_rate;

		/* check whether overflow*/
		if (tmp >> 32) {
			return ERR_INVALID_ARG;
		}

		period_cycles = (uint32_t)tmp;
		/* calc the register value corresponding to period cysles */
		switch (period_cycles) {
		case WDT_CLK_8CYCLE * WDT_PERIOD_RATE:
			timeout_period_reg = WDT_PERIOD_8CYCLE;
			break;
		case WDT_CLK_16CYCLE * WDT_PERIOD_RATE:
			timeout_period_reg = WDT_PERIOD_16CYCLE;
			break;
		case WDT_CLK_32CYCLE * WDT_PERIOD_RATE:
			timeout_period_reg = WDT_PERIOD_32CYCLE;
			break;
		case WDT_CLK_64CYCLE * WDT_PERIOD_RATE:
			timeout_period_reg = WDT_PERIOD_64CYCLE;
			break;
		case WDT_CLK_128CYCLE * WDT_PERIOD_RATE:
			timeout_period_reg = WDT_PERIOD_128CYCLE;
			break;
		case WDT_CLK_256CYCLE * WDT_PERIOD_RATE:
			timeout_period_reg = WDT_PERIOD_256CYCLE;
			break;
		case WDT_CLK_512CYCLE * WDT_PERIOD_RATE:
			timeout_period_reg = WDT_PERIOD_512CYCLE;
			break;
		case WDT_CLK_1024CYCLE * WDT_PERIOD_RATE:
			timeout_period_reg = WDT_PERIOD_1024CYCLE;
			break;
		case WDT_CLK_2048CYCLE * WDT_PERIOD_RATE:
			timeout_period_reg = WDT_PERIOD_2048CYCLE;
			break;
		case WDT_CLK_4096CYCLE * WDT_PERIOD_RATE:
			timeout_period_reg = WDT_PERIOD_4096CYCLE;
			break;
		case WDT_CLK_8192CYCLE * WDT_PERIOD_RATE:
			timeout_period_reg = WDT_PERIOD_8192CYCLE;
			break;
		case WDT_CLK_16384CYCLE * WDT_PERIOD_RATE:
			timeout_period_reg = WDT_PERIOD_16384CYCLE;
			break;
		default:
			return ERR_INVALID_ARG;
		}
	}

	hri_wdt_write_CONFIG_PER_bf(dev->hw, (uint8_t)timeout_period_reg);

	return ERR_NONE;
}

/**
 * \brief get timeout period for WDT instance
 */
uint32_t _wdt_get_timeout_period(const struct wdt_dev *const dev,
		const uint32_t clk_rate)
{
	uint32_t period_cycles;
	uint32_t timeout_period;
	uint8_t timeout_period_reg;
	/* Sanity check arguments */
	ASSERT(dev && dev->hw);

	/* get the register value */
	timeout_period_reg = hri_wdt_read_CONFIG_PER_bf(dev->hw);
	/* convert register value to period cycles(clock cycles) */
	switch (timeout_period_reg) {
	case WDT_PERIOD_8CYCLE:
		period_cycles = WDT_CLK_8CYCLE;
		break;
	case WDT_PERIOD_16CYCLE:
		period_cycles = WDT_CLK_16CYCLE;
		break;
	case WDT_PERIOD_32CYCLE:
		period_cycles = WDT_CLK_32CYCLE;
		break;
	case WDT_PERIOD_64CYCLE:
		period_cycles = WDT_CLK_64CYCLE;
		break;
	case WDT_PERIOD_128CYCLE:
		period_cycles = WDT_CLK_128CYCLE;
		break;
	case WDT_PERIOD_256CYCLE:
		period_cycles = WDT_CLK_256CYCLE;
		break;
	case WDT_PERIOD_512CYCLE:
		period_cycles = WDT_CLK_512CYCLE;
		break;
	case WDT_PERIOD_1024CYCLE:
		period_cycles = WDT_CLK_1024CYCLE;
		break;
	case WDT_PERIOD_2048CYCLE:
		period_cycles = WDT_CLK_2048CYCLE;
		break;
	case WDT_PERIOD_4096CYCLE:
		period_cycles = WDT_CLK_4096CYCLE;
		break;
	case WDT_PERIOD_8192CYCLE:
		period_cycles = WDT_CLK_8192CYCLE;
		break;
	case WDT_PERIOD_16384CYCLE:
		period_cycles = WDT_CLK_16384CYCLE;
		break;
	default:
		return (uint32_t)-1;
	}

	/* convert period cycles(clock cycles) to timeout period(ms) */
	timeout_period = period_cycles * 1000 / clk_rate;

	return timeout_period;
}

/**
 * \brief enbale watchdog timer
 */
int32_t _wdt_enable(struct wdt_dev *const dev)
{
	/* Sanity check arguments */
	ASSERT(dev && dev->hw);

	hri_wdt_set_CTRLA_ENABLE_bit(dev->hw);

	return ERR_NONE;
}

/**
 * \brief disable watchdog timer
 */
int32_t _wdt_disable(struct wdt_dev *const dev)
{
	/* Sanity check arguments */
	ASSERT(dev && dev->hw);

	if(hri_wdt_get_CTRLA_ALWAYSON_bit(dev->hw)) {
		return ERR_DENIED;
	} else {
		hri_wdt_clear_CTRLA_ENABLE_bit(dev->hw);
	}

	return ERR_NONE;
}

/**
 * \brief reset watchdog timer to make wdt work from start
 */
int32_t _wdt_feed(struct wdt_dev *const dev)
{
	/* Sanity check arguments */
	ASSERT(dev && dev->hw);

	hri_wdt_write_CLEAR_reg(dev->hw, WDT_CLEAR_CLEAR_KEY);

	return ERR_NONE;
}
