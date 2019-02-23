/*
* port_sgs.h
*
* Created: 17.06.2016 10:30:04
*  Author: Marc Adolf
*/


#ifndef PORT_SGS_H_
#define PORT_SGS_H_



#ifdef __cplusplus
extern "C" {
	#endif


	#include <hal_gpio.h>
	#include <hal_usart_async.h>
	#include <hal_usart_sync.h>
	#include <hal_spi_m_sync.h>
	#include <hal_timer.h>
	#include <hpl_tc2_v200_base.h>
	#include <hal_pwm.h>
	#include <hpl_tcc_v101_base.h>
	#include <hal_calendar.h>
	#include <hal_evsys.h>
	#include <hal_ext_irq.h>
	#include <hal_adc_sync.h>
	#include <hal_wdt.h>
	#include "hal_usb_device.h"
	#include <serial_sgs.h>
	#include <hal_delay.h>
	#include <hal_crc_sync.h>
	#include <hal_flash.h>



	#define BAT_voltage			GPIO(GPIO_PORTA, 2)
	#define USB_voltage			GPIO(GPIO_PORTA, 3)
	#define PA04				GPIO(GPIO_PORTA, 4)
	#define PA05				GPIO(GPIO_PORTA, 5)
	#define PA06				GPIO(GPIO_PORTA, 6)
	#define PA07				GPIO(GPIO_PORTA, 7)
	#define PA08				GPIO(GPIO_PORTA, 8)
	#define PA09				GPIO(GPIO_PORTA, 9)
	#define PA10				GPIO(GPIO_PORTA, 10)
	#define PA11				GPIO(GPIO_PORTA, 11)
	#define IMU_int1			GPIO(GPIO_PORTA, 10)
	#define IMU_int2			GPIO(GPIO_PORTA, 11)
	#define PA12				GPIO(GPIO_PORTA, 12)
	#define GSM_ring			GPIO(GPIO_PORTA, 12)
	#define PA13				GPIO(GPIO_PORTA, 13)
	#define BLE_APP_SW			GPIO(GPIO_PORTA, 13)
	#define PA14				GPIO(GPIO_PORTA, 14)
	#define PA15				GPIO(GPIO_PORTA, 15)
	#define PA16				GPIO(GPIO_PORTA, 16)
	#define LED_2_ye			GPIO(GPIO_PORTA, 16)
	#define LED_0_gn			GPIO(GPIO_PORTA, 14)
	#define LED_1_rd			GPIO(GPIO_PORTA, 15)
	#define LED_2_ye			GPIO(GPIO_PORTA, 16)
	#define PA17				GPIO(GPIO_PORTA, 17)
	#define PA18				GPIO(GPIO_PORTA, 18)
	#define PA19				GPIO(GPIO_PORTA, 19)
	#define PA21				GPIO(GPIO_PORTA, 21)
	#define PA22				GPIO(GPIO_PORTA, 22)
	#define PA24				GPIO(GPIO_PORTA, 24)
	#define PA25				GPIO(GPIO_PORTA, 25)
	#define PB00				GPIO(GPIO_PORTB, 0)
	#define PB01				GPIO(GPIO_PORTB, 1)
	#define PB03				GPIO(GPIO_PORTB, 3)
	#define CHARGE_STATE		GPIO(GPIO_PORTB, 4)
	#define PB05				GPIO(GPIO_PORTB, 5)
	#define PB06				GPIO(GPIO_PORTB, 6)
	#define PB07				GPIO(GPIO_PORTB, 7)
	#define CE_PIN				GPIO(GPIO_PORTB, 6)
	#define BTLC1000_CHIP_ENABLE_PIN  GPIO(GPIO_PORTB, 6)
	#define WAKEUP_PIN			GPIO(GPIO_PORTB, 7)
	#define BTLC1000_WAKEUP_PIN GPIO(GPIO_PORTB, 7)
	#define PB08				GPIO(GPIO_PORTB, 8)
	#define PB09				GPIO(GPIO_PORTB, 9)
	#define PB10				GPIO(GPIO_PORTB, 10)
	#define PB11				GPIO(GPIO_PORTB, 11)
	#define PB12				GPIO(GPIO_PORTB, 12)
	#define PB13				GPIO(GPIO_PORTB, 13)
	#define GSM_power_enable	GPIO(GPIO_PORTB, 10)
	#define GSM_power_key		GPIO(GPIO_PORTB, 11)
	#define GSM_DTR_wake		GPIO(GPIO_PORTB, 12)
	#define GPS_power_enable	GPIO(GPIO_PORTB, 13)
	#define DATAFLASH_CS		GPIO(GPIO_PORTB, 30)
	#define IMU_CS				GPIO(GPIO_PORTB, 31)
	#define PB30				GPIO(GPIO_PORTB, 30)
	#define PB31				GPIO(GPIO_PORTB, 31)

	volatile uint8_t IMU_interrupt_config;
	volatile uint16_t imu1_count, imu2_count, button_count;

	extern struct usart_async_descriptor BLE_FC_UART;
	extern struct usart_async_descriptor SER1_GPS;
	extern struct usart_async_descriptor SER2_GSM;
	extern struct usart_sync_descriptor SER3_DEBUG;
	extern struct usart_async_descriptor BLE_UART;
	extern struct spi_m_sync_descriptor SER5_SPI_IMU_FLASH;
	extern struct timer_descriptor TIMER_;
	extern struct pwm_descriptor PWM_x;
	extern struct calendar_descriptor CALENDAR_X;
	extern struct adc_sync_descriptor ADC_;
	extern struct wdt_descriptor WDT_;
	extern struct crc_sync_descriptor CRC_;
	extern struct flash_descriptor FLASH_;

	#define CONSOLE_UART SER3_DEBUG

	void BLE_FC_UART_PORT_init(void);
	void BLE_FC_UART_CLOCK_init(void);
	void BLE_FC_UART_init(void);

	void SER1_GPS_PORT_init(void);
	void SER1_GPS_CLOCK_init(void);
	void SER1_GPS_init(void);


	void SER2_GSM_PORT_init(void);
	void SER2_GSM_CLOCK_init(void);
	void SER2_GSM_init(void);


	void SER3_DEBUG_PORT_init(void);
	void SER3_DEBUG_CLOCK_init(void);
	void SER3_DEBUG_init(void);


	void BLE_UART_PORT_init(void);
	void BLE_UART_CLOCK_init(void);
	void BLE_UART_init(void);


	void SER5_SPI_IMU_FLASH_PORT_init(void);
	void SER5_SPI_IMU_FLASH_CLOCK_init(void);
	void SER5_SPI_IMU_FLASH_init(void);


	void PWM__PORT_init(void);
	void PWM__CLOCK_init(void);
	void PWM__init(void);


	void CALENDAR__CLOCK_init(void);
	void CALENDAR__init(void);
	void CALENDAR__example(void);
	void CALENDAR_X_example(void);

	void ADC__PORT_init(void);
	void ADC__CLOCK_init(void);
	void ADC__init(void);
	void ADC__example(void);

	void WDT__CLOCK_init(void);
	void WDT__init(void);
	void WDT__example(void);

	void USB_DEVICE_INSTANCE_CLOCK_init(void);
	void USB_DEVICE_INSTANCE_init(void);

	void delay_driver_init(void);
	void delay_example(void);

	void FLASH__init(void);
	void FLASH__example(void);
	void FLASH__CLOCK_init(void);

	#define CONF_DMAC_MAX_USED_DESC ( /*SERCOM0*/ 0 + /*SERCOM1*/ 0 + \
	/*SERCOM2*/ 0 + /*SERCOM3*/ 0 + /*SERCOM4*/ 0 + /*SERCOM5*/ 0 +	\
	/*TC1*/ 0 +	\
	/*TCC0*/ 0 + /*RTC*/ 0 + /*EVSYS*/ 0 + /*EIC*/ 0 + /*ADC*/ 0 + \
	/*WDT*/ 0 +	\
	/*GCLK*/ 0 + /*MCLK*/ 0 + /*OSCCTRL*/ 0 + /*OSC32KCTRL*/ 0 + \
	/*USB*/ 0 +	\
	/*SysTick*/ 0 + /*DSU*/ 0 + /*NVMCTRL*/ 0 )

	#define CONF_DMAC_MAX_USED_CH ( /*SERCOM0*/ 0 + /*SERCOM1*/ 0 + /*SERCOM2*/ 0 +	\
	/*SERCOM3*/ 0 + /*SERCOM4*/ 0 + /*SERCOM5*/ 0 + /*TC1*/ 0 +	\
	/*TCC0*/ 0 + \
	/*RTC*/ 0 + /*EVSYS*/ 0 + /*EIC*/ 0 + /*ADC*/ 0 + /*WDT*/ 0 + \
	/*GCLK*/ 0 + \
	/*MCLK*/ 0 + /*OSCCTRL*/ 0 + /*OSC32KCTRL*/ 0 + /*USB*/ 0 +	\
	/*SysTick*/ 0 +	\
	/*DSU*/ 0 + /*NVMCTRL*/ 0 )

	/**
	* \brief Perform system initialization, initialize pins and clocks for
	* peripherals
	*/
	void system_init(void);

	#ifdef __cplusplus
}
#endif




#endif /* PORT_SGS_H_ */