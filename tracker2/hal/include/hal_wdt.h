/**
 * \file
 *
 * \brief Generic WDT functionality declaration.
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
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _HAL_WDT_H_INCLUDED
#define _HAL_WDT_H_INCLUDED

#include "hpl_wdt.h"
#include <utils_assert.h>

#ifdef __cplusplus
	extern "C" {
#endif

struct wdt_descriptor;

/** \brief WDT HAL driver struct
 *
 */
struct wdt_descriptor {
	struct wdt_dev dev;
};

/**
 * \brief Initialize the WDT HAL instance and hardware
 *
 * Initialize WDT HAL.
 *
 * \param[out] wdt The pointer to the HAL WDT instance.
 * \param[in]  hw The pointer to the hardware instance.
 * \return Operation status of init
 * \retval 0 Completed sucessfully.
 * \retval -1 Always on or enabled, needn't init again.
 */
static inline int32_t wdt_init(struct wdt_descriptor *const wdt, const void *hw)
{
	ASSERT(wdt && hw);

	wdt->dev.hw = (void *)hw;

	return _wdt_init(&wdt->dev);
}

/**
 * \brief Deinitialize the WDT HAL instance and hardware
 *
 * Deinitialize WDT HAL.
 *
 * \param[in] wdt The pointer to the HAL WDT instance.
 * \return Operation status of init
 * \retval 0 Completed sucessfully.
 * \retval -1 Always on, can't deinitialize.
 */
static inline int32_t wdt_deinit(struct wdt_descriptor *const wdt)
{
	ASSERT(wdt);

	return _wdt_deinit(&wdt->dev);
}
/**
 * \brief Config the timeout period for wdt HAL instance and hardware
 *
 * Set the timeout period to wdt instance.
 *
 * \param[in] wdt The pointer to the HAL WDT instance.
 * \param[in] clk_rate The current clock rate of generic clock(GCLK_WDT) in HZ
 * \param[in] timeout_period The desired timeout period(ms).
 * \return Operation status of init
 * \retval 0 Completed sucessfully.
 * \retval -1 Always on or enabled, can't set again.
 * \retval -2 invalid timeout period
 */
static inline int32_t wdt_set_timeout_period(struct wdt_descriptor *const wdt,
		const uint32_t clk_rate,
		const uint16_t timeout_period)
{
	ASSERT(wdt && wdt->dev.hw);

	return _wdt_set_timeout_period(&wdt->dev, clk_rate, timeout_period);
}

/**
 * \brief Get the timeout period for wdt HAL instance and hardware
 *
 * \param[in] wdt The pointer to the HAL WDT instance.
 * \param[in] clk_rate The current clock rate of generic clock(GCLK_WDT) in HZ
 * \return current timeout period(ms)
 * \retval -1 invalid timeout period
 */
static inline uint32_t wdt_get_timeout_period(struct wdt_descriptor *const wdt,
		const uint32_t clk_rate)
{
	ASSERT(wdt && wdt->dev.hw);

	return _wdt_get_timeout_period(&wdt->dev, clk_rate);
}

/**
 * \brief enbale watchdog timer
 *
 * \param[in] wdt The pointer to the HAL WDT instance.
 * \return Operation status of init
 * \retval 0  Completed sucessfully.
 */
static inline int32_t wdt_enable(struct wdt_descriptor *const wdt)
{
	ASSERT(wdt && wdt->dev.hw);

	return _wdt_enable(&wdt->dev);
}

/**
 * \brief disable watchdog timer
 *
 * \param[in] wdt The pointer to the HAL WDT instance.
 * \return Operation status of init
 * \retval 0  Completed sucessfully.
 * \retval -1 Always on, can't disable.
 */
static inline int32_t wdt_disable(struct wdt_descriptor *const wdt)
{
	ASSERT(wdt && wdt->dev.hw);

	return _wdt_disable(&wdt->dev);
}

/**
 * \brief feed watchdog timer to make wdt kick from start
 *
 * \param[in] wdt The pointer to the HAL WDT instance.
 * \return Operation status of init
 * \retval 0 Completed sucessfully.
 */
static inline int32_t wdt_feed(struct wdt_descriptor *const wdt)
{
	ASSERT(wdt && wdt->dev.hw);

	return _wdt_feed(&wdt->dev);
}

/**
 * \brief Retrieve the current driver version
 *
 * \return Current driver version
 */
uint32_t wdt_get_version(void);


#ifdef __cplusplus
	}
#endif

#endif /* _HAL_WDT_H_INCLUDED */
