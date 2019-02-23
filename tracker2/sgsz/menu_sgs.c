/*
 * menu_sgs.c
 *
 * Created: 22.04.2016 16:18:11
 *  Author: Marc Adolf
 */ 




#include <sgs/port_sgs.h>
#include "main.h"
#include "serial_sgs.h"
#include "menu_sgs.h"
#include "bma222_sgs.h"
#include "dataflash_sgs.h"

void display_menu(char *title);
void spi_menu(void);


#define NEW_START_ADDRESS 0x0
volatile uint8_t uc_done,show_debug, uc_key, uc_index , uc_index_old = 0;
void main_menu(void)
{
	
	display_menu("main");
 	while (!uc_done) 
	{
		//if Button is pressed do a hardware-Test startet by Interrupt
		if (doHardwaretest==1)
		{
			
			doHardwaretest = 0;
			gpio_set_pin_level(led_pwm0, 0);
			gpio_set_pin_level(led_pwm1, 0);
			gpio_set_pin_level(BLE_APP_LED, 0);
			delay_ms(1000);
			gpio_set_pin_level(led_pwm0, 1);
			delay_ms(500);
			gpio_set_pin_level(led_pwm1, 1);
			delay_ms(500);
			gpio_set_pin_level(BLE_APP_LED, 1);
		}
		
		///check for new keypress
	uc_index= SER3_DEBUG.rx.write_index;
	if (uc_index != uc_index_old)
	{
		 	 uc_key =SER3_DEBUG.rx.buf[uc_index-1];
	 
	}
	uc_index_old = uc_index;

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
				/* Rebase the Stack Pointer */
			/* Load the Reset Handler address of the application */
// 			uint32_t new_start_address;
// 	        new_start_address = *(uint32_t *)(NEW_START_ADDRESS + 4);	
// 			__set_MSP(*(uint32_t *) NEW_START_ADDRESS);
// 
// 			/* Rebase the vector table base address */
// 			SCB->VTOR = ((uint32_t) NEW_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);
// 		
// 			/* Jump to application Reset Handler in the application */
// 			asm("bx %0"::"r"(new_start_address));
			
			uc_key = 0;
			break;
				
			case 'd':
			gpio_toggle_pin_level(GPS_power_enable);
			if  (gpio_get_pin_level(GPS_power_enable))
			send_debug("Satelite Receiver on\n\r");
			else 
			send_debug("Satelite Receiver off \n\r");
			
			redirect_GPSrx_DEBUG = gpio_get_pin_level(GPS_power_enable);
			delay_ms(2000);
			send_gps("at/n/r/0");
			//send_gps("PMTK314,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,5,0*2F");
			delay_ms(2000);
			uc_key = 0;
			break;
			
			case 's':
			send_debug("SPI Menu \n\r\0");
			spi_menu();
			uc_key = 0;
			break;
			
			
			
			case 'h':
			send_debug(" hardware menue \n\r");
			uc_key = 0;
			break;
			
			
			case 'm':
			 bma222_set_fifo(1);
	 
 bma222_get_acceleration(buf_mad);
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
	uint8_t show_debug = 0;

	display_menu("gsm");

	while (!uc_done)
	{
		
		//get new single input charakters
			///check for new keypress
		uc_index= SER3_DEBUG.rx.write_index;
		if (uc_index != uc_index_old)
		{
		 	 uc_key =SER3_DEBUG.rx.buf[uc_index-1];
		}
		uc_index_old = uc_index;
		

		switch (uc_key) {
			
			
			case 's':
		    send_debug(" Start modem \n\r");
			powerUp_gsm_modem();
			redirect_GPSrx_DEBUG = gpio_get_pin_level(GSM_power_enable);
			uc_key = 0;
			break;
			
			
			
			case 'e':
		    send_debug(" Stop Modem \n\r");
			powerDown_gsm_modem();
			redirect_GPSrx_DEBUG = gpio_get_pin_level(GSM_power_enable);
			uc_key = 0;
			break;
			
			
			
			case 'g':
			//show learn menu
			send_debug(" get Status \n\r");
			
			check_status_gsm_modem();
     		uc_key = 0;
			delay_ms(50);
			
			break;
		
			
			case 'c':
			//show learn menu
			send_debug(" call a number\n\r");
			gsm_answer=1;
			testcall();
			uc_key = 0;
			break;
			
			case 'd':
			//show learn menu
			send_debug(" do SMS \n\r");
			
			uc_key = 0;
			show_debug = 0;
			break;
			
			case 'r':
			//show learn menu
			send_debug(" repeat test cycle \n\r");
			uc_key = 0;
			break;
			
			case 'h':
			//show learn menu
			send_debug(" toggle hibernate modem \n\r");
			sleep_gsm_modem();
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
// 	uint8_t uc_key = 0;
// 	uint8_t uc_done = 0;
// 	uint8_t show_debug = 0;

	display_menu("spi");

	while (!uc_done)
	{
		
		//get new single input charakters
			///check for new keypress
		uc_index= SER3_DEBUG.rx.write_index;
		if (uc_index != uc_index_old)
		{
		 	 uc_key =SER3_DEBUG.rx.buf[uc_index-1];
		}
		uc_index_old = uc_index;
		
int x=0 ;


		switch (uc_key) {
			
			
		case '1':
	     DF_PowerDown();
		 delay_ms(5000);
		 DF_PowerUp();	 
		 delay_ms(2000);
		 // gpio_toggle_pin_level(IMU_CS);
		 // gpio_get_pin_level(IMU_CS)?send_debug(" switch off IMU CS \n\r"):send_debug(" switch on IMU CS \n\r");
		  uc_key = 0;
		break;
		case '2':
			gpio_toggle_pin_level(DATAFLASH_CS);
		    gpio_get_pin_level(DATAFLASH_CS)?send_debug(" switch off Dataflash CS \n\r"):send_debug(" switch on Dataflash CS \n\r");
			uc_key = 0;
			break;
				
		case '3':
			//show Statusbytes of Dataflash
			gpio_set_pin_level(IMU_CS,1);
			gpio_set_pin_level(DATAFLASH_CS,0);
			delay_ms(2);
			testData[0]= 0x9F;
			send_debug(" get Manufacturer code\n\r");
		    io_write(spi_io, testData, 1);
					
			io_read(spi_io, testData, 8);
		    delay_ms(2);
			/* testData= 0x57;*/
			sprintf(buf_mad, "Hersteller:%x\n\rProdukt:%x\n\r",testData[0], testData[1] );
			send_debug(buf_mad);
			delay_ms(4);
			delay_ms(4);
			gpio_set_pin_level(DATAFLASH_CS,1);
			uc_key = 0;
		break;			
			
	    case '4':
			//show Statusbytes of IMU
			gpio_set_pin_level(IMU_CS,0);
			gpio_set_pin_level(DATAFLASH_CS,1);
			delay_ms(2);
			testData[0]= 0x80;
			send_debug(" get Manufacturer code\n\r");
		    io_write(spi_io, testData, 1);
			io_read(spi_io, testData, 64);
		    delay_ms(2);
			/* testData= 0x57;*/
			sprintf(buf_mad, "Produkt_ID:%x\n\rAxis X:%x Y: %x Z:%x\n\r",testData[0], testData[3], testData[5] , testData[7]  );
			send_debug(buf_mad);
			delay_ms(4);
			gpio_set_pin_level(IMU_CS,1);
			sprintf(buf_mad, "Temperature:%i\n\r",  bma222_get_temperature());
			send_debug(buf_mad);
			delay_ms(4);
	    	
			uc_key = 0;
			break;	

	   case '5':
			// IMU TAP testing
			// enable interrupt source tap
			gpio_set_pin_level(IMU_CS,0);
			gpio_set_pin_level(DATAFLASH_CS,1);
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
			send_debug(" set Tap interrupt\n\r");
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

			uc_key = 0;
		break;	
	
		case '7':
			//debug IMU
			send_debug(" do a complete test-cycle of the IMU \n\r");
			gpio_set_pin_level(PA13,0);
			gpio_set_pin_level(PA14,0);
			gpio_set_pin_level(PA15,0);
			uc_key = 0;
			delay_ms(4);
// 			sprintf(buf_mad, "Produkt_ID:%x\n\r",bma222_get_id() );
// 			send_debug(buf_mad);
// 			bma222_get_acceleration(buf_mad);
// 			sprintf(buf_mad, "Axelleration Axis X:%x Y: %x Z:%x\n\r",buf_mad[0], buf_mad[1], buf_mad[2] );
// 			send_debug(buf_mad);
	
 			bma222_reset_tap_interrupts();
			  bma222_set_fifo(2);
    // 		bma222_set_tap_interrupt1();
 	//		bma222_set_tap_interrupt2();
			bma222_set_motion_interrupt1();
		#if 0
	df_read_status ();
			delay_ms(4);
			buf_mad[0]='s';
			buf_mad[1]='t';
			DF_BufferWriteStr(1,0, 256, buf_mad);
			
			DF_BufferToPage(1,2);
			DF_PageToBuffer(2,2);
			buf_mad[0]='o';
     		buf_mad[1]='o';
			DF_BufferReadStr (2, 0, 256, buf_mad);
#endif
// 			buf_mad[0]='o';
// 			buf_mad[1]='o';
// 			delay_ms(1);
// 			DF_BufferReadStr (1, 0, 256, buf_mad);
// 			buf_mad[100]='G';
// 			buf_mad[101]='G';
// 			delay_ms(1);
// 			DF_BufferWriteStr (1, 20, 234, &buf_mad[100]);
// 			delay_ms(1);
// 			DF_BufferReadStr (1, 10, 246, buf_mad);
// 			sprintf(debug_buf,"SingleByte:%x\n\r", DF_BufferReadByte(1, 10)) ;
// 			send_debug(debug_buf);
// 			delay_ms(4);
		break;
	
	
					
		case 'b':
			//back to main menu
			send_debug(" back to main menu \n\r");
			uc_key = 0;
			display_menu("main");
			return;
			break;
			
			
			case '6':
			//show toggle sleep Dataflash
			gpio_set_pin_level(IMU_CS,1);
			gpio_set_pin_level(DATAFLASH_CS,0);
			delay_ms(2);
			testData[0]= 0x80;
			send_debug(" get Manufacturer code\n\r");
			io_write(spi_io, testData, 1);
			
			io_read(spi_io, testData, 64);
			delay_ms(2);
			/* testData= 0x57;*/
			sprintf(buf_mad, "Produkt_ID:%x\n\rAxis X:%x Y: %x Z:%x\n\r",testData[0], testData[3], testData[5] , testData[7]  );
			send_debug(buf_mad);
			delay_ms(4);
			delay_ms(4);
			gpio_set_pin_level(IMU_CS,1);
			uc_key = 0;
			break;
			
			
			default:
			break;
		}
	}
}

void display_menu(char *title)
{
	!strcmp( title ,"main") ? sprintf(buf_mad, MENU_HEADER):
	!strcmp( title ,"spi") ?  sprintf(buf_mad, SPI_MENU_HEADER):
	!strcmp( title ,"gsm") ?  sprintf(buf_mad, GSM_MENU_HEADER):
	!strcmp( title ,"gps") ?  sprintf(buf_mad, GPS_MENU_HEADER):sprintf(buf_mad, "Menue missing\0");	
	delay_ms(3);
 	send_debug(buf_mad);
	delay_ms(3);
    
	
}