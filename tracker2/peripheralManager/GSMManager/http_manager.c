#include <peripheralManager/GSMManager/http_manager.h>

static void clearStatusMask(void);
static void printSessionExpirationAlarm(void);
static uint8_t startHTTP(void);
static void scheduleHttpSessionExpirationAlarm(uint8_t expirationTime);
static void removeHttpSessionExpirationAlarm(void);
static bool isSessionExpirationAlarmScheduled(void);
static uint8_t stopHTTP(void);
static uint8_t configureHTTP(void);
static void httpSessionExpirationCallback(struct calendar_descriptor *const calendar);
static uint8_t parseGPRSRegistration(char* responseToParse);
static bool isHttpConfigured(void);
static E_BEARER_STATUS getBearerStatus(void);
static uint8_t parseBearerStatusFromGSMBuf(void);
static bool isGPRSAttached(void);
static bool isGPRSDetached(void);
static bool attachGPRSService(uint32_t timeout);
static bool detachGPRSService(uint32_t timeout);
static bool openBearer(uint32_t timeout);
static bool closeBearer(uint32_t timeout);
static E_HTTP_REQUEST_STATUS httpGET(char* url, char* httpMessage);
static E_HTTP_REQUEST_STATUS getMessageLength(char* msgLength);
static void httpReadTimerCallback(void);
static E_HTTP_REQUEST_STATUS readHttpMessage(const char* msgLength, char* httpMessage);

static EventGroupHandle_t _gprsStatusMaskHandle;
static EventGroupHandle_t _pendingRequestsMaskHandle;
static struct calendar_alarm _httpSessionExpirationAlarm;

void httpManager_init(HttpService* httpServicePtr)
{
	_pendingRequestsMaskHandle = xEventGroupCreate();
	_gprsStatusMaskHandle = xEventGroupCreate();
	clearStatusMask();
	httpServicePtr->printSessionExpirationAlarm = printSessionExpirationAlarm;
	httpServicePtr->startHTTP = startHTTP;
	httpServicePtr->scheduleHttpSessionExpirationAlarm = scheduleHttpSessionExpirationAlarm;
	httpServicePtr->removeHttpSessionExpirationAlarm = removeHttpSessionExpirationAlarm;
	httpServicePtr->isSessionExpirationAlarmScheduled = isSessionExpirationAlarmScheduled;
	httpServicePtr->stopHTTP = stopHTTP;
	httpServicePtr->httpGET = httpGET;
}

static void clearStatusMask()
{
	xEventGroupClearBits(_gprsStatusMaskHandle, GPRS_STATUS_BIT_HTTP_CONFIGURED & GPRS_STATUS_BIT_HTTP_INITIALIZED);
}

static void printSessionExpirationAlarm()
{
	USARTManager_printUsbWait("Session expiration scheduled : ");
	RTCManager_printTimeFromTimestamp(_httpSessionExpirationAlarm.data.timestamp);
	USARTManager_printUsbWait("\r\n");
}

static uint8_t startHTTP()
{	
	if(!isHttpConfigured()){
		if(configureHTTP() == FUNCTION_FAILURE){
			return FUNCTION_FAILURE;
		}
	}
	
	volatile UBaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
	UNUSED(wm);
	if(isHttpConfigured())
	{
		EventBits_t gprsMask = xEventGroupGetBits(_gprsStatusMaskHandle);

		if(!attachGPRSService(LONG_WAIT)){
			return FUNCTION_FAILURE;
		}

		if(!openBearer(LONG_WAIT))
		{
			return FUNCTION_FAILURE;
		}
		
		if(!(gprsMask & GPRS_STATUS_BIT_HTTP_INITIALIZED))
		{
			if(!USARTManager_repeatATCommandUntilSuccess(2, "AT+HTTPINIT\r\n", LONG_WAIT)){
				return FUNCTION_FAILURE;
			}
			else
			{
				xEventGroupSetBits(_gprsStatusMaskHandle, GPRS_STATUS_BIT_HTTP_INITIALIZED);
				if(!USARTManager_repeatATCommandUntilSuccess(2, "AT+HTTPPARA=\"CID\",1\r\n", LONG_WAIT)){
					return FUNCTION_FAILURE;
				}

				if(!USARTManager_repeatATCommandUntilSuccess(2, "AT+HTTPPARA=\"TIMEOUT\",45\r\n", LONG_WAIT)){
					return FUNCTION_FAILURE;
				}
			}
		}
	}
	USARTManager_printUsbWait("HTTP Session OPEN !\r\n");
	return FUNCTION_SUCCESS;
}

/* Updates the current GPRS expiration time. It takes as parameter the
current Seekios Status (the GPRS Expiration time depends on the Running status */
static void scheduleHttpSessionExpirationAlarm(uint8_t expirationTime){
	removeHttpSessionExpirationAlarm();
	USARTManager_printUsbWait("GPRS SESSION EXPIRATION : ");
	RTCManager_setDelayedCallback(expirationTime*60, & _httpSessionExpirationAlarm, httpSessionExpirationCallback);
}

static void removeHttpSessionExpirationAlarm(){
	if(RTCManager_removeAlarm(&_httpSessionExpirationAlarm))
	{
		USARTManager_printUsbWait("Http expiration alarm removed !\r\n");
	}
}

static bool isSessionExpirationAlarmScheduled(){
	return RTCManager_isAlarmScheduled(&_httpSessionExpirationAlarm);
}

static uint8_t stopHTTP()
{
	uint8_t function_status = FUNCTION_SUCCESS;

	EventBits_t gprsMask = xEventGroupGetBits(_gprsStatusMaskHandle);

	if(!closeBearer(VERY_LONG_WAIT))
	{
		function_status = FUNCTION_FAILURE;
	}
	
	if(gprsMask & GPRS_STATUS_BIT_HTTP_INITIALIZED)
	{
		if(USARTManager_sendATCommand(LONG_WAIT, 1, "AT+HTTPTERM\r\n")!=SERIAL_ANSWER_OK){
			function_status = FUNCTION_FAILURE;
		}
		else
		{
			xEventGroupClearBits(_gprsStatusMaskHandle, GPRS_STATUS_BIT_HTTP_INITIALIZED);
		}
		vTaskDelay(50);
	}

	if(!detachGPRSService(VERY_LONG_WAIT))
	{
		function_status = FUNCTION_FAILURE;
	}
	
	if(function_status == FUNCTION_SUCCESS)
	{
		USARTManager_printUsbWait("HTTP SESSION CLOSED !\r\n");
	}

	return function_status;
}

/* Configure la con type et le connection bearer.
Cette partie est dépendante tu type de carte SIM */
static uint8_t configureHTTP(){

	USARTManager_printUsbWait("HTTP Configuration with VODAFONE.\r\n");

	if(!USARTManager_repeatATCommandUntilSuccess(MAX_TRIES_AT_COMMAND, "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n",LONG_WAIT)){
		return FUNCTION_FAILURE;
	}

	if(!USARTManager_repeatATCommandUntilSuccess(MAX_TRIES_AT_COMMAND, "AT+SAPBR=3,1,\"APN\",\"\"\r\n",LONG_WAIT)){
		return FUNCTION_FAILURE;
	}

	xEventGroupSetBits(_gprsStatusMaskHandle, GPRS_STATUS_BIT_HTTP_CONFIGURED);
	USARTManager_printUsbWait("HTTP Configuration ENDED.\r\n");

	return FUNCTION_SUCCESS;
}

static void httpSessionExpirationCallback(struct calendar_descriptor *const calendar){
	maskUtilities_setRequestMaskBitsFromISR(REQUEST_BIT_HTTP_SESSION_EXPIRED);
	UNUSED(calendar);
}

/* Returns the status of the GPRS registration (<stat> returned by the AT Command CGREG */
static uint8_t parseGPRSRegistration(char* responseToParse)
{
	char* start = strstr(responseToParse, "+CGREG:");
	strtok(start, ": ,\r\n");
	strtok(NULL, ": ,\r\n");
	char* GPRSRegistrationStatusStr = strtok(NULL, ": ,\r\n");
	uint8_t GPRSRegistrationStatus = atoi(GPRSRegistrationStatusStr);
	return GPRSRegistrationStatus;
}


static bool isHttpConfigured(){
	EventBits_t gsmStatusBits = xEventGroupGetBits(_gprsStatusMaskHandle);
	return gsmStatusBits & GPRS_STATUS_BIT_HTTP_CONFIGURED;
}

static E_BEARER_STATUS getBearerStatus()
{
	if(USARTManager_sendATCommand(LONG_WAIT,1, "AT+SAPBR=2,1\r\n") != SERIAL_ANSWER_OK){
		return false;
	}
	
	return parseBearerStatusFromGSMBuf();
}

// GSM_buf should contain ' +SAPBR: <cid>,<status>,"<IP_addr>" ' (example values)
static uint8_t parseBearerStatusFromGSMBuf(){
	char* start = strstr(gsm_buf,"+SAPBR:");
	char *token = NULL;
	uint8_t itemCounter=0;
	uint8_t bearerStatus = 0;

	for (token = strtok(start, ","); token != NULL; token = strtok(NULL, ","))
	{
		itemCounter++;
		if(itemCounter==2)
		{
			bearerStatus=atoi(token);
			break;
		}
	}

	if(bearerStatus>=0
	&& bearerStatus<=3)
	{
		return bearerStatus;
	}
	else
	{
		return -1; // unable to parse
	}
}

static bool isGPRSAttached()
{
	if(USARTManager_sendATCommand(VERY_LONG_WAIT, 1, "AT+CGREG?\r\n") == SERIAL_ANSWER_OK)
	{
		volatile E_GPRS_REGISTRATION_STATUS networkRegistrationStatus = parseGPRSRegistration(gsm_buf);
		if(networkRegistrationStatus == GPRS_REGISTRATION_REGISTERED_HOME
		|| networkRegistrationStatus == GPRS_REGISTRATION_REGISTERED_ROAMING)
		{
			return true;
		}
	}
	return false;
}

static bool isGPRSDetached()
{
	if(USARTManager_sendATCommand(VERY_LONG_WAIT, 1, "AT+CGREG?\r\n") == SERIAL_ANSWER_OK)
	{
		volatile E_GPRS_REGISTRATION_STATUS networkRegistrationStatus = parseGPRSRegistration(gsm_buf);
		if(networkRegistrationStatus == GPRS_REGISTRATION_NOT_REGISTERED
		|| networkRegistrationStatus == GPRS_REGISTRATION_DENIED)
		{
			return true;
		}
	}
	return false;
}

/* Attaches the GPRS service. If it doesn't work before the given timeout, returns false. Otherwise returns true */
static bool attachGPRSService(uint32_t timeout)
{
	TickType_t startingTime = xTaskGetTickCount();
	while(!isGPRSAttached())
	{
		TickType_t currentTime = xTaskGetTickCount();
		TickType_t elapsedTime = currentTime - startingTime;
		if(elapsedTime >= timeout)
		{
			return false;
		}
		USARTManager_sendATCommand(timeout - elapsedTime, 1, "AT+CGATT=1\r\n");
	}
	return true;
}

/* Detaches the GPRS service. If it doesn't work before the given timeout, returns false. Otherwise returns true */
static bool detachGPRSService(uint32_t timeout)
{
	TickType_t startingTime = xTaskGetTickCount();
	while(!isGPRSDetached())
	{
		TickType_t currentTime = xTaskGetTickCount();
		TickType_t elapsedTime = currentTime - startingTime;
		if(elapsedTime >= timeout)
		{
			return false;
		}
		USARTManager_sendATCommand(timeout - elapsedTime, 1, "AT+CGATT=0\r\n");
	}
	return true;
}

static bool openBearer(uint32_t timeout)
{
	TickType_t startingTime = xTaskGetTickCount();
	while(getBearerStatus() != BEARER_STATUS_CONNECTED)
	{
		TickType_t currentTime = xTaskGetTickCount();
		TickType_t elapsedTime = currentTime - startingTime;
		if(elapsedTime >= timeout)
		{
			return false;
		}
		USARTManager_sendATCommand(timeout - elapsedTime, 1, "AT+SAPBR=1,1\r\n");
	}
	return true;
}

static bool closeBearer(uint32_t timeout)
{
	TickType_t startingTime = xTaskGetTickCount();
	while(getBearerStatus() != BEARER_STATUS_CLOSED)
	{
		TickType_t currentTime = xTaskGetTickCount();
		TickType_t elapsedTime = currentTime - startingTime;
		if(elapsedTime >= timeout)
		{
			return false;
		}
		USARTManager_sendATCommand(timeout - elapsedTime, 1, "AT+SAPBR=0,1\r\n");
	}
	return true;
}

/* Demande une data au serveur*/
static E_HTTP_REQUEST_STATUS httpGET(char* url, char* httpMessage){
	
	volatile E_HTTP_REQUEST_STATUS functionResult = HTTP_REQUEST_STATUS_NONE;

	if(USARTManager_sendATCommand(LONG_WAIT, 3, "AT+HTTPPARA=\"URL\",\"", url, "\"\r\n")==SERIAL_ANSWER_OK)
	{
		char httpLength[6] = "";
		functionResult = getMessageLength(httpLength);
		if(functionResult==HTTP_REQUEST_STATUS_OK)
		{
			functionResult = readHttpMessage(httpLength, httpMessage);
		}
	}
	else
	{
		functionResult = HTTP_REQUEST_STATUS_UNKNOWN_ERROR;
	}

	return functionResult;
}

/* Envoie une HTTP Action, renvoie si elle a fonctionné ou non (FUNCTION_SUCCESS ou
FUNCTION_FAILURE), et stocke la longueur du message à lire dans msgLength
Le HTTP Action fonctionne s'il reçoit un status code de 200 et un msgLength > 0 */
static E_HTTP_REQUEST_STATUS getMessageLength(char* msgLength){

	E_SERIAL_ANSWER answer;
	if(USARTManager_sendATCommand(LONG_WAIT, 1, "AT+HTTPACTION=0\r\n")==SERIAL_ANSWER_OK){
		answer = USARTManager_readAnswerAndValidate(VERY_LONG_WAIT, &httpReadTimerCallback);
		if(answer==SERIAL_ANSWER_OK){ // attente de +HTTPACTION:0,200,XXX
			if(strlen(gsm_buf)>0){
				USARTManager_printUsbWait("parsing http response...\r\n");
				char *token = NULL;
				uint8_t itemCounter=0;
				uint16_t httpStatus = 0;
				char httpLength[6] = ""; //nombre de caractères à lire, max 99999
				char* start = strstr(gsm_buf, "+HTTPACTION:");
				USARTManager_printUsbWait("start : ");
				USARTManager_printUsbWait(start);
				USARTManager_printUsbWait("\r\n");
				for (token = strtok(start, ",\r\n"); token != NULL; token = strtok(NULL, ",\r\n"))
				{
					itemCounter++;
					if(itemCounter==2)
					{
						httpStatus=atoi(token);
					}
					else if(itemCounter==3)
					{
						strcpy(httpLength,token);
					}
				}
				USARTManager_printUsbWait("httpLength : ");
				USARTManager_printUsbWait(httpLength);
				USARTManager_printUsbWait("\r\n");

				switch(httpStatus)
				{
					case 200:
						USARTManager_printUsbWait("parsing http response success.\r\n");
						strcpy(msgLength, httpLength);
						return HTTP_REQUEST_STATUS_OK;
					case 400:
						return HTTP_REQUEST_STATUS_BAD_REQUEST;
					case 403:
						return HTTP_REQUEST_STATUS_FORBIDDEN;
					case 404:
						return HTTP_REQUEST_STATUS_NOT_FOUND;
					case 408:
						return HTTP_REQUEST_STATUS_REQUEST_TIMEOUT;
					case 500:
						return HTTP_REQUEST_STATUS_SERVER_ERROR;
					case 601:
						return HTTP_REQUEST_STATUS_NETWORK_ERROR;
					default:
						return HTTP_REQUEST_STATUS_UNKNOWN_ERROR;					
				}
			}
		}
	}
	return HTTP_REQUEST_STATUS_UNKNOWN_ERROR;
}

static void httpReadTimerCallback(){
	char* start = strstr(gsm_buf,"+HTTPACTION:");
	if(start != NULL)
	{
		if(strstr(start,"\r\n"))
		{
			serialAnswerFlag = SERIAL_ANSWER_OK;
		}
	}
	else if (strstr(gsm_buf,"ERROR"))
	{
		serialAnswerFlag = SERIAL_ANSWER_ERROR;
	}
	else
	{
		return;
	}
}

/* Lit un message http d'une longueur msgLength et stocke le resultat dans result */
static E_HTTP_REQUEST_STATUS readHttpMessage(const char* msgLength, char* httpMessage){
	USARTManager_sendATCommand(LONG_WAIT, 3, "AT+HTTPREAD=0,", msgLength, "\r\n");
	char* token = NULL;
	char* start = strstr(gsm_buf,"HTTPREAD:");
	uint8_t itemCounter = 0;
	for (token = strtok(start, "\r\n"); token != NULL; token = strtok(NULL, "\r\n"))
	{
		itemCounter++;
		if(itemCounter==2)
		{
			strcpy(httpMessage,token);
			return HTTP_REQUEST_STATUS_OK;
		}
	}
	return HTTP_REQUEST_STATUS_UNKNOWN_ERROR;
}