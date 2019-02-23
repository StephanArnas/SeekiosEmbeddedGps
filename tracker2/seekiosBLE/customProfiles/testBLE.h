/*
 * testBLE.h
 *
 * Created: 20/09/2016 16:45:09
 *  Author: Luc
 */ 


#ifndef TESTBLE_H_
#define TESTBLE_H_

#include <peripheralManager/BLE_manager_adapted.h>
#include <seekiosBLE/customServices/test_char_exchange.h>

typedef void (*testble_callback_t)(uint8_t);
typedef void (*testble_state_callback_t)(bool);
typedef void (*testble_testchar_received_callback_t)(uint8_t*);

#define APP_TB_FAST_ADV	(1600)// 1000 ms
//#define APP_DMB_ADV_TIMEOUT (655) // 10 min
#define APP_TB_ADV_TIMEOUT (40) // 40 sec

void tb_register_char_changed_callback(testble_testchar_received_callback_t testchar_fn);
void tb_register_state_callback(testble_state_callback_t connected_fn);
void tb_register_adv_report_callback(testble_callback_t dmb_adv_report_cb);
bool testble_configure(void* param);
void testble_deconfigure(void);
void testble_disconnect(void);
void testble_init(void);




#endif /* TESTBLE_H_ */