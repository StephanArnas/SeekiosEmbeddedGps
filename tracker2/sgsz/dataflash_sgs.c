/*
 * dataflash_sgs.c
 *
 * Created: 31.05.2016 17:00:53
 *  Author: Marc Adolf
 */ 


#include "dataflash_sgs.h"
#include "stdint.h"
#include "menu_sgs.h"
//Dataflash opcodes
#define ZERO			         	0x00	// nix
#define FlashPageRead				0x52	// Main memory page read
#define FlashToBuf1Transfer 		0x53	// Main memory page to buffer 1 transfer
#define Buf1Read					0xD4    //0x54 not working // Buffer 1 read
#define FlashToBuf2Transfer 		0x55	// Main memory page to buffer 2 transfer
#define Buf2Read					0xD6    //0x56	// Buffer 2 read
#define StatusReg					0xD7    //0x57	// Status register
#define AutoPageReWrBuf1			0x58	// Auto page rewrite through buffer 1
#define AutoPageReWrBuf2			0x59	// Auto page rewrite through buffer 2
#define FlashToBuf1Compare    		0x60	// Main memory page to buffer 1 compare
#define FlashToBuf2Compare	    	0x61	// Main memory page to buffer 2 compare
#define ContArrayRead				0x68	// Continuous Array Read (Note : Only A/B-parts supported)
#define FlashProgBuf1				0x82	// Main memory page program through buffer 1
#define Buf1ToFlashWE   			0x83	// Buffer 1 to main memory page program with built-in erase
#define Buf1Write					0x84    // Buffer 1 write
#define FlashProgBuf2				0x85	// Main memory page program through buffer 2
#define Buf2ToFlashWE   			0x86	// Buffer 2 to main memory page program with built-in erase
#define Buf2Write					0x87	// Buffer 2 write
#define Buf1ToFlash     			0x88	// Buffer 1 to main memory page program without built-in erase
#define Buf2ToFlash		         	0x89	// Buffer 2 to main memory page program without built-in erase
#define PowerDown    	         	0xB9	// Go into Deep Power-down mode
#define PowerUp                  	0xAB	// Resume from Deep Power-down mode to standby

void DF_BufferToPage (unsigned char BufferNo, unsigned int PageAdr);

unsigned char df_read_status (void);

uint8_t dataflash_cs(uint8_t mode)
{
	gpio_set_pin_level(DATAFLASH_CS,!mode);
	return 1;
}

void DF_Init(void)
{
	df_read_status();
}


void DF_PowerDown(void)
{
	gpio_set_pin_level(DATAFLASH_CS,0);
	spi_tx[0] =PowerDown;
	io_write(spi_io,spi_tx,1);	 
	gpio_set_pin_level(DATAFLASH_CS,1);
}

void DF_PowerUp(void)
{
	gpio_set_pin_level(DATAFLASH_CS,0);
	spi_tx[0] =PowerUp;
	io_write(spi_io,spi_tx,1);
	gpio_set_pin_level(DATAFLASH_CS,1);
}



unsigned char DF_BufferReadByte(unsigned char BufferNo, unsigned int IntPageAdr)
{
	unsigned char data = 0;
	
	//	DF_CS_HIGH();
	gpio_set_pin_level(DATAFLASH_CS,0);

	if(1 == BufferNo)							//read byte from buffer 1
	{
    					
		spi_tx[0] =Buf1Read;
		spi_tx[1] =0x00;  //don't cares
		spi_tx[2]=((char) IntPageAdr>>8);
		spi_tx[3]=(char)IntPageAdr;
		spi_tx[4] =0x00;  //don't cares
		io_write(spi_io,spi_tx,5);	//lower part of internal buffer address
		io_read(spi_io,spi_rx,1);	//read byte
	}


	else
	if (2 == BufferNo)							//read byte from buffer 2
	{
		spi_tx[0] =Buf2Read;
		spi_tx[1] =0x00;  //don't cares
		spi_tx[2]=((char) IntPageAdr>>8);
		spi_tx[3]=(char)IntPageAdr;
		spi_tx[4] =0x00;  //don't cares
		io_write(spi_io,spi_tx,5);	//lower part of internal buffer address
		io_read(spi_io,spi_rx,1);	//read byte
	}

	gpio_set_pin_level(DATAFLASH_CS,1);
	return spi_rx[0];								//return the read data byte
}


void DF_BufferWriteByte(unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data)
{
	//	DF_CS_HIGH();
	DF_CS_LOW();
	
	if (1 == BufferNo)							//write byte to buffer 1
	{
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(Data);						//write data byte
	}

	#ifdef USE_BUFFER2
	else
	if (2 == BufferNo)							//write byte to buffer 2
	{
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(Data);						//write data byte
	}
	#endif

	DF_CS_HIGH();
}



// Globals
unsigned char PageBits;
unsigned int  PageSize;
// Functions

/*****************************************************************************
*
*	Function name : df_read_status
*
*	Returns :		One status byte. Consult Dataflash datasheet for further
*					decoding info
*
*	Parameters :	None
*
*	Purpose :		Status info concerning the Dataflash is busy or not.
*					Status info concerning compare between buffer and flash page
*					Status info concerning size of actual device
*
******************************************************************************/
unsigned char df_read_status (void)
{
	unsigned char result,index_copy, cmd;
	//Look-up table for these sizes ->  512k, 1M,  2M,  4M,  8M, 16M, 32M, 64M
	unsigned int DF_pagesize[] ={264,264, 264, 264, 264, 528, 528,1056};
	//Look-up table for these sizes ->  512k, 1M, 2M, 4M, 8M, 16M, 32M, 64M
	unsigned char DF_pagebits[] ={  9,  9,  9,  9,  9,  10,  10,  11};
	
	gpio_set_pin_level(DATAFLASH_CS,0);         //make sure to toggle CS signal in order
	delay_ms(1);     
	spi_tx[0]= StatusReg;//0x57;        					//to reset dataflash command decoder
	result = io_write(spi_io, spi_tx , 1);      //send status register read op-code
	result = io_read(spi_io, spi_rx, 2);		//dummy write to get result
	//delay_ms(1);
	
	index_copy = ((spi_rx[0] & 0x38) >> 3);	//get the size info from status register

	PageBits   = DF_pagebits[index_copy];	//get number of internal page address bits from look-up table
	PageSize   = DF_pagesize[index_copy];   //get the size of the page (in bytes)

	gpio_set_pin_level(DATAFLASH_CS,1);
	return spi_rx[0];							//return the read status register value
}


/*****************************************************************************
*
*	Function name : DF_PageToBuffer
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of page to be transferred to buffer
*
*	Purpose :		Transfers a page from flash to dataflash SRAM buffer
*
******************************************************************************/
void DF_PageToBuffer(unsigned char BufferNo, unsigned int PageAdr)
{
    gpio_set_pin_level(DATAFLASH_CS,0);								
	
	if (1 == BufferNo)											//transfer flash page to buffer 1
	{
		spi_tx[0] =FlashToBuf1Transfer;                   //transfer to buffer 1 op-code
		spi_tx[1] = (char)(PageAdr >> (16 - PageBits));   //upper part of page address
		spi_tx[2]=  (char)(PageAdr << (PageBits - 8));    //lower part of page address
		spi_tx[3] =0x00;                                  //don't cares
		io_write(spi_io,spi_tx,4);	                      //lower part of internal buffer address	 
	}
	else
	if (2 == BufferNo)											//transfer flash page to buffer 2
	{
		spi_tx[0] =FlashToBuf2Transfer;                   //transfer to buffer 1 op-code
		spi_tx[1] = (char)(PageAdr >> (16 - PageBits));   //upper part of page address
		spi_tx[2]=  (char)(PageAdr << (PageBits - 8));    //lower part of page address
		spi_tx[3] =0x00;                                  //don't cares
		io_write(spi_io,spi_tx,4);	                      //lower part of internal buffer address
	}

	
	gpio_set_pin_level(DATAFLASH_CS,1);
	gpio_set_pin_level(DATAFLASH_CS,0);												//initiate the transfer

	
	while(!(df_read_status() & 0x80));							//monitor the status register, wait until busy-flag is high
	
	gpio_set_pin_level(DATAFLASH_CS,1);
}

void DF_BufferToPage (unsigned char BufferNo, unsigned int PageAdr)
{
    gpio_set_pin_level(DATAFLASH_CS,0);
	
	if (1 == BufferNo)											//program flash page from buffer 1
	{
		spi_tx[0] =Buf1ToFlashWE;                         //buffer 1 to flash with erase op-code
		spi_tx[1] = (char)(PageAdr >> (16 - PageBits));   //upper part of page address
		spi_tx[2]=  (char)(PageAdr << (PageBits - 8));    //lower part of page address
		spi_tx[3] =0x00;                                  //don't cares
		io_write(spi_io,spi_tx,4);	                    
	
	}


	else
	if (2 == BufferNo)											//program flash page from buffer 2
	{
		spi_tx[0] =Buf2ToFlashWE;                         //buffer 2 to flash with erase op-code
		spi_tx[1] = (char)(PageAdr >> (16 - PageBits));   //upper part of page address
		spi_tx[2]=  (char)(PageAdr << (PageBits - 8));    //lower part of page address
		spi_tx[3] =0x00;                                  //don't cares
		io_write(spi_io,spi_tx,4);	                          
	}

	
	gpio_set_pin_level(DATAFLASH_CS,1);
	gpio_set_pin_level(DATAFLASH_CS,0);												//initiate the transfer

	
	while(!(df_read_status() & 0x80));							//monitor the status register, wait until busy-flag is high
	
	gpio_set_pin_level(DATAFLASH_CS,1);
}


/*****************************************************************************
*
*	Function name : DF_BufferReadStr
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*					No_of_bytes	->	Number of bytes to be read
*					*BufferPtr	->	address of buffer to be used for read bytes
*
*	Purpose :		Reads one or more bytes from one of the dataflash
*					internal SRAM buffers, and puts read bytes into
*					buffer pointed to by *BufferPtr
*
******************************************************************************/
void DF_BufferReadStr (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
{
	unsigned int i;

	
	gpio_set_pin_level(DATAFLASH_CS,0);					
	
	if (1 == BufferNo)							//read byte(s) from buffer 1
	{
		spi_tx[0] =Buf1Read;
		spi_tx[1] =0x00;
		spi_tx[2]=((char) IntPageAdr>>8);
	    spi_tx[3]=(char)IntPageAdr;
		spi_tx[4] =0x00;  //don't cares
		io_write(spi_io,spi_tx,5);	//lower part of internal buffer address
		io_read(spi_io,BufferPtr,No_of_bytes);
	}
	
	else
	if (2 == BufferNo)							//read byte(s) from buffer 2
	{
	spi_tx[0] = Buf2Read;					//buffer 2 read op-code
	spi_tx[1] = 0x00;
	spi_tx[2] = ((char) IntPageAdr>>8);
	spi_tx[3] = (char)IntPageAdr;
	spi_tx[4] = 0x00;  //don't cares
	io_write(spi_io,spi_tx,5);	//lower part of internal buffer address
	io_read(spi_io,BufferPtr,No_of_bytes);
	}
	gpio_set_pin_level(DATAFLASH_CS,1);
}



/*****************************************************************************
*
*	Function name : DF_BufferWriteEnable
*
*	Returns :		None
*
*	Parameters :	IntPageAdr	->	Internal page address to start writing from
*					BufferAdr	->	Decides usage of either buffer 1 or 2
*
*	Purpose :		Enables continuous write functionality to one of the dataflash buffers
*					buffers. NOTE : User must ensure that CS goes high to terminate
*					this mode before accessing other dataflash functionalities
*
******************************************************************************/
void DF_BufferWriteEnable (unsigned char BufferNo, unsigned int IntPageAdr)
{
	gpio_set_pin_level(DATAFLASH_CS,0);
	
	if (1 == BufferNo)							//write enable to buffer 1
	{
		spi_tx[0] =Buf1Write;
		spi_tx[1] =0x00;
		spi_tx[2]=((char) IntPageAdr>>8);
		spi_tx[3]=(char)IntPageAdr;
		io_write(spi_io,spi_tx,4);	//lower part of internal buffer address
	}
	
	else
	if (2 == BufferNo)							//write enable to buffer 2
	{
		spi_tx[0] =Buf2Write;
		spi_tx[1] =0x00;
		spi_tx[2]=((char) IntPageAdr>>8);
		spi_tx[3]=(char)IntPageAdr;
		io_write(spi_io,spi_tx,4);	//lower part of internal buffer address
	}

	gpio_set_pin_level(DATAFLASH_CS,1);
}


/*****************************************************************************
*
*	Function name : DF_BufferWriteStr
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*					No_of_bytes	->	Number of bytes to be written
*					*BufferPtr	->	address of buffer to be used for copy of bytes
*									from AVR buffer to dataflash buffer 1 (or 2)
*
*	Purpose :		Copies one or more bytes to one of the dataflash
*					internal SRAM buffers from AVR SRAM buffer
*					pointed to by *BufferPtr
*
******************************************************************************/
void DF_BufferWriteStr (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
{
	unsigned int i;
    char cmd;

	 gpio_set_pin_level(DATAFLASH_CS,0);        							
	
	if (1 == BufferNo)							//write byte(s) to buffer 1
	{
		spi_tx[0] =Buf1Write;
		spi_tx[1] =0x00;
		spi_tx[2]=((char) IntPageAdr>>8);
	    spi_tx[3]=(char)IntPageAdr;
		io_write(spi_io,spi_tx,4);	//lower part of internal buffer address
        io_write(spi_io,BufferPtr,No_of_bytes);
	}


	else
	if (2 == BufferNo)							//write byte(s) to buffer 2
	{
		spi_tx[0] =Buf2Write;
		spi_tx[1] =0x00;
		spi_tx[2]=((char) IntPageAdr>>8);
		spi_tx[3]=(char)IntPageAdr;
		io_write(spi_io,spi_tx,4);	//lower part of internal buffer address
		io_write(spi_io,BufferPtr,No_of_bytes);
	}

	gpio_set_pin_level(DATAFLASH_CS,1);       
}


/*****************************************************************************
*
*	Function name : Cont_Flash_Read_Enable
*
*	Returns :		None
*
*	Parameters :	PageAdr		->	Address of flash page where cont.read starts from
*					IntPageAdr	->	Internal page address where cont.read starts from
*
*	Purpose :		Initiates a continuous read from a location in the DataFlash
*
******************************************************************************/
void Cont_Flash_Read_Enable (unsigned int PageAdr, unsigned int IntPageAdr)
{
	//	DF_CS_HIGH();																//make sure to toggle CS signal in order
	DF_CS_LOW();																//to reset dataflash command decoder
	
	DF_SPI_RW(ContArrayRead);													//Continuous Array Read op-code
	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));						//upper part of page address
	DF_SPI_RW((unsigned char)((PageAdr << (PageBits - 8))+ (IntPageAdr>>8)));	//lower part of page address and MSB of int.page adr.
	DF_SPI_RW((unsigned char)(IntPageAdr));										//LSB byte of internal page address
	DF_SPI_RW(0x00);															//perform 4 dummy writes
	DF_SPI_RW(0x00);															//in order to intiate DataFlash
	DF_SPI_RW(0x00);															//address pointers
	DF_SPI_RW(0x00);
	
	DF_CS_HIGH();
}
