/*
* port_sgs.c
*
* Created: 17.06.2016 10:29:49
*  Author: Marc Adolf
*/



#include "port_sgs.h"
#include "main.h"
#include <seekiosManager/seekios_manager.h>
#include <utils.h>
#include <hal_init.h>
#include <hpl_irq.h>
#include <hri_gclk2_v111.h>
#include <hri_mclk_v101.h>
#include <peripheral_gclk_config.h>

#if CONF_DMAC_MAX_USED_DESC > 0
#endif

/*! The buffer size for SER0*/
#define BLE_FC_UART_BUFFER_SIZE 16
/*! The buffer size for SER1 */
#define SER1_GPS_BUFFER_SIZE 64
/*! The buffer size for SER2 */
#define SER2_GSM_BUFFER_SIZE 64
/*! The buffer size for SER3 */
#define SER3_DEBUG_BUFFER_SIZE 64
/*! The buffer size for SER4 */
#define BLE_UART_BUFFER_SIZE 16

#define CONSOLE_UART SER3_DEBUG


extern struct _irq_descriptor *_irq_table[PERIPH_COUNT_IRQn];
extern void Default_Handler(void);

struct usart_async_descriptor BLE_FC_UART;		//SER0
struct usart_async_descriptor BLE_UART;			//SER4
struct usart_async_descriptor SER0_BLE;
struct usart_async_descriptor SER1_GPS;
struct usart_async_descriptor SER2_GSM;
struct usart_sync_descriptor SER3_DEBUG;
struct usart_async_descriptor SER4_BLE;
struct spi_m_sync_descriptor SER5_SPI_IMU_FLASH;
struct timer_descriptor TIMER_;
struct crc_sync_descriptor CRC_;


static uint8_t BLE_FC_UART_buffer[BLE_FC_UART_BUFFER_SIZE];
static uint8_t SER1_GPS_buffer[SER1_GPS_BUFFER_SIZE];
static uint8_t SER2_GSM_buffer[SER2_GSM_BUFFER_SIZE];
//static uint8_t SER3_DEBUG_buffer[SER3_DEBUG_BUFFER_SIZE];
static uint8_t BLE_UART_buffer[BLE_UART_BUFFER_SIZE];

static struct timer_task TIMER__task1, TIMER__task2;
struct pwm_descriptor PWM_x;
struct calendar_descriptor CALENDAR_X;
static struct calendar_alarm alarm;
struct adc_sync_descriptor ADC_;
struct wdt_descriptor WDT_;
struct flash_descriptor FLASH_;

/**
* \brief USART Clock initialization function
*
* Enables register interface and peripheral clock
*/
void BLE_FC_UART_CLOCK_init()
{
	hri_gclk_write_PCHCTRL_reg(GCLK,SERCOM0_GCLK_ID_CORE,	CONF_GCLK_SERCOM0_CORE_SRC |	( 1 << GCLK_PCHCTRL_CHEN_Pos ));
	hri_gclk_write_PCHCTRL_reg(GCLK,SERCOM0_GCLK_ID_SLOW,	CONF_GCLK_SERCOM0_SLOW_SRC |	( 1 << GCLK_PCHCTRL_CHEN_Pos ));
	hri_mclk_set_APBCMASK_SERCOM0_bit(MCLK);
}

/**
* \brief USART pinmux initialization function
*
* Set each required pin to USART functionality
*/
void BLE_FC_UART_PORT_init()
{
	gpio_set_pin_function(PA04, PINMUX_PA04D_SERCOM0_PAD0);
	gpio_set_pin_function(PA05, PINMUX_PA05D_SERCOM0_PAD1);
	gpio_set_pin_function(PA06, PINMUX_PA06D_SERCOM0_PAD2);
	gpio_set_pin_function(PA07, PINMUX_PA07D_SERCOM0_PAD3);
}

/**
* \brief USART initialization function
*
* Enables USART peripheral, clocks and initializes USART driver
*/
void BLE_FC_UART_init(void)
{
	BLE_FC_UART_CLOCK_init();
	usart_async_init(&BLE_FC_UART, SERCOM0, BLE_FC_UART_buffer, BLE_FC_UART_BUFFER_SIZE);
	BLE_FC_UART_PORT_init();
}

/**
* \brief USART Clock initialization function
*
* Enables register interface and peripheral clock
*/
void SER1_GPS_CLOCK_init()
{
	hri_gclk_write_PCHCTRL_reg(GCLK,SERCOM1_GCLK_ID_CORE,CONF_GCLK_SERCOM1_CORE_SRC |( 1 << GCLK_PCHCTRL_CHEN_Pos ));
	hri_gclk_write_PCHCTRL_reg(GCLK,SERCOM1_GCLK_ID_SLOW,CONF_GCLK_SERCOM1_SLOW_SRC |( 1 << GCLK_PCHCTRL_CHEN_Pos ));
	hri_mclk_set_APBCMASK_SERCOM1_bit(MCLK);
}

/**
* \brief USART pinmux initialization function
*
* Set each required pin to USART functionality
*/
void SER1_GPS_PORT_init()
{
	gpio_set_pin_function(PA17, PINMUX_PA17C_SERCOM1_PAD1);
	gpio_set_pin_function(PA18, PINMUX_PA18C_SERCOM1_PAD2);
}

/**
* \brief USART initialization function
*
* Enables USART peripheral, clocks and initializes USART driver
*/
void SER1_GPS_init(void)
{
	SER1_GPS_CLOCK_init();
	usart_async_init(&SER1_GPS, SERCOM1, SER1_GPS_buffer, SER1_GPS_BUFFER_SIZE);
	SER1_GPS_PORT_init();
}

/**
* \brief USART Clock initialization function
*
* Enables register interface and peripheral clock
*/
void SER2_GSM_CLOCK_init()
{
	hri_gclk_write_PCHCTRL_reg(GCLK,SERCOM2_GCLK_ID_CORE,CONF_GCLK_SERCOM2_CORE_SRC |( 1 << GCLK_PCHCTRL_CHEN_Pos ));
	hri_gclk_write_PCHCTRL_reg(GCLK,SERCOM2_GCLK_ID_SLOW,CONF_GCLK_SERCOM2_SLOW_SRC |( 1 << GCLK_PCHCTRL_CHEN_Pos ));
	hri_mclk_set_APBCMASK_SERCOM2_bit(MCLK);
}

/**
* \brief USART pinmux initialization function
*
* Set each required pin to USART functionality
*/
void SER2_GSM_PORT_init()
{
	gpio_set_pin_function(PA08, PINMUX_PA08D_SERCOM2_PAD0);
	gpio_set_pin_function(PA09, PINMUX_PA09D_SERCOM2_PAD1);
}

/**
* \brief USART initialization function
*
* Enables USART peripheral, clocks and initializes USART driver
*/
void SER2_GSM_init(void)
{
	SER2_GSM_CLOCK_init();
	usart_async_init(&SER2_GSM, SERCOM2, SER2_GSM_buffer, SER2_GSM_BUFFER_SIZE);
	SER2_GSM_PORT_init();
}

/**
* \brief USART Clock initialization function
*
* Enables register interface and peripheral clock
*/
void SER3_DEBUG_CLOCK_init()
{
	hri_gclk_write_PCHCTRL_reg(GCLK,SERCOM3_GCLK_ID_CORE,CONF_GCLK_SERCOM3_CORE_SRC |( 1 << GCLK_PCHCTRL_CHEN_Pos ));
	hri_gclk_write_PCHCTRL_reg(GCLK,SERCOM3_GCLK_ID_SLOW,CONF_GCLK_SERCOM3_SLOW_SRC |( 1 << GCLK_PCHCTRL_CHEN_Pos ));
	hri_mclk_set_APBCMASK_SERCOM3_bit(MCLK);
}

/**
* \brief USART pinmux initialization function
*
* Set each required pin to USART functionality
*/
void SER3_DEBUG_PORT_init()
{
	gpio_set_pin_function(PA22, PINMUX_PA22C_SERCOM3_PAD0);
	gpio_set_pin_function(PA21, PINMUX_PA21D_SERCOM3_PAD3);
}

/**
* \brief USART initialization function
*
* Enables USART peripheral, clocks and initializes USART driver
*/
void SER3_DEBUG_init(void)
{
	SER3_DEBUG_CLOCK_init();
	usart_sync_init(&SER3_DEBUG,SERCOM3);
	SER3_DEBUG_PORT_init();
}

/**
* \brief USART SERCOM4  Clock initialization function
*
* Enables register interface and peripheral clock
*/
void BLE_UART_CLOCK_init()
{
	hri_gclk_write_PCHCTRL_reg(GCLK,	SERCOM4_GCLK_ID_CORE,	CONF_GCLK_SERCOM4_CORE_SRC |		( 1 << GCLK_PCHCTRL_CHEN_Pos ));
	hri_gclk_write_PCHCTRL_reg(GCLK,	SERCOM4_GCLK_ID_SLOW,	CONF_GCLK_SERCOM4_SLOW_SRC |		( 1 << GCLK_PCHCTRL_CHEN_Pos ));
	hri_mclk_set_APBCMASK_SERCOM4_bit(MCLK);
}


/**
* \brief USART SERCOM4 pinmux initialization function
*
* Set each required pin to USART functionality
*/
void BLE_UART_PORT_init()
{
	gpio_set_pin_function(PB08, PINMUX_PB08D_SERCOM4_PAD0);
	gpio_set_pin_function(PB09, PINMUX_PB09D_SERCOM4_PAD1);
}
/**
* \brief USART SERCOM4 initialization function
*
* Enables USART peripheral, clocks and initializes USART driver
*/
void BLE_UART_init(void)
{
	BLE_UART_CLOCK_init();
	usart_async_init(&BLE_UART, SERCOM4, BLE_UART_buffer, BLE_UART_BUFFER_SIZE);
	BLE_UART_PORT_init();
}

void SER5_SPI_IMU_FLASH_PORT_init(void)
{
	gpio_set_pin_direction(PB03, GPIO_DIRECTION_OUT);           // Set pin direction to output
	gpio_set_pin_level(PB03,false);
	// <y> Initial level
	// <id> pad_initial_level
	// <false"> Low
	// <true"> High
	
	gpio_set_pin_function(PB03, PINMUX_PB03D_SERCOM5_PAD1);
	gpio_set_pin_direction(PB00, GPIO_DIRECTION_IN);				// Set pin direction to input
	gpio_set_pin_pull_mode(PB00,	GPIO_PULL_OFF);
	// <y> Pull configuration
	// <id> pad_pull_config
	// <GPIO_PULL_OFF"> Off
	// <GPIO_PULL_UP"> Pull-up
	// <GPIO_PULL_DOWN"> Pull-down
	gpio_set_pin_function(PB00, PINMUX_PB00D_SERCOM5_PAD2);
	gpio_set_pin_direction(PB01, GPIO_DIRECTION_OUT);				// Set pin direction to output
	gpio_set_pin_level(PB01,false);
	gpio_set_pin_function(PB01, PINMUX_PB01D_SERCOM5_PAD3);
}
void SER5_SPI_IMU_FLASH_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK,SERCOM5_GCLK_ID_CORE,CONF_GCLK_SERCOM5_CORE_SRC |( 1 << GCLK_PCHCTRL_CHEN_Pos ));
	hri_gclk_write_PCHCTRL_reg(GCLK,SERCOM5_GCLK_ID_SLOW,CONF_GCLK_SERCOM5_SLOW_SRC |( 1 << GCLK_PCHCTRL_CHEN_Pos ));
	hri_mclk_set_APBDMASK_SERCOM5_bit(MCLK);
}

void SER5_SPI_IMU_FLASH_init(void)
{
	SER5_SPI_IMU_FLASH_CLOCK_init();
	spi_m_sync_init(&SER5_SPI_IMU_FLASH, SERCOM5);
	SER5_SPI_IMU_FLASH_PORT_init();
}

/**
* \brief Timer initialization function
*
* Enables Timer peripheral, clocks and initializes Timer driver
*/
static void TIMER__init(void)
{
	hri_mclk_set_APBCMASK_TC1_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TC1_GCLK_ID, CONF_GCLK_TC1_SRC |( 1 << GCLK_PCHCTRL_CHEN_Pos ));
	timer_init(&TIMER_, TC1, _tc_get_timer());
}

void PWM__PORT_init(void)
{
	gpio_set_pin_function(LED_0_gn, PINMUX_PA14F_TCC0_WO4);
	gpio_set_pin_function(LED_1_rd, PINMUX_PA15F_TCC0_WO5);
	gpio_set_pin_function(LED_2_ye, PINMUX_PA16F_TCC0_WO6);
}

void PWM__CLOCK_init(void)
{
	hri_mclk_set_APBCMASK_TCC0_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TCC0_GCLK_ID, CONF_GCLK_TCC0_SRC |	( 1 << GCLK_PCHCTRL_CHEN_Pos ));
}

void PWM__init(void)
{
	PWM__CLOCK_init();
	PWM__PORT_init();
	//	pwm_init(&PWM_x, TCC0, _tcc_get_pwm());
}

/**
* Example of using PWM_.
*/
void PWM__example(void)
{
	pwm_set_parameters(&PWM_x, 10000, 5000);
	pwm_enable(&PWM_x);
}

void CALENDAR_X_CLOCK_init(void)
{
	hri_mclk_set_APBAMASK_RTC_bit(MCLK);
}

void CALENDAR_X_init(void)
{
	CALENDAR_X_CLOCK_init();
	calendar_init(&CALENDAR_X, RTC);
	calendar_disable(&CALENDAR_X);
}

/**
* Example of using CALENDAR_.
*/
struct calendar_date_time currentDt;
volatile uint8_t alarm_count;
static void alarm_cb(struct calendar_descriptor *const descr)
{
	gpio_toggle_pin_level(LED_2_ye);
	alarm_count++;
	calendar_get_date_time(&CALENDAR_X, &currentDt);
	UNUSED(descr);
	/* alarm expired */
}

void CALENDAR_X_example(void)
{
	struct calendar_date date;
	struct calendar_time time;

	calendar_enable(&CALENDAR_X);

	date.year = 2016;
	date.month = 06;
	date.day = 29;
	time.hour = 17;
	time.min = 45;
	time.sec = 55;
	calendar_set_date(&CALENDAR_X, &date);
	calendar_set_time(&CALENDAR_X, &time);
	alarm.data.datetime.time.min = 46;
	alarm.data.datetime.time.sec = 5;
	alarm.option = CALENDAR_ALARM_MATCH_MIN;
	alarm.mode = REPEAT;
	calendar_set_alarm(&CALENDAR_X, &alarm, alarm_cb);
}

void EVENT_SYSTEM_init(void)
{
	hri_mclk_set_APBDMASK_EVSYS_bit(MCLK);
	event_system_init();
}

void EXTERNAL_IRQ_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, EIC_GCLK_ID, CONF_GCLK_EIC_SRC |( 1 << GCLK_PCHCTRL_CHEN_Pos ));
	hri_mclk_set_APBAMASK_EIC_bit(MCLK);
	ext_irq_init();
	gpio_set_pin_direction(PA10, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(PA10, GPIO_PULL_OFF);
	gpio_set_pin_function (PA10,  PINMUX_PA10A_EIC_EXTINT10);
	gpio_set_pin_direction(PA11, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(PA11, GPIO_PULL_UP);
	gpio_set_pin_function( PA11, PINMUX_PA11A_EIC_EXTINT11);
	gpio_set_pin_direction(PA12, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(PA12, GPIO_PULL_OFF);
	gpio_set_pin_function (PA12,  PINMUX_PA12A_EIC_EXTINT12);
	gpio_set_pin_direction(PA13, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(PA13, GPIO_PULL_UP);
	gpio_set_pin_function( PA13, PINMUX_PA13A_EIC_EXTINT13);
}

void ADC__PORT_init(void)
{
	gpio_set_pin_direction(BAT_voltage, GPIO_DIRECTION_OFF); // Disable digital pin circuitry
	gpio_set_pin_function(BAT_voltage, PINMUX_PA02B_ADC_AIN0);
	gpio_set_pin_direction(USB_voltage, GPIO_DIRECTION_OFF);
	gpio_set_pin_function(USB_voltage, PINMUX_PA03B_ADC_AIN1);
	gpio_set_pin_direction(CHARGE_STATE, GPIO_DIRECTION_OFF);
	gpio_set_pin_function(CHARGE_STATE, PINMUX_PB04B_ADC_AIN12);
	gpio_set_pin_direction(PB05, GPIO_DIRECTION_OFF);              //reserve, not connected to any circuit
	gpio_set_pin_function(PB05, PINMUX_PB05B_ADC_AIN13);
}

void ADC__CLOCK_init(void)
{
	hri_mclk_set_APBDMASK_ADC_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, ADC_GCLK_ID, CONF_GCLK_ADC_SRC |( 1 << GCLK_PCHCTRL_CHEN_Pos ));
}

void ADC__init(void)
{
	ADC__CLOCK_init();
	ADC__PORT_init();
	//adc_sync_init(&ADC_, ADC);
}


void ADC__example(void)
{
	uint16_t buffer[16];
	//adc_sync_disable(&ADC_);
	

	ADC->CTRLA.bit.ENABLE = 0x00;					//ADC zur Parametrierung abschalten
	ADC->REFCTRL.reg = 0x05;						//Referenzspannung VDDANA
	ADC->SAMPCTRL.bit.SAMPLEN = 0x5;				//AMPCTRL.SAMPLEN Sampling Zeit erhöhen
	ADC->AVGCTRL.bit.SAMPLENUM =0x7 ;				//Sample-anzahl erhöhen, addiert die einzelnen Werte
	ADC->AVGCTRL.bit.ADJRES =0x4 ;					//Durchschnittsbildung durch oversampling
	ADC->CTRLC.bit.RESSEL =0x1  ;
	ADC->INPUTCTRL.reg = 0x0000;					//Kanal AI0 wählen
	ADC->CTRLB.reg= 0x04;							//set Timer prescaler /128 - 6
	
	ADC->SWTRIG.reg = 0x2;							//Wandlung Startbit setzen  ADC->SWTRIG.bit.START = 1;
	ADC->CTRLA.bit.ENABLE = 0x01;					//ADC wieder einschalten USB
	while(!ADC->INTFLAG.bit.RESRDY);
	buffer[12]=(uint16_t)(ADC->RESULT.reg*1.49)+20;          //start conversion
	ADC->CTRLA.bit.ENABLE = 0x00;
	ADC->INPUTCTRL.reg = 0x0000;  //BAT AI0
	ADC->CTRLA.bit.ENABLE = 0x01;
	ADC->SWTRIG.bit.START = 1;
	while(!ADC->INTFLAG.bit.RESRDY);
	buffer[12]=(uint16_t)(ADC->RESULT.reg*1.66)+20;//start conversion
	
	ADC->CTRLA.bit.ENABLE = 0x00;
	ADC->INPUTCTRL.reg = 0x0001;  //USB AI0
	
	ADC->CTRLA.bit.ENABLE = 0x01;
	ADC->SWTRIG.bit.START = 1;
	while(!ADC->INTFLAG.bit.RESRDY);
	//	buffer[13]=(uint16_t)(ADC->RESULT.reg*1.49)+20;//start conversion HW.02
	buffer[13]=(uint16_t)(ADC->RESULT.reg*1.66)+20;//start conversion HW.01
	sprintf(genericBuf,"BAT:%i USB:%i -\n\r ", buffer[12], buffer[13]);//

}

void WDT__CLOCK_init(void)
{
	hri_mclk_set_APBAMASK_WDT_bit(MCLK);
}

void WDT__init(void)
{
	WDT__CLOCK_init();
	wdt_init(&WDT_, WDT);
}

/**
* Example of using WDT_.
*/
void WDT__example(void)
{
	uint32_t clk_rate;
	uint16_t timeout_period;

	clk_rate = 1000;
	timeout_period = 4096;
	wdt_set_timeout_period(&WDT_, clk_rate, timeout_period);
	wdt_enable(&WDT_);
}

void USB_DEVICE_INSTANCE_PORT_init(void)
{
	gpio_set_pin_direction(PA24,GPIO_DIRECTION_OUT);
	gpio_set_pin_level(PA24,false);
	gpio_set_pin_pull_mode(PA24,GPIO_PULL_OFF);
	gpio_set_pin_function(PA24, PINMUX_PA24G_USB_DM);
	gpio_set_pin_direction(PA25,GPIO_DIRECTION_OUT);
	gpio_set_pin_level(PA25, false);
	gpio_set_pin_pull_mode(PA25,GPIO_PULL_OFF);
	gpio_set_pin_function(PA25, PINMUX_PA25G_USB_DP);
}

/* The USB module requires a GCLK_USB of 48 MHz ~ 0.25% clock
* for low speed and full speed operation. */
#if ( CONF_GCLK_USB_FREQUENCY > ( 48000000 + 48000000/400 )) ||	\
( CONF_GCLK_USB_FREQUENCY < ( 48000000 - 48000000/400 ))
#    warning USB clock should be 48MHz ~ 0.25% clock, check your configuration!
#endif

void USB_DEVICE_INSTANCE_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, USB_GCLK_ID, CONF_GCLK_USB_SRC |GCLK_PCHCTRL_CHEN);
	hri_mclk_set_AHBMASK_USB_bit(MCLK);
	hri_mclk_set_APBBMASK_USB_bit(MCLK);
}

void USB_DEVICE_INSTANCE_init(void)
{
	USB_DEVICE_INSTANCE_CLOCK_init();
	usb_d_init();
	USB_DEVICE_INSTANCE_PORT_init();
}

void delay_driver_init(void)
{
	delay_init(SysTick);
}

void delay_example(void)
{
	delay_ms(5000);
}

/**
* \brief CRC initialization function
*
* Enables CRC peripheral, clocks and initializes CRC driver
*/
void CRC__init(void)
{
	hri_mclk_set_APBBMASK_DSU_bit(MCLK);
	crc_sync_init(&CRC_, DSU);
}

void FLASH__CLOCK_init(void)
{
	hri_mclk_set_AHBMASK_NVMCTRL_bit(MCLK);
}

void FLASH__init(void)
{
	FLASH__CLOCK_init();
	flash_init(&FLASH_, NVMCTRL);
}

static uint8_t src_data[128];
static uint8_t chk_data[128];

/**
* Example of using FLASH_ to read and write buffer.
*/
void FLASH__example(void)
{
	uint32_t page_size;
	uint16_t i;
	/* Init source data */
	page_size = flash_get_page_size(&FLASH_);
	for(i = 0; i < page_size; i++)
	{
		src_data[i] = i;
	}

	/* Write data to flash */
	flash_write(&FLASH_, 0x3200, src_data, page_size);

	/* Read data from flash */
	flash_read(&FLASH_, 0x3200, chk_data, page_size);
}

void SERCOM0_Handler(void)
{
	if (_irq_table[ SERCOM0_IRQn + 0 ])
	{
		_irq_table[ SERCOM0_IRQn + 0 ]->handler(_irq_table[ SERCOM0_IRQn + 0 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
void SERCOM1_Handler(void)
{
	if (_irq_table[ SERCOM0_IRQn + 1 ])
	{
		_irq_table[ SERCOM0_IRQn + 1 ]->handler(_irq_table[ SERCOM0_IRQn + 1 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
void SERCOM2_Handler(void)
{
	if (_irq_table[ SERCOM0_IRQn + 2 ]) {_irq_table[ SERCOM0_IRQn + 2 ]->handler(_irq_table[ SERCOM0_IRQn + 2 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
void SERCOM3_Handler(void)
{
	if (_irq_table[ SERCOM0_IRQn + 3 ])
	{
		_irq_table[ SERCOM0_IRQn + 3 ]->handler(_irq_table[ SERCOM0_IRQn + 3 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
void SERCOM4_Handler(void)
{
	if (_irq_table[ SERCOM0_IRQn + 4 ])
	{
		_irq_table[ SERCOM0_IRQn + 4 ]->handler(_irq_table[ SERCOM0_IRQn + 4 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
void SERCOM5_Handler(void)
{
	if (_irq_table[ SERCOM0_IRQn + 5 ])
	
	{
		_irq_table[ SERCOM0_IRQn + 5 ]->handler(_irq_table[ SERCOM0_IRQn + 5 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
void TC1_Handler(void)
{
	if (_irq_table[ TC0_IRQn + 1 ])
	{
		_irq_table[ TC0_IRQn + 1 ]->handler(_irq_table[ TC0_IRQn + 1 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
void TCC0_Handler(void)
{
	if (_irq_table[ TCC0_IRQn + 0 ])
	{
		_irq_table[ TCC0_IRQn + 0 ]->handler(_irq_table[ TCC0_IRQn + 0 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
volatile uint16_t ich_bin_ein_systick;
//  void SysTick_Handler(void)
//  {
// 	 ich_bin_ein_systick++;
// 	 if (ich_bin_ein_systick == 100)
// 	 {
// 		 //	gpio_toggle_pin_level(PA15);
// 		 ich_bin_ein_systick=0;
// 	 }
//
// 	 NVIC_DisableIRQ(SysTick_IRQn);

// 	 NVIC_ClearPendingIRQ(SysTick_IRQn);

//

// 	 //callTimerInterrupt();                 Chose your function here, you want to call each second. No delays allowed. Beware of interrupts.

// 	 NVIC_EnableIRQ(SysTick_IRQn);
// }

//  void RTC_Handler(void)
// {
//	NVIC_DisableIRQ(RTC_IRQn);
//  	RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
//  	NVIC_ClearPendingIRQ(RTC_IRQn);
//  	ich_bin_eine_sekunde++;
//  	//callTimerInterrupt();                 Chose your function here, you want to call each second. No delays allowed. Beware of interrupts.
//  	NVIC_EnableIRQ(RTC_IRQn);
// }


void RTC_Handler(void)
{
	if (_irq_table[ RTC_IRQn + 0 ])
	{
		_irq_table[ RTC_IRQn + 0 ]->handler(_irq_table[ RTC_IRQn + 0 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}

void EVSYS_Handler(void)
{
	if (_irq_table[ EVSYS_IRQn + 0 ])
	{
		_irq_table[ EVSYS_IRQn + 0 ]->handler(_irq_table[ EVSYS_IRQn + 0 ]->parameter);
	}
	else {
		Default_Handler();
	}
}


// void EIC_Handler(void)
// {
// 	NVIC_DisableIRQ(EIC_IRQn);
//
// 	NVIC_ClearPendingIRQ(EIC_IRQn);
// 	//callTimerInterrupt();                 Chose your function here, you want to call each second. No delays allowed. Beware of interrupts.
// 	NVIC_EnableIRQ(EIC_IRQn);
//
void EIC_Handler(void)
{
	if (_irq_table[ EIC_IRQn + 0 ])
	{
		_irq_table[ EIC_IRQn + 0 ]->handler(_irq_table[ EIC_IRQn + 0 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
void ADC_Handler(void)
{
	if (_irq_table[ ADC_IRQn + 0 ])
	{
		_irq_table[ ADC_IRQn + 0 ]->handler(_irq_table[ ADC_IRQn + 0 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
void WDT_Handler(void)
{
	if (_irq_table[ WDT_IRQn + 0 ])
	{
		_irq_table[ WDT_IRQn + 0 ]->handler(_irq_table[ WDT_IRQn + 0 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
void MCLK_Handler(void)
{
	if (_irq_table[  +0 ])
	{
		_irq_table[  +0 ]->handler(_irq_table[  +0 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
void OSCCTRL_Handler(void)
{
	if (_irq_table[  +0 ])
	{
		_irq_table[  +0 ]->handler(_irq_table[  +0 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
void OSC32KCTRL_Handler(void)
{
	if (_irq_table[  +0 ]) {
		_irq_table[  +0 ]->handler(_irq_table[  +0 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
void USB_Handler(void)
{
	if (_irq_table[ USB_IRQn + 0 ])
	{
		_irq_table[ USB_IRQn + 0 ]->handler(_irq_table[ USB_IRQn + 0 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}
void NVMCTRL_Handler(void)
{
	if (_irq_table[  +0 ])
	{
		_irq_table[  +0 ]->handler(_irq_table[  +0 ]->parameter);
	}
	else
	{
		Default_Handler();
	}
}

/**
* Example of using TIMER_.
*/
static void TIMER__task1_cb(const struct timer_task *const timer_task)
{
	UNUSED(timer_task);
}

static void TIMER__task2_cb(const struct timer_task *const timer_task)
{
	UNUSED(timer_task);
}

void TIMER__example(void)
{
	TIMER__task1.interval = 100;
	TIMER__task1.cb = TIMER__task1_cb;
	TIMER__task1.mode = TIMER_TASK_REPEAT;
	TIMER__task2.interval = 200;
	TIMER__task2.cb = TIMER__task2_cb;
	TIMER__task2.mode = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_, &TIMER__task1);
	timer_add_task(&TIMER_, &TIMER__task2);
	timer_start(&TIMER_);
}

static void irq_PA10_IMU1(void)
{
	if(seekiosManagerStarted)
	{
		maskUtilities_setInterruptMaskBitsFromISR(INTERRUPT_BIT_MOTION_DETECTED);
	}

	#if (POWER_TESTS_ACTIVATED)
	gpio_toggle_pin_level(LED_1_rd);
	#endif
}
static void irq_PA11_IMU2(void)
{
	imu2_count++;
}

static void irq_PA12_gsm_ring(void)
{
	if(seekiosManagerStarted && GSMManager_isModulePoweredOn())
	{
		maskUtilities_setInterruptMaskBitsFromISR(INTERRUPT_BIT_RING_INDICATOR);
	}
}


static void irq_PA13_button(void)
{
	button_count++;
	doHardwaretest = 1;
	if(seekiosManagerStarted && (maskUtilities_getRunningMaskFromISR() & RUNNING_BIT_TEST_FUNCTIONALITIES) != 0) 
	// The INTERRUPT_BIT_BUTTON is relevant only in the functionalities tests, because otherwise it's the button taks that tells if the button was pressed
	{
		maskUtilities_setInterruptMaskBitsFromISR(INTERRUPT_BIT_BUTTON);
	}
	
	#if (SIMULATE_SENDING_FAILURES==1)
	fakeSendingFailure = ((button_count/2)%2)==1;
	#endif
}

/**
* Example of using EXTERNAL_IRQ
*/
void EXTERNAL_IRQ_register(void)
{

	ext_irq_register(PIN_PA10, irq_PA10_IMU1);   // that is motion detection 1
	ext_irq_register(PIN_PA11, irq_PA11_IMU2);  // this is motion detection 2
	ext_irq_register(PIN_PA12, irq_PA12_gsm_ring);   // that is the ring indicator PA12
	ext_irq_register(PIN_PA13, irq_PA13_button);  // this is the real button PA13

	
	
}

/* CRC Data in flash */
COMPILER_ALIGNED(4)
static const uint32_t crc_datas[] = {
	0x00000000, 0x11111111, 0x22222222, 0x33333333, 0x44444444,
	0x55555555, 0x66666666, 0x77777777, 0x88888888, 0x99999999
};

/**
* Example of using CRC_ to Calculate CRC32 for a buffer.
*/
void CRC__example(void)
{
	/* The initial value used for the CRC32 calculation usually be 0xFFFFFFFF,
	* but can be, for example, the result of a previous CRC32 calculation if
	* generating a common CRC32 of separate memory blocks.
	*/
	uint32_t crc = 0xFFFFFFFF;
	uint32_t crc2;
	uint32_t ind;

	crc_sync_enable(&CRC_);
	crc_sync_crc32(&CRC_, (uint32_t *) crc_datas, 10, &crc);

	/* The read value must be complemented to match standard CRC32
	* implementations or kept non-inverted if used as starting point for
	* subsequent CRC32 calculations.
	*/
	crc ^= 0xFFFFFFFF;
	/* Calculate the same data with subsequent CRC32 calculations, the result
	* should be same as previous way.
	*/
	crc2 = 0xFFFFFFFF;
	for (ind = 0; ind < 10; ind++) {
		crc_sync_crc32(&CRC_, (uint32_t *)&crc_datas[ind], 1, &crc2);
	}
	crc2 ^= 0xFFFFFFFF;
	/* The calculate result should be same. */
	while (crc != crc2) {;
	}
}

void system_init(void)
{
	SUPC->VREG.bit.SEL = 1;  // switch to buck regulator
	
	// This function is also used inside the deep sleep function. Which is why we should protect some code to be executed 2 times
	static uint8_t initflag = 0;
	if (!initflag)
	{
		init_mcu();
	}
	
	if(!initflag)
	{
		// GPIO on CS Dataflash	output
		gpio_set_pin_direction(DATAFLASH_CS, GPIO_DIRECTION_OUT);
		gpio_set_pin_level(DATAFLASH_CS,true);
		gpio_set_pin_function(DATAFLASH_CS, GPIO_PIN_FUNCTION_OFF);

		// GPIO on CS Motion detector BMA222 output
		gpio_set_pin_direction(IMU_CS, GPIO_DIRECTION_OUT);
		gpio_set_pin_level(IMU_CS,true);
		gpio_set_pin_function(IMU_CS, GPIO_PIN_FUNCTION_OFF);

		// GPIO on PB10 output
		gpio_set_pin_direction(GSM_DTR_wake, GPIO_DIRECTION_OUT);
		gpio_set_pin_level    (GSM_DTR_wake,false);
		gpio_set_pin_function (GSM_DTR_wake, GPIO_PIN_FUNCTION_OFF);

		// GPIO on PB10 output
		gpio_set_pin_direction(GSM_power_enable, GPIO_DIRECTION_OUT);
		gpio_set_pin_level    (GSM_power_enable,false);
		gpio_set_pin_function (GSM_power_enable, GPIO_PIN_FUNCTION_OFF);

		// GPIO on PB11 output
		gpio_set_pin_direction(GSM_power_key, GPIO_DIRECTION_OUT);
		gpio_set_pin_level    (GSM_power_key,false);
		gpio_set_pin_function (GSM_power_key, GPIO_PIN_FUNCTION_OFF);

		// GPIO on PB13 output
		gpio_set_pin_direction(GPS_power_enable, GPIO_DIRECTION_OUT);
		gpio_set_pin_level    (GPS_power_enable,false);
		gpio_set_pin_function (GPS_power_enable, GPIO_PIN_FUNCTION_OFF);

		// GPIO on PB06 output
		gpio_set_pin_direction(CE_PIN, GPIO_DIRECTION_OUT);
		gpio_set_pin_level    (CE_PIN,false);
		gpio_set_pin_function (CE_PIN, GPIO_PIN_FUNCTION_OFF);
		// GPIO on PB07 output
		gpio_set_pin_direction(WAKEUP_PIN, GPIO_DIRECTION_OUT);
		gpio_set_pin_level    (WAKEUP_PIN,false);
		gpio_set_pin_function (WAKEUP_PIN, GPIO_PIN_FUNCTION_OFF);
	
		// GPIO on PA14 output
		gpio_set_pin_direction(LED_0_gn, GPIO_DIRECTION_OUT);
		gpio_set_pin_level    (LED_0_gn,false);
		gpio_set_pin_function (LED_0_gn, GPIO_PIN_FUNCTION_OFF);

		// GPIO on PA15 output
		gpio_set_pin_direction(LED_1_rd, GPIO_DIRECTION_OUT);
		gpio_set_pin_level    (LED_1_rd,false);
		gpio_set_pin_function (LED_1_rd, GPIO_PIN_FUNCTION_OFF);
		// GPIO on PA16 output
		gpio_set_pin_direction(LED_2_ye, GPIO_DIRECTION_OUT);
		gpio_set_pin_level    (LED_2_ye,false);
		gpio_set_pin_function (LED_2_ye, GPIO_PIN_FUNCTION_OFF);
	}
	
// 	BLE_FC_UART_init();
// 	BLE_UART_init();
	SER1_GPS_init();
	SER2_GSM_init();
	SER3_DEBUG_init();
	SER5_SPI_IMU_FLASH_init();
	ADC__init();
	USB_DEVICE_INSTANCE_init();
	if (!initflag)
	{
		delay_driver_init();
		EXTERNAL_IRQ_init();
		EXTERNAL_IRQ_register();
		CALENDAR_X_init();
	}
	
	initflag = 1;
}

