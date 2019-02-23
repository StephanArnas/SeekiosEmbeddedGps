/**
 * \file
 *
 * \brief SAM TC
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

#include <utils_assert.h>
#include <hpl_tc2_v200_base.h>
#include <hpl_tc2_v200_config.h>
#include <hpl_timer.h>
#include <hpl_irq.h>
#include <utils.h>
#include <hri_tc2_v200.h>

#ifndef CONF_TC0_ENABLE
#    define CONF_TC0_ENABLE 0
#endif
#ifndef CONF_TC1_ENABLE
#    define CONF_TC1_ENABLE 0
#endif
#ifndef CONF_TC2_ENABLE
#    define CONF_TC2_ENABLE 0
#endif
#ifndef CONF_TC3_ENABLE
#    define CONF_TC3_ENABLE 0
#endif
#ifndef CONF_TC4_ENABLE
#    define CONF_TC4_ENABLE 0
#endif

/**
 * \brief Macro is used to fill usart configuration structure based on its
 * number
 *
 * \param[in] n The number of structures
 */
#define TC_CONFIGURATION(n)	\
	{n,	\
	 TC_CTRLA_MODE(CONF_TC ## n ## _MODE) |	\
	 TC_CTRLA_PRESCSYNC(CONF_TC ## n ## _PRESCSYNC) | \
	 ( CONF_TC ## n ## _RUNSTDBY << TC_CTRLA_RUNSTDBY_Pos ) | \
	 ( CONF_TC ## n ## _ONDEMAND << TC_CTRLA_ONDEMAND_Pos ) | \
	 TC_CTRLA_PRESCALER(CONF_TC ## n ## _PRESCALER) | \
	 ( CONF_TC ## n ## _ALOCK << TC_CTRLA_ALOCK_Pos ), \
	 ( CONF_TC ## n ## _OVFEO << TC_EVCTRL_OVFEO_Pos ) | \
	 ( CONF_TC ## n ## _TCEI << TC_EVCTRL_TCEI_Pos ) | \
	 ( CONF_TC ## n ## _TCINV << TC_EVCTRL_TCINV_Pos ) | \
	 ( CONF_TC ## n ## _EVACT << TC_EVCTRL_EVACT_Pos ) | \
	 ( CONF_TC ## n ## _MCEO0 << TC_EVCTRL_MCEO0_Pos ) | \
	 ( CONF_TC ## n ## _MCEO1 << TC_EVCTRL_MCEO1_Pos ),	\
	 ( CONF_TC ## n ## _DBGRUN << TC_DBGCTRL_DBGRUN_Pos ), \
	 CONF_TC ## n ## _PER, \
	 CONF_TC ## n ## _CC0, \
	 CONF_TC ## n ## _CC1, \
	}
/**
 * \brief TC configuration type
 */
struct tc_configuration {
	uint8_t number;
	hri_tc_ctrla_reg_t ctrl_a;
	hri_tc_evctrl_reg_t event_ctrl;
	hri_tc_dbgctrl_reg_t dbg_ctrl;
	hri_tc_per_reg_t per;
	hri_tc_cc32_reg_t cc0;
	hri_tc_cc32_reg_t cc1;
};

/**
 * \brief Array of TC configurations
 */
static struct tc_configuration _tcs[] = {
#if CONF_TC0_ENABLE == 1
		TC_CONFIGURATION(0),
#endif
#if CONF_TC1_ENABLE == 1
		TC_CONFIGURATION(1),
#endif
#if CONF_TC2_ENABLE == 1
		TC_CONFIGURATION(2),
#endif
#if CONF_TC3_ENABLE == 1
		TC_CONFIGURATION(3),
#endif
#if CONF_TC4_ENABLE == 1
		TC_CONFIGURATION(4),
#endif
};
static void           tc_interrupt_handler(void *p);
static int8_t         get_tc_index(const void *const hw);
static uint8_t        tc_get_hardware_index(const void *const hw);
static inline uint8_t _get_hardware_offset(const void *const hw);
/**
 * \brief Initialize TC
 */
int32_t _timer_init(struct _timer_device *const device, void *const hw)
{
	int8_t i = get_tc_index(hw);

	device->hw = hw;
	ASSERT(ARRAY_SIZE(_tcs));

	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_SWRST);

	if (hri_tc_get_CTRLA_ENABLE_bit(hw)) {
		return ERR_DENIED;
	}

	hri_tc_set_CTRLA_SWRST_bit(hw);
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_SWRST);

	hri_tc_write_CTRLA_reg(hw, _tcs[i].ctrl_a);
	hri_tc_write_DBGCTRL_reg(hw, _tcs[i].dbg_ctrl);
	hri_tc_write_EVCTRL_reg(hw, _tcs[i].event_ctrl);
	hri_tc_write_WAVE_reg(hw, TC_WAVE_WAVEGEN_MFRQ);

	if (( _tcs[i].ctrl_a & TC_CTRLA_MODE_Msk ) == TC_CTRLA_MODE_COUNT32) {
		hri_tccount32_write_CC_reg(hw, 0, _tcs[i].cc0);
		hri_tccount32_write_CC_reg(hw, 1, _tcs[i].cc1);
	} else if (( _tcs[i].ctrl_a & TC_CTRLA_MODE_Msk ) ==
			TC_CTRLA_MODE_COUNT16) {
		hri_tccount16_write_CC_reg(hw, 0, (hri_tc_count16_reg_t)_tcs[i].cc0);
		hri_tccount16_write_CC_reg(hw, 1, (hri_tc_count16_reg_t)_tcs[i].cc1);
	} else if (( _tcs[i].ctrl_a & TC_CTRLA_MODE_Msk ) ==
			TC_CTRLA_MODE_COUNT8) {
		hri_tccount8_write_CC_reg(hw, 0, (hri_tc_count8_reg_t)_tcs[i].cc0);
		hri_tccount8_write_CC_reg(hw, 1, (hri_tc_count8_reg_t)_tcs[i].cc1);
		hri_tc_write_PER_reg(hw, _tcs[i].per);
	}

	hri_tc_set_INTEN_OVF_bit(hw);

	device->irq.handler = tc_interrupt_handler;
	device->irq.parameter = (void *)device;
	_irq_disable((IRQn_Type)((uint8_t)TC0_IRQn + _get_hardware_offset(hw)));
	_irq_clear((IRQn_Type)((uint8_t)TC0_IRQn + _get_hardware_offset(hw)));
	_irq_register((IRQn_Type)((uint8_t)TC0_IRQn + _get_hardware_offset(hw)),
			&device->irq);
	_irq_enable((IRQn_Type)((uint8_t)TC0_IRQn + _get_hardware_offset(hw)));

	return ERR_NONE;
}
/**
 * \brief De-initialize TC
 */
void _timer_deinit(struct _timer_device *const device)
{
	void *const hw = device->hw;

	_irq_disable((IRQn_Type)((uint8_t)TC0_IRQn + tc_get_hardware_index(hw)));

	hri_tc_clear_CTRLA_ENABLE_bit(hw);
	hri_tc_set_CTRLA_SWRST_bit(hw);
}
/**
 * \brief Retrieve offset of the given tc hardware instance
 */
uint8_t _timer_get_hardware_offset(const struct _timer_device *const device)
{
	return _get_hardware_offset(device->hw);
}
/**
 * \brief Start hardware timer
 */
void _timer_start(struct _timer_device *const device)
{
	hri_tc_set_CTRLA_ENABLE_bit(device->hw);
}
/**
 * \brief Stop hardware timer
 */
void _timer_stop(struct _timer_device *const device)
{
	hri_tc_clear_CTRLA_ENABLE_bit(device->hw);
}
/**
 * \brief Set timer period
 */
void _timer_set_period(struct _timer_device *const device,
		const uint32_t clock_cycles)
{
	void *const hw = device->hw;

	if (TC_CTRLA_MODE_COUNT32_Val == hri_tc_read_CTRLA_MODE_bf(hw)) {
		hri_tccount32_write_CC_reg(hw, 0, clock_cycles);
	} else if (TC_CTRLA_MODE_COUNT16_Val == hri_tc_read_CTRLA_MODE_bf(hw)) {
		hri_tccount16_write_CC_reg(hw, 0, (hri_tc_count16_reg_t)clock_cycles);
	} else if (TC_CTRLA_MODE_COUNT8_Val == hri_tc_read_CTRLA_MODE_bf(hw)) {
		hri_tc_write_PER_reg(hw, (hri_tc_per_reg_t)clock_cycles);
	}
}
/**
 * \brief Retrieve timer period
 */
uint32_t _timer_get_period(const struct _timer_device *const device)
{
	void *const hw = device->hw;

	if (TC_CTRLA_MODE_COUNT32_Val == hri_tc_read_CTRLA_MODE_bf(hw)) {
		return hri_tccount32_read_CC_reg(hw, 0);
	} else if (TC_CTRLA_MODE_COUNT16_Val == hri_tc_read_CTRLA_MODE_bf(hw)) {
		return hri_tccount16_read_CC_reg(hw, 0);
	} else if (TC_CTRLA_MODE_COUNT8_Val == hri_tc_read_CTRLA_MODE_bf(hw)) {
		return hri_tc_read_PER_reg(hw);
	}

	return 0;
}
/**
 * \brief Check if timer is running
 */
bool _timer_is_started(const struct _timer_device *const device)
{
	return hri_tc_get_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Retrieve timer helper functions
 */
struct _timer_hpl_interface *_tc_get_timer(void)
{
	return NULL;
}

/**
 * \brief Retrieve pwm helper functions
 */
struct _pwm_hpl_interface *_tc_get_pwm(void)
{
	return NULL;
}
/**
 * \brief Set timer IRQ
 *
 * \param[in] hw The pointer to hardware instance
 */
void _timer_set_irq(struct _timer_device *const device)
{
	void *const hw = device->hw;

	_irq_set((IRQn_Type)((uint8_t)TC0_IRQn + _get_hardware_offset(hw)));
}
/**
 * \internal TC interrupt handler
 *
 * \param[in] instance TC instance number
 */
static void tc_interrupt_handler(void *p)
{
	struct _timer_device *device = (struct _timer_device *)p;
	void *const hw = device->hw;

	if (hri_tc_get_interrupt_OVF_bit(hw)) {
		hri_tc_clear_interrupt_OVF_bit(hw);
		device->timer_cb.period_expired(device);
	}
}
/**
 * \internal Retrieve TC hardware index
 *
 * \param[in] hw The pointer to hardware instance
 */
static uint8_t tc_get_hardware_index(const void *const hw)
{
	if ((uint32_t)TC4 == (uint32_t)hw) {
		return 4;
	}

	return ((uint32_t)hw - (uint32_t)TC0 ) >> 10;
}

/**
 * \internal Retrieve TC index
 *
 * \param[in] hw The pointer to hardware instance
 *
 * \return The index of TC configuration
 */
static int8_t get_tc_index(const void *const hw)
{
	uint8_t index = tc_get_hardware_index(hw);
	uint8_t i;

	for (i = 0; i < ARRAY_SIZE(_tcs); i++) {
		if (_tcs[i].number == index) {
			return i;
		}
	}

	ASSERT(false);
	return -1;
}

static inline uint8_t _get_hardware_offset(const void *const hw)
{
	return (((uint32_t)hw - (uint32_t)TC0 ) >> 10 );
}
