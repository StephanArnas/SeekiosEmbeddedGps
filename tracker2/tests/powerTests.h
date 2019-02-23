#ifndef POWERTESTS_H_
#define POWERTESTS_H_

#include <seekiosCore/seekios.h>
//#include <peripheralManager/BLE_manager_adapted.h>
#include <peripheralManager/bma222_adapted.h>
#include <peripheralManager/dataflash_manager.h>
#include <peripheralManager/GPS_manager.h>
#include <peripheralManager/LED_manager.h>
#include <peripheralManager/WDT_manager.h>
#include <seekiosManager/seekios_manager.h>

void powerTests_init(void);
void task_powerTest(void* param);

#endif /* POWERTESTS_H_ */
