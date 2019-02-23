#include <peripheralManager/dataflash_manager.h>

static void erasePage(uint32_t pageIndex);

const char emptyBuffer[EXT_FLASH_PAGE_SIZE]={0x0};

void dataflashManager_init(){
	DF_Init();
	uint8_t dummyBuff[3];
	dataflashManager_readPage(PAGE_INDEX_TEST_DATAFLASH, 3, dummyBuff);
}

void dataflashManager_powerUp(){
	DF_PowerUp();
	DELAY_MS(100);
}

void dataFlashManager_powerDown(){
	DELAY_MS(100);
	DF_PowerDown();
	DELAY_MS(100);
}

void dataflashManager_writeToPage(unsigned int pageAdr, unsigned int dataLength, char* dataToWrite){
	dataflashManager_powerUp();
	DF_BufferWriteStr(1, 0, dataLength, (unsigned char*)dataToWrite);
	DF_BufferToPage(1,pageAdr);
	dataFlashManager_powerDown();
}

unsigned char* dataflashManager_readPage(unsigned int pageAdr, unsigned int dataLength, unsigned char* readBuf){
	dataflashManager_powerUp();
	DF_PageToBuffer(1, pageAdr);
	DF_BufferReadStr(1, 0, dataLength, readBuf);
	dataFlashManager_powerDown();
	return readBuf;
}

/* Debug function : if requested, erases some pages in the ext flash at the start of the program.  */
void dataflashManager_eraseUsedPages()
{

	#if (DELETE_FLASH_PAGE_INDEX_SEEKOIS_ID				== 1)
	erasePage(PAGE_INDEX_SEEKOIS_ID);
	#endif

	#if (DELETE_FLASH_PAGE_INDEX_SAVE_MESSAGES			== 1)
	erasePage(PAGE_INDEX_SAVE_MESSAGES);
	#endif

	#if (DELETE_FLASH_PAGE_PAGE_INDEX_SEEKIOS_VERSION	== 1)
	erasePage(PAGE_INDEX_SEEKIOS_VERSION);
	#endif

	#if (DELETE_FLASH_PAGE_PAGE_INDEX_FIRST_RUN			== 1)
	erasePage(PAGE_INDEX_FIRST_RUN);
	#endif
	
	#if (DELETE_FLASH_PAGE_PAGE_INDEX_TEST_DATAFLASH	== 1)
	erasePage(PAGE_INDEX_TEST_DATAFLASH);
	#endif

	#if (DELETE_FLASH_PAGE_PAGE_INDEX_SEEKIOS_PEERED	== 1)
	erasePage(PAGE_INDEX_SEEKIOS_PEERED);
	#endif
	
	#if (DELETE_FLASH_PAGE_INDEX_IN_POWER_SAVING	== 1)
	erasePage(PAGE_INDEX_SEEKIOS_IN_POWER_SAVING);
	#endif
}

static void erasePage(uint32_t pageIndex)
{
	dataflashManager_writeToPage(pageIndex, EXT_FLASH_PAGE_SIZE, emptyBuffer);
}
