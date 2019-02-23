#include <peripheralManager/GSMManager/cs_manager.h>

/* The cs manager contains all the functions to manager the connexion to the gsm network (not including packet network functions.*/

static E_NETWORK_REGISTRATION_STATUS getNetworkRegistrationStatus(void);
static bool isNetworkDetached(void);
static uint8_t parseNetworkRegistration(char* responseToParse);
static void connectToFirstAvailableNetwork(uint32_t timeout);
static void networkCheckAlarmCallback(struct calendar_descriptor *const calendar);
static int getPreferedOperatorIndex(char* operator);
static bool isNetworkAttached(void);
static void setNetworkCheckAlarm(bool attachmentWorked);
static bool attachSpecificNetworkManually(const char* networkIdentifier, uint32_t timeout);
static bool isSpecificNetworkAttached(const char* networkIdentifier);
static void connectSpecificNetwork(const char* networkIdentifier, uint32_t timeout);

static bool detachNetworkService(uint32_t timeout);
static void removeNetworkCheckAlarm(void);
static void getLastCheckNetwork(char buff[10]);
static bool attachNetworkServiceManually(uint32_t timeout);
static void printNetworkCheckAlarm(void);
static bool isNetworkCheckAlarmScheduled(void);
static void setNetworkCheckAlarm(bool attachmentWorked);

static struct calendar_alarm _networkCheckAlarm;
#define NETWORK_NAME_MAX_LENGTH 32
static char	_lastCheckedNetwork[NETWORK_NAME_MAX_LENGTH] = "NONE"; // the lastly checked network; can be shown as

#define NETWORK_OPERATORS_COUNT 137
static const char* _networkOperators[NETWORK_OPERATORS_COUNT] = {
"27602", //Vodafone"},
"722310", //Claro"},
"722310", //Claro"},
"28305", //VivaCell"},
"50503", //Vodafone"},
"23201", //Mobilkom"},
"42602", //ZainBH"},
"20601", //Proximus"},
"73601", //Nuevatel"},
"72405", //Claro"},
"28401", //M-Tel"},
"302610", //BellMobility"},
"302720", //RogersWireless"},
"302780", //Sasktel"},
"302220", //Telus"},
"73003", //Claro"},
"46000", //ChinaMobile"},
"732101", //Claro"},
"71203", //Claro"},
"21910", //VIPnet"},
"28001", //Cytamobile-Vodafone"},
"23003", //Vodafone"},
"23801", //TDC"},
"37002", //Claro"},
"74001", //Conecel"},
"60202", //Vodafone"},
"70601", //Claro"},
"24802", //Elisa"},
"28802", //Vodafone"},
"24405", //Elisa"},
"20801", //Orange"},
"20820", //Bouygues"},
"20810", //SFR"},
"54715", //Vodafone"},
"26202", //Vodafone"},
"62002", //Vodafone"},
"20205", //Vodafone"},
"70401", //Claro"},
"23403", //Airtel-Vodafone"},
"708001", //Claro"},
"45403", //3"},
"45404", //Hutchison"},
"21670", //Vodafone"},
"27402", //Vodafone"},
"40413", //Vodafone"},
"405752", //Vodafone"},
"40420", //Vodafone"},
"40484", //Vodafone"},
"40411", //Vodafone"},
"40405", //Vodafone"},
"40401", //Vodafone"},
"405754", //Vodafone"},
"40486", //Vodafone"},
"40446", //Vodafone"},
"40430", //Vodafone"},
"405756", //Vodafone"},
"40427", //Vodafone"},
"40420", //Vodafone"},
"405753", //Vodafone"},
"40488", //Vodafone"},
"40460", //Vodafone"},
"40443", //Vodafone"},
"40415", //Vodafone"},
"40566", //Vodafone"},
"40567", //Vodafone"},
"27201", //Vodafone"},
"22210", //Vodafone"},
"44010", //NTTDoCoMo"},
"23403", //Airtel-Vodafone"},
"41601", //Zain"},
"63902", //Safaricom"},
"41902", //Zain"},
"24705", //BiteLatvija"},
"65101", //VODACOM"},
"22801", //Swisscom"},
"24602", //BiteGSM"},
"27077", //Tango"},
"50213", //Celcom"},
"50219", //Celcom"},
"27801", //Vodafone"},
"334020", //Telcel"},
"20810", //SFR"},
"64304", //Vodacom"},
"20404", //Vodafone"},
"53001", //Vodafone"},
"71021", //Claro"},
"24201", //Telenor"},
"71403", //Claro"},
"74402", //Claro"},
"71610", //Claro"},
"51503", //SMARTGold"},
"26001", //Plus"},
"26801", //Vodafone"},
"310380", //AT&T"},
"310410", //AT&T"},
"330110", //Claro"},
"310260", //T-Mobile"},
"42702", //Vodafone"},
"22601", //Vodafone"},
"25001", //MTS"},
"22210", //Vodafone"},
"42004", //Zain"},
"22005", //VIP"},
"52505", //StarHub"},
"29340", //SIMobil"},
"65501", //Vodacom"},
"21401", //Vodafone"},
"41302", //Dialog"},
"24201", //Telenor"},
"24008", //Telenor"},
"24004", //Telenor"},
"24024", //Telenor(Net4Mobility)"},
"22801", //Swisscom"},
"46692", //Chunghwa"},
"64004", //Vodacom"},
"52004", //RealFuture"},
"52099", //TrueMove"},
"28602", //Vodafone"},
"42403", //du"},
"23415", //Vodafone"},
"310380", //AT&T"},
"310410", //AT&T"},
"310260", //T-Mobile"},
"310160", //T-Mobile"},
"310200", //T-Mobile"},
"310210", //T-Mobile"},
"310220", //T-Mobile"},
"310230", //T-Mobile"},
"310240", //T-Mobile"},
"310250", //T-Mobile"},
"310270", //T-Mobile"},
"310310", //T-Mobile"},
"310490", //T-Mobile"},
"74810", //Claro"},
"22210", //Vodafone"},
"45202", //Vinaphone"},
"310410", //AT&T"},
};


/* This task checks the network status : is it connected to a fully-working network, and if it is connected
to a non-working network, deconnect/reconnects ?*/
/* TODO improvements :
- try to MANUALLY connect to a prefered network if the currently connected network is not one we want
- */

void csManager_init(CsService *_csService)
{
	_csService->attachNetworkServiceManually = attachNetworkServiceManually;
	_csService->attachSpecificNetworkManually = attachSpecificNetworkManually;
	_csService->getLastCheckNetwork = getLastCheckNetwork;
	_csService->detachNetworkService = detachNetworkService;
	_csService->printNetworkCheckAlarm = printNetworkCheckAlarm;
	_csService->setNetworkCheckAlarm = setNetworkCheckAlarm;
	_csService->removeNetworkCheckAlarm = removeNetworkCheckAlarm;
	_csService->isNetworkCheckAlarmScheduled = isNetworkCheckAlarmScheduled;
}

static bool attachNetworkServiceManually(uint32_t timeout)
{
	TickType_t startingTime = xTaskGetTickCount();
	USARTManager_printUsbWait("Starting to attach the network services...\r\n");
	USARTManager_sendATCommand(timeout, 1, "AT+COPS=3,2\r\n"); // Set operator format to 2 (numeric)
	while(!isNetworkAttached())
	{
		TickType_t currentTime = xTaskGetTickCount();
		uint32_t elapsedTime = currentTime - startingTime;
		if(elapsedTime >= timeout)
		{
			USARTManager_printUsbWait("Failed to attach Network (timeout).\r\n");
			return false;
		}

		if(getNetworkRegistrationStatus() == NETWORK_REGISTRATION_NOT_REGISTERED)
		{
			USARTManager_sendATCommand(timeout - elapsedTime, 1, "AT+COPS=0,2\r\n"); // To go into the searching state. If completely unregistered to network
		}

		connectToFirstAvailableNetwork(timeout - elapsedTime);
	}
	USARTManager_printUsbWait("Network attached.\r\n");
	return true;
}

static bool attachSpecificNetworkManually(const char* networkIdentifier, uint32_t timeout)
{
		TickType_t startingTime = xTaskGetTickCount();
		USARTManager_printUsbWait("Starting to attach the network services...\r\n");
		USARTManager_sendATCommand(timeout, 1, "AT+COPS=3,2\r\n"); // Set operator format to 2 (numeric)
		while(!isSpecificNetworkAttached(networkIdentifier))
		{
			TickType_t currentTime = xTaskGetTickCount();
			uint32_t elapsedTime = currentTime - startingTime;
			if(elapsedTime >= timeout)
			{
				USARTManager_printUsbWait("Failed to attach Network : ");
				USARTManager_printUsbWait(networkIdentifier);
				USARTManager_printUsbWait(" (timeout).\r\n");
				return false;
			}

			if(getNetworkRegistrationStatus() == NETWORK_REGISTRATION_NOT_REGISTERED)
			{
				USARTManager_sendATCommand(timeout - elapsedTime, 1, "AT+COPS=0,2\r\n"); // To go into the searching state. If completely unregistered to network
			}

			connectSpecificNetwork(networkIdentifier, timeout - elapsedTime);
		}
		USARTManager_printUsbWait("Network attached.\r\n");
		return true;
}

/* /!\ it seems that the command "AT+COPS=?" doesn't work if the device is in state : "MT is not currently searching a new
operator to register to" (CREG status 0)*/
static void connectToFirstAvailableNetwork(uint32_t timeout)
{
	if(USARTManager_sendATCommand(timeout, 1,"AT+COPS=?\r\n")==SERIAL_ANSWER_OK) // Output should look like : "AT+COPS=?\r\n+COPS: (2,\"SFR\",\"SFR\",\"20810\"),(1,\"Orange F\",\"Orange\",\"20801\"),(1,\"BOUYGUES TELECOM\",\"BYTEL\",\"20820\"),,(0-4),(0-2)\r\nOK"
	{	
		char* start = strstr(gsm_buf, "+COPS:");
		char* operatorEntryDelimiter = "()";
		char* savedOperatorEntryPtr;
		char* operatorEntry = strtok_r(start, operatorEntryDelimiter, &savedOperatorEntryPtr);

		for(operatorEntry = strtok_r(NULL, operatorEntryDelimiter, &savedOperatorEntryPtr);\
		operatorEntry != NULL;\
		operatorEntry = strtok_r(NULL, operatorEntryDelimiter, &savedOperatorEntryPtr))// One entry will be a correct one, then it will be an entry containing only ",".
		{
			if(strcmp(operatorEntry, ",,") == 0) // if we reach ",," it means that the module will start return the possible entires for COPS command, which doesn't interest us.
			{
				return;
			}
			else if (strcmp(operatorEntry, ",") == 0)
			{
				continue;
			}
			char* operatorInfoDelimiter = "\",";
			char* savedOperatorInfoPtr;
			char* operatorStatus = strtok_r(operatorEntry, operatorInfoDelimiter, &savedOperatorInfoPtr);
			if(operatorStatus != NULL)
			{
				if(atoi(operatorStatus) == 1)
				{
					char* operatorLongName	= strtok_r(NULL, operatorInfoDelimiter, &savedOperatorInfoPtr);
					char* operatorShortName = strtok_r(NULL, operatorInfoDelimiter, &savedOperatorInfoPtr);
					UNUSED(operatorLongName);
					UNUSED(operatorShortName);
					char* operatorNumName	= strtok_r(NULL, operatorInfoDelimiter, &savedOperatorInfoPtr);
					int preferedOperatorIndex = -1;
					preferedOperatorIndex = getPreferedOperatorIndex(operatorNumName);
					if(preferedOperatorIndex != -1)
					{
						USARTManager_sendATCommand(timeout, 3, "AT+COPS=4,2,\"",_networkOperators[preferedOperatorIndex],"\"\r\n");
						return;
					}
				}
			}
		}
	}
}

static void connectSpecificNetwork(const char* networkIdentifier, uint32_t timeout)
{
	if(USARTManager_sendATCommand(timeout, 1,"AT+COPS=?\r\n")==SERIAL_ANSWER_OK) // Output should look like : "AT+COPS=?\r\n+COPS: (2,\"SFR\",\"SFR\",\"20810\"),(1,\"Orange F\",\"Orange\",\"20801\"),(1,\"BOUYGUES TELECOM\",\"BYTEL\",\"20820\"),,(0-4),(0-2)\r\nOK"
	{
		char* start = strstr(gsm_buf, "+COPS:");
		char* operatorEntryDelimiter = "()";
		char* savedOperatorEntryPtr;
		char* operatorEntry = strtok_r(start, operatorEntryDelimiter, &savedOperatorEntryPtr);

		for(operatorEntry = strtok_r(NULL, operatorEntryDelimiter, &savedOperatorEntryPtr);\
		operatorEntry != NULL;\
		operatorEntry = strtok_r(NULL, operatorEntryDelimiter, &savedOperatorEntryPtr))// One entry will be a correct one, then it will be an entry containing only ",".
		{
			if(strcmp(operatorEntry, ",,") == 0) // if we reach ",," it means that the module will start return the possible entires for COPS command, which doesn't interest us.
			{
				return;
			}
			else if (strcmp(operatorEntry, ",") == 0)
			{
				continue;
			}
			char* operatorInfoDelimiter = "\",";
			char* savedOperatorInfoPtr;
			char* operatorStatus = strtok_r(operatorEntry, operatorInfoDelimiter, &savedOperatorInfoPtr);
			if(operatorStatus != NULL)
			{
				if(atoi(operatorStatus) == 1)
				{
					char* operatorLongName	= strtok_r(NULL, operatorInfoDelimiter, &savedOperatorInfoPtr);
					char* operatorShortName = strtok_r(NULL, operatorInfoDelimiter, &savedOperatorInfoPtr);
					UNUSED(operatorLongName);
					UNUSED(operatorShortName);
					char* operatorNumName	= strtok_r(NULL, operatorInfoDelimiter, &savedOperatorInfoPtr);
					if(strcmp(networkIdentifier, operatorNumName))
					{
						USARTManager_sendATCommand(timeout, 3, "AT+COPS=1,2,\"",networkIdentifier,"\"\r\n");
						return;
					}
				}
			}
		}
	}
}

void getLastCheckNetwork(char buff[10])
{
	strcpy(buff, _lastCheckedNetwork);
}

static bool detachNetworkService(uint32_t timeout)
{
	bool functionResult = true;
	TickType_t startingTime = xTaskGetTickCount();
	while(!isNetworkDetached())
	{
		TickType_t currentTime = xTaskGetTickCount();
		uint16_t elapsedTime = currentTime - startingTime;
		if(elapsedTime >= timeout)
		{
			functionResult = false;
			USARTManager_printUsbWait("Failed to detach network\r\n");
			break;
		}
		USARTManager_sendATCommand(timeout - elapsedTime, 1, "AT+COPS=2\r\n");
	}
	USARTManager_printUsbWait("Network detached\r\n");
	return functionResult;
}

static void printNetworkCheckAlarm()
{
	USARTManager_printUsbWait("Network check alarm : ");
	RTCManager_printTimeFromTimestamp(_networkCheckAlarm.data.timestamp);
	USARTManager_printUsbWait("\r\n");
}

// Check if the GSM Is regitered, and to the right network.
static E_NETWORK_REGISTRATION_STATUS getNetworkRegistrationStatus()
{
	// Check if registered to network
	if(USARTManager_sendATCommand(LONG_WAIT, 1, "AT+CREG?\r\n") == SERIAL_ANSWER_OK)
	{
		return parseNetworkRegistration(gsm_buf);
	}
	
	return NETWORK_REGISTRATION_UNKNOWN;
}

static bool isNetworkAttached()
{
	// Check if registered to prefered network
	if(USARTManager_sendATCommand(LONG_WAIT, 1, "AT+COPS?\r\n") == SERIAL_ANSWER_OK)
	{
		// Format : +COPS: <mode>[,<format>,<oper>]
		char* start = strstr(gsm_buf, "+COPS:");
		if(start == NULL)
		{
			return false;
		}
		char* delimiters = " ,\r\n\"";
		char* token = strtok(start, delimiters);
		if(token == NULL)
		{
			return false;
		}
		token = strtok(NULL, delimiters); // go past COPS mode
		if(token == NULL)
		{
			return false;
		}
		token = strtok(NULL, delimiters); // go pas COPS format
		if(token == NULL)
		{
			return false;
		}
		char* copsOperator = strtok(NULL, delimiters);
		if(copsOperator == NULL)
		{
			return false;
		}
		// copy last checked network
		strcpy(_lastCheckedNetwork, copsOperator);
		_lastCheckedNetwork[NETWORK_NAME_MAX_LENGTH-1] = 0;
		if(getPreferedOperatorIndex(copsOperator) != -1){
			return true;
		}
	}

	return false;
}

/* Checks if the currently attached network is a specific one */
static bool isSpecificNetworkAttached(const char* networkIdentifier)
{
	// Check if registered to prefered network
	if(USARTManager_sendATCommand(LONG_WAIT, 1, "AT+COPS?\r\n") == SERIAL_ANSWER_OK)
	{
		// Format : +COPS: <mode>[,<format>,<oper>]
		char* start = strstr(gsm_buf, "+COPS:");
		if(start == NULL)
		{
			return false;
		}
		char* delimiters = " ,\r\n\"";
		char* token = strtok(start, delimiters);
		if(token == NULL)
		{
			return false;
		}
		token = strtok(NULL, delimiters); // go past COPS mode
		if(token == NULL)
		{
			return false;
		}
		token = strtok(NULL, delimiters); // go pas COPS format
		if(token == NULL)
		{
			return false;
		}
		char* copsOperator = strtok(NULL, delimiters);
		if(copsOperator == NULL)
		{
			return false;
		}
		// copy last checked network
		strcpy(_lastCheckedNetwork, copsOperator);
		_lastCheckedNetwork[NETWORK_NAME_MAX_LENGTH-1] = 0;
		if(strcmp(copsOperator,networkIdentifier) == 0){
			return true;
		}
	}
	return false;
}


/* Returns the index of the prefered operator. Returns -1 if it wasn't found. */
static int getPreferedOperatorIndex(char* operator)
{
	uint32_t i = 0;

	for(i=0;i<NETWORK_OPERATORS_COUNT;i++)
	{
		if(strcmp(_networkOperators[i], operator) == 0)
		{
			return i;
		}
	}

	return -1;
}

static bool isNetworkDetached()
{
	if(USARTManager_sendATCommand(VERY_LONG_WAIT, 1, "AT+CREG?\r\n") == SERIAL_ANSWER_OK)
	{
		volatile E_NETWORK_REGISTRATION_STATUS networkRegistrationStatus = parseNetworkRegistration(gsm_buf);
		if(networkRegistrationStatus == NETWORK_REGISTRATION_NOT_REGISTERED
		|| networkRegistrationStatus == NETWORK_REGISTRATION_DENIED)
		{
			return true;
		}
	}
	return false;
}

static uint8_t parseNetworkRegistration(char* responseToParse)
{
	char* start = strstr(responseToParse, "+CREG:");
	strtok(start, ": ,\r\n");
	strtok(NULL, ": ,\r\n");
	char* networkRegistrationStatusStr = strtok(NULL, ": ,\r\n");
	uint8_t networkRegistrationStatus = atoi(networkRegistrationStatusStr);
	return networkRegistrationStatus;
}

static void networkCheckAlarmCallback(struct calendar_descriptor *const calendar)
{
	UNUSED(calendar);
	maskUtilities_setRequestMaskBitsFromISR(REQUEST_BIT_CHECK_NETWORK_STATUS);
}

// TODO : set network check alarm request bit at beginnig of GSM Start ?
static void setNetworkCheckAlarm(bool attachmentWorked)
{
	removeNetworkCheckAlarm();
	USARTManager_printUsbWait("Network check alarm :");
	uint32_t networkCheckPeriod;
	if(attachmentWorked)
	{
		networkCheckPeriod = NETWORK_CHECK_PERIOD;
	}
	else
	{
		networkCheckPeriod = NETWORK_SEARCH_PERIOD;
	}
	RTCManager_setDelayedCallback(networkCheckPeriod, & _networkCheckAlarm, networkCheckAlarmCallback);
}

static void removeNetworkCheckAlarm()
{
	if(RTCManager_removeAlarm(&_networkCheckAlarm))
	{
		USARTManager_printUsbWait("Removing network check alarm\r\n");
	}
}

static bool isNetworkCheckAlarmScheduled()
{
	return RTCManager_isAlarmScheduled(&_networkCheckAlarm);
}