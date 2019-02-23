/*
 * serial_sgs.h
 *
 * Created: 20.04.2016 11:13:17
 *  Author: Marc Adolf
 */ 


#ifndef SERIAL_SGS_H_
#define SERIAL_SGS_H_


struct io_descriptor *usart_debug_io;
struct io_descriptor *usart_gps_io;
struct io_descriptor *usart_gsm_io;
struct io_descriptor *spi_io;

//volatile char uc_key;

void serial_io_init(void);
void SER3_DEBUG_io_init(void);
void SER5_SPI_IMU_FLASH_io_init(void);
void process_data(const uint8_t *text);


#define     USART_BUF_SIZE   256
volatile uint8_t     debug_buf[USART_BUF_SIZE];
volatile uint8_t     gps_buf[USART_BUF_SIZE];
volatile  uint8_t     GSM_buf[512];
volatile uint8_t gsm_answer;
volatile uint8_t redirect_GPSrx_DEBUG;
volatile uint8_t redirect_GSMrx_DEBUG;
volatile  uint8_t  gsm_data_received;
volatile  uint8_t  data_received;
uint16_t copy_ringbuffer(char *buf);

#endif /* SERIAL_SGS_H_ */