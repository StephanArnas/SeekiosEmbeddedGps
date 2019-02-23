/**
 * \file
 *
 * \brief USART related functionality declaration.
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

#ifndef _HAL_SYNC_USART_H_INCLUDED
#define _HAL_SYNC_USART_H_INCLUDED

#include "hal_io.h"
#include <hpl_usart_sync.h>

/**
 * \addtogroup usart HAL Synchronous USART driver
 *
 * \section hal_usart_sync_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Synchronous USART descriptor
 */
struct usart_sync_descriptor {
	struct io_descriptor io;
	struct _usart_sync_device device;
};

/**
 * \brief Initialize usart interface
 *
 * This function initializes the given IO descriptor to be used
 * as USART interface descriptor.
 * It checks if the given hardware is not initialized and
 * if the given hardware is permitted to be initialized.
 *
 * \param[out] descr An usart descriptor which is used to communicate via USART
 * \param[in] hw The pointer to hardware instance
 *
 * \return Initialization status.
 */
int32_t usart_sync_init(struct usart_sync_descriptor *const descr,
		void *const hw);

/**
 * \brief De-initialize usart interface
 *
 * This function de-initializes the given IO descriptor.
 * It checks if the given hardware is initialized and
 * if the given hardware is permitted to be de-initialized.
 *
 * \param[in] descr An usart descriptor which is used to communicate via USART
 *
 * \return De-initialization status.
 */
int32_t usart_sync_deinit(struct usart_sync_descriptor *const descr);

/**
 * \brief Enable usart interface
 *
 * Enables the usart interface
 *
 * \param[in] descr An usart descriptor which is used to communicate via USART
 *
 * \return Enabling status.
 */
int32_t usart_sync_enable(struct usart_sync_descriptor *const descr);

/**
 * \brief Disable usart interface
 *
 * Disables the usart interface
 *
 * \param[in] descr An usart descriptor which is used to communicate via USART
 *
 * \return Disabling status.
 */
int32_t usart_sync_disable(struct usart_sync_descriptor *const descr);

/**
 * \brief Retrieve IO descriptor
 *
 * This function retrieves the IO descriptor of the given USART descriptor.
 *
 * \param[in] descr An usart descriptor which is used to communicate via USART
 * \param[out] io An io descriptor to retrieve
 *
 * \return The status of IO descriptor retrieving.
 */
int32_t usart_sync_get_io_descriptor(
		struct usart_sync_descriptor *const descr,
		struct io_descriptor **io);

/**
 * \brief Specify action for flow control pins
 *
 * This function sets action (or state) for flow control pins
 * if the flow control is enabled.
 * It sets state of flow control pins only if automatic support of
 * the flow control is not supported by the hardware.
 *
 * \param[in] descr An usart descriptor which is used to communicate via USART
 * \param[in] state A state to set to flow control pins
 *
 * \return The status of flow control action setup.
 */
int32_t usart_sync_set_flow_control(struct usart_sync_descriptor *const descr,
		const union usart_flow_control_state state);

/**
 * \brief Set usart baud rate
 *
 * \param[in] descr An usart descriptor which is used to communicate via USART
 * \param[in] baud_rate A baud rate to set
 *
 * \return The status of baud rate setting.
 */
int32_t usart_sync_set_baud_rate(struct usart_sync_descriptor *const descr,
		const uint32_t baud_rate);

/**
 * \brief Set usart data order
 *
 * \param[in] descr An usart descriptor which is used to communicate via USART
 * \param[in] data_order A data order to set
 *
 * \return The status of data order setting.
 */
int32_t usart_sync_set_data_order(struct usart_sync_descriptor *const descr,
		const enum usart_data_order data_order);

/**
 * \brief Set usart mode
 *
 * \param[in] descr An usart descriptor which is used to communicate via USART
 * \param[in] mode A mode to set
 *
 * \return The status of mode setting.
 */
int32_t usart_sync_set_mode(struct usart_sync_descriptor *const descr,
		const enum usart_mode mode);

/**
 * \brief Set usart parity
 *
 * \param[in] descr An usart descriptor which is used to communicate via USART
 * \param[in] parity A parity to set
 *
 * \return The status of parity setting.
 */
int32_t usart_sync_set_parity(struct usart_sync_descriptor *const descr,
		const enum usart_parity parity);

/**
 * \brief Set usart stop bits
 *
 * \param[in] descr An usart descriptor which is used to communicate via USART
 * \param[in] stop_bits Stop bits to set
 *
 * \return The status of stop bits setting.
 */
int32_t usart_sync_set_stopbits(struct usart_sync_descriptor *const descr,
		const enum usart_stop_bits stop_bits);

/**
 * \brief Set usart character size
 *
 * \param[in] descr An usart descriptor which is used to communicate via USART
 * \param[in] size A character size to set
 *
 * \return The status of character size setting.
 */
int32_t usart_sync_set_character_size(struct usart_sync_descriptor *const descr,
		const enum usart_character_size size);

/**
 * \brief Retrieve the state of flow control pins
 *
 * This function retrieves the of flow control pins
 * if the flow control is enabled.
 * Function can return USART_FLOW_CONTROL_STATE_UNAVAILABLE in case
 * if the flow control is done by the hardware
 * and pins state can not be read out.
 *
 * \param[in] descr An usart descriptor which is used to communicate via USART
 * \param[out] state The state of flow control pins
 *
 * \return The status of flow control state reading.
 */
int32_t usart_sync_flow_control_status(
		const struct usart_sync_descriptor *const descr,
		union usart_flow_control_state *const state);

/**
 * \brief Check if the usart transmitter is empty
 *
 * \param[in] descr An usart descriptor which is used to communicate via USART
 *
 * \return The status of USART TX empty checking.
 * \retval 0 the usart transmitter is not empty
 * \retval 1 the usart transmitter is empty
 */
int32_t usart_sync_is_tx_empty(
		const struct usart_sync_descriptor *const descr);

/**
 * \brief Check if the usart receiver is not empty
 *
 * \param[in] descr An usart descriptor which is used to communicate via USART
 *
 * \return The status of USART RX empty checking.
 * \retval 1 the usart receiver is not empty
 * \retval 0 the usart receiver is empty
 */
int32_t usart_sync_is_rx_not_empty(
		const struct usart_sync_descriptor *const descr);

/**
 * \brief Retrieve the current driver version
 *
 * \return Current driver version
 */
uint32_t usart_sync_get_version(void);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HAL_SYNC_USART_H_INCLUDED */
