/*
 * NVM_Manager.h
 *
 * Created: 23/08/2016 16:45:00
 *  Author: Luc
 */ 


#ifndef NVM_MANAGER_H_
#define NVM_MANAGER_H_

#include <stdint.h>
#include <hri_nvmctrl_v302.h>

/* Flash page size is 64 bytes */
#define PAGE_SIZE	64
/* Memory pointer for flash memory */
#define NVM_MEMORY        ((volatile uint16_t *)FLASH_ADDR)

void nvm_write_buffer(const uint32_t destination_address, const uint8_t *buffer, uint16_t length);
void nvm_erase_row(const uint32_t row_address);

#endif /* NVM_MANAGER_H_ */