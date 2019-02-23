/*
 * helper_sgs.h
 *
 * Created: 27.04.2016 07:36:00
 *  Author: Marc Adolf
 */ 


#ifndef HELPER_SGS_H_
#define HELPER_SGS_H_

#include "usb_main.h"
#include "ble_config.h"
#include "sleep_manager_main.h"
#include "stdio_start.h"
#include "hal_usart_async.h"
#include "stdbool.h"
#include "hal_delay.h"
#include "serial_sgs.h"
#include "menu_sgs.h"

volatile int testcall_flag;
volatile int debug_callback_receive_flag;
volatile uint8_t unreadSmsCounter;
volatile char buf_mad[512]; 
volatile uint32_t g_ms_ticks;

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
void check_status_gsm_modem(void);
void wakeup_gsm_modem(void);
void sleep_gsm_modem(void);
void stop_gsm_modem(void);
void start_gsm_modem(void);
void powerDown_gsm_modem(void);
void testcall(void);
void send_gps(const char *text);
void send_debug(const char *text);
void send_gsm_modem(const char *text);
void init_sgs(void);
void loop_sgs(void);

#endif /* HELPER_SGS_H_ */