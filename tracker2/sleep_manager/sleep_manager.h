/**
 * \file
 *
 * \brief Sleep manager declaration.
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

#ifndef _SLEEP_MANAGER_H_INCLUDED
#define _SLEEP_MANAGER_H_INCLUDED

#ifdef __cplusplus
	extern "C" {
#endif

#include <utils_event.h>

/**
 * \brief Wake up sources.
 */
enum sleepmgr_wakeup_source {
	SM_WAKEUP_SOURCE_EXT_IRQ,
	SM_WAKEUP_SOURCE_IRQ
};

struct sleepmgr_ready_to_sleep;

/**
 * \brief The type of a callback called before going to sleep
 */
typedef void (*sleepmgr_ready_to_sleep_callback_t)(
		struct sleepmgr_ready_to_sleep *const ready);

/**
 * \brief The type of a callback called right before going to sleep
 */
typedef void (*sleepmgr_prepare_to_sleep_callback_t)(const uint8_t mode);

/**
 * \brief The type of a callback called after waking up
 */
typedef void (*sleepmgr_wake_up_callback_t)(
		const enum sleepmgr_wakeup_source source);

/**
 * \brief The is ready to sleep callback structure
 */
struct sleepmgr_ready_to_sleep_cb {
	struct is_ready_to_sleep_service {
		struct list_element elem; /*! The pointer to next callback */
	} service; /*! For internal use only */
	/*! The callback to be called to ask modules about thier active status */
	sleepmgr_ready_to_sleep_callback_t cb;
};

/**
 * \brief The prepare to sleep callback structure
 */
struct sleepmgr_prepare_to_sleep_cb {
	struct prepare_to_sleep_service {
		struct list_element elem; /*! The pointer to next callback */
	} service; /*! For internal use only */
	/*! The callback to be called right before going to sleep */
	sleepmgr_prepare_to_sleep_callback_t cb;
};

/**
 * \brief The wake up callback structure
 */
struct sleepmgr_wake_up_cb {
	struct wake_up_service {
		struct list_element elem; /*! The pointer to next callback */
	} service; /*! For internal use only */
	/*! The callback to be called after waking up */
	sleepmgr_wake_up_callback_t cb;
};

/**
 * \brief The structure used in ready to sleep callback
 */
struct sleepmgr_ready_to_sleep {
	bool ready; /*! Ready to sleep flag */
	uint8_t mode; /*! The sleep mode to be used */
};

/**
 * \brief Initialize sleep manager
 */
void sleepmgr_init(void);

/**
 * \brief Register is ready to sleep callback
 *
 * \param[in] cb Callback to register
 */
void sleepmgr_register_ready_to_sleep_callback(
		struct sleepmgr_ready_to_sleep_cb *const cb);

/**
 * \brief Register wake up callback
 *
 * \param[in] cb Callback to register
 */
void sleepmgr_register_prepare_to_sleep_callback(
		struct sleepmgr_prepare_to_sleep_cb *const cb);

/**
 * \brief Register wake up callback
 *
 * \param[in] cb Callback to register
 */
void sleepmgr_register_wake_up_callback(struct sleepmgr_wake_up_cb *const cb);

/**
 * \brief Go to sleep
 *
 * \param[in] mode Sleep mode to use
 */
void sleepmgr_sleep(const uint8_t mode);

/**
 * \brief Send wake-up notification
 */
void sleepmgr_wakeup(const enum sleepmgr_wakeup_source source);

/**
 * \brief Check if device is ready to sleep
 *
 * \param[in] mode Sleep mode to use
 *
 * \return True if device is ready to sleep, false otherwise
 */
bool sleepmgr_is_ready_to_sleep(const uint8_t mode);

#ifdef __cplusplus
	}
#endif

#endif /* _SLEEP_MANAGER_H_INCLUDED */
