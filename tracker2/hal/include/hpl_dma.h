/**
 * \file
 *
 * \brief DMA related functionality declaration.
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

#ifndef _HPL_DMA_H_INCLUDED
#define _HPL_DMA_H_INCLUDED

/**
 * \addtogroup HPL DMA
 *
 * \section hpl_dma_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <compiler.h>
#include <hpl_irq.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _dma_resource;

/**
 * \brief DMA callback types
 */
enum _dma_callback_type {
	DMA_TRANSFER_COMPLETE_CB,
	DMA_TRANSFER_ERROR_CB
};

/**
 * \brief DMA device structure 
 */
struct _dma_dev {
	struct _irq_descriptor irq;
	void *hw;
};

/**
 * \brief DMA related functions, pointers to low-level functions
 */
typedef	int32_t (*dma_get_trigger_number_t)(void *const dev, uint32_t const trigger);

/**
 * \brief DMA interrupt callbacks
 */
struct _dma_callbacks {
	void (* transfer_done)(struct _dma_dev **dev,
			struct _dma_resource *const resource);
	void (* error)(struct _dma_dev **dev,
			struct _dma_resource *const resource);
};

/**
 * \brief DMA resource struct 
 */
struct _dma_resource {
	uint32_t id;
  uint32_t transfered_size;
	struct _dma_callbacks _dma_cb;
  void  *config;
  dma_get_trigger_number_t get_trigger_num;
  int32_t status; 
};



/**
 * \brief Initialize DMA
 *
 * This function does low level DMA configuration.
 *
 * \param[in] dev The pointer to pointer to device instance
 *
 * \return initialize status 
 *
 */
int32_t _dma_init(struct _dma_dev **dev);

/**
 * \brief De-initialize DMA
 *
 * \param[in] dev The pointer to device instance
 *
 * \return de-initialize status
 *
 */
int32_t _dma_deinit(struct _dma_dev *const dev);

/**
 * \brief Allocate resource for DMA
 *
 * \param[in] dev The pointer to device instance
 * \param[out] resource The pointer to pointer to one dma resource
 * \param[in] num The counter of resource in one transaction
 *
 * \return Allocation status
 *
 */
int32_t _dma_allocate(struct _dma_dev *const dev, struct _dma_resource **const resource, uint32_t num);

/**
 * \brief Config trigger source for DMA Resource if needed
 *
 * \param[in] dev The pointer to device instance
 * \param[in] resource The pointer to pointer to one dma resource
 * \param[in] peripheral_dev  The pointer to peripheral device 
 * \param[in] trigger The pointer to trigger source for dma resource 
 *
 * \return Allocation status
 *
 */
int32_t _dma_config_resource(struct _dma_dev *const dev, struct _dma_resource *const resource,
    void *const peripheral_dev, uint32_t const trigger);

/**
 * \brief Enable DMA resource
 *
 * \param[in] dev The pointer to device instance
 * \param[in] resource The pointer to one dma resource
 *
 * \return Enable status
 *
 */
int32_t _dma_enable(struct _dma_dev *const dev, struct _dma_resource *const resource);

/**
 * \brief Disable DMA resource
 *
 * \param[in] dev The pointer to device instance
 * \param[in] resource The pointer to one dma resource
 *
 * \return Disable status
 *
 */
int32_t _dma_disable(struct _dma_dev *const dev, struct _dma_resource *const resource);

/**
 * \brief enable/disable SRCINC 
 *
 * \param[in] dev The pointer to device instance
 * \param[in] resource The pointer to one dma resource
 * \param[in] state 0=disable,1=enable  
 *
 * \return the status of operation 
 */
int32_t _dma_srcinc_enable(struct _dma_dev *const dev, struct _dma_resource *const resource,
    bool state);

/**
 * \brief  Set source/destination address and the size for transfer
 *
 * \param[in] dev The pointer to device instance
 * \param[in] resource The pointer to one dma resource
 * \param[in] src The pointer to source address for transfer
 * \param[in] dst The pointer to destination address for transfer
 * \param[in] size The transfer size for M2P|the receive buffer size for P2M  
 *
 * \return the status of transfer config
 */
int32_t _dma_config_transfer(struct _dma_dev *const dev, struct _dma_resource *const resource,
    void *src, void *dst, uint32_t size);

/**
 * \brief  Set software trigger command for transfer
 *
 * \param[in] dev The pointer to device instance
 * \param[in] resource The pointer to one dma resource
 *
 * \return the status of set command
 */	
int32_t _dma_software_trigger_set(struct _dma_dev *const dev, struct _dma_resource *const resource);

/**
 * \brief Enable/disable DMA interrupt
 *
 * \param[in] device The pointer to device instance
 * \param[in] resource The pointer to one dma resource
 * \param[in] type The type of interrupt to disable/enable if applicable
 * \param[in] state Enable or disable
 */
void _dma_set_irq_state(struct _dma_dev *const dev, struct _dma_resource *const resource,
const enum _dma_callback_type type, const bool state);

#ifdef __cplusplus
}
#endif

#endif /* HPL_DMA_H_INCLUDED */
