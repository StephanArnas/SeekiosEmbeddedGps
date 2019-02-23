#ifndef DONT_MOVE_NOTIFIER_H_
#define DONT_MOVE_NOTIFIER_H_

#include <seekiosBLE/customServices/seekios_auth.h>
#include <peripheralManager/BLE_manager_adapted.h>

typedef void (*dontmoveble_callback_t)(uint8_t);
typedef void (*dontmoveble_state_callback_t)(bool);
typedef void (*dontmoveble_key_received_callback_t)(uint8_t*);

#define APP_DMB_FAST_ADV	(1600)// 1000 ms
//#define APP_DMB_ADV_TIMEOUT (655) // 10 min
#define APP_DMB_ADV_TIMEOUT (40) // 40 sec

void dmb_register_seekiosauth_handler(dontmoveble_key_received_callback_t seekiosauth_fn);
void dmb_register_state_handler(dontmoveble_state_callback_t dontmoveble_connected_fn);
void dmb_register_adv_report_handler(dontmoveble_callback_t dmb_adv_report_cb);
bool dontmoveble_configure(void* param);
void dontmoveble_deconfigure(void);
void dontmoveble_disconnect(void);
void dontMoveBle_init(void);


#endif /* DONT_MOVE_NOTIFIER_H_ */