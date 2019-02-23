#include <peripheralManager/GSMManager/module_manager.h>

/* Every public function of http_manager.c, module_manager.c and cs_manager.c should surround their GSM manipulations by this mutex */
static EventGroupHandle_t _moduleStatusMaskHandle;
static uint8_t parseRSSI(char* signalFrame);
static uint8_t getSignalLevelPctageFromRSSI(uint8_t signalLevelRSSIIndex);
static void catCellDataFromGSMBuff(char* msgString);
static uint8_t powerOnModule(uint32_t timeout);
static void powerOffModule(void);
static void sleepModule(void);
static void wakeModule(void);
static bool getRSSI(uint8_t * result);
static uint8_t getRSSIInt(void);
static bool getSignalLevelPctage(uint8_t* buff);
static void getGSMBatteryData(GSMBatData *batDataPtr);
static void parseBatteryLevel(char* batteryFrame, GSMBatData *batDataPtr);
static bool  getIMSI(uint8_t* resultBuff);
static bool getIMEI(char* result);
static uint8_t removeAllSMS(void);
static uint8_t parseRSSI(char* signalFrame);
static E_RSSI_FLOOR getRSSIFloor(void);
static int RSSIConverter(uint8_t RSSI);
static uint8_t catCellsData(char* msgString);
static void catCellDataFromGSMBuff(char* msgString);
static bool waitModuleUse(void);
static bool isModuleStarted(void);
static bool isModulePoweredOn(void);
static uint8_t hasSMSBeenReceived(void);

void ModuleManager_init(ModuleService *moduleService)
{
	_moduleStatusMaskHandle = xEventGroupCreate();
	moduleService->powerOnModule = powerOnModule;
	moduleService->powerOffModule = powerOffModule;
	moduleService->sleepModule = sleepModule;
	moduleService->wakeModule = wakeModule;
	moduleService->getRSSI = getRSSI;
	moduleService->getRSSIInt = getRSSIInt;
	moduleService->getSignalLevelPctage = getSignalLevelPctage;
	moduleService->getGSMBatteryData = getGSMBatteryData;
	moduleService->parseBatteryLevel = parseBatteryLevel;
	moduleService->getIMSI = getIMSI;
	moduleService->getIMEI = getIMEI;
	moduleService->removeAllSMS = removeAllSMS;
	moduleService->getRSSIFloor = getRSSIFloor;
	moduleService->RSSIConverter = RSSIConverter;
	moduleService->catCellsData = catCellsData;
	moduleService->waitModuleUse = waitModuleUse;
	moduleService->isModuleStarted = isModuleStarted;
	moduleService->isModulePoweredOn = isModulePoweredOn;
	moduleService->hasSMSBeenReceived = hasSMSBeenReceived;
}

// TODO : if power on fails : stop/restart GSM at the beginning ? (to disable any previous config, including cslck=1)
static uint8_t powerOnModule(uint32_t timeout)
{
	uint8_t functionResult = FUNCTION_SUCCESS;
	USARTManager_printUsbWait("Powering ON GSM Module...\r\n");
	gpio_set_pin_level(GSM_power_enable, true);
	vTaskDelay(200);
	uint32_t atCommandsTimeout = timeout / MAX_TRIES_AT_COMMAND;
	uint8_t iteration = 0;
	
	while(functionResult != FUNCTION_FAILURE && iteration < 5)
	{
		switch(iteration)
		{
			case 0:
			if(!USARTManager_repeatATCommandUntilSuccess(MAX_TRIES_AT_COMMAND, "AT\r\n", atCommandsTimeout)){ // Identify SIM
				functionResult = FUNCTION_FAILURE;
			}
			break;
			case 1:
			if(!USARTManager_repeatATCommandUntilSuccess(MAX_TRIES_AT_COMMAND, "AT+CFUN=1\r\n", atCommandsTimeout)){
				functionResult = FUNCTION_FAILURE;
			}
			break;
			case 2:
			if(!USARTManager_repeatATCommandUntilSuccess(MAX_TRIES_AT_COMMAND, "AT+IPR=115200\r\n", atCommandsTimeout)){
				functionResult = FUNCTION_FAILURE;
			}
			break;
			case 3:
			if(!USARTManager_repeatATCommandUntilSuccess(MAX_TRIES_AT_COMMAND, "AT+CIMI\r\n", atCommandsTimeout)){ // Identify SIM
				functionResult = FUNCTION_FAILURE;
			}
			break;
			case 4:
			if(!USARTManager_repeatATCommandUntilSuccess(MAX_TRIES_AT_COMMAND, "AT+CSCLK=1\r\n", atCommandsTimeout)){
				functionResult = FUNCTION_FAILURE;
			}
			break;
			default:
			break;
		}
		iteration++;
	}

	USARTManager_sendATCommand(MEDIUM_WAIT, 1, "AT+CREG=1\r\n"); // enacble URC for CS
	USARTManager_sendATCommand(MEDIUM_WAIT, 1, "AT+CGREG=1\r\n"); // enable URC for PS

	if(functionResult == FUNCTION_SUCCESS)
	{
		xEventGroupSetBits(_moduleStatusMaskHandle, MODULE_STATUS_BIT_STARTED);
		USARTManager_printUsbWait("GSM Module powered on.\r\n");
	}

	return functionResult;
}

static void powerOffModule()
{
	USARTManager_printUsbWait("Powering off GSM...");
	if(isModulePoweredOn())
	{
		xEventGroupClearBits(_moduleStatusMaskHandle, MODULE_STATUS_BIT_STARTED);
		gpio_set_pin_level(GSM_power_enable, false);
		USARTManager_printUsbWait("...GSM POWERED OFF.\r\n");
	}
	else
	{
		USARTManager_printUsbWait("...ALREADY POWERED OFF.\r\n");
	}
}

static void sleepModule(){
	gpio_set_pin_level(GSM_DTR_wake, true); // high = enter sleep
	vTaskDelay(100);
}

static void wakeModule(){
	gpio_set_pin_level(GSM_DTR_wake, false); // low = wake
	vTaskDelay(100);
}

/* Renvoie le RSSI (Received Signal Strength Information) */
static bool getRSSI(uint8_t * result)
{
	if(USARTManager_sendATCommand(LONG_WAIT, 1, "AT+CSQ\r\n") == SERIAL_ANSWER_OK){
		uint8_t signalLevelRSSIIndex = parseRSSI(gsm_buf);
		stringHelper_intToString(signalLevelRSSIIndex,(uint8_t*) result);
		return true;
	}
	return false;
}

static uint8_t getRSSIInt()
{
	if(USARTManager_sendATCommand(LONG_WAIT, 1, "AT+CSQ\r\n") == SERIAL_ANSWER_OK){
		 return parseRSSI(gsm_buf);
	}
	return 0;
}

/* Renvoie  le RSSI (Received Signal Strength Information)
En %age */
static bool getSignalLevelPctage(uint8_t* buff)
{
	if(USARTManager_sendATCommand(LONG_WAIT, 1, "AT+CSQ\r\n") == SERIAL_ANSWER_OK){
		uint8_t signalLevelRSSIIndex = parseRSSI(gsm_buf);
		uint8_t signalLevelPctage = getSignalLevelPctageFromRSSI(signalLevelRSSIIndex);
		stringHelper_intToString(signalLevelPctage,(uint8_t*) buff);
		return true;
	}
	return false;
}

/* Stocke dans "values" la battery data perçue par le GSM : respectivement le bat %age et le bat Vage*/
static void getGSMBatteryData(GSMBatData *batDataPtr)
{
	if(USARTManager_sendATCommand(LONG_WAIT, 1, "AT+CBC\r\n") == SERIAL_ANSWER_OK){
		parseBatteryLevel(gsm_buf, batDataPtr);
	}
}

/* parse une trame de réponse de batterie de la forme :
+CBC:<bcs>,<bcl>,<voltage>
avec bcs = charge status (not charging, charging, charging finished)
bcl = battery level (%age) */
static void parseBatteryLevel(char* batteryFrame, GSMBatData *batDataPtr){
	//AT+CBC\r\r\n+CBC: 0,64,3917\r\n\r\nOK\r\n
	if(batDataPtr == NULL)
	{
		return;
	}
	
	batDataPtr->batteryPercentage = 0;
	batDataPtr->batteryVoltage = 0;
	
	batteryFrame = strstr(batteryFrame, "+CBC:");
	if(batteryFrame == NULL)
	{
		return;
	}
	
	char* part = strtok(batteryFrame, ",");
	if(part == NULL)
	{
		return;
	}
	// part = "+CBC: 0"

	part = strtok(NULL, ",");
	if(part == NULL)
	{
		return;
	}
	// part = "64" (bat percentage)
	batDataPtr->batteryPercentage = atoi(part);

	part = strtok(NULL, ",");
	if(part == NULL)
	{
		return;
	}
	// part = "3917" (bat voltage)
	batDataPtr->batteryVoltage = atoi(part);
}

/*
Writes the number to the MISISDN phone book, then reads it
Returns the SIM Card phone number */
static bool  getIMSI(uint8_t* resultBuff)
{
	E_SERIAL_ANSWER answer = USARTManager_sendATCommand(LONG_WAIT, 1, "AT+CIMI\r\n");
	char* IMSI = NULL;
	if(answer == SERIAL_ANSWER_OK){
		strtok(gsm_buf,"\r\n");
		IMSI = strtok(NULL,"\r\n");
	}
	if(strlen(IMSI) == 15){
		strcpy((char*)resultBuff, (char*)IMSI);
		return true;
	}
	return false;
}

static bool getIMEI(char* result){
	E_SERIAL_ANSWER answer = SERIAL_ANSWER_NONE;
	answer = USARTManager_sendATCommand(LONG_WAIT, 1, "AT+GSN\r\n");
	char* IMEI = NULL;
	if(answer == SERIAL_ANSWER_OK){
		strtok(gsm_buf,"\r\n");
		IMEI = strtok(NULL,"\r\n");
	}
	
	if(strlen(IMEI) == 15){
		strcpy(result, IMEI);
		return true;
	}
	return false;
}

/* Retire de la liste le premier message du buffer de SMS (le premier arrivé) */
static uint8_t removeAllSMS(){
	USARTManager_printUsbWait("REMOVING SMS...\r\n");
	if(USARTManager_sendATCommand(MEDIUM_WAIT, 1, "AT+CMGD=1,4\r\n") != SERIAL_ANSWER_OK)
	{
		USARTManager_printUsbWait("SMS REMOVING FAILED.\r\n");
		return FUNCTION_FAILURE;
	}
	else
	{
		USARTManager_printUsbWait("SMS REMOVED SUCCESFULLY.\r\n");
		return FUNCTION_SUCCESS;
	}
}

/* Parse la trame +CSQ:<rssi>,<ber> et retourne le RSSI */
static uint8_t parseRSSI(char* signalFrame)
{
	//GSM_buf = [AT+CSQ\r\n+CSQ: 18,0\r\n\r\nOK\r\n]
	char* start = strstr(signalFrame,"+CSQ:");
	//start = [+CSQ: 18,0\r\n\r\nOK\r\n]
	start += 6;

	if(strtok(start, ",") == NULL)	return 0;
	return atoi(start);
}

/* Returns the current rssi floor we are at with the command AT+CSQ. The RSSI Floor represents what you can do (sms, data etc.) with the current RSSI */
static E_RSSI_FLOOR getRSSIFloor()
{
	E_RSSI_FLOOR rssiFloor = RSSI_FLOOR_UNEXPLOITABLE;
	if(USARTManager_sendATCommand(LONG_WAIT, 1, "AT+CSQ\r\n") == SERIAL_ANSWER_OK){
		volatile uint8_t rssi = parseRSSI(gsm_buf);
		if(rssi >= RSSI_FLOOR_UNEXPLOITABLE && rssi < RSSI_FLOOR_SMS)
		{
			rssiFloor = RSSI_FLOOR_UNEXPLOITABLE;
		}
		else if (rssi >= RSSI_FLOOR_SMS && rssi < RSSI_FLOOR_DATA_BAD)
		{
			rssiFloor = RSSI_FLOOR_SMS;
		}
		else if (rssi >= RSSI_FLOOR_DATA_BAD && rssi < RSSI_FLOOR_DATA_GOOD)
		{
			rssiFloor = RSSI_FLOOR_DATA_BAD;
		}
		else if (rssi >= RSSI_FLOOR_DATA_GOOD && rssi < RSSI_FLOOR_DATA_VERY_GOOD)
		{
			rssiFloor = RSSI_FLOOR_DATA_GOOD;
		}
		else if (rssi >= RSSI_FLOOR_DATA_VERY_GOOD && rssi < RSSI_FLOOR_DATA_BEST)
		{
			rssiFloor = RSSI_FLOOR_DATA_VERY_GOOD;
		}
		else if (rssi >= RSSI_FLOOR_DATA_BEST && rssi < 99)
		{
			rssiFloor = RSSI_FLOOR_DATA_BEST;
		}
		else
		{
			rssiFloor = RSSI_FLOOR_UNEXPLOITABLE;
		}
	}
	return rssiFloor;
}

static uint8_t getSignalLevelPctageFromRSSI(uint8_t signalLevelRSSIIndex){
	if(signalLevelRSSIIndex > 0 && signalLevelRSSIIndex < 32){ // not known/not detectable
		float ratio = signalLevelRSSIIndex/31.0f;
		return ratio*100;
	}
	else
	{
		return 0;
	}

}

/* Convertit un RSSI en dbm APPROXIMATIVEMENT. Renvoie 99 si RSSI inconnu/indétecté
A SAVOIR : le RSSI est à titre indicatif, il ne donne pas le dbm EXACT */
static int RSSIConverter(uint8_t RSSI){
	if(RSSI == 0){
		return -115; // actually -115 or less
	}
	else if(RSSI == 1){
		return -111;
	}
	else if(RSSI >= 2 && RSSI <= 30){
		float stepVal = (-54 + 110)/28;
		int stepNum = RSSI - 2;
		return stepNum*stepVal - 110;
	}
	else if (RSSI == 31){
		return -52; // actually -52 or greater
	}
	else {
		return 99; // actually undefined/undetected
	}
}

/* Stocke les infos sur les cells détectées par le module GSM dans responseBuff, et les
retourne sous forme brute */
static uint8_t catCellsData(char* msgString){
	
	uint8_t functionResult = FUNCTION_SUCCESS;
	// activation du mode 2 engineering
	if(USARTManager_sendATCommand(2000, 1, "AT+CENG=3\r\n") == SERIAL_ANSWER_OK)
	{
		if(USARTManager_sendATCommand(2000, 1, "AT+CENG?\r\n") != SERIAL_ANSWER_OK)
		{
			functionResult = FUNCTION_FAILURE;
		}
		else
		{
			catCellDataFromGSMBuff(msgString);
		}
	}
	USARTManager_sendATCommand(2000, 1, "AT+CENG=0\r\n");

	return functionResult;
}

static void catCellDataFromGSMBuff(char* msgString){
	uint8_t nbValidCellsData = 0;
	char* startData = strstr(gsm_buf,"+CENG: 0,\"");
	char MCCBuf[5] = "";
	char MNCBuf[8] = "";
	char CellIdBuf[12] = "";
	char LACBuf[12] = "";
	if(startData != NULL){
		uint8_t i = 0;
		char* lineSavePtr = NULL;
		char* line = strtok_r(startData, "\r\n", &lineSavePtr);
		for(i = 0; i<MAX_CELL_DATA && line != NULL;i++){
			char* lineDataSavePtr = NULL;
			char* lineData = strtok_r(line,"\",",&lineDataSavePtr);
			uint8_t j = 0;
			while(lineData != NULL){
				
				if(j == 1){
					strcpy(MCCBuf, lineData);
				}
				if(j == 2){
					strcpy(MNCBuf, lineData);
				}
				if(j == 3){
					strcpy(LACBuf, lineData);
				}
				if(j == 4){
					strcpy(CellIdBuf, lineData);
				}
				if(j == 6){
					nbValidCellsData++;
				}
				j++;
				lineData = strtok_r(NULL,"\",",&lineDataSavePtr);
			}
			if((nbValidCellsData - 1) == i){
				if(i == 0){
					strcat(msgString, "/");
					strcat(msgString, MCCBuf);
					strcat(msgString, ";");
					strcat(msgString, MNCBuf);
				}
				strcat(msgString, ";");
				strcat(msgString, CellIdBuf);
				strcat(msgString, ",");
				strcat(msgString, LACBuf);
				strcat(msgString, ",");
				strcat(msgString, MCCBuf);
				strcat(msgString, ",");
				strcat(msgString, MNCBuf);
			}
			line = strtok_r(NULL,"\r\n", &lineSavePtr);
		}
	}
}

static bool waitModuleUse()
{
	EventBits_t moduleStatusMask = xEventGroupWaitBits(_moduleStatusMaskHandle, MODULE_STATUS_BIT_STARTED, pdFALSE, pdFALSE, VERY_LONG_WAIT);
	return (moduleStatusMask & MODULE_STATUS_BIT_STARTED)!=0;
}

static bool isModuleStarted()
{
	EventBits_t moduleStatusMask = xEventGroupGetBits(_moduleStatusMaskHandle);
	return (moduleStatusMask & MODULE_STATUS_BIT_STARTED) != 0;
}

static bool isModulePoweredOn()
{
	return gpio_get_pin_level(GSM_power_enable);
}

static uint8_t hasSMSBeenReceived()
{
	USARTManager_sendATCommand(VERY_LONG_WAIT, 1, "AT+CMGL=4\r\n");
	return strstr(gsm_buf, "+CMGL:") != NULL ? FUNCTION_SUCCESS : FUNCTION_FAILURE;
}