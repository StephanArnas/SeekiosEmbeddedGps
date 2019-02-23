#include <peripheralManager/GSMManager.h>

/* Le but ultime du GSMManager est d'englober le httpManager, le csManager et le moduleManager, sans que ces derniers soient accessibles
depuis n'importe quel autre fichier. Pour ce faire, il faut encapsuler les http, cs et module managers et en faire des structures de
données qui sont placées en privées (static) dans le GSMManager. */

static SemaphoreHandle_t _gsmMutex; // To take every time operations on the GSM are made (including sending AT Commands)

static BaseType_t takeGsmMutexAndWakeModule(void);
static BaseType_t giveGsmMutexAndSleepGSMModule(void);
static uint8_t hasSMSBeenReceived(void);

static ModuleService	_moduleService;
static HttpService		_httpService;
static CsService		_csService;

void GSMManager_init()
{
	_gsmMutex = xSemaphoreCreateRecursiveMutex();
	ModuleManager_init(&_moduleService);
	httpManager_init(&_httpService);
	csManager_init(&_csService);
}

void task_GSMTask(void* param)
{
	maskUtilities_setRunningMaskBits(RUNNING_BIT_GSM_TASK);
	
	
	if(strcmp((char*)param, "PON-NORMAL") == 0) // Powering on while Seekios ON
	{
		GSMManager_powerOnGSM();
	}
	else if(strcmp((char*)param, "POFF") == 0)
	{
		if(takeGsmMutexAndWakeModule()==pdPASS){

			// Remove alarms linked to the GSM use
			maskUtilities_clearRequestMaskBits(REQUEST_BIT_HTTP_SESSION_EXPIRED);
			maskUtilities_clearRequestMaskBits(REQUEST_BIT_CHECK_NETWORK_STATUS);
			USARTManager_printUsbWait("DELETING BATTERY CHECK BIT\r\n");
			maskUtilities_clearRequestMaskBits(REQUEST_BIT_CHECK_BATTERY);
			_httpService.removeHttpSessionExpirationAlarm();
			_csService.removeNetworkCheckAlarm();

			_httpService.stopHTTP();
			_csService.detachNetworkService(LONG_WAIT);
			_moduleService.powerOffModule();
			
			giveGsmMutexAndSleepGSMModule();
		}
	}

	maskUtilities_clearRunningMaskBits(RUNNING_BIT_GSM_TASK);
	FreeRTOSOverlay_taskDelete(NULL);
}

/* Allume le GSM, et entame la configuration de base du GSM. 
Return FUNCTION_SUCCESS ou FUNCTION_FAILURE de l'issue, et également
stocke la valeur de la batterie de la variable "batDataPtr"*/
uint8_t  GSMManager_powerOnGSM()
{
	uint8_t functionResult = FUNCTION_FAILURE;
	if(takeGsmMutexAndWakeModule()==pdPASS){
		if(_moduleService.powerOnModule(LONG_WAIT) == FUNCTION_SUCCESS)
		{
			functionResult = FUNCTION_SUCCESS;
			batteryManager_updateAndAnalyzeBattery();
			maskUtilities_setRequestMaskBits(REQUEST_BIT_CHECK_NETWORK_STATUS);
		}
		else
		{
			maskUtilities_setRequestTurnOffSeekios();
		}
		giveGsmMutexAndSleepGSMModule();
	}
	return functionResult;
}

/* This task checks is there is a SMS that was received (verify that the RI interrupt was not a false alarm)
If a SMS was received, it requests the Listener. If the Seekios is off, it stores the request into the pending requests*/
void task_handleRingInterrupt(void* param)
{
	UNUSED(param);
	maskUtilities_setRunningMaskBits(RUNNING_BIT_RING_INDICATOR_HANDLE);

	if(_moduleService.waitModuleUse())
	{
		uint8_t functionResult = hasSMSBeenReceived();
		
		if(functionResult == FUNCTION_SUCCESS)
		{
			maskUtilities_clearInterruptMaskBits(INTERRUPT_BIT_RING_INDICATOR);
			#if (ADDITIONAL_LED_INSTRUCTIONS==1)
			ledUtilities_runSmsWakeupHandlingInstruction();
			#endif
			if(!seekiosInfoManager_isSeekiosPeered())
			{
				seekiosInfoManager_setSeekiosPeered();
			}

			maskUtilities_setRequestMaskBits(REQUEST_BIT_LISTENER);
		}
		else if(functionResult == FUNCTION_FAILURE)
		{
			maskUtilities_clearInterruptMaskBits(INTERRUPT_BIT_RING_INDICATOR);
			USARTManager_printUsbWait("False RI Interrupt.\r\n");
		}
		// If we couldn't get if a SMS has been received or not, we will just 
	}

	maskUtilities_clearRunningMaskBits(RUNNING_BIT_RING_INDICATOR_HANDLE);
	FreeRTOSOverlay_taskDelete(NULL);
}

/* To use to lock the use of the GSM Module */
static BaseType_t takeGsmMutexAndWakeModule(){
	BaseType_t result = xSemaphoreTakeRecursive( _gsmMutex, ( TickType_t ) 20000 );
	if(result == pdTRUE){
		_moduleService.wakeModule();
		//USARTManager_printUsbWait("\t> GSM Semaphore taken.\r\n");
	}
	else
	{
		//USARTManager_printUsbWait("\t> GSM Semaphore taking FAILED.\r\n");
	}
	return result;
}

/* To use to UNlock the use of the GSM Module */
static BaseType_t giveGsmMutexAndSleepGSMModule(){
	BaseType_t result = xSemaphoreGiveRecursive( _gsmMutex);
	if(result == pdTRUE)
	{
		_moduleService.sleepModule();
		//USARTManager_printUsbWait("\t< GSM Semaphore given.\r\n");
	}
	else
	{
		//USARTManager_printUsbWait("\t< GSM Semaphore giving FAILED.\r\n");
	}
	return result;
}

void task_checkNetworkStatus()
{
	maskUtilities_setRunningMaskBits(RUNNING_BIT_CHECK_NETWORK_STATUS);
	volatile uint32_t wm = uxTaskGetStackHighWaterMark(NULL);
	if(_moduleService.waitModuleUse())
	{
		if(takeGsmMutexAndWakeModule()==pdPASS)
		{
			wm = uxTaskGetStackHighWaterMark(NULL);
			maskUtilities_clearRequestMaskBits(REQUEST_BIT_CHECK_NETWORK_STATUS);
			bool attachmentWorked = _csService.attachNetworkServiceManually(VERY_LONG_WAIT);
			wm = uxTaskGetStackHighWaterMark(NULL);
			giveGsmMutexAndSleepGSMModule();
			_csService.setNetworkCheckAlarm(attachmentWorked);
			wm = uxTaskGetStackHighWaterMark(NULL);
		}
	}
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_CHECK_NETWORK_STATUS);
	FreeRTOSOverlay_taskDelete(NULL);
}

bool GSMManager_getRSSI(uint8_t* buff)
{
	bool functionResult = false;
	if(takeGsmMutexAndWakeModule()==pdPASS)
	{
		functionResult = _moduleService.getRSSI(buff);
		giveGsmMutexAndSleepGSMModule();
	}
	return functionResult;
}

// Get RSSI as int
uint8_t  GSMManager_getRSSIInt()
{
	uint8_t RSSIInt = 0;
	if(takeGsmMutexAndWakeModule()==pdPASS)
	{
		RSSIInt = _moduleService.getRSSIInt();
		giveGsmMutexAndSleepGSMModule();
	}
	return RSSIInt;
}

bool GSMManager_getSignalLevelPctage(uint8_t* buff)
{
	bool functionResult = false;
	if(takeGsmMutexAndWakeModule()==pdPASS)
	{
		functionResult = _moduleService.getSignalLevelPctage(buff);
		giveGsmMutexAndSleepGSMModule();
	}
	return functionResult;
}

bool GSMManager_getGSMBatteryData(GSMBatData *batDataPtr)
{
	if(takeGsmMutexAndWakeModule()==pdPASS)
	{
		_moduleService.getGSMBatteryData(batDataPtr);
		giveGsmMutexAndSleepGSMModule();
		return true;
	}
	return false;
}

bool GSMManager_getIMSI(uint8_t* buff)
{
	bool functionResult = false;
	if(takeGsmMutexAndWakeModule()==pdPASS)
	{
		functionResult = _moduleService.getIMSI(buff);
		giveGsmMutexAndSleepGSMModule();
	}
	return functionResult;
}

bool GSMManager_getIMEI(uint8_t* buff)
{
	bool functionResult = false;
	if(takeGsmMutexAndWakeModule()==pdPASS)
	{
		functionResult = _moduleService.getIMEI((char*)buff);
		giveGsmMutexAndSleepGSMModule();
	}
	return functionResult;
}

uint8_t GSMManager_removeAllSMS()
{
	uint8_t functionResult = FUNCTION_FAILURE;
	if(takeGsmMutexAndWakeModule()==pdPASS)
	{
		functionResult = _moduleService.removeAllSMS();
		giveGsmMutexAndSleepGSMModule();
	}
	return functionResult;
}

uint8_t GSMManager_catCellsData(uint8_t* msgString)
{
	uint8_t functionResult = FUNCTION_SUCCESS;

	if(takeGsmMutexAndWakeModule()==pdPASS)
	{
		functionResult = _moduleService.catCellsData((char*)msgString);
		giveGsmMutexAndSleepGSMModule();
	}

	return functionResult;
}

/* Function to check if the Seekios received an SMS.
It exists as a security to check if the Ring indicator interrupt was made because of a false alarm or not */
/* If there is an SMS, it should look like :
AT+CMGL=4
+CMGL: 1,1,"",18
07911356046907412409B913000002F200006111705113914000

OK*/
static uint8_t hasSMSBeenReceived(void)
{
	uint8_t smsReceived = FUNCTION_RESULT_UNKNOWN;
	if(takeGsmMutexAndWakeModule() == pdPASS)
	{
		smsReceived = _moduleService.hasSMSBeenReceived();
		giveGsmMutexAndSleepGSMModule();
	}
	return smsReceived;
}

bool GSMManager_useHTTP(uint8_t expirationTime)
{
	bool functionResult = false;
	if(_moduleService.waitModuleUse())
	{
		if(takeGsmMutexAndWakeModule() == pdPASS)
		{
			functionResult = GSMManager_startHTTP();
			if(functionResult == FUNCTION_SUCCESS)
			{
				GSMManager_scheduleHttpSessionExpirationAlarm(expirationTime);
			}
			else
			{
				GSMManager_stopHTTP();
			}
			
			giveGsmMutexAndSleepGSMModule();
		}
	}
	return functionResult;
}

uint8_t GSMManager_attachNetworkServiceManually(uint32_t timeout)
{
	uint8_t functionResult = FUNCTION_FAILURE;
	if(takeGsmMutexAndWakeModule()==pdPASS)
	{
		
		if(_csService.attachNetworkServiceManually(timeout))
		{
			functionResult = FUNCTION_SUCCESS;
		}
		giveGsmMutexAndSleepGSMModule();
	}
	return functionResult;
}

uint8_t GSMManager_attachSpecificNetworkManually(const char* networkIdentifier, uint32_t timeout)
{
	uint8_t functionResult = FUNCTION_FAILURE;
	if(takeGsmMutexAndWakeModule()==pdPASS)
	{
		
		if(_csService.attachSpecificNetworkManually(networkIdentifier, timeout))
		{
			functionResult = FUNCTION_SUCCESS;
		}
		giveGsmMutexAndSleepGSMModule();
	}
	return functionResult;
}

uint8_t GSMManager_startHTTP()
{
	uint8_t functionResult = FUNCTION_FAILURE;
	if(takeGsmMutexAndWakeModule()==pdPASS)
	{
		
		if(!_csService.attachNetworkServiceManually(LONG_WAIT))
		{
			functionResult = FUNCTION_FAILURE;
		}
		else
		{
			volatile E_RSSI_FLOOR rssiFloor = _moduleService.getRSSIFloor();
			if(rssiFloor < RSSI_FLOOR_DATA_BAD)
			{
				functionResult = FUNCTION_FAILURE;
			}
			else
			{
				functionResult = _httpService.startHTTP();
			}
		}
		giveGsmMutexAndSleepGSMModule();
	}
	return functionResult;
}

uint8_t GSMManager_stopHTTP()
{
	uint8_t functionResult = FUNCTION_FAILURE;
	if(takeGsmMutexAndWakeModule()==pdPASS)
	{
		functionResult = _httpService.stopHTTP();
		giveGsmMutexAndSleepGSMModule();
	}
	return functionResult;
}

E_HTTP_REQUEST_STATUS GSMManager_httpGET(char* url, char* httpMessage)
{
	volatile E_HTTP_REQUEST_STATUS functionResult = HTTP_REQUEST_STATUS_NONE;
	if(takeGsmMutexAndWakeModule()==pdPASS)
	{
		functionResult = _httpService.httpGET(url, httpMessage);
		giveGsmMutexAndSleepGSMModule();
	}
	return functionResult;
}

void task_HttpSession(void* param){

	maskUtilities_setRunningMaskBits(RUNNING_BIT_HTTP_SESSION_TASK);
	if (strcmp(param, "EXPIRED") == 0)
	{
		if(_moduleService.waitModuleUse())
		{
			if(takeGsmMutexAndWakeModule()==pdPASS)
			{
				if(_httpService.stopHTTP()==FUNCTION_SUCCESS)
				{
					maskUtilities_clearRequestMaskBits(REQUEST_BIT_HTTP_SESSION_EXPIRED);
				}
				giveGsmMutexAndSleepGSMModule();
			}
		}
	}
	volatile uint32_t wm = uxTaskGetStackHighWaterMark(NULL);
	UNUSED(wm);
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_HTTP_SESSION_TASK);
	FreeRTOSOverlay_taskDelete(NULL);
}

bool GSMManager_waitModuleUse()
{
	return _moduleService.waitModuleUse();
}

bool GSMManager_isModuleStarted()
{
	return _moduleService.isModuleStarted();
}

bool GSMManager_isModulePoweredOn()
{
	return _moduleService.isModulePoweredOn();
}

bool GSMManager_isSessionExpirationAlarmScheduled()
{
	return _httpService.isSessionExpirationAlarmScheduled();
}

void GSMManager_printSessionExpirationAlarm()
{
	return _httpService.printSessionExpirationAlarm();
}

void GSMManager_scheduleHttpSessionExpirationAlarm(uint8_t expirationTime)
{
	_httpService.scheduleHttpSessionExpirationAlarm(expirationTime);
}

bool GSMManager_isNetworkCheckAlarmScheduled()
{
	return _csService.isNetworkCheckAlarmScheduled();
}

void GSMManager_getLastCheckNetwork(char *buff)
{
	_csService.getLastCheckNetwork(buff);
}

void GSMManager_printNetworkCheckAlarm()
{
	_csService.printNetworkCheckAlarm();
}

void GSMManager_removeNetworkCheckAlarm()
{
	_csService.removeNetworkCheckAlarm();
}

void GSMManager_removeHttpSessionExpirationAlarm()
{
	_httpService.removeHttpSessionExpirationAlarm();
}

void GSMManager_testCopsBug()
{
	GSMManager_powerOnGSM();

	if(takeGsmMutexAndWakeModule() == pdTRUE)
	{
		_csService.detachNetworkService(100000);
		GSMManager_startHTTP();
		USARTManager_sendATCommand(VERY_LONG_WAIT, 1, "AT+COPS=0,2\r\n");
		
		giveGsmMutexAndSleepGSMModule();
	}

}