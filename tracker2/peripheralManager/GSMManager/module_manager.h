#ifndef GSM_MODULE_MANAGER_H_
#define GSM_MODULE_MANAGER_H_

#include <seekiosCore/seekios.h>
#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <semphr.h>
#include <stdbool.h>
#include <sgs/port_sgs.h>
#include <peripheralManager/USART_manager.h>
#include <seekiosManager/mask_utilities.h>
#include <peripheralManager/RTC_manager.h>

#define MODULE_STATUS_BIT_STARTED		(1 << 0)

/* The different RSSI floors (=what you can do with such RSSIs) that you can get with the AT command AT+CSQ. */
typedef enum{
	RSSI_FLOOR_UNEXPLOITABLE =	0,
	RSSI_FLOOR_SMS =			3,		// Martin ref value : 4
	RSSI_FLOOR_DATA_BAD	=		5,		// Martin ref value : 7
	RSSI_FLOOR_DATA_GOOD =		12,		// Martin ref value : 12
	RSSI_FLOOR_DATA_VERY_GOOD = 14,		// Martin ref value : 14
	RSSI_FLOOR_DATA_BEST =		18,		// Martin ref value : 18
}E_RSSI_FLOOR;

typedef struct{
	uint16_t batteryPercentage;
	uint16_t batteryVoltage;
}GSMBatData;

typedef struct
{
	uint8_t (*powerOnModule)(uint32_t timeout);
	void (*powerOffModule)(void);
	void (*sleepModule)(void);
	void (*wakeModule)(void);
	bool (*getRSSI)(uint8_t* buff);
	uint8_t (*getRSSIInt)(void);
	bool (*getSignalLevelPctage)(uint8_t* buff);
	bool (*parseSignalLevel)(char* signalFrame, char* result);
	void (*getGSMBatteryData)(GSMBatData *batDataPtr);
	void (*parseBatteryLevel)(char* batteryFrame, GSMBatData *batDataPtr);
	bool (*getIMSI)(uint8_t* resultBuff);
	bool (*getIMEI)(char* result);
	uint8_t (*removeAllSMS)(void);
	uint8_t (*parseRSSI)(char* signalFrame);
	E_RSSI_FLOOR (*getRSSIFloor)(void);
	int (*RSSIConverter)(uint8_t RSSI);
	uint8_t (*catCellsData)(char* msgString);
	void (*catCellDataFromGSMBuff)(char* msgString);
	bool (*waitModuleUse)(void);
	bool (*isModuleStarted)(void);
	bool (*isModulePoweredOn)(void);
	uint8_t (*hasSMSBeenReceived)(void);
}ModuleService;

void ModuleManager_init(ModuleService *moduleService);

#endif /* GSM_MODULE_MANAGER_H_ */