/*
 * seekiosManager.h
 *
 * Created: 23/05/2016 16:26:54
 *  Author: Luc
 */ 


#ifndef SEEKIOSMANAGER_H_
#define SEEKIOSMANAGER_H_

#include <FreeRTOS.h>
#include <task.h>
#include <peripheralManager/GPS_manager.h>
#include <messageListener/message_listener.h>
#include <event_groups.h>
#include <peripheralManager/GSMManager.h>
#include <peripheralManager/dataflash_manager.h>
#include <peripheralManager/bma222_adapted.h>
#include <peripheralManager/button_manager.h>
#include <peripheralManager/LED_manager.h>
#include <stdlib.h>
#include <math.h>
#include <seekiosManager/mask_utilities.h>
#include <hal/include/hal_sleep.h>
#include <seekiosManager/task_management_utilities.h>
#include <tests/functionnalities_test.h>
#include <modesManager/modes.h>
#include <peripheralManager/battery_manager.h>
#include <seekiosManager/power_state_manager.h>
#include <seekiosManager/seekios_info_manager.h>
#include <peripheralManager/NVM_Manager.h>
#include <sgs/powersaving_sgs.h>

void task_seekiosManager(void* param);
void preSleepConfig(void);
void seekiosManager_hibernate(void);
bool seekiosManager_disableSystick(void);
void seekiosManager_enableSystick(bool countFlag);

#endif /* SEEKIOSMANAGER_H_ */