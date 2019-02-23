/*
* serial_sgs.c
*
* Created: 20.04.2016 11:12:53
*  Author: Marc Adolf
*/

#include "main.h"
#include "helper_sgs.h"
#include <peripheralManager/GPS_manager.h>
#include <stdio_redirect/stdio_io.h>

void process_data(const uint8_t *text)
{
	io_write(usart_debug_io,(const uint8_t *) text, strlen(text));
	delay_ms(3);
}


volatile uint32_t tmp1,tmp2,tmp3,tmp4,tmp6;
volatile char tmp5;

#define START_LENGTH	6
#define STARTGPGGA		"$GPGGA"
#define STARTGNGGA		"$GNGGA"
#define STARTGPRMC		"$GPRMC"
#define STARTGNRMC		"$GNRMC"
/* Callback that gets the GPS data and filters only the GGA data */
static void rx_cb_SER1_GPS_GGA(const struct usart_async_descriptor *const io_descr)
{
	static uint8_t nmeaIndex = 0;
	static uint32_t recordAllBitIndex = 0;
	EventBits_t gpsBits = GPSManager_getGPSBitsFromISR();
	bool shouldRecordGGA = (gpsBits & GPS_BIT_NMEA_AVAILABLE) == 0;
	bool testUsartBit = (gpsBits & GPS_BIT_REQ_GPS_USART_TEST) != 0;
	volatile char readChar;
	static bool ggaFound=false;
	static uint8_t ggalenght = 0;
	if(shouldRecordGGA || testUsartBit)
	{
		uint8_t nbRead = io_read(usart_gps_io, &readChar,  1);

		if(nbRead > 0)
		{
			if(testUsartBit)
			{
				GPSManager_setGPSBitsFromISR(GPS_BIT_REQ_GPS_USART_TEST);
			}

			if(shouldRecordGGA)
			{
				if (!ggaFound)
				{
					if(nmeaIndex < START_LENGTH) // if first 6 char of the frame corresponds to GXGGA, we keep recording. Otherwise we restart it.
					{
						if(readChar == STARTGPGGA[nmeaIndex] || readChar == STARTGNGGA[nmeaIndex])
						{
							gps_buf[nmeaIndex] = readChar;
							nmeaIndex++;
						}
						else
						{
							nmeaIndex = 0;
						}
					}
					else // if the start was OK, we record the rest of the frame until the EOL
					{
						gps_buf[nmeaIndex] = readChar;
						nmeaIndex++;
						gps_buf[nmeaIndex] = '\0';
						if(readChar == '\n')
						{
							ggalenght=nmeaIndex;
							ggaFound=true;
						}
					}
				}
				else if (ggaFound)
				{
					if(nmeaIndex <ggalenght + START_LENGTH) // if first 6 char of the frame corresponds to GXGGA, we keep recording. Otherwise we restart it.
					{
						if(readChar == STARTGPRMC[nmeaIndex-ggalenght] || readChar == STARTGNRMC[nmeaIndex-ggalenght])
						{
							gps_buf[nmeaIndex] = readChar;
							nmeaIndex++;
						}
						else
						{
							nmeaIndex = ggalenght;
						}
					}
					else // if the start was OK, we record the rest of the frame until the EOL
					{
						gps_buf[nmeaIndex] = readChar;
						nmeaIndex++;
						gps_buf[nmeaIndex] = '\0';
						if(readChar == '\n')
						{
							ggaFound=false;
							GPSManager_setGPSBitsFromISR(GPS_BIT_NMEA_AVAILABLE);
							nmeaIndex = 0;
						}
					}
				}
				
			}
		}
	}
	
	UNUSED(io_descr);
}

//void copy_ringbuffer(int *buf, const *char)
uint16_t copy_ringbuffer(char *buf)
{
	uint16_t data_received =0;
	data_received += io_read(usart_debug_io, (uint8_t*)buf,  255);
	//buf[255] = '\0';
	return 1;
}


static void rx_cb_SER2_GSM(const struct usart_async_descriptor *const io_descr)
{
	static int32_t  gsm_data_received;
	
	if(gsm_answer == 1){
		gsm_data_received=0;
		gsm_answer=2;
	}

	if (gsm_answer>1)
	{
		gsm_data_received += io_read(usart_gsm_io, &gsm_buf[gsm_data_received],  GSM_BUF_SIZE - gsm_data_received);
		gsm_buf[gsm_data_received+1] ='\0';
	}
	
	if(gsm_data_received >= GSM_BUF_SIZE-1)
	{
		gsm_answer = 0; // Reset received data counter
		gsm_data_received = 0; // Reset received data counter
	}
	UNUSED(io_descr);
}

static void tx_cb_SER2_GSM(const struct usart_async_descriptor *const io_descr)
{
	UNUSED(io_descr);
	if(seekiosManagerStarted)
	{
		USARTManager_gsmUsartMaskSetBitsFromISR(GSM_USART_BIT_TRANSFER_COMPLETE);
	}
}

void SER1_GPS_io_init(void)
{


	//usart_async_register_callback(&SER1_GPS, USART_ASYNC_TXC_CB, tx_cb_SER1_GPS);
	usart_async_register_callback(&SER1_GPS, USART_ASYNC_RXC_CB, rx_cb_SER1_GPS_GGA);
	//usart_async_register_callback(&SER1_GPS, USART_ASYNC_ERROR_CB, err_cb_SER1_GPS);*/ // IMPLEMENT ??????
	usart_async_get_io_descriptor(&SER1_GPS, &usart_gps_io);
	usart_async_enable(&SER1_GPS);
	io_write(usart_gps_io,(uint8_t*)"at\n\r\0", 12);

}

void  SER2_GSM_io_init(void)
{
	usart_async_register_callback(&SER2_GSM, USART_ASYNC_TXC_CB, tx_cb_SER2_GSM);
	usart_async_register_callback(&SER2_GSM, USART_ASYNC_RXC_CB, rx_cb_SER2_GSM);
	//  usart_async_register_callback(&SER2_GSM, USART_ASYNC_ERROR_CB, err_cb_SER2_GSM);*/
	usart_async_get_io_descriptor(&SER2_GSM, &usart_gsm_io);
	usart_async_enable(&SER2_GSM);
	io_write(usart_gsm_io, (uint8_t*)"at\n\r\0", 5);
}

void SER3_DEBUG_io_init(void)
{
	//usart_async_register_callback(&SER3_DEBUG, USART_ASYNC_TXC_CB, tx_cb_SER3_DEBUG);
	//usart_async_register_callback(&SER3_DEBUG, USART_ASYNC_RXC_CB, rx_cb_SER3_DEBUG);
	//usart_async_register_callback(&SER3_DEBUG, USART_ASYNC_ERROR_CB, err_cb_SER3_DEBUG);*/
	//usart_async_get_io_descriptor(&SER3_DEBUG, &usart_debug_io);
	usart_sync_get_io_descriptor(&SER3_DEBUG, &usart_debug_io);
	usart_sync_enable(&SER3_DEBUG);
	//stdio_io_init(&SER3_DEBUG.io);
}

void SER5_SPI_IMU_FLASH_io_init(void)
{
	spi_m_sync_get_io_descriptor(&SER5_SPI_IMU_FLASH, &spi_io);
	spi_m_sync_enable(&SER5_SPI_IMU_FLASH);

}

void serial_io_init(void)
{
	SER1_GPS_io_init();
	SER2_GSM_io_init();
	SER3_DEBUG_io_init();
	SER5_SPI_IMU_FLASH_io_init();
}