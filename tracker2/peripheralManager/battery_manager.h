#ifndef BATTERY_MANAGER_H_
#define BATTERY_MANAGER_H_

#include <sgs/port_sgs.h>
#include <messageSender/message_sender.h>
#include <seekiosCore/seekios.h>
#include <tools/led_utilities.h>
#include <seekiosManager/power_state_manager.h>

#define USB_LEVEL_MINIMUM_CHARGE_VOLTAGE	4000

#define BATTERY_LEVEL_100_VOLTAGE			4150 // 100% // The real 100% is 4200 but take too much time to reach ?
#define BATTERY_LEVEL_0_VOLTAGE				3500 // Level threshold for extinction

#define BATTERY_LEVEL_66_VOLTAGE			((BATTERY_LEVEL_100_VOLTAGE - BATTERY_LEVEL_0_VOLTAGE)*0.66666666)+BATTERY_LEVEL_0_VOLTAGE // 3960 // 66.6%
#define BATTERY_LEVEL_40_VOLTAGE			((BATTERY_LEVEL_100_VOLTAGE - BATTERY_LEVEL_0_VOLTAGE)*0.40)+BATTERY_LEVEL_0_VOLTAGE //3810 // 40% // Used for as a threshold for checking more regularly the battery status
#define BATTERY_LEVEL_33_VOLTAGE			((BATTERY_LEVEL_100_VOLTAGE - BATTERY_LEVEL_0_VOLTAGE)*0.33333333)+BATTERY_LEVEL_0_VOLTAGE //3730 // 33.3%
#define BATTERY_LEVEL_20_VOLTAGE			((BATTERY_LEVEL_100_VOLTAGE - BATTERY_LEVEL_0_VOLTAGE)*0.20)+BATTERY_LEVEL_0_VOLTAGE //3630 // 33.3%
#define BATTERY_LEVEL_10_VOLTAGE			((BATTERY_LEVEL_100_VOLTAGE - BATTERY_LEVEL_0_VOLTAGE)*0.1)+BATTERY_LEVEL_0_VOLTAGE // Critical level
// We check the battery level on a regular basis (in minutes)
#if (DEBUG_MODE==1)
#define BATTERY_CHECK_PERIOD_LOW	10
#define BATTERY_CHECK_PERIOD_HIGH	30
#else
#define BATTERY_CHECK_PERIOD_LOW	10
#define BATTERY_CHECK_PERIOD_HIGH	30
#endif


typedef enum{
	CHARGE_STATUS_DISCHARGING,
	CHARGE_STATUS_CHARGING,
}E_CHARGE_STATUS;

#if (ACTIVATE_BATTERY_LOG == 1)
typedef struct {
	uint32_t logId;
	uint32_t timestamp;
	uint16_t voltageShown;
	uint16_t voltageReal;
}BatteryLog;
#endif

typedef struct  
{
	uint16_t batteryVoltage;
	uint16_t USBVoltage;
}ADCData;

void batteryLevel_init(void);
bool batteryManager_isBatteryLevelCritical(void);
void batteryManager_convertVoltageToPercentage(uint16_t batVoltage, uint8_t* buff);
void batteryManager_printBatteryLevels(void);
E_LED_BATTERY_TIER batteryManager_getLedPercentageTier(void);
bool batteryManager_isBatteryCharging(void);
bool batteryMaanger_addBatteryLog(void);
void batteryManager_printBatteryCheckAlarm(void);
void batterymanager_printLastSamples(void);
bool batteryManager_isBatteryCheckAlarmScheduled(void);
void batteryManager_getBatteryPercentageStr(uint8_t* buff);
void task_checkBattery(void* param);
void batteryManager_removeBatteryCheckAlarm(void);
void batteryManager_updateChargeStatus(void);
void batteryManager_printBatteryLogs(void);
void batteryManager_updateAndAnalyzeBattery(void);


#endif /* BATTERY_MANAGER_H_ */