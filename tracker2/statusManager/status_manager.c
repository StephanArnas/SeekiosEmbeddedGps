#include <statusManager/status_manager.h>

static void initParameter(ModeParameters* parameter);
static void copyConfig(ModeConfig* dest, ModeConfig* src);
static uint8_t buildLastParsedParameters(char *parameterStr, E_MODE_STATUS status);
static uint8_t messageParser(E_MODE_STATUS status, char* message);
static char* isolateMessageParameters(char* message);
static E_MODE_STATUS parseSeekiosStatus(char* message);
static void printRunningMode(void);
static void parseFunctionalityMessage(const char* message);
static bool isFunctionalityMessage(const char* message);
static bool isModeMessage(char* message);
static bool isStateChangeMessage(char* message);
static bool isDateConfigMessage(char* message);
static bool isPowerSavingConfigurationMessage(char* message);
static bool isAdminMessage(const char* message);
static void parseAdminMessage(char* message);

static ModeConfig _lastParsedMode;	// the last parsed config we received
static ModeConfig _runningMode;		// the currently running config

static SemaphoreHandle_t _lastParsedModeSemaphore;
static SemaphoreHandle_t _runningModeSemaphore;

static bool _isSOSAuthorized;

void statusManager_initStatusManager(){
	
	statusManager_initLastParsedMode();
	statusManager_initRunningMode();
	_isSOSAuthorized = false;
	_lastParsedModeSemaphore = xSemaphoreCreateRecursiveMutex();
	_runningModeSemaphore = xSemaphoreCreateRecursiveMutex();
}

void statusManager_initLastParsedMode()
{
	_lastParsedMode.modeParameters.modeID = 0;
	_lastParsedMode.modeStatus.status= MODE_STATUS_WAITING;
	_lastParsedMode.modeStatus.state = 0;
	_lastParsedMode.powerSavingConfig.isPowerSavingEnabled = false;
	_lastParsedMode.powerSavingConfig.powerSavingCultureHoursOffset = 0;
	initParameter(&(_lastParsedMode.modeParameters));
}

void statusManager_initRunningMode(){
	_runningMode.modeParameters.modeID = 0;
	_runningMode.modeStatus.status= MODE_STATUS_NONE;
	_runningMode.modeStatus.state = 0;
	_runningMode.powerSavingConfig.isPowerSavingEnabled = false;
	_runningMode.powerSavingConfig.powerSavingCultureHoursOffset = 0;
	initParameter(&(_runningMode.modeParameters));
}

void statusManager_getLastParsedStatus(ModeStatus* seekiosStatus){
	seekiosStatus->status = MODE_STATUS_NONE;
	seekiosStatus->state	= 0;
	if(xSemaphoreTakeRecursive(_lastParsedModeSemaphore, LONG_WAIT)==pdPASS)
	{
		seekiosStatus->status = _lastParsedMode.modeStatus.status;
		seekiosStatus->state = _lastParsedMode.modeStatus.state;
		xSemaphoreGiveRecursive(_lastParsedModeSemaphore);
	}
}

uint8_t statusManager_getLastParsedStatusState(){
	uint8_t state = 0;
	if(xSemaphoreTakeRecursive(_lastParsedModeSemaphore, LONG_WAIT)==pdPASS)
	{
		state = _lastParsedMode.modeStatus.state;
		xSemaphoreGiveRecursive(_lastParsedModeSemaphore);
	}

	return state;
}

bool statusManager_getLastParsedIsPowerSavingEnabled()
{
	bool isPowerSavingEnabled = false;
	if(xSemaphoreTakeRecursive(_lastParsedModeSemaphore, LONG_WAIT)==pdPASS)
	{
		isPowerSavingEnabled = _lastParsedMode.powerSavingConfig.isPowerSavingEnabled;
		xSemaphoreGiveRecursive(_lastParsedModeSemaphore);
	}

	return isPowerSavingEnabled;
}

void statusManager_getLastParsedPowerSavingConfig(PowerSavingConfig *configPtr)
{
	configPtr->isPowerSavingEnabled = false;
	configPtr->powerSavingCultureHoursOffset = 0;
	if(xSemaphoreTakeRecursive(_lastParsedModeSemaphore, LONG_WAIT)==pdPASS)
	{
		configPtr->isPowerSavingEnabled = _lastParsedMode.powerSavingConfig.isPowerSavingEnabled;
		configPtr->powerSavingCultureHoursOffset = _lastParsedMode.powerSavingConfig.powerSavingCultureHoursOffset;
		xSemaphoreGiveRecursive(_lastParsedModeSemaphore);
	}
}

void statusManager_getRunningStatus(ModeStatus* seekiosStatus){
	seekiosStatus->status = MODE_STATUS_NONE;
	seekiosStatus->state	= 0;
	if(xSemaphoreTakeRecursive(_runningModeSemaphore, LONG_WAIT)==pdPASS)
	{
		seekiosStatus->status = _runningMode.modeStatus.status;
		seekiosStatus->state = _runningMode.modeStatus.state;
		xSemaphoreGiveRecursive(_runningModeSemaphore);
	}
}

uint8_t statusManager_getRunningStatusState(){
	uint8_t state = 0;
	if(xSemaphoreTakeRecursive(_runningModeSemaphore, LONG_WAIT)==pdPASS)
	{
		state = _runningMode.modeStatus.state;
		xSemaphoreGiveRecursive(_runningModeSemaphore);
	}
	return state;
}

uint16_t statusManager_getRunningRefreshRate(){
	uint16_t refreshRate = 1;
	if(xSemaphoreTakeRecursive(_runningModeSemaphore, LONG_WAIT)==pdPASS)
	{
		refreshRate = _runningMode.modeParameters.refreshRate;
		xSemaphoreGiveRecursive(_runningModeSemaphore);
	}
	return refreshRate;
}

bool statusManager_getRunningIsPowerSavingEnabled()
{
	bool isPowerSavingEnabled = false;
	if(xSemaphoreTakeRecursive(_runningModeSemaphore, LONG_WAIT)==pdPASS)
	{
		isPowerSavingEnabled = _runningMode.powerSavingConfig.isPowerSavingEnabled;
		xSemaphoreGiveRecursive(_runningModeSemaphore);
	}
	return isPowerSavingEnabled;
}

void statusManager_getRunningPowerSavingConfig(PowerSavingConfig *configPtr)
{
	configPtr->isPowerSavingEnabled = false;
	configPtr->powerSavingCultureHoursOffset = 0;
	if(xSemaphoreTakeRecursive(_runningModeSemaphore, LONG_WAIT)==pdPASS)
	{
		configPtr->isPowerSavingEnabled = _runningMode.powerSavingConfig.isPowerSavingEnabled;
		configPtr->powerSavingCultureHoursOffset = _runningMode.powerSavingConfig.powerSavingCultureHoursOffset;
		xSemaphoreGiveRecursive(_runningModeSemaphore);
	}
}


/* Stores the coordinates of the running config in the array passed as parameters
and returns the number of coordinates copied */
uint8_t statusManager_getRunningConfigCoordinates(Coordinate coordinates[NB_MAX_COORDINATES]){
	uint8_t nbCoordinates = 0;
	if(xSemaphoreTakeRecursive(_runningModeSemaphore, LONG_WAIT)==pdPASS)
	{
		memcpy(coordinates, _runningMode.modeParameters.coordinates, sizeof(Coordinate)*NB_MAX_COORDINATES);
		nbCoordinates = _runningMode.modeParameters.nbCoordinates;
		xSemaphoreGiveRecursive(_runningModeSemaphore);
	}
	return nbCoordinates;
}

static void printRunningMode()
{
	if(xSemaphoreTakeRecursive(_runningModeSemaphore, LONG_WAIT)==pdPASS)
	{
		USARTManager_printUsbWait("Running mode params :\r\n");
		uint8_t buff[10];
		bool hasParams;
		hasParams = false;
		switch(_runningMode.modeStatus.status)
		{
			case MODE_STATUS_TRACKING:
			USARTManager_printUsbWait("\tTracking\r\n");
			stringHelper_intToString(_runningMode.modeParameters.modeID, buff);
			hasParams = true;
			break;
			case MODE_STATUS_DONT_MOVE:
			USARTManager_printUsbWait("\tDon't move\r\n");
			stringHelper_intToString(_runningMode.modeParameters.modeID, buff);
			hasParams = true;
			break;
			case MODE_STATUS_ZONE:
			USARTManager_printUsbWait("\tZone");
			stringHelper_intToString(_runningMode.modeParameters.modeID, buff);
			hasParams = true;
			break;
			case MODE_STATUS_NONE:
			USARTManager_printUsbWait("\tNone\r\n");
			break;
			case MODE_STATUS_WAITING:
			USARTManager_printUsbWait("\tWaiting\r\n");
			break;
			default:
			USARTManager_printUsbWait("\t No infos on mode.\r\n");
			break;
		}
		
		if(hasParams)
		{
			USARTManager_printUsbWait("\tMode ID: ");
			USARTManager_printUsbWait(buff);
			USARTManager_printUsbWait("\r\n\tState : ");
			stringHelper_intToString(_runningMode.modeStatus.state, buff);
			USARTManager_printUsbWait(buff);
			USARTManager_printUsbWait("\r\n\tIsPowerSavingActivated : ");
			USARTManager_printUsbWait(_runningMode.powerSavingConfig.isPowerSavingEnabled ? "True" : "False" );
			USARTManager_printUsbWait("\r\n\tPower Saving Hour Offset : ");
			stringHelper_intToString(_runningMode.powerSavingConfig.powerSavingCultureHoursOffset, buff);
			USARTManager_printUsbWait(buff);
			USARTManager_printUsbWait("\r\n\tState : ");
			stringHelper_intToString(_runningMode.modeStatus.state, buff);
			USARTManager_printUsbWait(buff);
			USARTManager_printUsbWait("\r\n\tRefresh rate: ");
			stringHelper_intToString(_runningMode.modeParameters.refreshRate, buff);
			USARTManager_printUsbWait(buff);
			USARTManager_printUsbWait("\r\n");
		}

		
		xSemaphoreGiveRecursive(_runningModeSemaphore);
	}
}

/* Returns true of the last parsed mode is newer than the currently running mode */
bool statusManager_isLastParsedModeNew()
{
	uint32_t lastParsedModeID	= 0;
	uint32_t runningModeID		= 0;
	
	if(xSemaphoreTakeRecursive(_lastParsedModeSemaphore, LONG_WAIT)==pdPASS)
	{
		lastParsedModeID = _lastParsedMode.modeParameters.modeID;
		xSemaphoreGiveRecursive(_lastParsedModeSemaphore);
	}
	
	if(xSemaphoreTakeRecursive(_runningModeSemaphore, LONG_WAIT)==pdPASS)
	{
		runningModeID = _runningMode.modeParameters.modeID;
		xSemaphoreGiveRecursive(_runningModeSemaphore);
	}
	
	if(lastParsedModeID != 0 && (lastParsedModeID != runningModeID))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/* A mode is canceled if the last parsed config is the NONE or WAITING mode status */
bool statusManager_isModeCanceled()
{
	if(xSemaphoreTakeRecursive(_lastParsedModeSemaphore, LONG_WAIT)==pdPASS)
	{
		E_MODE_STATUS lastParsedModeStatus = _lastParsedMode.modeStatus.status;
		xSemaphoreGiveRecursive(_lastParsedModeSemaphore);
		return (lastParsedModeStatus == MODE_STATUS_NONE || lastParsedModeStatus == MODE_STATUS_WAITING);
	}
	else
	{
		return false;
	}
}

static void initParameter(ModeParameters* parameter){
	uint8_t i = 0;
	parameter->refreshRate = 0;
	parameter->nbCoordinates = 0;
	parameter->modeID = 0;
	parameter->useExtraOptions = 0;
	for (i = 0; i < NB_MAX_COORDINATES; i++)
	{
		parameter->coordinates[i].lat = 0.0;
		parameter->coordinates[i].lon = 0.0;
	}
}

void statusManager_setRunningConfig(ModeConfig* configPtr){
	if(xSemaphoreTakeRecursive(_runningModeSemaphore, (TickType_t) 1000) == pdTRUE)
	{
		copyConfig(&_runningMode, configPtr);
		xSemaphoreGiveRecursive(_runningModeSemaphore);
	}
}

void statusManager_setRunningConfigStatusState(uint8_t state){
	if(xSemaphoreTakeRecursive(_runningModeSemaphore, (TickType_t) 1000) == pdTRUE)
	{
		_runningMode.modeStatus.state = state;
		xSemaphoreGiveRecursive(_runningModeSemaphore);
	}
}

void statusManager_setRunningIsPowerSavingEnabled(bool isPowerSavingEnabled)
{
	if(xSemaphoreTakeRecursive(_runningModeSemaphore, (TickType_t) 1000) == pdTRUE)
	{
		_runningMode.powerSavingConfig.isPowerSavingEnabled = isPowerSavingEnabled;
		xSemaphoreGiveRecursive(_runningModeSemaphore);
	}
}

void statusManager_getRunningConfig(ModeConfig* configPtr){
	if(xSemaphoreTakeRecursive(_runningModeSemaphore, (TickType_t) 1000) == pdTRUE)
	{
		copyConfig(configPtr, &_runningMode);
		xSemaphoreGiveRecursive(_runningModeSemaphore);
	}
}

uint32_t statusManager_getRunningConfigModeId(){
	uint32_t modeId = 0;
	if(xSemaphoreTakeRecursive(_runningModeSemaphore, (TickType_t) 1000) == pdTRUE)
	{
		modeId = _runningMode.modeParameters.modeID;
		xSemaphoreGiveRecursive(_runningModeSemaphore);
	}
	return modeId;
}

/* Takes the values of the lastParsedConfig and use it as the runningConfig */
void statusManager_useLastParsedConfigAsRunningConfig(){
	if(xSemaphoreTakeRecursive( _lastParsedModeSemaphore, ( TickType_t ) 1000 ) == pdTRUE)
	{
		statusManager_setRunningConfig(&_lastParsedMode);
		xSemaphoreGiveRecursive(_lastParsedModeSemaphore);
	}
}

/* Clears the request mode bits, and sets the running config as the last parsed config if necessary */
void statusManager_setRunningConfigAndClearRequestModeConfigBits()
{
	if(maskUtilities_useLastParsedConfig())
	{
		maskUtilities_clearRequestMaskBits(REQUEST_BIT_START_MODE_FROM_LPC);
		statusManager_useLastParsedConfigAsRunningConfig();
		printRunningMode();
	}
	else
	{
		maskUtilities_clearRequestMaskBits(REQUEST_BIT_START_MODE_FROM_RC);
	}
}



/* Copie les valeurs de la last parsed config dans une struct passée en paramètre */
void statusManager_getLastParsedConfig(ModeConfig* destPtr){
	if(xSemaphoreTakeRecursive( _lastParsedModeSemaphore, ( TickType_t ) 1000 ) == pdTRUE ){
		copyConfig(destPtr,&_lastParsedMode);
		xSemaphoreGiveRecursive(_lastParsedModeSemaphore);
	}
}

static void copyConfig(ModeConfig* dest, ModeConfig* src){
	dest->powerSavingConfig.isPowerSavingEnabled = src->powerSavingConfig.isPowerSavingEnabled;
	dest->powerSavingConfig.powerSavingCultureHoursOffset = src->powerSavingConfig.powerSavingCultureHoursOffset;
	dest->modeParameters.modeID = src->modeParameters.modeID;
	dest->modeParameters.refreshRate = src->modeParameters.refreshRate;
	dest->modeStatus = src->modeStatus;
	dest->modeParameters.refreshRate = src->modeParameters.refreshRate;
	dest->modeStatus.state = src->modeStatus.state;
	memcpy(dest->modeParameters.coordinates,src->modeParameters.coordinates,NB_MAX_COORDINATES*sizeof(Coordinate));
	dest->modeParameters.nbCoordinates = src->modeParameters.nbCoordinates;
}

/* Met à jour les paramètres et les statuts du last parsed config et anciens en fonction
d'un message passé en paramètre */
void statusManager_updateLastParsedConfigFromMessage(char* message)
{
	E_MODE_STATUS parsedStatus = parseSeekiosStatus(message);
	if(	parsedStatus != MODE_STATUS_NONE
	&& (xSemaphoreTakeRecursive( _lastParsedModeSemaphore, ( TickType_t ) 1000 ) == pdTRUE)
	)
	{
		if(xSemaphoreTakeRecursive( _runningModeSemaphore, ( TickType_t ) 1000 ) == pdTRUE)
		{
			if(parsedStatus == MODE_STATUS_ON_DEMAND){
				maskUtilities_setRequestMaskBits(REQUEST_BIT_ON_DEMAND);
			}
			else
			{
				if(isModeMessage(message))	{
					statusManager_initLastParsedMode();
					_lastParsedMode.modeStatus.status= parsedStatus;
					if(messageParser(_lastParsedMode.modeStatus.status, message)==FUNCTION_SUCCESS)
					{
						maskUtilities_setRequestMaskBits(REQUEST_BIT_START_MODE_FROM_LPC);
					}
				}
			}
			xSemaphoreGiveRecursive(_runningModeSemaphore);
		}
		xSemaphoreGiveRecursive(_lastParsedModeSemaphore);
	}
}

/* Parse une string pour en tirer le statut actuel du seekios */
static E_MODE_STATUS parseSeekiosStatus(char *message)
{
	if(!isModeMessage(message) && !isStateChangeMessage(message))	return MODE_STATUS_NONE;
	
	char statusSubstring[2];
	statusSubstring[0] = message[2];
	statusSubstring[1] = message[3];
	
	if (strstr(statusSubstring,"01"))	return MODE_STATUS_WAITING;
	if (strstr(statusSubstring,"02"))	return MODE_STATUS_ON_DEMAND;
	if (strstr(statusSubstring,"03"))	return MODE_STATUS_TRACKING;
	if (strstr(statusSubstring,"04"))	return MODE_STATUS_DONT_MOVE;
	if (strstr(statusSubstring,"05"))	return MODE_STATUS_ZONE;
	if (strstr(statusSubstring,"08"))	return -1;
	
	return MODE_STATUS_NONE;
}

// TODO : define the different Admin messages
// A01 corresponds to the full-functionalities tests
void parseAdminMessage(char* message){
	char msgNumStr[3];
	msgNumStr[0] = message[2];
	msgNumStr[1] = message[3];
	msgNumStr[2] = 0;
	uint8_t msgNum = atoi(msgNumStr);
	uint8_t admParam = 3;
	char* parameterStr;
	switch(msgNum){
		case 1:
		maskUtilities_setRequestMaskBits(REQUEST_BIT_TEST_FUNCTIONALITIES);
		break;
		case 2:
		parameterStr = isolateMessageParameters(message);
		admParam = atoi(parameterStr);
		if(admParam == 0)
		{
			_isSOSAuthorized = 0;
		}
		else if (admParam == 1)
		{
			_isSOSAuthorized = 1;
		}
		break;
		case 3:
		parameterStr = isolateMessageParameters(message);
		admParam = atoi(parameterStr);
		if(admParam == 0) // Deactivate USB monitor
		{
			maskUtilities_clearRequestMaskBits(REQUEST_BIT_USB_DEBUGGING);
		}
		else if (admParam == 1) // Activate USB monitor
		{
			maskUtilities_setRequestMaskBits(REQUEST_BIT_USB_DEBUGGING);
		}
		break;
		default:
		break;
	}
}

/*
parse les messages liés aux changements de configuration du seekios
*/
static uint8_t messageParser(E_MODE_STATUS status, char* message)
{
	// message had wrong format
	if(!isModeMessage(message)) return FUNCTION_FAILURE;
	
	message = isolateMessageParameters(message);
	
	// at this point, if strlen(message) = 0, the mode should be M01
	if(strlen(message) == 0) return FUNCTION_SUCCESS;
	
	return buildLastParsedParameters(message, status);
}

/* Isole le contenu du message en enlevant le #M0X (ou #S0X) et le & */
static char* isolateMessageParameters(char* message){
	message[strlen(message) - 1] = '\0';
	return message + 4;
}

void parseZoneMode(char *msg,int nbPoints, Coordinate *coordinates)
{
	char *token = NULL;
	uint8_t itemCounter=0;
	//bool isEven=false;
	bool isEven=false;
	//initialisation des coordonnées
	for(uint8_t i=0;i<nbPoints;i++)
	{
		coordinates[i].lat=0.0;
		coordinates[i].lon=0.0;
	}
	
	//Parsage du message contenant uniquement "lat 1:lon 1;...;lat n:lon n" et stockage des valeurs dans la structure Coordinate
	for (token = strtok(msg, ":;"); token != NULL; token = strtok(NULL, ":;"))
	{
		if(isEven)
		{
			coordinates[itemCounter].lon=strtod(token,NULL);
			itemCounter++;
			isEven=false;
		}
		else
		{
			coordinates[itemCounter].lat=strtod(token,NULL);
			isEven=true;
		}
	}
}

/*
Construit les paramètre avec le message de changement de statut
*/
static uint8_t buildLastParsedParameters(char *parameterStr, E_MODE_STATUS status){

	char* idModeString = strtok(parameterStr, ";");
	_lastParsedMode.modeParameters.modeID = atoi(idModeString);
	parameterStr = parameterStr + strlen(idModeString)+1; // Go past mode ID and the null terminating char
	
	char* modeState = strtok(parameterStr, ";");
	uint8_t isRAS = atoi(modeState);
	parameterStr = parameterStr + strlen(modeState)+1; // Go past mode state and the null terminating char
	
	char* powerSavingEnabledStr = strtok(parameterStr, ";");
	_lastParsedMode.powerSavingConfig.isPowerSavingEnabled = atoi(powerSavingEnabledStr) == 1;
	parameterStr = parameterStr + strlen(powerSavingEnabledStr)+1; // Go past mode state and the null terminating char

	char* cultureHoursOffsetStr = strtok(parameterStr, ";");
	_lastParsedMode.powerSavingConfig.powerSavingCultureHoursOffset = atoi(cultureHoursOffsetStr);
	parameterStr = parameterStr + strlen(cultureHoursOffsetStr)+1; // Go past mode state and the null terminating char
		
	if(status == MODE_STATUS_TRACKING)
	{
		_lastParsedMode.modeParameters.refreshRate = atoi(parameterStr);
	}
	if(status == MODE_STATUS_DONT_MOVE)
	{
		_lastParsedMode.modeParameters.refreshRate = atoi(parameterStr);
		if(isRAS){
			_lastParsedMode.modeStatus.state = DONT_MOVE_STATE_RAS;
		}
		else if(_lastParsedMode.modeParameters.refreshRate > 0){
			_lastParsedMode.modeStatus.state = DONT_MOVE_STATE_TRACKING;
		}
		else{
			_lastParsedMode.modeStatus.state = DONT_MOVE_STATE_SUSPEND;
		}
	}
	else if(status == MODE_STATUS_ZONE)
	{
		uint8_t pointsCounter=0;
		char* parameter = strtok(parameterStr, ";");
		if(parameter == NULL) return FUNCTION_FAILURE;
		_lastParsedMode.modeParameters.refreshRate = atoi(parameter);
		parameterStr += strlen(parameter)+1;

		for (uint16_t i = 0; i < strlen(parameterStr); i++)
		{
			if(parameterStr[i]==':')
			{
				pointsCounter++;
			}
		}
		_lastParsedMode.modeParameters.nbCoordinates=pointsCounter;
		
		if(isRAS){
			_lastParsedMode.modeStatus.state = ZONE_STATE_CHECK_POSITION;
		}
		else if(_lastParsedMode.modeParameters.refreshRate > 0){
			_lastParsedMode.modeStatus.state = ZONE_STATE_TRACKING;
		}
		else{
			_lastParsedMode.modeStatus.state = ZONE_STATE_SUSPEND;
		}
		
		parseZoneMode(parameterStr,_lastParsedMode.modeParameters.nbCoordinates,_lastParsedMode.modeParameters.coordinates);
	}
	return FUNCTION_SUCCESS;
}

/*
Checks the rightness of the format of a message : #D...&
- at least 3 chars
- starting by #D, finishing by &
*/
bool isDateConfigMessage(char* message){
	int msglength = strlen(message);
	if(msglength < 3){
		return false;
	}
	if(message[0] != '#'
	|| message[1] != 'D'
	|| message[msglength-1] != '&'){
		return false;
	}
	
	return true;
}

/*
Checks the rightness of the format of a message : #M0X...&
- at least 5 chars
- starting by #MXX where XX is a number with max 2 chars (01, 02, 12, 99...), finishing by &
TODO : check that XX is a number within the right boundaries.
*/
bool isModeMessage(char* message){
	int msglength = strlen(message);
	if(msglength < 5){
		return false;
	}
	if(message[0] != '#'
	|| message[1] != 'M'
	|| message[msglength-1] != '&'){
		return false;
	}
	
	return true;
}

/*
Checks the rightness of the format of a message : #S0X...&
- at least 5 chars
- starting by #SXX where XX is a number with max 2 chars (01, 02, 12, 99...), finishing by &
TODO : check that XX is a number within the right boundaries.
*/
bool isStateChangeMessage(char* message){
	int msglength = strlen(message);
	if(msglength < 5){
		return false;
	}
	if(message[0] != '#'
	|| message[1] != 'S'
	|| message[msglength-1] != '&'){
		return false;
	}
	
	return true;
}

/*
Checks the rightness of the format of a message : #PX...&
- at least 4 chars
- starting by #PX where X is a number between 0 or 1, finishing by &
TODO : check that X is a number within the right boundaries.
*/
bool isPowerSavingConfigurationMessage(char* message){
	int msglength = strlen(message);
	if(msglength < 4){
		return false;
	}
	if(message[0] != '#'
	|| message[1] != 'P'
	|| message[msglength-1] != '&'){
		return false;
	}
	
	return true;
}

char* statusManager_getCurrentStatusString(char* statusBuff){
	switch(_runningMode.modeStatus.status){
		case MODE_STATUS_DONT_MOVE     : strcpy(statusBuff, "[  DONT_MOVE   ] "); break;
		case MODE_STATUS_NONE          : strcpy(statusBuff, "[     NONE     ] ");	break;
		case MODE_STATUS_ON_DEMAND     : strcpy(statusBuff, "[  ON_DEMAND   ] ");	break;
		case MODE_STATUS_TRACKING      : strcpy(statusBuff, "[   TRACKING   ] ");	break;
		case MODE_STATUS_WAITING       : strcpy(statusBuff, "[   WAITING    ] ");	break;
		case MODE_STATUS_ZONE          : strcpy(statusBuff, "[    ZONE      ] ");	break;
		case MODE_STATUS_SOS			  : strcpy(statusBuff, "[     SOS      ] "); break;
	}
	return statusBuff;
}

/* Checks if the message is of Admin type
Admin type message : #AXX& where XX is a 2 digits number*/
bool isAdminMessage(const char* message){
	int msglength = strlen(message);
	if(msglength < 5){
		return false;
	}
	if(message[0] != '#'
	|| message[1] != 'A'
	|| message[msglength-1] != '&'){
		return false;
	}
	
	return true;
}

bool isFunctionalityMessage(const char* message)
{
	int msglength = strlen(message);
	if(msglength < 5){
		return false;
	}
	if(message[0] != '#'
	|| message[1] != 'F'
	|| message[msglength-1] != '&'){
		return false;
	}
	
	return true;
}

void parseFunctionalityMessage(const char* message)
{
	char msgNumStr[3];
	msgNumStr[0] = message[2];
	msgNumStr[1] = message[3];
	msgNumStr[2] = 0;
	uint8_t msgNum = atoi(msgNumStr);

	switch(msgNum){
		case 1: // battery refresh message
		// the handle of this message is actually already handled : the getSeekiosInstruction answers directly to the battery update request
		break;
		default:
		break;
	}
}

bool statusManager_isSOSAuthorized()
{
	return _isSOSAuthorized;
}

/* Updates the GPRS Expiration time depending on the current Seekios configuration */
uint8_t statusManager_getGPRSExpirationTime()
{
	ModeStatus runningStatus;
	statusManager_getRunningStatus(&runningStatus);
	uint16_t refreshRate = statusManager_getRunningRefreshRate();

	if((runningStatus.status == MODE_STATUS_TRACKING
	|| (runningStatus.status == MODE_STATUS_ZONE && runningStatus.state == ZONE_STATE_TRACKING)
	|| (runningStatus.status == MODE_STATUS_DONT_MOVE && runningStatus.state == DONT_MOVE_STATE_TRACKING))
	&& refreshRate <= 15)
	{
		return GPRS_EXPIRATION_TIME_16_MIN;
	}
	else
	{
		return GPRS_EXPIRATION_TIME_5_MIN;
	}
}

/* Process le contenu du message : trouve son type (admin, mode, state, config message etc.)*/
void statusManager_processMessage(char* message){

	if(isModeMessage(message) || isStateChangeMessage(message)){
		statusManager_updateLastParsedConfigFromMessage(message);
	}
	else if(isDateConfigMessage(message)){
		if(RTCManager_setCalendarFromMessage(message))
		{
			maskUtilities_setRequestMaskBits(REQUEST_BIT_UPDATE_CALENDAR_PROCESSES);
		}
	}
	else if (isAdminMessage(message))
	{
		parseAdminMessage(message);
	}
	else if(isFunctionalityMessage(message))
	{
		parseFunctionalityMessage(message);
	}
}
