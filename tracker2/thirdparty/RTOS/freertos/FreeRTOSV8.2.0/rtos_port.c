/**
 * \file
 *
 * \brief FreeRtos HAL API implementation.
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

#include "hal_rtos.h"

/* Semaphore */

/**
 * \brief Semaphore initialization
 */
int32_t sem_init(sem_t *sem, uint32_t count)
{
	*sem = xSemaphoreCreateCounting((uint32_t) ~0, count);

	return *sem ? 0 : ERR_NOT_INITIALIZED;
}

/**
 * \brief Semaphore up 
 */
int32_t sem_up(sem_t *sem)
{
	return is_in_isr() ? (xSemaphoreGiveFromISR(*sem, pdFALSE) ? 0 : ERR_ABORTED)
			: (xSemaphoreGive(*sem) ? 0 : ERR_ABORTED);
}

/**
 * \brief Semaphore down, may suspend the caller thread
 */
int32_t sem_down(sem_t *sem, uint32_t timeout)
{
	return xSemaphoreTake(*sem, timeout) ? 0 : ERR_TIMEOUT;
}

/**
 * \brief Semaphore deinitialization
 */
int32_t sem_deinit(sem_t *sem)
{
	if (*sem != NULL) {
		vSemaphoreDelete(*sem);
		*sem = NULL;
	}

	return 0;
}


