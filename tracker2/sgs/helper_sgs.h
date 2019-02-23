/*
 * helper_sgs.h
 *
 * Created: 27.04.2016 07:36:00
 *  Author: Marc Adolf
 */ 


#ifndef HELPER_SGS_H_
#define HELPER_SGS_H_

#include "usb_main.h"
#include "sleep_manager_main.h"
#include "stdio.h"
#include "hal_usart_async.h"
#include "hal_usart_sync.h"
#include "stdbool.h"
#include "hal_delay.h"
#include "serial_sgs.h"
#include "menu_sgs.h"
#include "dataflash_sgs.h"
#include "port_sgs.h"

volatile int testcall_flag;
volatile int debug_callback_receive_flag;
volatile uint8_t unreadSmsCounter , doHardwaretest;
volatile char genericBuf[256];
volatile uint32_t g_ms_ticks;
void read_chip_id(uint32_t *IDs);

// 
// void time_tick_init(void);
// uint32_t time_tick_get(void);
// uint32_t time_tick_calc_delay(uint32_t tick_start, uint32_t tick_end);

void testcall(void);


	/**
	 * \brief xx
	 *
	 * This function xx
	 *
	 * \param[out] xx
	 * \param[in] xx
	 *
	 * \return Initialization status.
	 */
	
	
void test_chipID(void);
void test_led(void);
void test_adc(void);
void test_dataflash(void);
void test_imu(void);
void test_gps(void);
void test_gsm(void);
void test_ble(void);
void test_sleepmode(void);
void hardware_setup (void);	
void gsm_power_up(void);
void gsm_power_down(void);
void gsm_startup(void);
void gsm_sleep(void);
void gsm_wakeup(void);
void gsm_status(void);
void gsm_read_imei(void)	;
void testcall(void);
void send_gps(const char *text);
void send_debug(const char *text);
void send_gsm_modem(const char *text);
void init_sgs(void);
void loop_sgs(void);
void ADC_read_BAT_USB(uint16_t *voltages_BAT_USB);
void reg16_write(uint32_t reg_adress, uint16_t reg_value);
int32_t _calendar_init2(struct calendar_dev *const dev);
void RTC_Start_calenar_mode( void );
int init_pins(void);
void gsm_get_imei(void);

#endif /* HELPER_SGS_H_ */