/*
 * NVM_manager.c
 *
 * Created: 23/08/2016 16:44:22
 *  Author: Luc
 */ 
 #include <peripheralManager/NVM_Manager.h>

 void nvm_write_buffer(const uint32_t destination_address, const uint8_t *buffer, uint16_t length)
{

	/* Check if the module is busy */
	while(!NVMCTRL->INTFLAG.bit.READY);

	// 	/* Erase the page buffer before buffering new data */
	NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_PBC | NVMCTRL_CTRLA_CMDEX_KEY;

	/* Check if the module is busy */
	while(!NVMCTRL->INTFLAG.bit.READY);

	/* Clear error flags */
	NVMCTRL->STATUS.reg &= ~NVMCTRL_STATUS_MASK;


	volatile uint32_t nvm_address = destination_address / 2;
	volatile uint16_t *flash_adress;
	flash_adress = destination_address / 2;
	/* NVM _must_ be accessed as a series of 16-bit words, perform manual copy
	* to ensure alignment */
	for (uint16_t k = 0; k < length; k += 2)
	{
		static uint16_t data;
		/* Copy first byte of the 16-bit chunk to the temporary buffer */
		data = buffer[k];
		/* If we are not at the end of a write request with an odd byte count,
		* store the next byte of data as well */
		if (k < (length - 1))
		{
			data |= (buffer[k + 1] << 8);
		}
		/* Store next 16-bit chunk to the NVM memory space */
		NVM_MEMORY[nvm_address++] = data;
		// 		*flash_adress = data;
		// 		 flash_adress += 2;
		//#define NVM_MEMORY        ((volatile uint16_t *)FLASH_ADDR)
	}
	
	// 	/* SGS-Try */
	NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_WP | NVMCTRL_CTRLA_CMDEX_KEY;
	
	while(!NVMCTRL->INTFLAG.bit.READY);
}

void nvm_erase_row(const uint32_t row_address)
{
	/* Check if the module is busy */
	while(!NVMCTRL->INTFLAG.bit.READY);
	/* Clear error flags */
	NVMCTRL->STATUS.reg &= ~NVMCTRL_STATUS_MASK;
	/* Set address and command */
	NVMCTRL->ADDR.reg  = (uintptr_t)&NVM_MEMORY[row_address / 4];
	NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_ER | NVMCTRL_CTRLA_CMDEX_KEY;
	while(!NVMCTRL->INTFLAG.bit.READY);
}

void nvm_erase_user_row()
{	
	/* Check if the module is busy */
	while(!NVMCTRL->INTFLAG.bit.READY);
	
	/* Clear error flags */
	NVMCTRL->STATUS.reg &= ~NVMCTRL_STATUS_MASK;
	
	NVMCTRL->ADDR.reg  = (uintptr_t)(void*)0;
	NVMCTRL->CTRLA.bit.CMD = 0x05;
	//0x05 : EAR
	NVMCTRL->CTRLA.bit.CMDEX = 0xA5;

	/* Check if the module is busy */
	while(!NVMCTRL->INTFLAG.bit.READY);

	USARTManager_printUsbWait("Done.\r\n");

}