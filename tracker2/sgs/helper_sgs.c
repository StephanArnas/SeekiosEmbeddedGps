/*
 * helper_sgs.c
 *
 * Created: 27.04.2016 07:34:07
 *  Author: Marc Adolf
 */ 

#include "port_sgs.h"
#include "usb_main.h"
#include "sleep_manager_main.h"
#include "hal_usart_async.h"
#include "stdbool.h"
#include "hal_delay.h"
#include "serial_sgs.h"
#include "menu_sgs.h"
#include "helper_sgs.h"
#include "hri_rtc2_v110a.h"
#include "hpl_rtc2_v110_config.h"




volatile uint8_t sleep_counter =0;
volatile  uint8_t status_counter=0;
volatile int callreceive_counter=0;
    
	
	
	
/** Counts for 1ms time ticks. */
volatile uint32_t g_ms_ticks = 0;

#define TICK_US 1000



	
	
	
void RTC_Start( void )
{
	// $$$ Code to init the RTC.
	//clock for RTC : Xosc32k
	OSC32KCTRL->RTCCTRL.bit.RTCSEL = OSC32KCTRL_RTCCTRL_RTCSEL(OSC32KCTRL_RTCCTRL_RTCSEL_XOSC32K_Val);

	RTC->MODE0.CTRLA.bit.SWRST=1;
	while(RTC->MODE0.SYNCBUSY.bit.SWRST>0);

	RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENCLR_CMP0;

	RTC->MODE0.COMP[0].reg = 32768;
	
	while(RTC->MODE0.SYNCBUSY.bit.COMP0>0);

	//Mode 0
	RTC->MODE0.CTRLA.reg = RTC_MODE0_CTRLA_MODE(0)|RTC_MODE0_CTRLA_PRESCALER(RTC_MODE0_CTRLA_PRESCALER_DIV1_Val);
	RTC->MODE0.CTRLA.reg = RTC_MODE0_CTRLA_ENABLE|RTC_MODE0_CTRLA_MATCHCLR|RTC_MODE0_CTRLA_MODE(0)|RTC_MODE0_CTRLA_PRESCALER(RTC_MODE0_CTRLA_PRESCALER_DIV1_Val);
	//RTC->MODE0.CTRLA.reg = RTC_MODE0_CTRLA_MATCHCLR|RTC_MODE0_CTRLA_MODE(0)|RTC_MODE0_CTRLA_PRESCALER(RTC_MODE0_CTRLA_PRESCALER_DIV1_Val);
	while(RTC->MODE0.SYNCBUSY.bit.ENABLE>0);
}	
	
void RTC_Start_calenar_mode( void )
{
	// $$$ Code to init the RTC.
	//clock for RTC : Xosc32k
	OSC32KCTRL->RTCCTRL.bit.RTCSEL = OSC32KCTRL_RTCCTRL_RTCSEL(OSC32KCTRL_RTCCTRL_RTCSEL_XOSC32K_Val);

	RTC->MODE0.CTRLA.bit.SWRST=1;
	while(RTC->MODE0.SYNCBUSY.bit.SWRST>0);

	RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENCLR_CMP0;

	RTC->MODE0.COMP[0].reg = 32768;
	
	while(RTC->MODE0.SYNCBUSY.bit.COMP0>0);

	//Mode 0
// 	RTC->MODE0.CTRLA.reg = RTC_MODE0_CTRLA_MODE(0)|RTC_MODE0_CTRLA_PRESCALER(RTC_MODE0_CTRLA_PRESCALER_DIV1_Val);
// 	RTC->MODE0.CTRLA.reg = RTC_MODE0_CTRLA_ENABLE|RTC_MODE0_CTRLA_MATCHCLR|RTC_MODE0_CTRLA_MODE(0)|RTC_MODE0_CTRLA_PRESCALER(RTC_MODE0_CTRLA_PRESCALER_DIV1_Val);
	//RTC->MODE0.CTRLA.reg = RTC_MODE0_CTRLA_MATCHCLR|RTC_MODE0_CTRLA_MODE(0)|RTC_MODE0_CTRLA_PRESCALER(RTC_MODE0_CTRLA_PRESCALER_DIV1_Val);
	while(RTC->MODE0.SYNCBUSY.bit.ENABLE>0);
}	


	
void hardware_setup( void )
{

	//Running at 12MHz in PL0, requires 1 waitstate on flash
	NVMCTRL->CTRLB.bit.RWS = 2;
	//
	//     //Use xMHz system clock
	OSCCTRL->OSC16MCTRL.bit.FSEL= 0x00;
	MCLK->BUPDIV.reg = 0x80;
	MCLK->LPDIV.reg = 0x40;
	MCLK->CPUDIV.reg = 0x01;
	//set sleepmode
	PM->SLEEPCFG.reg = 0x4;      //0x2 APB clocks are off (IDLE)   0x4 Only RTC interrupt with ram retention
	
	//Workaround for errata #13674
	SUPC->VREF.reg |= 1<<8;
	
	//nLDO/Buck : Select Buck
	SUPC->VREG.bit.SEL = 1;
	while(SUPC->STATUS.bit.VREGRDY==0);
	
	//Use BOD for voltage integrity
	//BOD_init_sampled();
	
	//Enable RTC interrupts
	NVIC_ClearPendingIRQ(RTC_IRQn);
	NVIC_SetPriority(RTC_IRQn, 3);
	//NVIC_EnableIRQ(RTC_IRQn);
}






void led_init(void)
{
	gpio_set_pin_direction(LED_0_gn, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LED_0_gn, GPIO_PIN_FUNCTION_OFF);
   	gpio_set_pin_direction(LED_1_rd, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LED_1_rd, GPIO_PIN_FUNCTION_OFF);
  	gpio_set_pin_direction(LED_2_ye, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LED_2_ye, GPIO_PIN_FUNCTION_OFF);
}


void hardwaretest()
{
	//LED-Test
	send_debug(" Start LED test. \n\r");
	gpio_set_pin_level(LED_0_gn, 0);
	gpio_set_pin_level(LED_1_rd, 0);
	gpio_set_pin_level(LED_2_ye, 0);
	delay_ms(1000);
	gpio_set_pin_level(LED_0_gn, 1);
	delay_ms(500);
	gpio_set_pin_level(LED_1_rd, 1);
	delay_ms(500);
	gpio_set_pin_level(LED_2_ye, 1);
	delay_ms(1000);
	gpio_set_pin_level(LED_0_gn, 0);
	gpio_set_pin_level(LED_1_rd, 0);
	gpio_set_pin_level(LED_2_ye, 0);
}

/**
* \brief Get Voltage of Batteries and USB
*	
* \param[in] Array of Voltages to be read
*/
/*	voltages_BAT_USB[0] : BAT
	voltages_BAT_USB[1] : USB*/
void ADC_read_BAT_USB(uint16_t *voltages_BAT_USB)
{
	ADC->CTRLA.bit.ENABLE = 0x00;					// off for parametering // ADC zur Parametrierung abschalten
	ADC->REFCTRL.reg = 0x05;						// reference voltage VDDANA //Referenzspannung VDDANA
	ADC->SAMPCTRL.bit.SAMPLEN = 0x5;				// more sampling time // AMPCTRL.SAMPLEN Sampling Zeit erhöhen 
	ADC->AVGCTRL.bit.SAMPLENUM =0x7 ;				// more samples, add individual values //Sample-anzahl erhöhen, addiert die einzelnen Werte 
	ADC->AVGCTRL.bit.ADJRES =0x4 ;					// Division coeff of the result (divided by 8) //Durchschnittsbildung durch oversampling
	ADC->CTRLC.bit.RESSEL =0x1  ;
	ADC->INPUTCTRL.reg = 0x0000;					// chose analog channel AI0 //Kanal AI0 wählen 
	ADC->CTRLB.reg= 0x04;							//set Timer prescaler /128 - 6
	
	ADC->SWTRIG.reg = 0x2;							// set conversion start bit //Wandlung Startbit setzen  ADC->SWTRIG.bit.START = 1;
	ADC->CTRLA.bit.ENABLE = 0x01;					// ADC on again USB // ADC wieder einschalten USB
	while(!ADC->INTFLAG.bit.RESRDY);
	voltages_BAT_USB[1]=(uint16_t)(ADC->RESULT.reg*4.56)+20;//start conversion

	ADC->CTRLA.bit.ENABLE = 0x00;
	ADC->INPUTCTRL.reg = 0x0001;                   //USB AIN0 //BAT AIN1
	ADC->CTRLA.bit.ENABLE = 0x01;
	ADC->SWTRIG.bit.START = 1;
	while(!ADC->INTFLAG.bit.RESRDY);
	voltages_BAT_USB[1]=(uint16_t)(ADC->RESULT.reg*4.7)+1;	//start conversion
		
	ADC->CTRLA.bit.ENABLE = 0x00;
	ADC->INPUTCTRL.reg = 0x0000;					//BAT  //USB AI0
	// adc_sync_enable(&ADC_);
	ADC->CTRLA.bit.ENABLE = 0x01;
	ADC->SWTRIG.bit.START = 1;
	while(!ADC->INTFLAG.bit.RESRDY);				// conversion completed //INTFLAG.RESRDY Wandlung fertig
	voltages_BAT_USB[0]=(uint16_t)(ADC->RESULT.reg*3.8)-70;//start onversion HW.02
	//voltages_BAT_USB[0]=(uint16_t)(ADC->RESULT.reg*4.73);//try new conv
}

/*****************************************************************************
*
*	Function name :  read_chip_id
*
*	Returns :		nothing
*
*	Parameters :	A pointer to an array to be filled, 4*32bit large
*
*	Purpose :		Get a unique Chip-ID for registration purposes
*					
*
******************************************************************************/

void read_chip_id(uint32_t *IDs)   // 4*32bit large
{
//	Adresses in the controller
//	Word 0: 0x0080A00C
//	Word 1: 0x0080A040
//	Word 2: 0x0080A044
//	Word 3: 0x0080A048
	uint32_t *ptr1 = (volatile uint32_t *)0x0080A00C;
	IDs[0]=  *ptr1;
	uint32_t *ptr = (volatile uint32_t *)0x0080A040;
	IDs[1]=  *ptr;
	ptr++;
	IDs[2]=  *ptr;
	ptr++;
	IDs[3]=  *ptr;
}


void send_gsm_modem(const char *text)
{
	if(strlen(text) > 0)
	{
		io_write(usart_gsm_io,(const uint8_t *) text, strlen(text));
	}
}


void send_debug(const char *text)
{
	io_write(usart_debug_io, (const uint8_t *) text, strlen(text));
}



void send_gps(const char *text)
{

	io_write(usart_gps_io, (const uint8_t *) text, strlen(text));
}

int32_t read_gps(uint8_t *rxBuf, uint16_t length)
{
	io_read(usart_gps_io, rxBuf, length);
}

void testcall(void)
{
	gpio_set_pin_level(GSM_DTR_wake, false);  //false stays awake
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
	delay_ms(5000);
	gpio_set_pin_level(GSM_DTR_wake, true); 
	send_debug("Send modem to sleep \r\n");
	delay_ms(20);
}

void gsm_power_up(void)
{
	gpio_set_pin_level(GSM_power_enable, true);
	
}

void gsm_power_down(void)
{
	send_gsm_modem("AT+CPOWD=1\r\n\0");
	delay_ms(1000);
	gpio_set_pin_level(GSM_power_enable, false);
	gpio_set_pin_level(GSM_DTR_wake, false);
	gpio_set_pin_level(GSM_power_key, false);
}




void gsm_startup(void)
{
	send_gsm_modem("AT\r\n\0");
	delay_ms(20); 
	send_gps("test\r\n\0");
	delay_ms(20);                              // Key is high in default state
											   // pull low for 1 second to switch on
	gpio_set_pin_level(GSM_power_key, true);
	gpio_set_pin_level(GSM_power_enable, true);  
	gpio_set_pin_level(GSM_DTR_wake, false);  //false stays awake
	delay_ms(1000);
	gpio_set_pin_level(GSM_power_key, false);
	delay_ms(1000);
    gpio_set_pin_level(GSM_power_key, true);
	delay_ms(3000);
	send_gsm_modem("AT\r\n\0");
	delay_ms(20);
	send_debug("GSM_modem ready \r\n\0");	
	delay_ms(10);
	send_gsm_modem("AT\r\n\0");
	delay_ms(20);	
	delay_ms(20);
	send_gsm_modem("AT+CSCLK=1\r\n\0");
	delay_ms(30);
	send_gsm_modem("AT+IPR=115200\r\n\0");//set 115200Baud
	delay_ms(600);
	send_gsm_modem("AT+CFUN=1\r\n\0");
}


void gsm_sleep(void)
{
	gpio_set_pin_level(GSM_DTR_wake, false);
	delay_ms(300);
	send_gsm_modem("AT+CFUN=1\r\n\0");
	delay_ms(300);
//	send_gsm_modem("AT+CFUN=4\r\n\0");
	delay_ms(30);
	send_gsm_modem("AT+CSCLK=1\r\n\0");   //   2->Auto - 1->DTR
	delay_ms(300);
	//send_gsm_modem("AT+CSCLK=2\r\n\0");   //   2->Auto - 1->DTR
	delay_ms(30);
	send_gsm_modem("AT\r\n\0"); 
    delay_ms(300);
	gpio_set_pin_level(GSM_DTR_wake, true);  //high can enter sleep, low can leave sleep
}


void gsm_wakeup(void)
{
	send_gsm_modem("AT+CFUN=1\r\n\0");
	delay_ms(3);
	send_debug("modem woke up\r\n\0");
	
	delay_ms(100);
	send_gsm_modem("AT\r\n\0");
	delay_ms(100);
	gpio_set_pin_level(GSM_DTR_wake, false);
}

void gsm_get_imei(void)
{
	send_gsm_modem("AT+CGSN\r\n\0");
	delay_ms(3);
	}

void gsm_status(void)
{
	send_gsm_modem("AT\r\n");
	delay_ms(100);
	send_gsm_modem("AT+CPIN?\r\n");
	delay_ms(30);
	send_gsm_modem("AT+CREG?\r\n");
	delay_ms(30);
	send_debug("IMEI:");
	send_gsm_modem("AT+CGSN\r\n");//IMEI
	
	delay_ms(30);
	send_gsm_modem("AT+CSQ\r\n");//Signalstaerke
	delay_ms(30);
	send_gsm_modem("AT+CFUN?\r\n");//Function Mode
	delay_ms(30);
	send_gsm_modem("AT+CSCLK?\r\n");//sleepmode
	delay_ms(30);
	send_gsm_modem("AT+CBC\r\n");//sleepmode
	delay_ms(300);
	//ADC_read_BAT_USB(voltages);
	//sprintf(genericBuf, "Voltage ADC: %i\r\n", voltages[0]);
	send_debug(genericBuf);
	delay_ms(30);
	send_gsm_modem("AT+CMTE?\r\n");//temperatur
	
	delay_ms(300);
	send_gsm_modem("AT\r\n");//temperatur
	delay_ms(300);
	
}

void gsm_read_imei(void)
{
	
}

void systick_check(void)
{
// 	uint32_t *ptr2 = (volatile uint32_t *)0x0E00E010;
// 	val5 = *ptr2;
// 	delay_ms(1);
// 	val6 = *ptr2;
	
	// PM->APBBMASK.reg = 8;

	/* PA19: input with pulldown */
	PORT->Group[0].PINCFG[13].bit.INEN = 1;
	PORT->Group[0].PINCFG[13].bit.PULLEN = 1;
	PORT->Group[0].OUTCLR.bit.OUTCLR = (1 << 13);

	//   /* PA16: input with pullup */
	//   PORT->Group[0].PINCFG[16].bit.INEN = 1;
	//   PORT->Group[0].PINCFG[16].bit.PULLEN = 1;
	//   PORT->Group[0].OUTSET.bit.OUTSET = (1 << 16);

	/* LED0   PB30, active low */
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



static void _rtc_interrupt_handler(void *p)
{
	struct calendar_dev *dev = (struct calendar_dev *)p;
	/* Read and mask interrupt flag register */
	uint16_t interrupt_status = hri_rtcmode0_read_INTFLAG_reg(dev->hw);

	if (interrupt_status & RTC_MODE0_INTFLAG_CMP0) {
		dev->callback(dev);

		/* Clear interrupt flag */
		hri_rtcmode0_clear_interrupt_CMP0_bit(dev->hw);
	}
}





void reg16_write(uint32_t reg_adress, uint16_t reg_value)
{
	UNUSED(reg_adress);
	UNUSED(reg_value);
	/*tmp = ((Rtc *)hw)->MODE0.CTRLA.reg;*/
	//*reg_adress=reg_value;
}
