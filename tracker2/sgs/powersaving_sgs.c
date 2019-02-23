/*
 * powersaving_sgs.c
 *
 * Created: 20.06.2016 12:10:54
 *  Author: Marc Adolf
 */ 
#include <sgs/powersaving_sgs.h>

static bool disableSystick(void);
static void enableSystick(bool countFlag);

void shutdownPinPdown(uint8_t pin_)
{
  gpio_set_pin_function(pin_, GPIO_PIN_FUNCTION_OFF);
  gpio_set_pin_direction(pin_, GPIO_DIRECTION_OFF);
  gpio_set_pin_pull_mode(pin_, GPIO_PULL_DOWN);
}


void shutdownPin(uint8_t pin_)
{
  gpio_set_pin_function(pin_, GPIO_PIN_FUNCTION_OFF);
  gpio_set_pin_direction(pin_, GPIO_DIRECTION_OFF);
  gpio_set_pin_pull_mode(pin_, GPIO_PULL_UP);
}

void sleep_normal()
{
	/* Disable the Systick interrupt */
	bool countFlag = disableSystick();

	#if (WDT_ACTIVATED==1)
	/* Disable Watchdog Timer */
	WDTManager_turnOffWDT();
	#endif

	/* ### ENTERING SLEEP MODE ### */
	sleep(4);

	#if (WDT_ACTIVATED==1)
	/* Re-enable Watchdog Timer */
	WDTManager_turnOnWDT();
	#endif
	
	/* Re-enable the Systick interrupt */
	enableSystick(countFlag);
}

/* Lowest power consumption.
It is possible to improve it by managing in a better way the BMA power consumption. */
void sleep_deep(void)
{
  	//send_debug(" Set Controller in sleep-mode, wake up from Ring, Button or IMU \n\r\0");
  	DELAY_MS(8);
  	
  	// switch off Dataflash and bring IMU in lowpower-mode while reacting on movement
  	DF_PowerDown();

//   bma222_set_motion_interrupt(8);
//   bma222_set_bandwith(0x08);   //0x08 = 7,6 HZ
//   bma222_set_sleep_duration(0x0F);  //
//   bma222_reg_overwrite(0x12, 0x40);  

  	//NVIC_DisableIRQ(2);  //RTC
  	NVIC_DisableIRQ(11); // Sercom 3
  	
    usart_async_disable(&SER1_GPS);
  	usart_async_disable(&SER2_GSM);
  	usart_sync_disable(&SER3_DEBUG);
  	usart_async_disable(&BLE_UART);
  	usart_async_disable(&BLE_FC_UART);
  	spi_m_sync_disable(&SER5_SPI_IMU_FLASH);
  	
  	//SPI
  	shutdownPin(PB03);
  	shutdownPin(PB00);
  	shutdownPin(PB01);
//  shutdownPin(PB30);
//  shutdownPin(PB31);
  	// GSM
  	shutdownPinPdown(PA08);//down
  	shutdownPinPdown(PA09);
  	//GPS
  	shutdownPin(PA17);
  	shutdownPin(PA18);
  	//Debug
  	shutdownPin(PA21);
  	shutdownPin(PA22);
  	usart_sync_disable(&SER3_DEBUG);
  	// BLE
  	shutdownPin(PB08);
  	shutdownPin(PB09);
  	//BLE2
  	shutdownPin(PB08);
  	shutdownPin(PB09);
  	
  	shutdownPinPdown(BTLC1000_CHIP_ENABLE_PIN);
  	shutdownPinPdown(BTLC1000_WAKEUP_PIN);
  	
  	// LEDs
  	shutdownPinPdown(PA14);
  	shutdownPinPdown(PA15);
  	shutdownPinPdown(PA16);
  	//ADC
  	shutdownPinPdown(BAT_voltage); //down
  	shutdownPinPdown(USB_voltage); //down
  	shutdownPin(CHARGE_STATE);
  	shutdownPin(PB05);
  	//USB
  	shutdownPin(PA24);
  	shutdownPin(PA25);
  	
 	
  	SUPC->BOD33.bit.ENABLE = 0;
  	SUPC->BOD12.bit.ENABLE = 0;
  	
  	GCLK->GENCTRL[0].bit.SRC = 0x6;
  	ADC->CTRLA.bit.ENABLE = 0;
  	MCLK->LPDIV.reg = 0x80  ;
  	MCLK->CPUDIV.reg = 0x80;

  	SUPC->VREF.bit.ONDEMAND = 1;
  	SUPC->VREF.bit.RUNSTDBY = 1;  	SUPC->VREG.bit.LPEFF    = 1;
  	PM->STDBYCFG.bit.DPGPD0 = 1;
  	PM->STDBYCFG.bit.DPGPD1 = 1;
  	PM->CTRLA.bit.IORET = 1;  	PM->STDBYCFG.bit.VREGSMOD = 2;  	//nLDO/Buck : Select Buck
    SUPC->VREG.bit.SEL = 1;
  	while(SUPC->STATUS.bit.VREGRDY==0);
	bool countFlag = disableSystick();
	
   	sleep(4);

	enableSystick(countFlag);

  	GCLK->GENCTRL[0].bit.SRC = 0x7;
  	
  	MCLK->CPUDIV.reg = 0x01;
  	
    
   	system_init();
	serial_io_init();
    NVIC_EnableIRQ(11);
  	//NVIC_EnableIRQ(2);
	ADC->CTRLA.bit.ENABLE = 1;
    doHardwaretest = 0;

	vTaskDelay(100);
  }
  
static bool disableSystick()
{
	bool countFlag = false;
	if(seekiosManagerStarted)
	{
		/* Disable theY Systick interrupt */
		countFlag = ((SysTick_Type *)SysTick)->CTRL & SysTick_CTRL_COUNTFLAG_Msk;
		((SysTick_Type *)SysTick)->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	}
	return countFlag;
}

static void enableSystick(bool countFlag)
{
	if(seekiosManagerStarted)
	{
		/* Re-enable the Systick interrupt */
		((SysTick_Type *)SysTick)->CTRL |= SysTick_CTRL_ENABLE_Msk;
		if(countFlag) ((SysTick_Type *)SysTick)->CTRL |= SysTick_CTRL_COUNTFLAG_Msk;
	}
}