#ifndef WDT_MANAGER_H_
#define WDT_MANAGER_H_

#include <stdint.h>
#include <saml21j18b.h>
#include <FreeRTOS.h>
#include <timers.h>
#include <port_sgs.h>
#include <hri_wdt2_v100.h>
#include <hal_wdt.h>

#define SOFTWARE_WATCHDOG_PERIOD 2000

void WDTManager_init(void);
void WTDManager_reset(void);
void WDTManager_turnOffWDT(void);
void WDTManager_turnOnWDT(void);
bool WDTManager_isDogKickingAlarmScheduled(void);
void WDTManager_setDogKickingAlarm(void);
void WDTManager_removeDogKickingAlarm(void);

#endif /* WDT_MANAGER_H_ */