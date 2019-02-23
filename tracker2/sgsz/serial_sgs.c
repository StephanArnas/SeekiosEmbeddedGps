/*
 * serial_sgs.c
 *
 * Created: 20.04.2016 11:12:53
 *  Author: Marc Adolf
 */ 

#include <sgs/port_sgs.h>
#include "main.h"
#include "serial_sgs.h"

//struct io_descriptor *usart_gps_io;  >> serial_sgs.h
// 
// #define     USART_BUF_SIZE   512
// 
// volatile static uint8_t     gps_buf[USART_BUF_SIZE]="default_GPS-Puffer 128Byte";
 //volatile static uint8_t     GSM_buf[USART_BUF_SIZE]="default_GSM-Puffer 128Byte";


 process_data(const uint8_t *text)
 {
   io_write(usart_debug_io,(const uint8_t *) text, strlen(text));
   delay_ms(3);	 
 }


volatile uint32_t tmp1,tmp2,tmp3,tmp4,tmp6;
volatile char tmp5;

static void rx_cb_SER1_GPS(const struct usart_async_descriptor *const io_descr)
{
static uint8_t pos, gps_data_received ;
	gpio_toggle_pin_level(led_pwm0);
   gps_data_received += io_read(usart_gps_io, &gps_buf[gps_data_received],  USART_BUF_SIZE - gps_data_received);
				
if (redirect_GPSrx_DEBUG)
{
	io_write(usart_debug_io,  &gps_buf[gps_data_received-1], 1);
	
}


                 
        if(gps_data_received >= USART_BUF_SIZE) {                                                                       
          //  process_data(buf); // Process data
		               gps_data_received = 0; // Reset received data counter
         }

}

//void copy_ringbuffer(int *buf, const *char)
uint16_t copy_ringbuffer(char *buf)

{
uint16_t data_received =0;
data_received += io_read(usart_debug_io, buf,  255);
 //buf[255] = '\0';
}


static void rx_cb_SER2_GSM(const struct usart_async_descriptor *const io_descr)
{
static int32_t pos, gsm_data_received;	
static int32_t tmp1, tmp2, tmp3, tmp4;

	//gpio_toggle_pin_level(led_pwm1);
	
	if (redirect_GSMrx_DEBUG)
	{
		io_write(usart_debug_io,  &GSM_buf[gsm_data_received-1], 1);
		
	}
	
	if (gsm_answer==1)
	{
		gsm_data_received =0;
		gsm_answer=2;
		
		//io_write(usart_debug_io,  &GSM_buf[gsm_data_received-1], 1);
	}
	
	if (gsm_answer>1)
	{
		gsm_data_received += io_read(usart_gsm_io, &GSM_buf[gsm_data_received],  512 - gsm_data_received);
		//io_write(usart_debug_io,  &GSM_buf[gsm_data_received-1], 1);
		GSM_buf[gsm_data_received+1] ='\0';
	}
               
    if(gsm_data_received > 510) 
	{                                                                       
          //  process_data(buf); // Process data
            gsm_answer = 0; // Reset received data counter
    }

}
//volatile char uc_key;

static void rx_cb_SER3_DEBUG(const struct usart_async_descriptor *const io_descr)
{

	//data_received += io_read(usart_debug_io, &debug_buf[data_received],  USART_BUF_SIZE - data_received);
	// uc_key =debug_buf[SER3_DEBUG.rx.read_index-1];


	gpio_toggle_pin_level(led_pwm1);
//   data_received += io_read(&usart_gps_io, &buf[data_received],
//                 USART_BUF_SIZE - data_received);
//                 
        if(data_received == USART_BUF_SIZE) {                                                                       
         
            data_received = 0; // Reset received data counter
        }
tmp1 =SER3_DEBUG.rx.size;
tmp2 =SER3_DEBUG.rx_bytes_in_buffer;
tmp3 =SER3_DEBUG.rx.read_index;
tmp4 =SER3_DEBUG.rx.write_index;

tmp6 =SER3_DEBUG.rx.size;

}



static void tx_cb_SER1_GPS(const struct usart_async_descriptor *const io_descr)
{
	/* Transfer completed */
}

static void tx_cb_SER2_GSM(const struct usart_async_descriptor *const io_descr)
{
	/* Transfer completed */
}

static void tx_cb_SER3_DEBUG(const struct usart_async_descriptor *const io_descr)
{
	/* Transfer completed */
}





void SER1_GPS_io_init(void)
{


	usart_async_register_callback(&SER1_GPS, USART_ASYNC_TXC_CB, tx_cb_SER1_GPS);
	usart_async_register_callback(&SER1_GPS, USART_ASYNC_RXC_CB, rx_cb_SER1_GPS);
	//usart_async_register_callback(&SER1_GPS, USART_ASYNC_ERROR_CB, err_cb_SER1_GPS);*/
	usart_async_get_io_descriptor(&SER1_GPS, &usart_gps_io);
	usart_async_enable(&SER1_GPS);

	io_write(usart_gps_io, (uint8_t)"at\n\r\0", 12);
}


struct io_descriptor *usart_gsm_io;
void  SER2_GSM_io_init(void)
{
	

	usart_async_register_callback(&SER2_GSM, USART_ASYNC_TXC_CB, tx_cb_SER2_GSM);
	usart_async_register_callback(&SER2_GSM, USART_ASYNC_RXC_CB, rx_cb_SER2_GSM);
	 //  usart_async_register_callback(&SER2_GSM, USART_ASYNC_ERROR_CB, err_cb_SER2_GSM);*/
	usart_async_get_io_descriptor(&SER2_GSM, &usart_gsm_io);
	usart_async_enable(&SER2_GSM);

	io_write(usart_gsm_io,(uint8_t)"at\n\r\0", 12);
}




struct io_descriptor *usart_debug_io;

void SER3_DEBUG_io_init(void)
{
	

	usart_async_register_callback(&SER3_DEBUG, USART_ASYNC_TXC_CB, tx_cb_SER3_DEBUG);
	usart_async_register_callback(&SER3_DEBUG, USART_ASYNC_RXC_CB, rx_cb_SER3_DEBUG);
	 //  usart_async_register_callback(&SER3_DEBUG, USART_ASYNC_ERROR_CB, err_cb_SER3_DEBUG);*/
	usart_async_get_io_descriptor(&SER3_DEBUG, &usart_debug_io);
	usart_async_enable(&SER3_DEBUG);

	io_write(usart_debug_io,  "DEBUG_ready\n\r", 13);
	
}


static uint8_t example_SER5_SPI_IMU_FLASH[12] = "Hello World!";

	struct io_descriptor *spi_io;

void SER5_SPI_IMU_FLASH_io_init(void)
{
	spi_m_sync_get_io_descriptor(&SER5_SPI_IMU_FLASH, &spi_io);
	spi_m_sync_enable(&SER5_SPI_IMU_FLASH);
	io_write(spi_io, example_SER5_SPI_IMU_FLASH, 12);
}


// void SER4_BLE_example(void)
// {
// 	struct io_descriptor *io;
// 
// 	usart_async_register_callback(&SER4_BLE, USART_ASYNC_TXC_CB, tx_cb_SER4_BLE);
// 	/*usart_async_register_callback(&SER4_BLE, USART_ASYNC_RXC_CB, rx_cb);
// 	   usart_async_register_callback(&SER4_BLE, USART_ASYNC_ERROR_CB, err_cb);*/
// 	usart_async_get_io_descriptor(&SER4_BLE, &io);
// 	usart_async_enable(&SER4_BLE);
// 
// 	io_write(io, example_SER4_BLE, 12);
// }


void serial_io_init(void)
{
	SER1_GPS_io_init();
	SER2_GSM_io_init();
	SER3_DEBUG_io_init();
	SER5_SPI_IMU_FLASH_io_init();
}