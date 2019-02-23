#ifndef CS_MANAGER_H_
#define CS_MANAGER_H_

#include <peripheralManager/USART_manager.h>
#include <peripheralManager/RTC_manager.h>
#include <seekiosManager/mask_utilities.h>
#include <peripheralManager/battery_manager.h>

#define NETWORK_SEARCH_PERIOD	60 // 1 min // Period for trying to connect to the gsm network
#define NETWORK_CHECK_PERIOD	300 // 5 min // Period in second for checking the GSM network status (check that it is still connected)

typedef enum{
	NETWORK_REGISTRATION_NOT_REGISTERED = 			0,
	NETWORK_REGISTRATION_REGISTERED_HOME = 			1,
	NETWORK_REGISTRATION_NOT_REGISTERED_SEARCHING = 2,
	NETWORK_REGISTRATION_DENIED = 					3,
	NETWORK_REGISTRATION_UNKNOWN = 					4,
	NETWORK_REGISTRATION_REGISTERED_ROAMING = 		5,
}E_NETWORK_REGISTRATION_STATUS;

typedef enum{
	OPERATOR_FORMAT_LONG_ALPHANUMERIC	= 0,
	OPERATOR_FORMAT_SHORT_ALPHANUMERIC	= 1,
	OPERATOR_FORMAT_NUMERIC				= 2,
}E_OPERATOR_FORMAT;

typedef struct  
{
	bool (*detachNetworkService)(uint32_t timeout);
	void (*removeNetworkCheckAlarm)(void);
	void (*task_checkNetworkStatus)(void);
	void (*csManager_testCheckNetworkStatus)(void);
	void (*getLastCheckNetwork)(char buff[10]);
	bool (*attachNetworkServiceManually)(uint32_t timeout);
	bool (*attachSpecificNetworkManually)(const char* networkIdentifier, uint32_t timeout);
	void (*printNetworkCheckAlarm)(void);
	bool (*isNetworkCheckAlarmScheduled)(void);
	void (*setNetworkCheckAlarm)(bool attachmentWorked);
}CsService;

void csManager_init(CsService *_csService);

#endif /* CS_MANAGER_H_ */