/**
 * \file
 *
 * \brief SAM Peripheral Access Controller
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
#include <hri_pac2_v110a.h>

#include <utils_assert.h>
#include <hpl_pac.h>

static uint32_t _pac_get_peripheral_id(const void *const module)
{
	uint32_t peripheral = 10;

	if (((uint32_t)module & (uint32_t)HPB1_ADDR ) == (uint32_t)HPB1_ADDR) {
		peripheral = 13;
	}

	peripheral = (((uint32_t)module & 0x0F000000 ) >> 24 ) * 32 +
			(((uint32_t)module & 0x0000ff00 ) >> peripheral );

	return peripheral;
}
/**
 * \brief Enable write protect for the given hardware module
 */
int32_t _periph_lock(const void *const module)
{
	ASSERT((((uint32_t)module ) > (uint32_t)HPB0_ADDR ));

	uint32_t peripheral;
	int32_t timeout = 1000;
	bool stat;

	peripheral = _pac_get_peripheral_id(module);

	hri_pac_write_WRCTRL_reg(PAC, PAC_WRCTRL_PERID(
				peripheral) | PAC_WRCTRL_KEY_SET);

	do {
		_periph_get_lock_state(module, &stat);
	} while (!stat && timeout--);

	if (timeout < 0) {
		return ERR_TIMEOUT;
	}

	return ERR_NONE;
}

/**
 * \brief Disable write protect for the given hardware module
 */
int32_t _periph_unlock(const void *const module)
{
	ASSERT((((uint32_t)module ) > (uint32_t)HPB0_ADDR ));

	uint32_t peripheral;
	int32_t timeout = 1000;
	bool stat;

	peripheral = _pac_get_peripheral_id(module);

	hri_pac_write_WRCTRL_reg(PAC, PAC_WRCTRL_PERID(
				peripheral) | PAC_WRCTRL_KEY_CLR);

	do {
		_periph_get_lock_state(module, &stat);
	} while (stat && timeout--);

	if (timeout < 0) {
		return ERR_TIMEOUT;
	}

	return ERR_NONE;
}

/**
 * \brief Get write protect for the given hardware module
 */
int32_t _periph_get_lock_state(const void *const module, bool *const state)
{
	ASSERT((((uint32_t)module ) > (uint32_t)HPB0_ADDR ));

	uint32_t peripheral;

	peripheral = _pac_get_peripheral_id(module) & 0x1F;

	if (((uint32_t)module ) < (uint32_t)HPB1_ADDR) {
		*state = hri_pac_get_STATUSA_reg(PAC, 1 << peripheral);
	}
	else if (((uint32_t)module ) < (uint32_t)HPB2_ADDR) {
		*state = hri_pac_get_STATUSB_reg(PAC, 1 << peripheral);
	}
	else if (((uint32_t)module ) < (uint32_t)HPB3_ADDR) {
		*state = hri_pac_get_STATUSC_reg(PAC, 1 << peripheral);
	}
	else if (((uint32_t)module ) < (uint32_t)HPB4_ADDR) {
		*state = hri_pac_get_STATUSD_reg(PAC, 1 << peripheral);
	}
	else {
		*state =  hri_pac_get_STATUSE_reg(PAC, 1 << peripheral);
	}

	return ERR_NONE;
}
