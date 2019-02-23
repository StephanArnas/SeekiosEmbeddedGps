/**
 * \file
 *
 * \brief WDT related functionality declaration.
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

#ifndef _HPL_WDT_H_INCLUDED
#define _HPL_WDT_H_INCLUDED

/**
 * \addtogroup HPL WDT
 *
 * \section hpl_wdt_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <compiler.h>

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * \brief WDT device structure
 *
 * The WDT device structure
 */
struct wdt_dev {
	void *hw;
};

/**
 * \name HPL functions
 */

/**
 * \brief WDT initialization function
 *
 * \param[in] dev The pointer to hardware instance
 * \return Initialization status.
 * \retval 0  The initialization is completed successfully
 * \retval -1 Always on or enabled
 */
int32_t _wdt_init(struct wdt_dev *const dev);

/**
 * \brief De-initialize WDT
 *
 * \param[in] dev The pointer to hardware instance
 * \return operation status.
 * \retval 0  Operation is completed successfully
 * \retval -1 Always on
 */
int32_t _wdt_deinit(struct wdt_dev *const dev);

/**
 * \brief set timeout period for WDT instance
 *
 * \param[in] dev The pointer to hardware instance
 * \param[in] clk_rate The current clock rate of generic clock(GCLK_WDT) in HZ
 * \param[in] timeout_period The desired timeout period(ms)
 * \return operation status.
 * \retval 0 Operation is completed successfully
 * \retval -1 Always on or enabled
 * \retval -2 invalid timeout period
 */
int32_t _wdt_set_timeout_period(struct wdt_dev *const dev,
		const uint32_t clk_rate,
		const uint16_t timeout_period);

/**
 * \brief get timeout period for WDT instance
 *
 * \param[in] dev The pointer to hardware instance
 * \param[in] clk_rate The current clock rate of generic clock(GCLK_WDT) in HZ
 * \return current timeout period(ms)
 * \retval -1 invalid timeout period
 */
uint32_t _wdt_get_timeout_period(const struct wdt_dev *const dev,
		const uint32_t clk_rate);

/**
 * \brief enbale watchdog timer
 *
 * \param[in] dev The pointer to hardware instance
 * \return operation status.
 * \retval 0 Operation is completed successfully
 * \retval -1 Always on
 */
int32_t _wdt_enable(struct wdt_dev *const dev);

/**
 * \brief disable watchdog timer
 *
 * \param[in] dev The pointer to hardware instance
 * \return operation status.
 * \retval 0  Operation is completed successfully
 * \retval -1 Always on
 */
int32_t _wdt_disable(struct wdt_dev *const dev);

/**
 * \brief reset watchdog timer to make wdt work from start
 *
 * \param[in] dev The pointer to hardware instance
 * \return operation status.
 * \retval 0  Operation is completed successfully
 */
int32_t _wdt_feed(struct wdt_dev *const dev);

#ifdef __cplusplus
	}
#endif
/**@}*/
#endif /* _HPL_WDT_H_INCLUDED */
