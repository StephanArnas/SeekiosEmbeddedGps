/**
 * \file
 *
 * \brief RTOS HAL API declaration.
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
#ifndef _HAL_RTOS_H_INCLUDED
#define _HAL_RTOS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#ifndef is_in_isr
#   include "hpl_core.h"
/* Extern function implemented to check if it's in ISR.
 * Can directly check CPU core register, e.g., for cortex-m0p:
 * (*(uint32_t*)0xE000ED04 & 0x1ff) 
 */
#   define is_in_isr() _is_in_isr()
#endif

#include "compiler.h"
#include "rtos_port.h"

/* Semaphore */
#ifdef SEMAPHORE_ENABLED

/**
 * \brief Semaphore initialization
 *
 * \param[in] sem The pointer to a sem_t instance
 * \param[in] count Initializing semaphore number
 *
 * \return The result of initialization 
 * return 0 for success
 * return <0 for error
 */
int32_t sem_init(sem_t *sem, uint32_t count);

/**
 * \brief Semaphore up 
 *
 * \param[in] sem The pointer to a sem_t instance
 *
 * \return The result of up operation
 * return 0 for success
 * return <0 for error
 */
int32_t sem_up(sem_t *sem);

/**
 * \brief Semaphore down, may suspend the caller thread
 *
 * \param[in] sem The pointer to a sem_t instance
 * \param[in] timeout Timeout in ticks
 *
 * \return The result of down operation
 * \retval 0 for success
 * \retval <0 for error
 */
int32_t sem_down(sem_t *sem, uint32_t timeout);

/**
 * \brief Semaphore deinitialization
 *
 * \param[in] sem The pointer to a sem_t instance
 *
 * \return The result of deinitialize semaphore
 * \retval 0 for success
 * \retval <0 for error
 */
int32_t sem_deinit(sem_t *sem);

#endif

/* os lock */

#if !defined(os_lock) || !defined(os_unlock)
#undef os_lock
#undef os_unlock

#include "hal_atomic.h"

static volatile hal_atomic_t interrupt_flags;
static int lock_nesting;

#define os_lock() \
	do { \
		if (lock_nesting == 0) { \
			atomic_enter_critical(&interrupt_flags); \
		} \
\
		lock_nesting++; \
	} while (0)

#define os_unlock() \
	do { \
		lock_nesting--; \
\
		if (lock_nesting == 0) { \
			atomic_leave_critical(&interrupt_flags); \
		} \
	} while (0)

#endif

/* os sleep */
#if !defined(os_sleep)
#warning "No sleep method is provided by RTOS"
#endif
#ifdef __cplusplus
}
#endif

#endif

