/*
 * dataflash_manager.h
 *
 * Created: 07/06/2016 10:22:41
 *  Author: Luc
 */ 


#ifndef DATAFLASH_MANAGER_H_
#define DATAFLASH_MANAGER_H_

#include <stdlib.h>
#include <string.h>
#include <dataflash_sgs.h>
#include <sgs/helper_sgs.h>

#define PAGE_INDEX_SEEKOIS_ID					0
#define PAGE_INDEX_SAVE_MESSAGES				1
#define PAGE_INDEX_SEEKIOS_VERSION				2
#define PAGE_INDEX_FIRST_RUN					3
#define PAGE_INDEX_TEST_DATAFLASH				4
#define PAGE_INDEX_SEEKIOS_PEERED				5
#define PAGE_INDEX_SEEKIOS_IN_POWER_SAVING		6

#define EXT_FLASH_PAGE_SIZE		256

void dataflashManager_init(void);
void dataflashManager_powerUp(void);
void dataFlashManager_powerDown(void);
void dataflashManager_writeToPage(unsigned int intPageAdr, unsigned int dataLength, char* dataToWrite);
unsigned char* dataflashManager_readPage(unsigned int intPageAdr, unsigned int dataLength,unsigned char* readBuf);
void dataflashManager_eraseUsedPages(void);

#endif /* DATAFLASH_MANAGER_H_ */