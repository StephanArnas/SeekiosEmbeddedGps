/**
 * \file
 *
 * \brief SAM PAC
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
 */

#ifndef _HRI_PAC_V110_H_INCLUDED_
#define _HRI_PAC_V110_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_PAC_CRITICAL_SECTIONS)
#  define PAC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#  define PAC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#  define PAC_CRITICAL_SECTION_ENTER()
#  define PAC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_pac_intflaga_reg_t;
typedef uint32_t hri_pac_intflagahb_reg_t;
typedef uint32_t hri_pac_intflagb_reg_t;
typedef uint32_t hri_pac_intflagc_reg_t;
typedef uint32_t hri_pac_intflagd_reg_t;
typedef uint32_t hri_pac_intflage_reg_t;
typedef uint32_t hri_pac_statusa_reg_t;
typedef uint32_t hri_pac_statusb_reg_t;
typedef uint32_t hri_pac_statusc_reg_t;
typedef uint32_t hri_pac_statusd_reg_t;
typedef uint32_t hri_pac_statuse_reg_t;
typedef uint32_t hri_pac_wrctrl_reg_t;
typedef uint8_t hri_pac_evctrl_reg_t;
typedef uint8_t hri_pac_inten_reg_t;

static inline void hri_pac_set_INTEN_ERR_bit(const void *const hw)
{
	((Pac *)hw)->INTENSET.reg = PAC_INTENSET_ERR;
}

static inline bool hri_pac_get_INTEN_ERR_bit(const void *const hw)
{
	return (((Pac *)hw)->INTENSET.reg & PAC_INTENSET_ERR) >> PAC_INTENSET_ERR_Pos;
}

static inline void hri_pac_write_INTEN_ERR_bit(const void *const hw, bool value)
{
	((Pac *)hw)->INTENCLR.reg = PAC_INTENSET_ERR;
	((Pac *)hw)->INTENSET.reg = value << PAC_INTENSET_ERR_Pos;
}

static inline void hri_pac_clear_INTEN_ERR_bit(const void *const hw)
{
	((Pac *)hw)->INTENCLR.reg = PAC_INTENCLR_ERR;
}

static inline void hri_pac_set_INTEN_reg(const void *const hw, hri_pac_inten_reg_t mask)
{
	((Pac *)hw)->INTENSET.reg = mask;
}

static inline hri_pac_inten_reg_t hri_pac_get_INTEN_reg(const void *const hw, hri_pac_inten_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pac *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_inten_reg_t hri_pac_read_INTEN_reg(const void *const hw)
{
	return ((Pac *)hw)->INTENSET.reg;
}

static inline void hri_pac_write_INTEN_reg(const void *const hw, hri_pac_inten_reg_t data)
{
	((Pac *)hw)->INTENCLR.reg = 0x1;
	((Pac *)hw)->INTENSET.reg = data;
}

static inline void hri_pac_clear_INTEN_reg(const void *const hw, hri_pac_inten_reg_t mask)
{
	((Pac *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_pac_get_STATUSA_PM_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_PM) >> PAC_STATUSA_PM_Pos;
}

static inline bool hri_pac_get_STATUSA_MCLK_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_MCLK) >> PAC_STATUSA_MCLK_Pos;
}

static inline bool hri_pac_get_STATUSA_RSTC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_RSTC) >> PAC_STATUSA_RSTC_Pos;
}

static inline bool hri_pac_get_STATUSA_OSCCTRL_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_OSCCTRL) >> PAC_STATUSA_OSCCTRL_Pos;
}

static inline bool hri_pac_get_STATUSA_OSC32KCTRL_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_OSC32KCTRL) >> PAC_STATUSA_OSC32KCTRL_Pos;
}

static inline bool hri_pac_get_STATUSA_SUPC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_SUPC) >> PAC_STATUSA_SUPC_Pos;
}

static inline bool hri_pac_get_STATUSA_GCLK_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_GCLK) >> PAC_STATUSA_GCLK_Pos;
}

static inline bool hri_pac_get_STATUSA_WDT_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_WDT) >> PAC_STATUSA_WDT_Pos;
}

static inline bool hri_pac_get_STATUSA_RTC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_RTC) >> PAC_STATUSA_RTC_Pos;
}

static inline bool hri_pac_get_STATUSA_EIC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_EIC) >> PAC_STATUSA_EIC_Pos;
}

static inline bool hri_pac_get_STATUSA_PORT_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_PORT) >> PAC_STATUSA_PORT_Pos;
}

static inline bool hri_pac_get_STATUSA_TAL_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_TAL) >> PAC_STATUSA_TAL_Pos;
}

static inline hri_pac_statusa_reg_t hri_pac_get_STATUSA_reg(const void *const hw, hri_pac_statusa_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->STATUSA.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_statusa_reg_t hri_pac_read_STATUSA_reg(const void *const hw)
{
	return ((Pac *)hw)->STATUSA.reg;
}

static inline bool hri_pac_get_STATUSB_USB_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_USB) >> PAC_STATUSB_USB_Pos;
}

static inline bool hri_pac_get_STATUSB_DSU_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_DSU) >> PAC_STATUSB_DSU_Pos;
}

static inline bool hri_pac_get_STATUSB_NVMCTRL_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_NVMCTRL) >> PAC_STATUSB_NVMCTRL_Pos;
}

static inline bool hri_pac_get_STATUSB_MTB_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_MTB) >> PAC_STATUSB_MTB_Pos;
}

static inline hri_pac_statusb_reg_t hri_pac_get_STATUSB_reg(const void *const hw, hri_pac_statusb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->STATUSB.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_statusb_reg_t hri_pac_read_STATUSB_reg(const void *const hw)
{
	return ((Pac *)hw)->STATUSB.reg;
}

static inline bool hri_pac_get_STATUSC_SERCOM0_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_SERCOM0) >> PAC_STATUSC_SERCOM0_Pos;
}

static inline bool hri_pac_get_STATUSC_SERCOM1_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_SERCOM1) >> PAC_STATUSC_SERCOM1_Pos;
}

static inline bool hri_pac_get_STATUSC_SERCOM2_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_SERCOM2) >> PAC_STATUSC_SERCOM2_Pos;
}

static inline bool hri_pac_get_STATUSC_SERCOM3_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_SERCOM3) >> PAC_STATUSC_SERCOM3_Pos;
}

static inline bool hri_pac_get_STATUSC_SERCOM4_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_SERCOM4) >> PAC_STATUSC_SERCOM4_Pos;
}

static inline bool hri_pac_get_STATUSC_TCC0_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_TCC0) >> PAC_STATUSC_TCC0_Pos;
}

static inline bool hri_pac_get_STATUSC_TCC1_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_TCC1) >> PAC_STATUSC_TCC1_Pos;
}

static inline bool hri_pac_get_STATUSC_TCC2_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_TCC2) >> PAC_STATUSC_TCC2_Pos;
}

static inline bool hri_pac_get_STATUSC_TC0_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_TC0) >> PAC_STATUSC_TC0_Pos;
}

static inline bool hri_pac_get_STATUSC_TC1_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_TC1) >> PAC_STATUSC_TC1_Pos;
}

static inline bool hri_pac_get_STATUSC_TC2_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_TC2) >> PAC_STATUSC_TC2_Pos;
}

static inline bool hri_pac_get_STATUSC_TC3_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_TC3) >> PAC_STATUSC_TC3_Pos;
}

static inline bool hri_pac_get_STATUSC_DAC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_DAC) >> PAC_STATUSC_DAC_Pos;
}

static inline bool hri_pac_get_STATUSC_AES_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_AES) >> PAC_STATUSC_AES_Pos;
}

static inline bool hri_pac_get_STATUSC_TRNG_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_TRNG) >> PAC_STATUSC_TRNG_Pos;
}

static inline hri_pac_statusc_reg_t hri_pac_get_STATUSC_reg(const void *const hw, hri_pac_statusc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->STATUSC.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_statusc_reg_t hri_pac_read_STATUSC_reg(const void *const hw)
{
	return ((Pac *)hw)->STATUSC.reg;
}

static inline bool hri_pac_get_STATUSD_EVSYS_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_EVSYS) >> PAC_STATUSD_EVSYS_Pos;
}

static inline bool hri_pac_get_STATUSD_SERCOM5_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_SERCOM5) >> PAC_STATUSD_SERCOM5_Pos;
}

static inline bool hri_pac_get_STATUSD_TC4_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_TC4) >> PAC_STATUSD_TC4_Pos;
}

static inline bool hri_pac_get_STATUSD_ADC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_ADC) >> PAC_STATUSD_ADC_Pos;
}

static inline bool hri_pac_get_STATUSD_AC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_AC) >> PAC_STATUSD_AC_Pos;
}

static inline bool hri_pac_get_STATUSD_PTC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_PTC) >> PAC_STATUSD_PTC_Pos;
}

static inline bool hri_pac_get_STATUSD_OPAMP_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_OPAMP) >> PAC_STATUSD_OPAMP_Pos;
}

static inline bool hri_pac_get_STATUSD_CCL_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_CCL) >> PAC_STATUSD_CCL_Pos;
}

static inline hri_pac_statusd_reg_t hri_pac_get_STATUSD_reg(const void *const hw, hri_pac_statusd_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->STATUSD.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_statusd_reg_t hri_pac_read_STATUSD_reg(const void *const hw)
{
	return ((Pac *)hw)->STATUSD.reg;
}

static inline bool hri_pac_get_STATUSE_PAC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSE.reg & PAC_STATUSE_PAC) >> PAC_STATUSE_PAC_Pos;
}

static inline bool hri_pac_get_STATUSE_DMAC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSE.reg & PAC_STATUSE_DMAC) >> PAC_STATUSE_DMAC_Pos;
}

static inline hri_pac_statuse_reg_t hri_pac_get_STATUSE_reg(const void *const hw, hri_pac_statuse_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->STATUSE.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_statuse_reg_t hri_pac_read_STATUSE_reg(const void *const hw)
{
	return ((Pac *)hw)->STATUSE.reg;
}

static inline void hri_pac_set_WRCTRL_PERID_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg |= PAC_WRCTRL_PERID(mask);
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_wrctrl_reg_t hri_pac_get_WRCTRL_PERID_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->WRCTRL.reg;
	tmp = (tmp & PAC_WRCTRL_PERID(mask)) >> PAC_WRCTRL_PERID_Pos;
	return tmp;
}

static inline void hri_pac_write_WRCTRL_PERID_bf(const void *const hw, hri_pac_wrctrl_reg_t data)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->WRCTRL.reg;
	tmp &= ~PAC_WRCTRL_PERID_Msk;
	tmp |= PAC_WRCTRL_PERID(data);
	((Pac *)hw)->WRCTRL.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_WRCTRL_PERID_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg &= ~PAC_WRCTRL_PERID(mask);
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_WRCTRL_PERID_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg ^= PAC_WRCTRL_PERID(mask);
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_wrctrl_reg_t hri_pac_read_WRCTRL_PERID_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->WRCTRL.reg;
	tmp = (tmp & PAC_WRCTRL_PERID_Msk) >> PAC_WRCTRL_PERID_Pos;
	return tmp;
}

static inline void hri_pac_set_WRCTRL_KEY_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg |= PAC_WRCTRL_KEY(mask);
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_wrctrl_reg_t hri_pac_get_WRCTRL_KEY_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->WRCTRL.reg;
	tmp = (tmp & PAC_WRCTRL_KEY(mask)) >> PAC_WRCTRL_KEY_Pos;
	return tmp;
}

static inline void hri_pac_write_WRCTRL_KEY_bf(const void *const hw, hri_pac_wrctrl_reg_t data)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->WRCTRL.reg;
	tmp &= ~PAC_WRCTRL_KEY_Msk;
	tmp |= PAC_WRCTRL_KEY(data);
	((Pac *)hw)->WRCTRL.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_WRCTRL_KEY_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg &= ~PAC_WRCTRL_KEY(mask);
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_WRCTRL_KEY_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg ^= PAC_WRCTRL_KEY(mask);
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_wrctrl_reg_t hri_pac_read_WRCTRL_KEY_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->WRCTRL.reg;
	tmp = (tmp & PAC_WRCTRL_KEY_Msk) >> PAC_WRCTRL_KEY_Pos;
	return tmp;
}

static inline void hri_pac_set_WRCTRL_reg(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg |= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_wrctrl_reg_t hri_pac_get_WRCTRL_reg(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->WRCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pac_write_WRCTRL_reg(const void *const hw, hri_pac_wrctrl_reg_t data)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg = data;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_WRCTRL_reg(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg &= ~mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_WRCTRL_reg(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg ^= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_wrctrl_reg_t hri_pac_read_WRCTRL_reg(const void *const hw)
{
	return ((Pac *)hw)->WRCTRL.reg;
}

static inline void hri_pac_set_EVCTRL_ERREO_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->EVCTRL.reg |= PAC_EVCTRL_ERREO;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_EVCTRL_ERREO_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pac *)hw)->EVCTRL.reg;
	tmp = (tmp & PAC_EVCTRL_ERREO) >> PAC_EVCTRL_ERREO_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_EVCTRL_ERREO_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->EVCTRL.reg;
	tmp &= ~PAC_EVCTRL_ERREO;
	tmp |= value << PAC_EVCTRL_ERREO_Pos;
	((Pac *)hw)->EVCTRL.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_EVCTRL_ERREO_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->EVCTRL.reg &= ~PAC_EVCTRL_ERREO;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_EVCTRL_ERREO_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->EVCTRL.reg ^= PAC_EVCTRL_ERREO;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_EVCTRL_reg(const void *const hw, hri_pac_evctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->EVCTRL.reg |= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_evctrl_reg_t hri_pac_get_EVCTRL_reg(const void *const hw, hri_pac_evctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pac *)hw)->EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pac_write_EVCTRL_reg(const void *const hw, hri_pac_evctrl_reg_t data)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->EVCTRL.reg = data;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_EVCTRL_reg(const void *const hw, hri_pac_evctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->EVCTRL.reg &= ~mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_EVCTRL_reg(const void *const hw, hri_pac_evctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->EVCTRL.reg ^= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_evctrl_reg_t hri_pac_read_EVCTRL_reg(const void *const hw)
{
	return ((Pac *)hw)->EVCTRL.reg;
}

static inline void hri_pac_set_INTFLAGAHB_FLASH_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= PAC_INTFLAGAHB_FLASH;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGAHB_FLASH_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp = (tmp & PAC_INTFLAGAHB_FLASH) >> PAC_INTFLAGAHB_FLASH_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGAHB_FLASH_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= ~PAC_INTFLAGAHB_FLASH;
	tmp |= value << PAC_INTFLAGAHB_FLASH_Pos;
	((Pac *)hw)->INTFLAGAHB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_FLASH_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~PAC_INTFLAGAHB_FLASH;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_FLASH_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= PAC_INTFLAGAHB_FLASH;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGAHB_HSRAMCM0P_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= PAC_INTFLAGAHB_HSRAMCM0P;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGAHB_HSRAMCM0P_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp = (tmp & PAC_INTFLAGAHB_HSRAMCM0P) >> PAC_INTFLAGAHB_HSRAMCM0P_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGAHB_HSRAMCM0P_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= ~PAC_INTFLAGAHB_HSRAMCM0P;
	tmp |= value << PAC_INTFLAGAHB_HSRAMCM0P_Pos;
	((Pac *)hw)->INTFLAGAHB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_HSRAMCM0P_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~PAC_INTFLAGAHB_HSRAMCM0P;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_HSRAMCM0P_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= PAC_INTFLAGAHB_HSRAMCM0P;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGAHB_HSRAMDSU_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= PAC_INTFLAGAHB_HSRAMDSU;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGAHB_HSRAMDSU_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp = (tmp & PAC_INTFLAGAHB_HSRAMDSU) >> PAC_INTFLAGAHB_HSRAMDSU_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGAHB_HSRAMDSU_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= ~PAC_INTFLAGAHB_HSRAMDSU;
	tmp |= value << PAC_INTFLAGAHB_HSRAMDSU_Pos;
	((Pac *)hw)->INTFLAGAHB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_HSRAMDSU_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~PAC_INTFLAGAHB_HSRAMDSU;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_HSRAMDSU_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= PAC_INTFLAGAHB_HSRAMDSU;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGAHB_HPB1_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= PAC_INTFLAGAHB_HPB1;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGAHB_HPB1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp = (tmp & PAC_INTFLAGAHB_HPB1) >> PAC_INTFLAGAHB_HPB1_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGAHB_HPB1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= ~PAC_INTFLAGAHB_HPB1;
	tmp |= value << PAC_INTFLAGAHB_HPB1_Pos;
	((Pac *)hw)->INTFLAGAHB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_HPB1_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~PAC_INTFLAGAHB_HPB1;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_HPB1_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= PAC_INTFLAGAHB_HPB1;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGAHB_H2LBRIDGES_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= PAC_INTFLAGAHB_H2LBRIDGES;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGAHB_H2LBRIDGES_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp = (tmp & PAC_INTFLAGAHB_H2LBRIDGES) >> PAC_INTFLAGAHB_H2LBRIDGES_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGAHB_H2LBRIDGES_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= ~PAC_INTFLAGAHB_H2LBRIDGES;
	tmp |= value << PAC_INTFLAGAHB_H2LBRIDGES_Pos;
	((Pac *)hw)->INTFLAGAHB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_H2LBRIDGES_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~PAC_INTFLAGAHB_H2LBRIDGES;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_H2LBRIDGES_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= PAC_INTFLAGAHB_H2LBRIDGES;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGAHB_HPB0_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= PAC_INTFLAGAHB_HPB0;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGAHB_HPB0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp = (tmp & PAC_INTFLAGAHB_HPB0) >> PAC_INTFLAGAHB_HPB0_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGAHB_HPB0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= ~PAC_INTFLAGAHB_HPB0;
	tmp |= value << PAC_INTFLAGAHB_HPB0_Pos;
	((Pac *)hw)->INTFLAGAHB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_HPB0_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~PAC_INTFLAGAHB_HPB0;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_HPB0_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= PAC_INTFLAGAHB_HPB0;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGAHB_HPB2_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= PAC_INTFLAGAHB_HPB2;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGAHB_HPB2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp = (tmp & PAC_INTFLAGAHB_HPB2) >> PAC_INTFLAGAHB_HPB2_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGAHB_HPB2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= ~PAC_INTFLAGAHB_HPB2;
	tmp |= value << PAC_INTFLAGAHB_HPB2_Pos;
	((Pac *)hw)->INTFLAGAHB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_HPB2_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~PAC_INTFLAGAHB_HPB2;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_HPB2_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= PAC_INTFLAGAHB_HPB2;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGAHB_HPB3_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= PAC_INTFLAGAHB_HPB3;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGAHB_HPB3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp = (tmp & PAC_INTFLAGAHB_HPB3) >> PAC_INTFLAGAHB_HPB3_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGAHB_HPB3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= ~PAC_INTFLAGAHB_HPB3;
	tmp |= value << PAC_INTFLAGAHB_HPB3_Pos;
	((Pac *)hw)->INTFLAGAHB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_HPB3_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~PAC_INTFLAGAHB_HPB3;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_HPB3_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= PAC_INTFLAGAHB_HPB3;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGAHB_HPB4_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= PAC_INTFLAGAHB_HPB4;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGAHB_HPB4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp = (tmp & PAC_INTFLAGAHB_HPB4) >> PAC_INTFLAGAHB_HPB4_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGAHB_HPB4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= ~PAC_INTFLAGAHB_HPB4;
	tmp |= value << PAC_INTFLAGAHB_HPB4_Pos;
	((Pac *)hw)->INTFLAGAHB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_HPB4_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~PAC_INTFLAGAHB_HPB4;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_HPB4_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= PAC_INTFLAGAHB_HPB4;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGAHB_LPRAMHS_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= PAC_INTFLAGAHB_LPRAMHS;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGAHB_LPRAMHS_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp = (tmp & PAC_INTFLAGAHB_LPRAMHS) >> PAC_INTFLAGAHB_LPRAMHS_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGAHB_LPRAMHS_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= ~PAC_INTFLAGAHB_LPRAMHS;
	tmp |= value << PAC_INTFLAGAHB_LPRAMHS_Pos;
	((Pac *)hw)->INTFLAGAHB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_LPRAMHS_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~PAC_INTFLAGAHB_LPRAMHS;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_LPRAMHS_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= PAC_INTFLAGAHB_LPRAMHS;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGAHB_LPRAMPICOP_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= PAC_INTFLAGAHB_LPRAMPICOP;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGAHB_LPRAMPICOP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp = (tmp & PAC_INTFLAGAHB_LPRAMPICOP) >> PAC_INTFLAGAHB_LPRAMPICOP_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGAHB_LPRAMPICOP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= ~PAC_INTFLAGAHB_LPRAMPICOP;
	tmp |= value << PAC_INTFLAGAHB_LPRAMPICOP_Pos;
	((Pac *)hw)->INTFLAGAHB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_LPRAMPICOP_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~PAC_INTFLAGAHB_LPRAMPICOP;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_LPRAMPICOP_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= PAC_INTFLAGAHB_LPRAMPICOP;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGAHB_LPRAMDMAC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= PAC_INTFLAGAHB_LPRAMDMAC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGAHB_LPRAMDMAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp = (tmp & PAC_INTFLAGAHB_LPRAMDMAC) >> PAC_INTFLAGAHB_LPRAMDMAC_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGAHB_LPRAMDMAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= ~PAC_INTFLAGAHB_LPRAMDMAC;
	tmp |= value << PAC_INTFLAGAHB_LPRAMDMAC_Pos;
	((Pac *)hw)->INTFLAGAHB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_LPRAMDMAC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~PAC_INTFLAGAHB_LPRAMDMAC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_LPRAMDMAC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= PAC_INTFLAGAHB_LPRAMDMAC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGAHB_L2HBRIDGES_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= PAC_INTFLAGAHB_L2HBRIDGES;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGAHB_L2HBRIDGES_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp = (tmp & PAC_INTFLAGAHB_L2HBRIDGES) >> PAC_INTFLAGAHB_L2HBRIDGES_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGAHB_L2HBRIDGES_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= ~PAC_INTFLAGAHB_L2HBRIDGES;
	tmp |= value << PAC_INTFLAGAHB_L2HBRIDGES_Pos;
	((Pac *)hw)->INTFLAGAHB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_L2HBRIDGES_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~PAC_INTFLAGAHB_L2HBRIDGES;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_L2HBRIDGES_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= PAC_INTFLAGAHB_L2HBRIDGES;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGAHB_HSRAMLP_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= PAC_INTFLAGAHB_HSRAMLP;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGAHB_HSRAMLP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp = (tmp & PAC_INTFLAGAHB_HSRAMLP) >> PAC_INTFLAGAHB_HSRAMLP_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGAHB_HSRAMLP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= ~PAC_INTFLAGAHB_HSRAMLP;
	tmp |= value << PAC_INTFLAGAHB_HSRAMLP_Pos;
	((Pac *)hw)->INTFLAGAHB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_HSRAMLP_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~PAC_INTFLAGAHB_HSRAMLP;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_HSRAMLP_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= PAC_INTFLAGAHB_HSRAMLP;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGAHB_reg(const void *const hw, hri_pac_intflagahb_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg |= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_intflagahb_reg_t hri_pac_get_INTFLAGAHB_reg(const void *const hw, hri_pac_intflagahb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pac_write_INTFLAGAHB_reg(const void *const hw, hri_pac_intflagahb_reg_t data)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg = data;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGAHB_reg(const void *const hw, hri_pac_intflagahb_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg &= ~mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGAHB_reg(const void *const hw, hri_pac_intflagahb_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGAHB.reg ^= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_intflagahb_reg_t hri_pac_read_INTFLAGAHB_reg(const void *const hw)
{
	return ((Pac *)hw)->INTFLAGAHB.reg;
}

static inline void hri_pac_set_INTFLAGA_PM_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg |= PAC_INTFLAGA_PM;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGA_PM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp = (tmp & PAC_INTFLAGA_PM) >> PAC_INTFLAGA_PM_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGA_PM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp &= ~PAC_INTFLAGA_PM;
	tmp |= value << PAC_INTFLAGA_PM_Pos;
	((Pac *)hw)->INTFLAGA.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGA_PM_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg &= ~PAC_INTFLAGA_PM;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGA_PM_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg ^= PAC_INTFLAGA_PM;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGA_MCLK_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg |= PAC_INTFLAGA_MCLK;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGA_MCLK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp = (tmp & PAC_INTFLAGA_MCLK) >> PAC_INTFLAGA_MCLK_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGA_MCLK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp &= ~PAC_INTFLAGA_MCLK;
	tmp |= value << PAC_INTFLAGA_MCLK_Pos;
	((Pac *)hw)->INTFLAGA.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGA_MCLK_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg &= ~PAC_INTFLAGA_MCLK;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGA_MCLK_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg ^= PAC_INTFLAGA_MCLK;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGA_RSTC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg |= PAC_INTFLAGA_RSTC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGA_RSTC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp = (tmp & PAC_INTFLAGA_RSTC) >> PAC_INTFLAGA_RSTC_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGA_RSTC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp &= ~PAC_INTFLAGA_RSTC;
	tmp |= value << PAC_INTFLAGA_RSTC_Pos;
	((Pac *)hw)->INTFLAGA.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGA_RSTC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg &= ~PAC_INTFLAGA_RSTC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGA_RSTC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg ^= PAC_INTFLAGA_RSTC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGA_OSCCTRL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg |= PAC_INTFLAGA_OSCCTRL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGA_OSCCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp = (tmp & PAC_INTFLAGA_OSCCTRL) >> PAC_INTFLAGA_OSCCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGA_OSCCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp &= ~PAC_INTFLAGA_OSCCTRL;
	tmp |= value << PAC_INTFLAGA_OSCCTRL_Pos;
	((Pac *)hw)->INTFLAGA.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGA_OSCCTRL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg &= ~PAC_INTFLAGA_OSCCTRL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGA_OSCCTRL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg ^= PAC_INTFLAGA_OSCCTRL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGA_OSC32KCTRL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg |= PAC_INTFLAGA_OSC32KCTRL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGA_OSC32KCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp = (tmp & PAC_INTFLAGA_OSC32KCTRL) >> PAC_INTFLAGA_OSC32KCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGA_OSC32KCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp &= ~PAC_INTFLAGA_OSC32KCTRL;
	tmp |= value << PAC_INTFLAGA_OSC32KCTRL_Pos;
	((Pac *)hw)->INTFLAGA.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGA_OSC32KCTRL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg &= ~PAC_INTFLAGA_OSC32KCTRL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGA_OSC32KCTRL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg ^= PAC_INTFLAGA_OSC32KCTRL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGA_SUPC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg |= PAC_INTFLAGA_SUPC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGA_SUPC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp = (tmp & PAC_INTFLAGA_SUPC) >> PAC_INTFLAGA_SUPC_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGA_SUPC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp &= ~PAC_INTFLAGA_SUPC;
	tmp |= value << PAC_INTFLAGA_SUPC_Pos;
	((Pac *)hw)->INTFLAGA.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGA_SUPC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg &= ~PAC_INTFLAGA_SUPC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGA_SUPC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg ^= PAC_INTFLAGA_SUPC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGA_GCLK_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg |= PAC_INTFLAGA_GCLK;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGA_GCLK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp = (tmp & PAC_INTFLAGA_GCLK) >> PAC_INTFLAGA_GCLK_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGA_GCLK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp &= ~PAC_INTFLAGA_GCLK;
	tmp |= value << PAC_INTFLAGA_GCLK_Pos;
	((Pac *)hw)->INTFLAGA.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGA_GCLK_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg &= ~PAC_INTFLAGA_GCLK;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGA_GCLK_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg ^= PAC_INTFLAGA_GCLK;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGA_WDT_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg |= PAC_INTFLAGA_WDT;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGA_WDT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp = (tmp & PAC_INTFLAGA_WDT) >> PAC_INTFLAGA_WDT_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGA_WDT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp &= ~PAC_INTFLAGA_WDT;
	tmp |= value << PAC_INTFLAGA_WDT_Pos;
	((Pac *)hw)->INTFLAGA.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGA_WDT_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg &= ~PAC_INTFLAGA_WDT;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGA_WDT_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg ^= PAC_INTFLAGA_WDT;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGA_RTC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg |= PAC_INTFLAGA_RTC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGA_RTC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp = (tmp & PAC_INTFLAGA_RTC) >> PAC_INTFLAGA_RTC_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGA_RTC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp &= ~PAC_INTFLAGA_RTC;
	tmp |= value << PAC_INTFLAGA_RTC_Pos;
	((Pac *)hw)->INTFLAGA.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGA_RTC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg &= ~PAC_INTFLAGA_RTC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGA_RTC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg ^= PAC_INTFLAGA_RTC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGA_EIC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg |= PAC_INTFLAGA_EIC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGA_EIC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp = (tmp & PAC_INTFLAGA_EIC) >> PAC_INTFLAGA_EIC_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGA_EIC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp &= ~PAC_INTFLAGA_EIC;
	tmp |= value << PAC_INTFLAGA_EIC_Pos;
	((Pac *)hw)->INTFLAGA.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGA_EIC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg &= ~PAC_INTFLAGA_EIC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGA_EIC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg ^= PAC_INTFLAGA_EIC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGA_PORT_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg |= PAC_INTFLAGA_PORT;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGA_PORT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp = (tmp & PAC_INTFLAGA_PORT) >> PAC_INTFLAGA_PORT_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGA_PORT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp &= ~PAC_INTFLAGA_PORT;
	tmp |= value << PAC_INTFLAGA_PORT_Pos;
	((Pac *)hw)->INTFLAGA.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGA_PORT_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg &= ~PAC_INTFLAGA_PORT;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGA_PORT_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg ^= PAC_INTFLAGA_PORT;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGA_TAL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg |= PAC_INTFLAGA_TAL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGA_TAL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp = (tmp & PAC_INTFLAGA_TAL) >> PAC_INTFLAGA_TAL_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGA_TAL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp &= ~PAC_INTFLAGA_TAL;
	tmp |= value << PAC_INTFLAGA_TAL_Pos;
	((Pac *)hw)->INTFLAGA.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGA_TAL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg &= ~PAC_INTFLAGA_TAL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGA_TAL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg ^= PAC_INTFLAGA_TAL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGA_reg(const void *const hw, hri_pac_intflaga_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg |= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_intflaga_reg_t hri_pac_get_INTFLAGA_reg(const void *const hw, hri_pac_intflaga_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pac_write_INTFLAGA_reg(const void *const hw, hri_pac_intflaga_reg_t data)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg = data;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGA_reg(const void *const hw, hri_pac_intflaga_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg &= ~mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGA_reg(const void *const hw, hri_pac_intflaga_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGA.reg ^= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_intflaga_reg_t hri_pac_read_INTFLAGA_reg(const void *const hw)
{
	return ((Pac *)hw)->INTFLAGA.reg;
}

static inline void hri_pac_set_INTFLAGB_USB_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg |= PAC_INTFLAGB_USB;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGB_USB_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGB.reg;
	tmp = (tmp & PAC_INTFLAGB_USB) >> PAC_INTFLAGB_USB_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGB_USB_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGB.reg;
	tmp &= ~PAC_INTFLAGB_USB;
	tmp |= value << PAC_INTFLAGB_USB_Pos;
	((Pac *)hw)->INTFLAGB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGB_USB_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg &= ~PAC_INTFLAGB_USB;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGB_USB_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg ^= PAC_INTFLAGB_USB;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGB_DSU_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg |= PAC_INTFLAGB_DSU;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGB_DSU_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGB.reg;
	tmp = (tmp & PAC_INTFLAGB_DSU) >> PAC_INTFLAGB_DSU_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGB_DSU_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGB.reg;
	tmp &= ~PAC_INTFLAGB_DSU;
	tmp |= value << PAC_INTFLAGB_DSU_Pos;
	((Pac *)hw)->INTFLAGB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGB_DSU_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg &= ~PAC_INTFLAGB_DSU;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGB_DSU_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg ^= PAC_INTFLAGB_DSU;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGB_NVMCTRL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg |= PAC_INTFLAGB_NVMCTRL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGB_NVMCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGB.reg;
	tmp = (tmp & PAC_INTFLAGB_NVMCTRL) >> PAC_INTFLAGB_NVMCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGB_NVMCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGB.reg;
	tmp &= ~PAC_INTFLAGB_NVMCTRL;
	tmp |= value << PAC_INTFLAGB_NVMCTRL_Pos;
	((Pac *)hw)->INTFLAGB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGB_NVMCTRL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg &= ~PAC_INTFLAGB_NVMCTRL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGB_NVMCTRL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg ^= PAC_INTFLAGB_NVMCTRL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGB_MTB_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg |= PAC_INTFLAGB_MTB;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGB_MTB_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGB.reg;
	tmp = (tmp & PAC_INTFLAGB_MTB) >> PAC_INTFLAGB_MTB_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGB_MTB_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGB.reg;
	tmp &= ~PAC_INTFLAGB_MTB;
	tmp |= value << PAC_INTFLAGB_MTB_Pos;
	((Pac *)hw)->INTFLAGB.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGB_MTB_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg &= ~PAC_INTFLAGB_MTB;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGB_MTB_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg ^= PAC_INTFLAGB_MTB;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGB_reg(const void *const hw, hri_pac_intflagb_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg |= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_intflagb_reg_t hri_pac_get_INTFLAGB_reg(const void *const hw, hri_pac_intflagb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pac_write_INTFLAGB_reg(const void *const hw, hri_pac_intflagb_reg_t data)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg = data;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGB_reg(const void *const hw, hri_pac_intflagb_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg &= ~mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGB_reg(const void *const hw, hri_pac_intflagb_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGB.reg ^= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_intflagb_reg_t hri_pac_read_INTFLAGB_reg(const void *const hw)
{
	return ((Pac *)hw)->INTFLAGB.reg;
}

static inline void hri_pac_set_INTFLAGC_SERCOM0_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_SERCOM0;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_SERCOM0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_SERCOM0) >> PAC_INTFLAGC_SERCOM0_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_SERCOM0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_SERCOM0;
	tmp |= value << PAC_INTFLAGC_SERCOM0_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_SERCOM0_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_SERCOM0;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_SERCOM0_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_SERCOM0;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_SERCOM1_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_SERCOM1;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_SERCOM1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_SERCOM1) >> PAC_INTFLAGC_SERCOM1_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_SERCOM1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_SERCOM1;
	tmp |= value << PAC_INTFLAGC_SERCOM1_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_SERCOM1_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_SERCOM1;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_SERCOM1_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_SERCOM1;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_SERCOM2_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_SERCOM2;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_SERCOM2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_SERCOM2) >> PAC_INTFLAGC_SERCOM2_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_SERCOM2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_SERCOM2;
	tmp |= value << PAC_INTFLAGC_SERCOM2_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_SERCOM2_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_SERCOM2;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_SERCOM2_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_SERCOM2;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_SERCOM3_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_SERCOM3;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_SERCOM3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_SERCOM3) >> PAC_INTFLAGC_SERCOM3_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_SERCOM3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_SERCOM3;
	tmp |= value << PAC_INTFLAGC_SERCOM3_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_SERCOM3_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_SERCOM3;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_SERCOM3_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_SERCOM3;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_SERCOM4_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_SERCOM4;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_SERCOM4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_SERCOM4) >> PAC_INTFLAGC_SERCOM4_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_SERCOM4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_SERCOM4;
	tmp |= value << PAC_INTFLAGC_SERCOM4_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_SERCOM4_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_SERCOM4;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_SERCOM4_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_SERCOM4;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_TCC0_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_TCC0;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_TCC0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_TCC0) >> PAC_INTFLAGC_TCC0_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_TCC0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_TCC0;
	tmp |= value << PAC_INTFLAGC_TCC0_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_TCC0_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_TCC0;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_TCC0_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_TCC0;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_TCC1_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_TCC1;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_TCC1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_TCC1) >> PAC_INTFLAGC_TCC1_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_TCC1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_TCC1;
	tmp |= value << PAC_INTFLAGC_TCC1_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_TCC1_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_TCC1;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_TCC1_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_TCC1;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_TCC2_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_TCC2;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_TCC2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_TCC2) >> PAC_INTFLAGC_TCC2_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_TCC2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_TCC2;
	tmp |= value << PAC_INTFLAGC_TCC2_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_TCC2_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_TCC2;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_TCC2_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_TCC2;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_TC0_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_TC0;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_TC0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_TC0) >> PAC_INTFLAGC_TC0_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_TC0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_TC0;
	tmp |= value << PAC_INTFLAGC_TC0_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_TC0_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_TC0;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_TC0_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_TC0;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_TC1_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_TC1;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_TC1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_TC1) >> PAC_INTFLAGC_TC1_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_TC1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_TC1;
	tmp |= value << PAC_INTFLAGC_TC1_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_TC1_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_TC1;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_TC1_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_TC1;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_TC2_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_TC2;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_TC2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_TC2) >> PAC_INTFLAGC_TC2_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_TC2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_TC2;
	tmp |= value << PAC_INTFLAGC_TC2_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_TC2_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_TC2;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_TC2_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_TC2;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_TC3_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_TC3;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_TC3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_TC3) >> PAC_INTFLAGC_TC3_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_TC3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_TC3;
	tmp |= value << PAC_INTFLAGC_TC3_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_TC3_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_TC3;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_TC3_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_TC3;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_DAC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_DAC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_DAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_DAC) >> PAC_INTFLAGC_DAC_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_DAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_DAC;
	tmp |= value << PAC_INTFLAGC_DAC_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_DAC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_DAC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_DAC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_DAC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_AES_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_AES;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_AES_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_AES) >> PAC_INTFLAGC_AES_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_AES_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_AES;
	tmp |= value << PAC_INTFLAGC_AES_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_AES_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_AES;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_AES_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_AES;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_TRNG_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= PAC_INTFLAGC_TRNG;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGC_TRNG_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp = (tmp & PAC_INTFLAGC_TRNG) >> PAC_INTFLAGC_TRNG_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGC_TRNG_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= ~PAC_INTFLAGC_TRNG;
	tmp |= value << PAC_INTFLAGC_TRNG_Pos;
	((Pac *)hw)->INTFLAGC.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_TRNG_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~PAC_INTFLAGC_TRNG;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_TRNG_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= PAC_INTFLAGC_TRNG;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGC_reg(const void *const hw, hri_pac_intflagc_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg |= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_intflagc_reg_t hri_pac_get_INTFLAGC_reg(const void *const hw, hri_pac_intflagc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pac_write_INTFLAGC_reg(const void *const hw, hri_pac_intflagc_reg_t data)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg = data;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGC_reg(const void *const hw, hri_pac_intflagc_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg &= ~mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGC_reg(const void *const hw, hri_pac_intflagc_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGC.reg ^= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_intflagc_reg_t hri_pac_read_INTFLAGC_reg(const void *const hw)
{
	return ((Pac *)hw)->INTFLAGC.reg;
}

static inline void hri_pac_set_INTFLAGD_EVSYS_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg |= PAC_INTFLAGD_EVSYS;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGD_EVSYS_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp = (tmp & PAC_INTFLAGD_EVSYS) >> PAC_INTFLAGD_EVSYS_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGD_EVSYS_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp &= ~PAC_INTFLAGD_EVSYS;
	tmp |= value << PAC_INTFLAGD_EVSYS_Pos;
	((Pac *)hw)->INTFLAGD.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGD_EVSYS_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg &= ~PAC_INTFLAGD_EVSYS;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGD_EVSYS_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg ^= PAC_INTFLAGD_EVSYS;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGD_SERCOM5_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg |= PAC_INTFLAGD_SERCOM5;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGD_SERCOM5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp = (tmp & PAC_INTFLAGD_SERCOM5) >> PAC_INTFLAGD_SERCOM5_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGD_SERCOM5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp &= ~PAC_INTFLAGD_SERCOM5;
	tmp |= value << PAC_INTFLAGD_SERCOM5_Pos;
	((Pac *)hw)->INTFLAGD.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGD_SERCOM5_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg &= ~PAC_INTFLAGD_SERCOM5;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGD_SERCOM5_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg ^= PAC_INTFLAGD_SERCOM5;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGD_TC4_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg |= PAC_INTFLAGD_TC4;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGD_TC4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp = (tmp & PAC_INTFLAGD_TC4) >> PAC_INTFLAGD_TC4_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGD_TC4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp &= ~PAC_INTFLAGD_TC4;
	tmp |= value << PAC_INTFLAGD_TC4_Pos;
	((Pac *)hw)->INTFLAGD.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGD_TC4_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg &= ~PAC_INTFLAGD_TC4;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGD_TC4_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg ^= PAC_INTFLAGD_TC4;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGD_ADC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg |= PAC_INTFLAGD_ADC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGD_ADC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp = (tmp & PAC_INTFLAGD_ADC) >> PAC_INTFLAGD_ADC_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGD_ADC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp &= ~PAC_INTFLAGD_ADC;
	tmp |= value << PAC_INTFLAGD_ADC_Pos;
	((Pac *)hw)->INTFLAGD.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGD_ADC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg &= ~PAC_INTFLAGD_ADC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGD_ADC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg ^= PAC_INTFLAGD_ADC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGD_AC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg |= PAC_INTFLAGD_AC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGD_AC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp = (tmp & PAC_INTFLAGD_AC) >> PAC_INTFLAGD_AC_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGD_AC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp &= ~PAC_INTFLAGD_AC;
	tmp |= value << PAC_INTFLAGD_AC_Pos;
	((Pac *)hw)->INTFLAGD.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGD_AC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg &= ~PAC_INTFLAGD_AC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGD_AC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg ^= PAC_INTFLAGD_AC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGD_PTC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg |= PAC_INTFLAGD_PTC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGD_PTC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp = (tmp & PAC_INTFLAGD_PTC) >> PAC_INTFLAGD_PTC_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGD_PTC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp &= ~PAC_INTFLAGD_PTC;
	tmp |= value << PAC_INTFLAGD_PTC_Pos;
	((Pac *)hw)->INTFLAGD.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGD_PTC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg &= ~PAC_INTFLAGD_PTC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGD_PTC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg ^= PAC_INTFLAGD_PTC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGD_OPAMP_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg |= PAC_INTFLAGD_OPAMP;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGD_OPAMP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp = (tmp & PAC_INTFLAGD_OPAMP) >> PAC_INTFLAGD_OPAMP_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGD_OPAMP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp &= ~PAC_INTFLAGD_OPAMP;
	tmp |= value << PAC_INTFLAGD_OPAMP_Pos;
	((Pac *)hw)->INTFLAGD.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGD_OPAMP_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg &= ~PAC_INTFLAGD_OPAMP;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGD_OPAMP_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg ^= PAC_INTFLAGD_OPAMP;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGD_CCL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg |= PAC_INTFLAGD_CCL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGD_CCL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp = (tmp & PAC_INTFLAGD_CCL) >> PAC_INTFLAGD_CCL_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGD_CCL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp &= ~PAC_INTFLAGD_CCL;
	tmp |= value << PAC_INTFLAGD_CCL_Pos;
	((Pac *)hw)->INTFLAGD.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGD_CCL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg &= ~PAC_INTFLAGD_CCL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGD_CCL_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg ^= PAC_INTFLAGD_CCL;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGD_reg(const void *const hw, hri_pac_intflagd_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg |= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_intflagd_reg_t hri_pac_get_INTFLAGD_reg(const void *const hw, hri_pac_intflagd_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pac_write_INTFLAGD_reg(const void *const hw, hri_pac_intflagd_reg_t data)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg = data;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGD_reg(const void *const hw, hri_pac_intflagd_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg &= ~mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGD_reg(const void *const hw, hri_pac_intflagd_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGD.reg ^= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_intflagd_reg_t hri_pac_read_INTFLAGD_reg(const void *const hw)
{
	return ((Pac *)hw)->INTFLAGD.reg;
}

static inline void hri_pac_set_INTFLAGE_PAC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGE.reg |= PAC_INTFLAGE_PAC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGE_PAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGE.reg;
	tmp = (tmp & PAC_INTFLAGE_PAC) >> PAC_INTFLAGE_PAC_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGE_PAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGE.reg;
	tmp &= ~PAC_INTFLAGE_PAC;
	tmp |= value << PAC_INTFLAGE_PAC_Pos;
	((Pac *)hw)->INTFLAGE.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGE_PAC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGE.reg &= ~PAC_INTFLAGE_PAC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGE_PAC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGE.reg ^= PAC_INTFLAGE_PAC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGE_DMAC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGE.reg |= PAC_INTFLAGE_DMAC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_INTFLAGE_DMAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGE.reg;
	tmp = (tmp & PAC_INTFLAGE_DMAC) >> PAC_INTFLAGE_DMAC_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_INTFLAGE_DMAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->INTFLAGE.reg;
	tmp &= ~PAC_INTFLAGE_DMAC;
	tmp |= value << PAC_INTFLAGE_DMAC_Pos;
	((Pac *)hw)->INTFLAGE.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGE_DMAC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGE.reg &= ~PAC_INTFLAGE_DMAC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGE_DMAC_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGE.reg ^= PAC_INTFLAGE_DMAC;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_INTFLAGE_reg(const void *const hw, hri_pac_intflage_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGE.reg |= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_intflage_reg_t hri_pac_get_INTFLAGE_reg(const void *const hw, hri_pac_intflage_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGE.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pac_write_INTFLAGE_reg(const void *const hw, hri_pac_intflage_reg_t data)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGE.reg = data;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_INTFLAGE_reg(const void *const hw, hri_pac_intflage_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGE.reg &= ~mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_INTFLAGE_reg(const void *const hw, hri_pac_intflage_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->INTFLAGE.reg ^= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_intflage_reg_t hri_pac_read_INTFLAGE_reg(const void *const hw)
{
	return ((Pac *)hw)->INTFLAGE.reg;
}
#ifdef __cplusplus
}
#endif
#endif /* _HRI_PAC_V110_H_INCLUDED */
