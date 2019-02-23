#ifndef BUTTON_MANAGER_H_
#define BUTTON_MANAGER_H_

#include <peripheralManager/LED_manager.h>
#include <seekiosManager/mask_utilities.h>
#include <tools/led_utilities.h>
#include <peripheralManager/USART_manager.h>
#include <seekiosManager/power_state_manager.h>
#include <peripheralManager/NVM_Manager.h>

typedef enum{
	BUTTON_ACTION_NONE,
	BUTTON_ACTION_SHORT_PRESS,
	BUTTON_ACTION_SHORT_PRESS_FAIL,
	BUTTON_ACTION_2_SEC_PRESS,
	BUTTON_ACTION_3_SEC_PRESS,
	BUTTON_ACTION_6_SEC_PRESS,
	BUTTON_ACTION_SOS,
	BUTTON_ACTION_FAIL,
}E_BUTTON_ACTION;

void buttonManager_init(void);
void task_buttonManager(void* params);
void buttonManager_testRequestSeekiosUpdate(void);

#endif /* BUTTON_MANAGER_H_ */