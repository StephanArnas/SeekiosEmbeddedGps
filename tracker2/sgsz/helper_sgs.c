/*
 * helper_sgs.c
 *
 * Created: 27.04.2016 07:34:07
 *  Author: Marc Adolf
 */ 

#include <sgs/port_sgs.h>
#include "usb_main.h"
#include "ble_config.h"
#include "sleep_manager_main.h"
#include "stdio_start.h"
#include "hal_usart_async.h"
#include "stdbool.h"
#include "hal_delay.h"
#include "serial_sgs.h"
#include "menu_sgs.h"
#include "helper_sgs.h"




	volatile uint8_t sleep_counter =0;
	volatile  uint8_t status_counter=0;
    volatile int callreceive_counter=0;
    
	
	
	
	/** Counts for 1ms time ticks. */
volatile uint32_t g_ms_ticks = 0;

#define TICK_US 1000


//SysTick is the Hardware instance

/**
 * \brief Handler for Sytem Tick interrupt.
 *
 * Process System Tick Event
 * Increments the g_ms_ticks counter.
 */
//  void SysTick_Handler(void)
//  {
//  	g_ms_ticks++;
//  }

// void time_tick_init(void)
// {
// 	g_ms_ticks = 0;
// 
// 	/* Configure systick */
// 	if (SysTick_Config(sysclk_get_cpu_hz() / TICK_US)) {
// 		Assert(false);
// 	}
// }

// uint32_t time_tick_get(void)
// {
// 	return g_ms_ticks;
// }
// 
// uint32_t time_tick_calc_delay(uint32_t tick_start, uint32_t tick_end)
// {
// 	if (tick_end >= tick_start) {
// 		return (tick_end - tick_start) * (1000 / TICK_US);
// 	} else {
// 		/* In the case of 32-bit couter number overflow */
// 		return (tick_end + (0xFFFFFFFF - tick_start)) * (1000 / TICK_US);
// 	}
//	
	
	
	


void led_init(void)
{
	gpio_set_pin_direction(led_pwm0, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(led_pwm0, GPIO_PIN_FUNCTION_OFF);
   	gpio_set_pin_direction(led_pwm1, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(led_pwm1, GPIO_PIN_FUNCTION_OFF);
  	gpio_set_pin_direction(BLE_APP_LED, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(BLE_APP_LED, GPIO_PIN_FUNCTION_OFF);
}


hardwaretest()
{
	uint32_t xTickCount;
    uint32_t x = xTickCount;
	delay_ms(1);
	uint32_t y = xTickCount;
	//LED-Test
	send_debug(" Start LED test. \n\r");
	gpio_set_pin_level(led_pwm0, 0);
	gpio_set_pin_level(led_pwm1, 0);
	gpio_set_pin_level(BLE_APP_LED, 0);
	delay_ms(1000);
	gpio_set_pin_level(led_pwm0, 1);
	delay_ms(500);
	gpio_set_pin_level(led_pwm1, 1);
	delay_ms(500);
	gpio_set_pin_level(BLE_APP_LED, 1);
	delay_ms(1000);
	gpio_set_pin_level(led_pwm0, 0);
	gpio_set_pin_level(led_pwm1, 0);
	gpio_set_pin_level(BLE_APP_LED, 0);

	uint32_t z = xTickCount;
	#if 0
//set IMU for Tap-interrupt
	send_debug(" switched on IMU CS \n\r");
	gpio_set_pin_level(DATAFLASH_CS,1);
	gpio_set_pin_level(IMU_CS,1);
    delay_ms(1);
	gpio_set_pin_level(IMU_CS,0);
	delay_ms(1);
	testData[0]= 0x1E;  //register address
	testData[1]= 0b00010000;
	io_write(spi_io, testData, 2);		
	delay_ms(1);
	gpio_set_pin_level(IMU_CS,1);
	delay_ms(2);
		
// map Interrupt x19
	gpio_set_pin_level(IMU_CS,0);

	delay_ms(2);
	testData[0]= 0x19;  //register address
	testData[1]= 0b00100000;
	send_debug(" set Tap interrupt\n\r");
	io_write(spi_io, testData, 2);
			
	delay_ms(2);
	gpio_set_pin_level(IMU_CS,1);
	delay_ms(2);
	gpio_set_pin_level(IMU_CS,0);
	delay_ms(2);		
			
			
	// tap-enable x16
	gpio_set_pin_level(IMU_CS,0);

	delay_ms(2);
	testData[0]= 0x16;  //register address
	testData[1]= 0b00100000;
	io_write(spi_io, testData, 2);
			
	delay_ms(2);
	gpio_set_pin_level(IMU_CS,1);
	delay_ms(2);
	gpio_set_pin_level(IMU_CS,0);
	delay_ms(2);
	testData[0]= 0x80;                                    //check all Data settings
	io_write(spi_io, testData, 1);
	io_read(spi_io, testData, 64);
			
	delay_ms(2);
	/* testData= 0x57;*/
	sprintf(buf_mad, "Produkt_ID:%x\n\rAxis X:%x Y: %x Z:%x\n\r",testData[0], testData[3], testData[5] , testData[7]  );
	send_debug(buf_mad);
	delay_ms(4);
	gpio_set_pin_level(IMU_CS,1);
	delay_ms(4);
	send_debug(" Tap at least 3 times\n\r");
#endif
			
}


read_hardware_id()
{
//	Word 0: 0x0080A00C
//	Word 1: 0x0080A040
//	Word 2: 0x0080A044
//	Word 3: 0x0080A048

//	The following code snippet can be used to extract the values from these addresses.

	uint32_t val1, val2, val3, val4, val5, val6;
	uint32_t *ptr1 = (volatile uint32_t *)0x0080A00C;
	val1 = *ptr1;
	uint32_t *ptr = (volatile uint32_t *)0x0080A040;
	val2 = *ptr;
	ptr++;
	val3 = *ptr;
	ptr++;
	val4 = *ptr;
	
	uint32_t *ptr2 = (volatile uint32_t *)0x0E00E010;
	val5 = *ptr2;
	delay_ms(1);
    val6 = *ptr2;
	
	// PM->APBBMASK.reg = 8;

  /* PA19: input with pulldown */
  PORT->Group[0].PINCFG[13].bit.INEN = 1;
  PORT->Group[0].PINCFG[13].bit.PULLEN = 1;
  PORT->Group[0].OUTCLR.bit.OUTCLR = (1 << 13);

//   /* PA16: input with pullup */
//   PORT->Group[0].PINCFG[16].bit.INEN = 1;
//   PORT->Group[0].PINCFG[16].bit.PULLEN = 1;
//   PORT->Group[0].OUTSET.bit.OUTSET = (1 << 16);

  /* LED0 on XPlainedPro: PB30, active low */
  PORT->Group[0].DIRSET.bit.DIRSET = (1 << 14);
  PORT->Group[0].OUTSET.bit.OUTSET = (1 << 14);
	
//   for (;;)
//     {
//       if ((PORT->Group[0].IN.bit.IN & (1 << 13)) != 0)
//       {
// 		/* PA19 pulled high */
// 		/* turn on LED */
// 		PORT->Group[0].OUTCLR.bit.OUTCLR = (1 << 14);
// 		/* wait for button release */
// 		while ((PORT->Group[0].IN.bit.IN & (1 << 13)) != 0)
// 		  {
// 		  }
// 	   	
	
	//}
}




void init_sgs(void)
{
	
	system_init();
	serial_io_init();
	led_init();
	//PWM_LED1_PORT_init();
	delay_ms(3);
	//process_data("process_test\r\n\0");
	delay_ms(3);
	EXTERNAL_IRQ_example();
	read_hardware_id();
	//     strcpy( buf_mad,"Start\r\n\0");
	// 	SER3_DEBUG_print();
	delay_ms(1);
	hardwaretest();
	
	
	main_menu();
// 
// 	//Switch on GSM
// 	start_gsm_modem();
// 	delay_ms(20);
// 	check_status_gsm_modem();
// 	delay_ms(1000);
// 
// 	sleep_gsm_modem();
//     gpio_set_pin_level(GPS_power_enable, true);
	
}


void loop_sgs(void)
{
	if (testcall_flag)
	testcall();
	delay_ms(100);

	if (status_counter >10)
	{
		if (sleep_counter==4)
		{
			delay_ms(100);
			wakeup_gsm_modem();
			delay_ms(100);
		
		}
		if (sleep_counter >6)
		{
			delay_ms(100);
			sleep_gsm_modem();
			delay_ms(100);
			sleep_counter =0;
		}
	
		check_status_gsm_modem();
		status_counter=0;
		sleep_counter++;
	
}

status_counter++;
	
}



//struct io_descriptor *usart_debug_io;

void send_gsm_modem(const char *text)
{
	// 	struct io_descriptor *io;
	// 	usart_async_get_io_descriptor(&SER2_GSM, &io);
	// 	usart_async_enable(&SER2_GSM);
	io_write(usart_gsm_io,(const uint8_t *) text, strlen(text));
}

void send_debug(const char *text)
{
	io_write(usart_debug_io, (const uint8_t *) text, strlen(text));
}



void send_gps(const char *text)
{
// 	struct io_descriptor *io;
// 	usart_async_get_io_descriptor(&SER1_GPS, &io);
// 	usart_async_enable(&SER1_GPS);
	io_write(usart_gps_io, (const uint8_t *) text, strlen(text));
}

void testcall(void)
{
	
	send_gsm_modem("AT+CFUN=1\r\n\0");
	delay_ms(10);
	send_debug("Testcall starting \r\n");
	delay_ms(10);
	send_gsm_modem("AT\r\n\0");
	delay_ms(10);
	send_gsm_modem("AT+cpin?\r\n\0");
	delay_ms(10);
	send_gsm_modem("AT+CREG?\r\n\0");
	delay_ms(100);
	send_gsm_modem("ATD071959292263;\r\n\0");
	delay_ms(19000);
	send_gsm_modem("ATH\r\n\0");
	delay_ms(1);
	send_debug("Testcall ended \r\n");
	delay_ms(20);
	testcall_flag=0;
}

void powerUp_gsm_modem(void)
{
	gpio_set_pin_level(GSM_power_enable, true);
	delay_ms(5);
	
}

void powerDown_gsm_modem(void)
{
	gpio_set_pin_level(GSM_power_enable, false);
	
// 	gpio_set_pin_level(GSM_power_key, true);
// 	delay_ms(20);
// 	send_debug("power down GSM_modem \r\n");
// 	//gpio_set_pin_level(GSM_power_enable, false);
// 	gpio_set_pin_level(GSM_power_key, false);
// 	//gpio_set_pin_level(GSM_DTR_wake, false);
// 	delay_ms(1000);
// 	gpio_set_pin_level(GSM_power_key, true);
// 	delay_ms(1000);
	
}




void start_gsm_modem(void)
{
	send_gsm_modem("AT\r\n\0");
	delay_ms(20);
	
	gpio_set_pin_level(GPS_power_enable, true);
	//gpio_set_pin_level(GSM_power_enable, false);
	//gpio_set_pin_level(GSM_power_key, false);
	gpio_set_pin_level(GSM_DTR_wake, false);
	delay_ms(1000);
	gpio_set_pin_level(GSM_power_enable, true);
	delay_ms(1000);
	//gpio_set_pin_level(GSM_power_key, true);
	delay_ms(1000);
	gpio_set_pin_level(GSM_power_key, true);
	delay_ms(2000);
	send_gsm_modem("AT\r\n\0");
	delay_ms(20);
	send_debug("GSM_modem ready \r\n\0");
	
	delay_ms(1000);
	send_gsm_modem("AT\r\n\0");
	delay_ms(20);
	send_debug("1 \r\n");
		
	delay_ms(1000);
	send_gsm_modem("AT\r\n\0");
	delay_ms(20);
	send_debug("2 \r\n");
	
	delay_ms(20);
	send_gsm_modem("AT+CSCLK=1\r\n\0");
	delay_ms(30);
	send_gsm_modem("AT+IPR=115200\r\n\0");//set 115200Baud
	delay_ms(30);
	//send_gsm_modem("AT+CFUN=0\r\n\0");
	delay_ms(30);
}

void stop_gsm_modem(void)
{
	
}

void sleep_gsm_modem(void)
{
	delay_ms(30);
	send_debug("modem sent to sleep-mode\r\n\0");
	delay_ms(30);
	send_gsm_modem("AT+IPR=115200\r\n\0");
	delay_ms(30);
	send_gsm_modem("AT+CFUN=1\r\n\0");
	delay_ms(30);
	send_gsm_modem("AT+CSCLK=1\r\n\0");   //   2->Auto - 1->DTR
	delay_ms(30);
	gpio_set_pin_level(GSM_DTR_wake, false);  //high can enter sleep, low can leave sleep
	delay_ms(10000);
	gpio_set_pin_level(GSM_DTR_wake, true);
	//send_gsm_modem("AT+CPOWD=1\r\n\0");
	delay_ms(300);
	//powerDown_gsm_modem();
	//delay_ms(10000);
	// 	delay_ms(10000);
	// 	delay_ms(10000);
	// 	delay_ms(10000);
	// 	delay_ms(10000);
	// 	delay_ms(10000);
	// 	delay_ms(10000);
	// 	delay_ms(10000);
	// 	delay_ms(10000);	
}

void wakeup_gsm_modem(void)
{
	send_gsm_modem("AT+CFUN=1\r\n\0");
	delay_ms(3);
	send_debug("modem woke up\r\n\0");
	
	delay_ms(3);
	send_gsm_modem("AT\r\n\0");
	delay_ms(100);
	send_gsm_modem("AT+CFUN=1\r\n\0");
	delay_ms(100);
	send_gsm_modem("AT\r\n\0");
	delay_ms(100);
	gpio_set_pin_level(GSM_DTR_wake, true);
}

void check_status_gsm_modem(void)
{
	send_gsm_modem("AT\r\n\0");
	delay_ms(100);
	send_gsm_modem("AT+CPIN?\r\n\0");
	delay_ms(30);
	send_gsm_modem("AT+CREG?\r\n\0");
	
	delay_ms(30);
	send_gsm_modem("AT+CSQ\r\n\0");//Signalstaerke
	delay_ms(30);
	send_gsm_modem("AT+CFUN?\r\n\0");//Function Mode
	delay_ms(30);
	send_gsm_modem("AT+CSCLK?\r\n\0");//sleepmode
	delay_ms(30);
	send_gsm_modem("AT+CBC\r\n\0");//sleepmode
	
	delay_ms(300);
	send_gsm_modem("AT+CMTE?\r\n\0");//temperatur
	
	delay_ms(300);
	send_gsm_modem("AT\r\n\0");//temperatur
	delay_ms(30);
}



#if 0
/* Check for USART commands */
if (debug_callback_receive_flag)
{
	/* Reset USART flag */
	
	callreceive_counter++;
	debug_callback_receive_flag = false;
	
	// 	           /* Check what command is received */
	// 	           if (strcmp((char*)usart_rx_string, (char*)command_sid_request) == 0)
	// 	           {
	// 		           /* Handshaking */
	// 		           usart_write_buffer_job(&usart_instance, command_sid_response, strlen((char*)command_sid_response));
	// 	           }
	// 	           else if (strcmp((char*)usart_rx_string, (char*)command_stream_start) == 0)
	// 	           {
	// 		           /* Start Streaming */
	// 		           tc6_start_counter();
	// 	           }
	// 	           else if (strcmp((char*)usart_rx_string, (char*)command_stream_stop) == 0)
	// 	           {
	// 		           /* Stop Streaming */
	// 		           tc6_stop_counter();
	// 	           }
	// 	           else
	// 	           {
	// 		           /* an unknown command is received */
	// 		           usart_write_buffer_job(&usart_instance, command_unknown, strlen((char*)command_unknown));
	// 	           }
	//            }
	//
	//            /* Check for calibration flag */
	//            if (execute_calibration_flag)
	//            {
	// 	           execute_calibration_flag = false;
	// 	           bsx_docalibration();
	//            }
	//
	
	}/* While(True) */
#endif
	

