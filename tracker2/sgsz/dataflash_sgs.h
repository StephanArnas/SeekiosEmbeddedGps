/*
 * dataflash_sgs.h
 *
 * Created: 31.05.2016 17:01:19
 *  Author: Marc Adolf
 */ 


#ifndef DATAFLASH_SGS_H_
#define DATAFLASH_SGS_H_

#include "stdint.h"
#include "serial_sgs.h"
#include <sgs/port_sgs.h>

volatile char df_data[64];
volatile char spi_rx[16];
volatile char spi_tx[16];
#if 0
#define DF_SPI_INIT()				SPI0_Init()

#define DF_CS_BIT					BIT4
#define DF_CS_INIT()				Sbi(DDRG, DF_CS_BIT)
#define DF_CS_LOW()					Cbi(PORTG, DF_CS_BIT)
#define DF_CS_HIGH()				Sbi(PORTG, DF_CS_BIT)

#define DF_SPI_RW(Data)				SPI0_WriteReadByte(Data)
#endif

//Dataflash opcodes
#define FlashPageRead				0x52	// Main memory page read
#define FlashToBuf1Transfer 		0x53	// Main memory page to buffer 1 transfer
#define Buf1Read					0x54	// Buffer 1 read
#define FlashToBuf2Transfer 		0x55	// Main memory page to buffer 2 transfer
#define Buf2Read					0x56	// Buffer 2 read
#define StatusReg					0x57	// Status register
#define AutoPageReWrBuf1			0x58	// Auto page rewrite through buffer 1
#define AutoPageReWrBuf2			0x59	// Auto page rewrite through buffer 2
#define FlashToBuf1Compare    		0x60	// Main memory page to buffer 1 compare
#define FlashToBuf2Compare	    	0x61	// Main memory page to buffer 2 compare
#define ContArrayRead				0x68	// Continuous Array Read (Note : Only A/B-parts supported)
#define FlashProgBuf1				0x82	// Main memory page program through buffer 1
#define Buf1ToFlashWE   			0x83	// Buffer 1 to main memory page program with built-in erase
#define Buf1Write					0x84	// Buffer 1 write
#define FlashProgBuf2				0x85	// Main memory page program through buffer 2
#define Buf2ToFlashWE   			0x86	// Buffer 2 to main memory page program with built-in erase
#define Buf2Write					0x87	// Buffer 2 write
#define Buf1ToFlash     			0x88	// Buffer 1 to main memory page program without built-in erase
#define Buf2ToFlash		         	0x89	// Buffer 2 to main memory page program without built-in erase


//Function definitions
void DF_Init(void);
unsigned char DF_BufferReadByte(unsigned char BufferNo, unsigned int IntPageAdr);
void DF_BufferWriteByte(unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data);
void DF_PageToBuffer(unsigned char BufferNo, unsigned int PageAdr);
void DF_BufferToPage (unsigned char BufferNo, unsigned int PageAdr);

void DF_BufferReadStr (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr);
void DF_BufferWriteStr (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr);
void DF_BufferWriteEnable (unsigned char BufferNo, unsigned int IntPageAdr);

unsigned char df_read_status (void);
void Cont_Flash_Read_Enable (unsigned int PageAdr, unsigned int IntPageAdr);

// *****************************[ End Of DATAFLASH.H ]*****************************



#endif /* DATAFLASH_SGS_H_ */