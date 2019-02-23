/**
 * \file
 *
 * \brief ADC related functionality declaration.
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

#ifndef _HPL_ADC_SYNC_H_INCLUDED
#define _HPL_ADC_SYNC_H_INCLUDED

/**
 * \addtogroup HPL ADC
 *
 * \section hpl_adc_sync_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include "compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief ADC reference source
 */
typedef uint8_t adc_reference_t;

/**
 * \brief ADC resolution
 */
typedef uint8_t adc_resolution_t;

/**
 * \brief ADC positive channel
 */
typedef uint8_t adc_pos_channel_t;

/**
 * \brief ADC negative channel
 */
typedef uint8_t adc_neg_channel_t;

/**
 * \brief ADC threshold
 */
typedef uint16_t adc_threshold_t;

/**
 * \brief ADC gain
 */
typedef uint8_t adc_gain_t;

/**
 * \brief ADC conversion mode
 */
enum adc_conversion_mode {
	ADC_CONVERSION_MODE_SINGLE_CONVERSION = 0,
	ADC_CONVERSION_MODE_FREERUN
};

/**
 * \brief ADC differential mode
 */
enum adc_differential_mode {
	ADC_DIFFERENTIAL_MODE_SINGLE_ENDED = 0,
	ADC_DIFFERENTIAL_MODE_DIFFERENTIAL
};

/**
 * \brief ADC window mode
 */
typedef uint8_t adc_window_mode_t;

/**
 * \brief ADC threshold status
 */
typedef bool adc_threshold_status_t;

/**
 * \brief ADC sync descriptor device structure
 */
struct _adc_sync_device {
	void *hw;
};

/**
 * \name HPL functions
 */
//@{
/**
 * \brief Initialize synchronous ADC
 *
 * This function does low level ADC configuration.
 *
 * param[in] device The pointer to ADC device instance
 * param[in] hw The pointer to hardware instance
 *
 * \return Initialization status
 */
int32_t _adc_sync_init(struct _adc_sync_device *const device,
		void *const hw);

/**
 * \brief De-initialize ADC
 *
 * \param[in] device The pointer to ADC device instance
 */
void _adc_sync_deinit(struct _adc_sync_device *const device);

/**
 * \brief Enable ADC peripheral
 *
 * \param[in] device The pointer to ADC device instance
 */
void _adc_sync_enable(struct _adc_sync_device *const device);

/**
 * \brief Disable ADC peripheral
 *
 * \param[in] device The pointer to ADC device instance
 */
void _adc_sync_disable(struct _adc_sync_device *const device);

/**
 * \brief Retrieve ordinal number of the given adc hardware instance
 *
 * \param[in] device The pointer to ADC device instance
 *
 * \return The ordinal number of the given adc hardware instance
 */
uint8_t _adc_sync_get_hardware_index(const struct _adc_sync_device *const device);

/**
 * \brief Retrieve adc hardware instance by its ordinal number
 *
 * \param[in] instance The ordinal number of adc hardware instance
 *
 * \return The pointer to adc hardware instance
 */
void * _adc_sync_get_hardware_by_index(const uint8_t index);

/**
 * \brief Retrieve ADC conversion data size
 *
 * \param[in] device The pointer to ADC device instance
 *
 * \return The data size in bytes
 */
uint8_t _adc_sync_get_data_size(const struct _adc_sync_device *const device);

/**
 * \brief Check if conversion is done
 *
 * \param[in] device The pointer to ADC device instance
 *
 * \return The status of conversion
 * \retval true The conversion is done
 * \retval false The conversion is not done
 */
bool _adc_sync_is_conversion_done(const struct _adc_sync_device *const device);

/**
 * \brief Make conversion
 *
 * \param[in] device The pointer to ADC device instance
 */
void _adc_sync_convert(struct _adc_sync_device *const device);

/**
 * \brief Retrieve the conversion result
 *
 * \param[in] device The pointer to ADC device instance
 *
 * The result value
 */
uint16_t _adc_sync_read_data(const struct _adc_sync_device *const device);

/**
 * \brief Set reference source
 *
 * \param[in] device The pointer to ADC device instance
 * \param[in] reference A reference source to set
 */
void _adc_sync_set_reference_source(struct _adc_sync_device *const device,
		const adc_reference_t reference);

/**
 * \brief Set resolution
 *
 * \param[in] device The pointer to ADC device instance
 * \param[in] resolution A resolution to set
 */
void _adc_sync_set_resolution(struct _adc_sync_device *const device,
		const adc_resolution_t resolution);

/**
 * \brief Set positive channel source
 *
 * \param[in] device The pointer to ADC device instance
 * \param[in] channel A positive channel source to set
 */
void _adc_sync_set_pos_channel(struct _adc_sync_device *const device,
		const adc_pos_channel_t channel);

/**
 * \brief Set negative channel source
 *
 * \param[in] device The pointer to ADC device instance
 * \param[in] channel A negative channel source to set
 */
void _adc_sync_set_neg_channel(struct _adc_sync_device *const device,
		const adc_neg_channel_t channel);

/**
 * \brief Set lower threshold
 *
 * \param[in] device The pointer to ADC device instance
 * \param[in] threshold A lower threshold to set
 */
void _adc_sync_set_low_threshold(struct _adc_sync_device *const device,
		const adc_threshold_t threshold);

/**
 * \brief Set upper threshold
 *
 * \param[in] device The pointer to ADC device instance
 * \param[in] threshold A upper threshold to set
 */
void _adc_sync_set_up_threshold(struct _adc_sync_device *const device,
		const adc_threshold_t threshold);

/**
 * \brief Set gain
 *
 * \param[in] device The pointer to ADC device instance
 * \param[in] gain A gain to set
 */
void _adc_sync_set_gain(struct _adc_sync_device *const device, const adc_gain_t gain);

/**
 * \brief Set conversion mode
 *
 * \param[in] device The pointer to ADC device instance
 * \param[in] mode A conversion mode to set
 */
void _adc_sync_set_conversion_mode(struct _adc_sync_device *const device,
		const enum adc_conversion_mode mode);

/**
 * \brief Set differential mode
 *
 * \param[in] device The pointer to ADC device instance
 * \param[in] mode A differential mode to set
 */
void _adc_sync_set_differential_mode(struct _adc_sync_device *const device,
		const enum adc_differential_mode mode);

/**
 * \brief Set window mode
 *
 * \param[in] device The pointer to ADC device instance
 * \param[in] mode A mode to set
 */
void _adc_sync_set_window_mode(struct _adc_sync_device *const device,
		const adc_window_mode_t mode);

/**
 * \brief Retrieve threshold state
 *
 * \param[in] device The pointer to ADC device instance
 * \param[out] state The threshold state
 */
void _adc_sync_get_threshold_state(const struct _adc_sync_device *const device,
		adc_threshold_status_t *const state);

//@}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_ADC_SYNC_H_INCLUDED */
