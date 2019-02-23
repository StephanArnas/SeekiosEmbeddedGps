/**
 * \file
 *
 * \brief SAM0+ SysTick
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
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
 *    and/or other materials provided with the distributionn.
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
 */

#ifndef _HRI_SYSTICK_ARMV6_H_INCLUDED
#define _HRI_SYSTICK_ARMV6_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <compiler.h>

/**
 * \addtogroup hri_systick_group SysTick Low Level Driver
 *
 * \section hri_systick_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */

typedef uint32_t hri_systick_csr_reg_t;
typedef uint32_t hri_systick_rvr_reg_t;
typedef uint32_t hri_systick_crv_reg_t;
typedef uint32_t hri_systick_csr_calib_t;

/**
 * \name Access functions
 */
//@{
static inline void hri_systick_write_SYSTRVR_reg(void *const hw,
		const hri_systick_rvr_reg_t value)
{
	((SysTick_Type *)hw)->LOAD = value;
}

static inline void hri_systick_write_SYSTCSR_reg(void *const hw,
		const hri_systick_csr_reg_t value)
{
	((SysTick_Type *)hw)->CTRL = value;
}

static inline void hri_systick_clear_SYSTCSR_ENABLE_bit(void *const hw)
{
	((SysTick_Type *)hw)->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

static inline bool hri_systick_get_SYSTCSR_COUNTFLAG_bit(const void *const hw)
{
	return (bool)(((const SysTick_Type *)hw)->CTRL & SysTick_CTRL_COUNTFLAG_Msk);
}

static inline hri_systick_crv_reg_t hri_systick_read_SYSTCVR_reg(
		const void *const hw)
{
	return ((SysTick_Type *)hw)->VAL;
}

static inline void hri_systick_write_SYSTCVR_reg( const void *const hw,
		const hri_systick_crv_reg_t value)
{
	((SysTick_Type *)hw)->VAL = value;
}

/**@}*/

#ifdef __cplusplus
}
#endif
#endif /* _HRI_SYSTICK_ARMV6_H_INCLUDED */
