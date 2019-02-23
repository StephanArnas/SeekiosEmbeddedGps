/*
 * menu_sgs.c
 *
 * Created: 22.04.2016 16:18:11
 *  Author: Marc Adolf
 */ 




#include "port_sgs.h"
#include "main.h"
#include "bma222_sgs.h"
#include "helper_sgs.h"
#include "hal_sleep.h"
#include <modesManager/modes.h>
void display_menu(char *title);
void spi_menu(void);

void gsm_menu(void);

#define NEW_START_ADDRESS 0x0
volatile uint32_t uc_done,show_debug, uc_key, uc_index , uc_index_old = 0;
void main_menu(void)
{
	//DF_PowerDown();
	//init_pins();
	display_menu("main");
 	while (!uc_done) 
	{
		
		//if Button is pressed do a hardware-Test startet by Interrupt
		if (doHardwaretest==1)
		{
			doHardwaretest = 0;
			gpio_set_pin_level(LED_0_gn, 0);
			gpio_set_pin_level(LED_1_rd, 0);
			gpio_set_pin_level(LED_2_ye, 0);
			delay_ms(1000);
			gpio_set_pin_level(LED_0_gn, 1);
			delay_ms(500);
			gpio_set_pin_level(LED_1_rd, 1);
			delay_ms(500);
			gpio_set_pin_level(LED_2_ye, 1);
			delay_ms(3000);
			gpio_set_pin_level(LED_0_gn, 0);
			gpio_set_pin_level(LED_1_rd, 0);
			gpio_set_pin_level(LED_2_ye, 0);
			//sleep(4);
		}
	    delay_ms(10);
	    ///check for new keypress
		//uc_index= SER3_DEBUG.rx.write_index %64;  //Modulo 64 da Eingangspuppfer 64bit
		if (uc_index != uc_index_old)
		{
		 		 //uc_key =SER3_DEBUG.rx.buf[uc_index-1];
	 
		}
		uc_index_old = uc_index;
		printf("Pick an option :\r\n");
		uc_key = getchar();
		switch (uc_key) {
			case 'g':
			//start GSM modem
			send_debug("GSM-modem menue \n\r");
			delay_ms(3);
			gsm_menu();
			uc_key = 0;
			break;
			
					
			case 'r':
			//show learn menu
			send_debug(" Reset controller \n\r");
			NVIC_SystemReset();
			uc_key = 0;
			break;
				
			case 'd':
			gpio_toggle_pin_level(GPS_power_enable);
			if  (gpio_get_pin_level(GPS_power_enable))
			send_debug("Satelite Receiver on\n\r");
			else 
			send_debug("Satelite Receiver off \n\r");
			redirect_GPSrx_DEBUG = gpio_get_pin_level(GPS_power_enable);
			uc_key = 0;
			break;
			
			case 's':
			send_debug("SPI Menu \n\r\0");
			spi_menu();
			uc_key = 0;
			break;
			
			
			
			case '1':
			send_debug("calendar test in 8 seconds\n\r\0");
			delay_ms(4);
			CALENDAR_X_example();

		
			uc_key = 0;
			break;
			
			uc_key = 0;
			break;
			
			case '2':			
// 			bma222_set_bandwith(0x08);   //0x08 = 7,6 HZ
// 			bma222_set_sleep_duration(0x0F);  //
// 			bma222_reg_overwrite(0x12, 0x40);
			uc_key = 0;
			break;
			
	
			
			
			case 'h':
			send_debug(" Set Controller in sleep-mode, wake up from Ring, Button or IMU \n\r\0");
			delay_ms(8);
			sleep_deep();

			break;
			
			case 'm':
		    bma222_set_fifo(1); 
			bma222_get_acceleration(genericBuf);
			//flash routine
			send_debug(" Show menu \n\r\0");
			display_menu("main");
			uc_key = 0;
			show_debug = 0;
			break;
			case 'q':
			uc_key = 0;
			uc_done = 1;
			show_debug = 0;
			send_debug(" Quit Configuration \n\r");
			break;
			default:
			break;
		}
		
		
	}
}





void gsm_menu(void)
{
	uint8_t uc_key = 0;
	uint8_t uc_done = 0;

	display_menu("gsm");

	while (!uc_done)
	{
		
		//get new single input charakters
				 delay_ms(10);
		///check for new keypress
		//uc_index= SER3_DEBUG.rx.write_index %64;  //Modulo 64 da Eingangspuppfer 64bit
		
		if (uc_index != uc_index_old)
		{
			//uc_key =SER3_DEBUG.rx.buf[uc_index-1];
			
		}
		uc_index_old = uc_index;
		

		switch (uc_key) {
			
			
			case 's':
		    send_debug(" Start modem \n\r");
			gsm_power_up();
			redirect_GSMrx_DEBUG = gpio_get_pin_level(GSM_power_enable);
			delay_ms(5000);
			gsm_startup();
			
			uc_key = 0;
			break;
			
			case 'h':
			send_debug(" hibernate modem, call to wakeup \n\r");
			gpio_set_pin_level(GSM_DTR_wake, true);
			
			uc_key = 0;
			break;
			
			
			case 'e':
		    send_debug(" Stop Modem \n\r");
			gsm_power_down();
			redirect_GPSrx_DEBUG = gpio_get_pin_level(GSM_power_enable);
			uc_key = 0;
			break;
			
			
			case 'i':
			//show learn menu
			send_debug(" get IMEI: \n\r");
			gsm_get_imei();
			
			
			
			uc_key = 0;
			delay_ms(50);
			
			break;
			
			case 'g':
			//show learn menu
			send_debug(" get Status \n\r");
			gsm_status();		
     		uc_key = 0;
			delay_ms(50);		
			break;
		
			
			case 'c':
			//testcall
			send_debug(" call a number\n\r");
			gsm_answer=1;
			testcall();
			uc_key = 0;
			break;
			
			
			case 'm':
			//show learn menu
			send_debug("show this menu \n\r");
			display_menu("gsm");
			uc_key = 0;
			break;			
			
			case 'b':
			//back to main menu
			send_debug(" back to main menu \n\r");
			uc_key = 0;
			display_menu("main");
			return;
			break;
			
			
			default:
			break;
		}
	}
}


void spi_menu(void)
{
 	uint8_t uc_key = 0;
  	uint8_t uc_done = 0;
	int8_t acc[3];
	uint8_t spi_sleep_toggle;
	uint8_t spi_sleep_dur_toggle=0;
    uc_key=0;
	display_menu("spi");
	uint8_t reg_val;
	while (!uc_done)
	{
		//get new single input charakters
			///check for new keypress
	      delay_ms(10);
			///check for new keypress
		//uc_index= SER3_DEBUG.rx.write_index %64;  //Modulo 64 da Eingangspuppfer 64bit;
		
		if (uc_index != uc_index_old)
		{
		 	 //uc_key =SER3_DEBUG.rx.buf[uc_index-1];
		}
		uc_index_old = uc_index;
		printf("Chose an option : \r\n");
		uc_key = getchar();
		switch (uc_key) {
			
		case '1':  	    
		  gpio_toggle_pin_level(IMU_CS);
		  gpio_get_pin_level(IMU_CS)?send_debug(" switch off IMU CS \n\r"):send_debug(" switch on IMU CS \n\r");
		  uc_key = 0;
		break;
		
		case '2':
			gpio_toggle_pin_level(DATAFLASH_CS);
		    gpio_get_pin_level(DATAFLASH_CS)?send_debug(" switch off Dataflash CS \n\r"):send_debug(" switch on Dataflash CS \n\r");
			uc_key = 0;
			break;
				
		case '3':
			//show Statusbytes of Dataflash
			DF_PowerUp();
			gpio_set_pin_level(IMU_CS,1);
			gpio_set_pin_level(DATAFLASH_CS,0);
			delay_ms(2);
			genericBuf[0]= 0x9F;
			send_debug(" get Manufacturer code\n\r");
		    io_write(spi_io, genericBuf, 1);
					
			io_read(spi_io, genericBuf, 8);
		    delay_ms(2);
			/* testData= 0x57;*/
			sprintf(genericBuf, "Hersteller:%x\n\rProdukt:%x\n\r",genericBuf[0], genericBuf[1] );
			send_debug(genericBuf);
			delay_ms(4);
			delay_ms(4);
			gpio_set_pin_level(DATAFLASH_CS,1);
			uc_key = 0;
		break;			
			
	    case '4':
			//show Statusbytes of IMU
			for(int i=0;i<10;i++){
				bma222_get_acceleration(acc);
				printf("\rProdukt_ID:%x\n\rAxis X:%d Y: %d Z:%d\n\r",bma222_get_id(),acc[0], acc[1] , acc[2] );
				//send_debug(buf_mad);
				//delay_ms(4);
				//gpio_set_pin_level(IMU_CS,1);
				//sprintf(buf_mad, "Temperature:%i\n\r",  bma222_get_temperature());
				//send_debug(buf_mad);
				delay_ms(50);
			}
	    	
			uc_key = 0;
			break;	

	   case '5':
			// IMU TAP testing
			// set single tap interrupt 
			bma222_set_single_tap_interrupt();		
			uc_key = 0;
		break;	
	
	   case '6':
	   // IMU TAP testing
	   // set double tap interrupt
	   bma222_set_double_tap_interrupt2();
	   uc_key = 0;
	   break;
	
		case '7':
		// IMU testing
		// set motion interrupt
		//bma222_set_motion_interrupt(2);
		imu1_count = 0;
		IMU_interrupt_config = 1;
		bma222_set_slope_motion_interrupt(2);
		//bma222_set_slo_mo_interrupt(2);
		uc_key = 0;
		break;
	
	
		case '8':
		// IMU TAP testing
		// reset imu interrupts
		bma222_reset_tap_interrupts();
		uc_key = 0;
		break;


		case '9':
		// toggle sleep of all SPI-modules
		if (spi_sleep_toggle==0)
		{
			sprintf(genericBuf, "new sleepmode lowpower: %x \n\r", bma222_set_sleepmode(1));
			
			send_debug(genericBuf);
			delay_ms(1);
			spi_sleep_toggle=1;
			delay_ms(4);
		}
		
		else if (spi_sleep_toggle==1)
		{
			sprintf(genericBuf, "new sleepmode suspend: %x \n\r", bma222_set_sleepmode(2));
			send_debug(genericBuf);
			delay_ms(1);
			spi_sleep_toggle=2;
			delay_ms(4);
		}
		
		else if (spi_sleep_toggle==2)
		{
			DF_PowerDown();
			sprintf(genericBuf, "Dataflash powerdown: - \n\r");
			send_debug(genericBuf);
			delay_ms(1);
			spi_sleep_toggle=3;
			delay_ms(4);
		}
		else
		{
			sprintf(genericBuf, "new sleepmode is none: %x \n\r", bma222_set_sleepmode(0));
			send_debug(genericBuf);
			delay_ms(1);
			DF_PowerUp();
			delay_ms(4);
			spi_sleep_toggle =0;
		}
		
		uc_key = 0;
		break;
		case 's':
			reg_val = bma222_reg_read(0x11);
			printf("Power mode reg : %x\r\n",reg_val);
			break;
		case 't':
			if(spi_sleep_dur_toggle == 0){
				printf("Sleep duration 0.5ms\r\n");
				bma222_set_sleep_duration(0);
				spi_sleep_dur_toggle = 1;
			}
			else if(spi_sleep_dur_toggle == 1){
				printf("Sleep duration 10ms\r\n");
				bma222_set_sleep_duration(0b1010);
				spi_sleep_dur_toggle = 2;
			}
			else if(spi_sleep_dur_toggle == 2){
				printf("Sleep duration 100ms\r\n");
				bma222_set_sleep_duration(0b1101);
				spi_sleep_dur_toggle = 3;
			}
			else if(spi_sleep_dur_toggle == 3){
				printf("Sleep duration 1s\r\n");
				bma222_set_sleep_duration(0b1111);
				spi_sleep_dur_toggle = 0;
			}
			break;		
		case 'b':
			//back to main menu
			send_debug(" back to main menu \n\r");
			uc_key = 0;
			display_menu("main");
			return;
			break;
			case 'm':
			//back to main menu
			send_debug(" show SPI-Menue \n\r");
			uc_key = 0;
			display_menu("spi");
			break;
			
			
			
			default:
			break;
		}
	}
}






void display_menu(char *title)
{
	!strcmp( title ,"main") ? printf(MENU_HEADER):
	//!strcmp( title ,"hardware") ? sprintf(buf_mad, HARDWARE_MENU_HEADER):
	!strcmp( title ,"spi") ?  printf(SPI_MENU_HEADER):
	!strcmp( title ,"gsm") ?  printf(GSM_MENU_HEADER):
	!strcmp( title ,"gps") ?  printf(GPS_MENU_HEADER):printf("Menue missing");	
	delay_ms(3);
 	//send_debug(buf_mad);
	delay_ms(3);
   
}