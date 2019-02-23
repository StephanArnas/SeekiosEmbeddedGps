/**
 * \file
 *
 * \brief IO related functionality declaration.
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

#ifndef _HAL_IO_INCLUDED
#define _HAL_IO_INCLUDED

/**
 * \addtogroup io HAL IO driver
 *
 * \section io_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief IO descriptor
 *
 * The IO descriptor forward declaration.
 */
struct io_descriptor;

/**
 * \brief IO write function pointer type
 */
typedef int32_t (* io_write_t)(struct io_descriptor *const io_descr,
		const uint8_t *const buf, const uint16_t length);

/**
 * \brief IO read function pointer type
 */
typedef int32_t (* io_read_t)(struct io_descriptor *const io_descr,
		uint8_t *const buf, const uint16_t length);

/**
 * \brief IO descriptor
 */
struct io_descriptor {
	io_write_t write; /*! The write function pointer. */
	io_read_t  read;  /*! The read function pointer. */
};

/**
 * \brief IO write interface
 *
 * This function write up to \p length of bytes to a given IO descriptor,
 * It returns the number of bytes actually write.
 *
 * \param[in] descr An IO descriptor to write
 * \param[in] buf The buffer pointer to story the write data
 * \param[in] length number of bytes to write
 *
 * \return number of bytes written
 */
int32_t io_write(struct io_descriptor *const io_descr,
		const uint8_t *const buf, const uint16_t length);

/**
 * \brief IO read interface
 *
 * This function reads up to \p length bytes from a given IO descriptor, and
 * story it in the buffer pointed to by \p buf. It returns the number of bytes
 * actually read.
 *
 * \param[in] descr An IO descriptor to read
 * \param[in] buf The buffer pointer to story the read data
 * \param[in] length number of bytes to read
 *
 * \return number of bytes actually read
 */
int32_t io_read(struct io_descriptor *const io_descr,
		uint8_t *const buf, const uint16_t length);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HAL_IO_INCLUDED */
