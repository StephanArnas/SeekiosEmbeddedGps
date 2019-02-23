#ifndef GSM_MANAGER_H_
#define GSM_MANAGER_H_

#include <stdint.h>
#include <seekiosCore/seekios.h>
#include <seekiosManager/mask_utilities.h>
#include <peripheralManager/GSMManager/module_manager.h>
#include <peripheralManager/GSMManager/cs_manager.h>
#include <peripheralManager/GSMManager/http_manager.h>

typedef enum
{
	GSM_TASK_PARAM_POWER_ON_MIN_FUNCT =		0,
	GSM_TASK_PARAM_POWER_ON_NORM_FUNCT =	1,
	GSM_TASK_PARAM_POWER_OFF =				2,
}E_GSM_TASK_PARAM;

// typedef struct  
// {
// 	bool (*startHttp)(void);
// }HttpManager;

void task_GSMTask(void* param);
uint8_t GSMManager_startHTTP(void);
uint8_t GSMManager_stopHTTP(void);
E_HTTP_REQUEST_STATUS GSMManager_httpGET(char* url, char* httpMessage);
bool GSMManager_getRSSI(uint8_t* buff);
uint8_t  GSMManager_getRSSIInt(void);
bool GSMManager_getSignalLevelPctage(uint8_t* buff);
bool GSMManager_getGSMBatteryData(GSMBatData *batDataPtr);
bool GSMManager_getIMSI(uint8_t* buff);
bool GSMManager_getIMEI(uint8_t* buff);
uint8_t GSMManager_removeAllSMS();
uint8_t GSMManager_catCellsData(uint8_t* msgString);
bool GSMManager_useHTTP(uint8_t expirationTime);
void task_HttpSession(void* param);
uint8_t GSMManager_powerOnGSM();
void task_handleRingInterrupt(void* param);
bool GSMManager_waitModuleUse(void);
bool GSMManager_isModuleStarted(void);
bool GSMManager_isSessionExpirationAlarmScheduled(void);
void GSMManager_printSessionExpirationAlarm(void);
void GSMManager_scheduleHttpSessionExpirationAlarm(uint8_t expirationTime);
void task_checkNetworkStatus(void);
bool GSMManager_isNetworkCheckAlarmScheduled(void);
void GSMManager_getLastCheckNetwork(char *buff);
void GSMManager_printNetworkCheckAlarm(void);
void GSMManager_init(void);
void GSMManager_removeNetworkCheckAlarm(void);
void GSMManager_removeHttpSessionExpirationAlarm(void);
void GSMManager_updateGPRSExpirationTime(void);
void GSMManager_testCopsBug(void);
uint8_t GSMManager_attachNetworkServiceManually(uint32_t timeout);
bool GSMManager_isModulePoweredOn(void);
uint8_t GSMManager_attachSpecificNetworkManually(const char* networkIdentifier, uint32_t timeout);

#endif /* GSM_MANAGER_H_ */