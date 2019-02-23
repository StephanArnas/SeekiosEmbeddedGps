/**
 * \file
 *
 * \brief SAM Analog Digital Converter
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

#include <hpl_adc_sync.h>
#include <hpl_adc_async.h>
#include <utils_repeat_macro.h>
#include "hpl_adc2_v110_config.h"
#include <hri_adc2_v110.h>

#include <utils_assert.h>

#include <hpl_irq.h>

#ifndef CONF_ADC_0_ENABLE
#    define CONF_ADC_0_ENABLE 0
#endif
#ifndef CONF_ADC_1_ENABLE
#    define CONF_ADC_1_ENABLE 0
#endif

/**
 * \brief Macro is used to fill ADC configuration structure based on its number
 *
 * \param[in] n The number of structures
 */
#define ADC_CONFIGURATION(n) \
	{( CONF_ADC_ ## n ## _RUNSTDBY << ADC_CTRLA_RUNSTDBY_Pos ) | \
	 ( CONF_ADC_ ## n ## _ONDEMAND << ADC_CTRLA_ONDEMAND_Pos ),	 \
	 ADC_CTRLB_PRESCALER(CONF_ADC_ ## n ## _PRESCALER),	\
	 ( CONF_ADC_ ## n ## _REFCOMP << ADC_REFCTRL_REFCOMP_Pos ) |  \
	 ADC_REFCTRL_REFSEL(CONF_ADC_ ## n ## _REFSEL),	\
	 ( CONF_ADC_ ## n ## _WINMONEO << ADC_EVCTRL_WINMONEO_Pos ) | \
	 ( CONF_ADC_ ## n ## _RESRDYEO << ADC_EVCTRL_RESRDYEO_Pos ) | \
	 ( CONF_ADC_ ## n ## _STARTINV << ADC_EVCTRL_STARTINV_Pos ) | \
	 ( CONF_ADC_ ## n ## _FLUSHINV << ADC_EVCTRL_FLUSHINV_Pos ) | \
	 ( CONF_ADC_ ## n ## _STARTEI << ADC_EVCTRL_STARTEI_Pos ) |	\
	 ( CONF_ADC_ ## n ## _FLUSHEI << ADC_EVCTRL_FLUSHEI_Pos ), \
	 ADC_INPUTCTRL_MUXNEG(CONF_ADC_ ## n ## _MUXNEG) | \
	 ADC_INPUTCTRL_MUXPOS(CONF_ADC_ ## n ## _MUXPOS), \
	 ADC_CTRLC_WINMODE(CONF_ADC_ ## n ## _WINMODE) | \
	 ADC_CTRLC_RESSEL(CONF_ADC_ ## n ## _RESSEL) | \
	 ( CONF_ADC_ ## n ## _CORREN << ADC_CTRLC_CORREN_Pos ) | \
	 ( CONF_ADC_ ## n ## _FREERUN << ADC_CTRLC_FREERUN_Pos ) | \
	 ( CONF_ADC_ ## n ## _LEFTADJ << ADC_CTRLC_LEFTADJ_Pos ) | \
	 ( CONF_ADC_ ## n ## _DIFFMODE << ADC_CTRLC_DIFFMODE_Pos ),	\
	 ADC_AVGCTRL_ADJRES(CONF_ADC_ ## n ## _ADJRES) | \
	 ADC_AVGCTRL_SAMPLENUM(CONF_ADC_ ## n ## _SAMPLENUM), \
	 ( CONF_ADC_ ## n ## _OFFCOMP << ADC_SAMPCTRL_OFFCOMP_Pos ) | \
	 ADC_SAMPCTRL_SAMPLEN(CONF_ADC_ ## n ## _SAMPLEN), \
	 ADC_WINLT_WINLT(CONF_ADC_ ## n ## _WINLT),	\
	 ADC_WINUT_WINUT(CONF_ADC_ ## n ## _WINUT),	\
	 ADC_GAINCORR_GAINCORR(CONF_ADC_ ## n ## _GAINCORR), \
	 ADC_OFFSETCORR_OFFSETCORR(CONF_ADC_ ## n ## _OFFSETCORR), \
	 CONF_ADC_ ## n ## _DBGRUN << ADC_DBGCTRL_DBGRUN_Pos, \
	 ADC_SEQCTRL_SEQEN(CONF_ADC_ ## n ## _SEQEN), \
	},

/**
 * \brief ADC configuration
 */
struct adc_configuration {
	hri_adc_ctrla_reg_t ctrl_a;
	hri_adc_ctrlb_reg_t ctrl_b;
	hri_adc_refctrl_reg_t ref_ctrl;
	hri_adc_evctrl_reg_t ev_ctrl;
	hri_adc_inputctrl_reg_t input_ctrl;
	hri_adc_ctrlc_reg_t ctrl_c;
	hri_adc_avgctrl_reg_t avg_ctrl;
	hri_adc_sampctrl_reg_t samp_ctrl;
	hri_adc_winlt_reg_t win_lt;
	hri_adc_winut_reg_t win_ut;
	hri_adc_gaincorr_reg_t gain_corr;
	hri_adc_offsetcorr_reg_t offset_corr;
	hri_adc_dbgctrl_reg_t dbg_ctrl;
	hri_adc_seqctrl_reg_t seq_ctrl;
};

/**
 * \brief Array of ADC configurations
 */
static const struct adc_configuration _adcs[] = {
#if CONF_ADC_0_ENABLE == 1
		ADC_CONFIGURATION(0)
#endif
#if CONF_ADC_1_ENABLE == 1
		ADC_CONFIGURATION(1)
#endif
};

static uint8_t _adc_get_irq_num(
		const struct _adc_async_device *const device);
static void        _adc_interrupt_handler(void *p);
static inline void _adc_deinit(void *const hw);
static int32_t     _adc_init(void *const hw, const uint8_t i);
static void        _adc_set_reference_source(void *const hw,
		const adc_reference_t reference);

/**
 * \brief Initialize ADC
 */
int32_t _adc_sync_init(struct _adc_sync_device *const device, void *const hw)
{
	ASSERT(device);

	device->hw = hw;

	return _adc_init(hw, _adc_sync_get_hardware_index(device));
}

/**
 * \brief Initialize ADC
 */
int32_t _adc_async_init(struct _adc_async_device *const device, void *const hw)
{
	int32_t init_status;

	ASSERT(device);

	init_status = _adc_init(hw, _adc_async_get_hardware_index(device));

	if (init_status) {
		return init_status;
	}

	device->hw = hw;

	device->irq.handler = _adc_interrupt_handler;
	device->irq.parameter = (void *)device;
	_irq_disable(_adc_get_irq_num(device));
	_irq_clear(_adc_get_irq_num(device));
	_irq_register(_adc_get_irq_num(device), &device->irq);
	_irq_enable(_adc_get_irq_num(device));

	return ERR_NONE;
}

/**
 * \brief De-initialize ADC
 */
void _adc_sync_deinit(struct _adc_sync_device *const device)
{
	_adc_deinit(device->hw);
}

/**
 * \brief De-initialize ADC
 */
void _adc_async_deinit(struct _adc_async_device *const device)
{
	_irq_disable(_adc_get_irq_num(device));
	_irq_clear(_adc_get_irq_num(device));

	_adc_deinit(device->hw);
}

/**
 * \brief Enable ADC
 */
void _adc_sync_enable(struct _adc_sync_device *const device)
{
	hri_adc_set_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Enable ADC
 */
void _adc_async_enable(struct _adc_async_device *const device)
{
	hri_adc_set_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Disable ADC
 */
void _adc_sync_disable(struct _adc_sync_device *const device)
{
	hri_adc_clear_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Disable ADC
 */
void _adc_async_disable(struct _adc_async_device *const device)
{
	hri_adc_clear_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Retrieve ordinal number of the given adc hardware instance
 */
uint8_t _adc_sync_get_hardware_index(
		const struct _adc_sync_device *const device)
{
	(void)device;
	return 0;
}

/**
 * \brief Retrieve ordinal number of the given adc hardware instance
 */
uint8_t _adc_async_get_hardware_index(
		const struct _adc_async_device *const device)
{
	(void)device;
	return 0;
}

/**
 * \brief Retrieve adc hardware instance by its ordinal number
 */
void *_adc_sync_get_hardware_by_index(const uint8_t index)
{
	(void)index;
	return (void *)((uint32_t)ADC );
}

/**
 * \brief Retrieve adc hardware instance by its ordinal number
 */
void *_adc_async_get_hardware_by_index(const uint8_t index)
{
	(void)index;
	return (void *)((uint32_t)ADC );
}

/**
 * \brief Retrieve ADC conversion data size
 */
uint8_t _adc_sync_get_data_size(const struct _adc_sync_device *const device)
{
	return hri_adc_read_CTRLC_RESSEL_bf(device->hw) ==
		   ADC_CTRLC_RESSEL_8BIT_Val ? 1 : 2;
}

/**
 * \brief Retrieve ADC conversion data size
 */
uint8_t _adc_async_get_data_size(const struct _adc_async_device *const device)
{
	return hri_adc_read_CTRLC_RESSEL_bf(device->hw) ==
		   ADC_CTRLC_RESSEL_8BIT_Val ? 1 : 2;
}

/**
 * \brief Check if conversion is done
 */
bool _adc_sync_is_conversion_done(const struct _adc_sync_device *const device)
{
	return hri_adc_get_interrupt_RESRDY_bit(device->hw);
}

/**
 * \brief Check if conversion is done
 */
bool _adc_async_is_conversion_done(const struct _adc_async_device *const device)
{
	return hri_adc_get_interrupt_RESRDY_bit(device->hw);
}

/**
 * \brief Make conversion
 */
void _adc_sync_convert(struct _adc_sync_device *const device)
{
	hri_adc_set_SWTRIG_START_bit(device->hw);
}

/**
 * \brief Make conversion
 */
void _adc_async_convert(struct _adc_async_device *const device)
{
	hri_adc_set_SWTRIG_START_bit(device->hw);
}

/**
 * \brief Retrieve the conversion result
 */
uint16_t _adc_sync_read_data(const struct _adc_sync_device *const device)
{
	return hri_adc_read_RESULT_reg(device->hw);
}

/**
 * \brief Retrieve the conversion result
 */
uint16_t _adc_async_read_data(const struct _adc_async_device *const device)
{
	return hri_adc_read_RESULT_reg(device->hw);
}

/**
 * \brief Set reference source
 */
void _adc_sync_set_reference_source(struct _adc_sync_device *const device,
		const adc_reference_t reference)
{
	_adc_set_reference_source(device->hw, reference);
}

/**
 * \brief Set reference source
 */
void _adc_async_set_reference_source(struct _adc_async_device *const device,
		const adc_reference_t reference)
{
	_adc_set_reference_source(device->hw, reference);
}

/**
 * \brief Set resolution
 */
void _adc_sync_set_resolution(struct _adc_sync_device *const device,
		const adc_resolution_t resolution)
{
	hri_adc_write_CTRLC_RESSEL_bf(device->hw, resolution);
}

/**
 * \brief Set resolution
 */
void _adc_async_set_resolution(struct _adc_async_device *const device,
		const adc_resolution_t resolution)
{
	hri_adc_write_CTRLC_RESSEL_bf(device->hw, resolution);
}

/**
 * \brief Set positive channel source
 */
void _adc_sync_set_pos_channel(struct _adc_sync_device *const device,
		const adc_pos_channel_t channel)
{
	hri_adc_write_INPUTCTRL_MUXPOS_bf(device->hw, channel);
}

/**
 * \brief Set positive channel source
 */
void _adc_async_set_pos_channel(struct _adc_async_device *const device,
		const adc_pos_channel_t channel)
{
	hri_adc_write_INPUTCTRL_MUXPOS_bf(device->hw, channel);
}

/**
 * \brief Set negative channel source
 */
void _adc_sync_set_neg_channel(struct _adc_sync_device *const device,
		const adc_neg_channel_t channel)
{
	hri_adc_write_INPUTCTRL_MUXNEG_bf(device->hw, channel);
}

/**
 * \brief Set negative channel source
 */
void _adc_async_set_neg_channel(struct _adc_async_device *const device,
		const adc_neg_channel_t channel)
{
	hri_adc_write_INPUTCTRL_MUXNEG_bf(device->hw, channel);
}

/**
 * \brief Set lower threshold
 */
void _adc_sync_set_low_threshold(struct _adc_sync_device *const device,
		const adc_threshold_t threshold)
{
	hri_adc_write_WINLT_reg(device->hw, threshold);
}

/**
 * \brief Set lower threshold
 */
void _adc_async_set_low_threshold(struct _adc_async_device *const device,
		const adc_threshold_t threshold)
{
	hri_adc_write_WINLT_reg(device->hw, threshold);
}

/**
 * \brief Set upper threshold
 */
void _adc_sync_set_up_threshold(struct _adc_sync_device *const device,
		const adc_threshold_t threshold)
{
	hri_adc_write_WINUT_reg(device->hw, threshold);
}

/**
 * \brief Set upper threshold
 */
void _adc_async_set_up_threshold(struct _adc_async_device *const device,
		const adc_threshold_t threshold)
{
	hri_adc_write_WINUT_reg(device->hw, threshold);
}

/**
 * \brief Set gain
 */
void _adc_sync_set_gain(struct _adc_sync_device *const device,
		const adc_gain_t gain)
{
	(void)device, (void)gain;
}

/**
 * \brief Set gain
 */
void _adc_async_set_gain(struct _adc_async_device *const device,
		const adc_gain_t gain)
{
	(void)device, (void)gain;
}

/**
 * \brief Set conversion mode
 */
void _adc_sync_set_conversion_mode(struct _adc_sync_device *const device,
		const enum adc_conversion_mode mode)
{
	if (ADC_CONVERSION_MODE_FREERUN == mode) {
		hri_adc_set_CTRLC_FREERUN_bit(device->hw);
	} else {
		hri_adc_clear_CTRLC_FREERUN_bit(device->hw);
	}
}

/**
 * \brief Set conversion mode
 */
void _adc_async_set_conversion_mode(struct _adc_async_device *const device,
		const enum adc_conversion_mode mode)
{
	if (ADC_CONVERSION_MODE_FREERUN == mode) {
		hri_adc_set_CTRLC_FREERUN_bit(device->hw);
	} else {
		hri_adc_clear_CTRLC_FREERUN_bit(device->hw);
	}
}

/**
 * \brief Set differential mode
 */
void _adc_sync_set_differential_mode(struct _adc_sync_device *const device,
		const enum adc_differential_mode mode)
{
	if (ADC_DIFFERENTIAL_MODE_DIFFERENTIAL == mode) {
		hri_adc_set_CTRLC_DIFFMODE_bit(device->hw);
	} else {
		hri_adc_clear_CTRLC_DIFFMODE_bit(device->hw);
	}
}

/**
 * \brief Set differential mode
 */
void _adc_async_set_differential_mode(struct _adc_async_device *const device,
		const enum adc_differential_mode mode)
{
	if (ADC_DIFFERENTIAL_MODE_DIFFERENTIAL == mode) {
		hri_adc_set_CTRLC_DIFFMODE_bit(device->hw);
	} else {
		hri_adc_clear_CTRLC_DIFFMODE_bit(device->hw);
	}
}

/**
 * \brief Set window mode
 */
void _adc_sync_set_window_mode(struct _adc_sync_device *const device,
		const adc_window_mode_t mode)
{
	hri_adc_write_CTRLC_WINMODE_bf(device->hw, mode);
}

/**
 * \brief Set window mode
 */
void _adc_async_set_window_mode(struct _adc_async_device *const device,
		const adc_window_mode_t mode)
{
	hri_adc_write_CTRLC_WINMODE_bf(device->hw, mode);
}

/**
 * \brief Retrieve threshold state
 */
void _adc_sync_get_threshold_state(const struct _adc_sync_device *const device,
		adc_threshold_status_t *const state)
{
	*state = hri_adc_get_interrupt_WINMON_bit(device->hw);
}

/**
 * \brief Retrieve threshold state
 */
void _adc_async_get_threshold_state(
		const struct _adc_async_device *const device,
		adc_threshold_status_t *const state)
{
	*state = hri_adc_get_interrupt_WINMON_bit(device->hw);
}

/**
 * \brief Enable/disable ADC interrupt
 *
 * param[in] device The pointer to ADC device instance
 * param[in] type The type of interrupt to disable/enable if applicable
 * param[in] state Enable or disable
 */
void _adc_async_set_irq_state(struct _adc_async_device *const device,
		const enum _adc_async_callback_type type,
		const bool state)
{
	void *const hw = device->hw;

	if (ADC_ASYNC_DEVICE_MONITOR_CB == type) {
		hri_adc_write_INTEN_WINMON_bit(hw, state);
	}
	else if (ADC_ASYNC_DEVICE_ERROR_CB == type) {
		hri_adc_write_INTEN_OVERRUN_bit(hw, state);
	}
	else if (ADC_ASYNC_DEVICE_CONVERT_CB == type) {
		hri_adc_write_INTEN_RESRDY_bit(hw, state);
	}
}

/**
 * \internal ADC interrupt handler
 *
 * \param[in] p The pointer to interrupt parameter
 */
static void _adc_interrupt_handler(void *p)
{
	struct _adc_async_device *device = (struct _adc_async_device *)p;
	void *const hw = device->hw;

	if  (hri_adc_get_interrupt_RESRDY_bit(hw)) {
		hri_adc_clear_interrupt_RESRDY_bit(hw);
		device->adc_async_cb.convert_done(device, hri_adc_read_RESULT_reg(hw));
	} else if (hri_adc_get_interrupt_OVERRUN_bit(hw)) {
		hri_adc_clear_interrupt_OVERRUN_bit(hw);
		device->adc_async_cb.error_cb(device);
	} else if (hri_adc_get_interrupt_WINMON_bit(hw)) {
		hri_adc_clear_interrupt_WINMON_bit(hw);
		device->adc_async_cb.window_cb(device);
	}
}

/**
 * \brief Retrieve IRQ number for the given hardware instance
 */
static uint8_t _adc_get_irq_num(const struct _adc_async_device *const device)
{
	(void)device;
	return ADC_IRQn;
}

/**
 * \brief Initialize ADC
 *
 * \param[in] hw The pointer to hardware instance
 * \param[in] i The number of hardware instance
 */
static int32_t _adc_init(void *const hw, const uint8_t i)
{
	ASSERT(hw == ADC);

	hri_adc_wait_for_sync(hw, ADC_SYNCBUSY_SWRST);

	if (hri_adc_get_CTRLA_ENABLE_bit(hw)) {
		return ERR_DENIED;
	}

	hri_adc_set_CTRLA_SWRST_bit(hw);
	hri_adc_wait_for_sync(hw, ADC_SYNCBUSY_SWRST);

	hri_adc_write_CTRLB_reg(hw, _adcs[i].ctrl_b);
	hri_adc_write_REFCTRL_reg(hw, _adcs[i].ref_ctrl);
	hri_adc_write_EVCTRL_reg(hw, _adcs[i].ev_ctrl);
	hri_adc_write_INPUTCTRL_reg(hw, _adcs[i].input_ctrl);
	hri_adc_write_CTRLC_reg(hw, _adcs[i].ctrl_c);
	hri_adc_write_AVGCTRL_reg(hw, _adcs[i].avg_ctrl);
	hri_adc_write_SAMPCTRL_reg(hw, _adcs[i].samp_ctrl);
	hri_adc_write_WINLT_reg(hw, _adcs[i].win_lt);
	hri_adc_write_WINUT_reg(hw, _adcs[i].win_ut);
	hri_adc_write_GAINCORR_reg(hw, _adcs[i].gain_corr);
	hri_adc_write_OFFSETCORR_reg(hw, _adcs[i].offset_corr);
	hri_adc_write_DBGCTRL_reg(hw, _adcs[i].dbg_ctrl);
	hri_adc_write_SEQCTRL_reg(hw, _adcs[i].seq_ctrl);
	hri_adc_write_CTRLA_reg(hw, _adcs[i].ctrl_a);

	return ERR_NONE;
}

/**
 * \brief De-initialize ADC
 *
 * \param[in] hw The pointer to hardware instance
 */
static inline void _adc_deinit(void *hw)
{
	hri_adc_clear_CTRLA_ENABLE_bit(hw);
	hri_adc_set_CTRLA_SWRST_bit(hw);
}

/**
 * \brief Set ADC reference source
 *
 * \param[in] hw The pointer to hardware instance
 * \param[in] reference The reference to set
 */
static void _adc_set_reference_source(void *const hw,
		const adc_reference_t reference)
{
	bool enabled = hri_adc_get_CTRLA_ENABLE_bit(hw);

	hri_adc_clear_CTRLA_ENABLE_bit(hw);
	hri_adc_write_REFCTRL_REFSEL_bf(hw, reference);

	if (enabled) {
		hri_adc_set_CTRLA_ENABLE_bit(hw);
	}
}
