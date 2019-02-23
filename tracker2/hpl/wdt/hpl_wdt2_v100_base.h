/**
 * \file
 *
 * \brief SAM WDT
 *
 * Copyright (C) 2015-2016 Atmel Corporation. All rights reserved.
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

#ifndef _HPL_WDT2_V1XX_BASE_H_INCLUDED
#define _HPL_WDT2_V1XX_BASE_H_INCLUDED

#ifdef __cplusplus
		extern "C" {
#endif

	/**
	 * \brief The register value of PER which SAMD21/SAMR21 supports
	 */
	enum wdt_period_reg {
		/** Corresponding to 8 clock cycles */
		WDT_PERIOD_8CYCLE,
		/** Corresponding to 16 clock cycles */
		WDT_PERIOD_16CYCLE,
		/** Corresponding to 32 clock cycles */
		WDT_PERIOD_32CYCLE,
		/** Corresponding to 64 clock cycles */
		WDT_PERIOD_64CYCLE,
		/** Corresponding to 128 clock cycles */
		WDT_PERIOD_128CYCLE,
		/** Corresponding to 256 clock cycles */
		WDT_PERIOD_256CYCLE,
		/** Corresponding to 512 clock cycles */
		WDT_PERIOD_512CYCLE,
		/** Corresponding to 1024 clock cycles */
		WDT_PERIOD_1024CYCLE,
		/** Corresponding to 2048 clock cycles */
		WDT_PERIOD_2048CYCLE,
		/** Corresponding to 4096 clock cycles */
		WDT_PERIOD_4096CYCLE,
		/** Corresponding to 8192 clock cycles */
		WDT_PERIOD_8192CYCLE,
		/** Corresponding to 16384 clock cycles */
		WDT_PERIOD_16384CYCLE
	};
#ifdef __cplusplus
		}
#endif
#endif /* _HPL_WDT2_V1XX_BASE_H_INCLUDED */
