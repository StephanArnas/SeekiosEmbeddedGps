#include <messageListener/message_listener.h>

struct tm lastWakeUpMessageTime;
static uint8_t _failListenCount;

static uint8_t processMessages(char* httpMessage);
static void setRetryListener(void);
static void resetListenFailCount(void);
static void incrementListenFailCount(void);
static void setAlarmListenerRetryDelay(int delayMin);
static void listenerRetryCallback(struct calendar_descriptor *const calendar);
static uint8_t buildGetSeekiosInstructionURL(char* url);
static void addFakeFrames(char* httpMessage);

static struct calendar_alarm _listenerRetryAlarm;	// used to wake-up listener for a re-try of getting the instructions

void messageListener_init(){
	_failListenCount = 0;
}

/* Cette tâche va écouter en permanence les messages provenant de TCP/IP et GSM
(excepté utilisation du GPS en cours */
void task_messageListener(void* param){
	UNUSED(param);
	messageListener_clearAlarmListenerRetryIfSet();
	maskUtilities_setRunningMaskBits(RUNNING_BIT_LISTENER);
	maskUtilities_clearRequestMaskBits(REQUEST_BIT_LISTENER);
	// We wait for the seekiosManager to launch the task that will check the http avaibility
	bool httpOpen = GSMManager_useHTTP(statusManager_getGPRSExpirationTime());
	volatile E_HTTP_REQUEST_STATUS requestStatus = HTTP_REQUEST_STATUS_NONE;

	if(httpOpen)
	{
		char httpMessage[GSM_BUF_SIZE];
		
		char url[70];
		if(buildGetSeekiosInstructionURL(url))
		{
			//requestStatus = GSMManager_httpGET("http://www.google.fr/webhp?nord=1", httpMessage);
			requestStatus = GSMManager_httpGET(url, httpMessage);

			#if (TEST_FAKE_DONT_MOVE == 1 || TEST_FAKE_ADMIN_TEST == 1 || TEST_FAKE_ON_DEMAND == 1 || TEST_FAKE_TRACKING == 1 || TEST_FAKE_ZONE == 1 || TEST_FULL_FRAME == 1)
			//requestStatus = HTTP_REQUEST_STATUS_OK;
			if(requestStatus == HTTP_REQUEST_STATUS_OK)
			{
				addFakeFrames(httpMessage);
			}
			#endif
		}

		if(requestStatus == HTTP_REQUEST_STATUS_OK)
		{
			resetListenFailCount();
			if(processMessages(httpMessage) == FUNCTION_SUCCESS)
			{
				ledUtilities_instructionReceivedLedInstruction();
			}
		}

		GSMManager_removeAllSMS();
		vTaskDelay(2000);
	}
	
	if(requestStatus == HTTP_REQUEST_STATUS_BAD_REQUEST
	|| requestStatus == HTTP_REQUEST_STATUS_FORBIDDEN
	|| requestStatus == HTTP_REQUEST_STATUS_NOT_FOUND
	|| requestStatus == HTTP_REQUEST_STATUS_SERVER_ERROR) /* If it is an irreversible error, don't try again */
	{
		USARTManager_printUsbWait("GETTING INSTRUCTION FAILED. NOT Retrying. \r\n");
		resetListenFailCount();
	}
	else if(!httpOpen
	|| requestStatus == HTTP_REQUEST_STATUS_UNKNOWN_ERROR
	|| requestStatus == HTTP_REQUEST_STATUS_NETWORK_ERROR
	|| requestStatus == HTTP_REQUEST_STATUS_REQUEST_TIMEOUT
	|| requestStatus == HTTP_REQUEST_STATUS_NONE) /* If it is a temporary error, set a retry */
	{
		USARTManager_printUsbWait("GETTING INSTRUCTION FAILED. Retrying.\r\n");
		incrementListenFailCount();
		setRetryListener();
	}

	volatile BaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
	UNUSED(wm);
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_LISTENER);
	FreeRTOSOverlay_taskDelete(NULL);
}

/* Sets a fake predetermined frames when reading HTTP data from the server.
TODO : it's a debug function, it should not be in the final build */
static void addFakeFrames(char* httpMessage)
{
	static uint32_t fakeIdCount = 1;
	if(strstr(httpMessage,"[")==NULL)
	{
		strcpy(httpMessage, "[]");
	}
	char* endFrame = NULL;
	UNUSED(endFrame);
	// At this point, frame is equal to : ["D120984744", or [

	#if (TEST_FAKE_DONT_MOVE==1)
	
	#endif

	#if (TEST_FAKE_ZONE==1)

	#endif

	#if (TEST_FAKE_TRACKING==1)
	endFrame = strstr(httpMessage, "]");
	if(endFrame != NULL)
	{
		if((endFrame-1)[0] == '"')
		{
			endFrame[0] = ',';
			endFrame[1] = 0; 	
		}
		else
		{
			endFrame[0] = 0;
		}
	}
	strcat(endFrame, "\"#M03");
	char buff[10];
	stringHelper_intToString(fakeIdCount, buff);
	strcat(endFrame, buff);
	strcat(endFrame, ";1;1-5;30&\"]");
	endFrame = endFrame + strlen(endFrame - 1);
	#endif

	#if (TEST_FAKE_ADMIN_TEST==1)
	endFrame = strstr(httpMessage, "]");
	if(endFrame != NULL)
	{
		if((endFrame-1)[0] == '"')
		{
			endFrame[0] = ',';
			endFrame[1] = 0;
		}
		else
		{
			endFrame[0] = 0;
		}
	}
	strcat(endFrame, "\"#A031&\"]"); // idmode;isRas;isPS;RR
	#endif

	#if (TEST_FAKE_ON_DEMAND==1)
	endFrame = strstr(httpMessage, "]");
	if(endFrame != NULL)
	{
		if((endFrame-1)[0] == '"')
		{
			endFrame[0] = ',';
			endFrame[1] = 0; 	
		}
		else
		{
			endFrame[0] = 0;
		}
	}
	strcat(endFrame, "\"#M02&\"]");
	#endif

	#if (TEST_FULL_FRAME==1)
	strcpy(httpMessage, "[\"#M053966;1;1;43.488576246:-1.533029303;43.488529786:-1.534961835;43.490594673:-1.534990668;43.490490081:-1.533243544&\",\"#A030&\"]");
	#endif

	fakeIdCount++;
}

static uint8_t buildGetSeekiosInstructionURL(char* url)
{
	strcpy(url,SEEKIOS_EMBEDDED_SERVICE_URL"GSI/");

	uint8_t id[9] = "";
	seekiosInfoManager_getSeekiosUID(id);
	strcat(url,(char*)id);
	strcat(url,"/");

	uint8_t battery[4] = "";
	batteryManager_getBatteryPercentageStr(battery);
	strcat(url, (char*)battery);
	strcat(url,"/");
	uint8_t signal[4] = "";
	if (!GSMManager_getSignalLevelPctage(signal))
	{
		return FUNCTION_FAILURE;
	}
	strcat(url, (char*)signal);

	if(!RTCManager_isCalendarInitialized()){
		strcat(url, "/1/0");
	}
	else
	{
		strcat(url, "/0/");
		uint8_t buff[12] = "";
		stringHelper_intToString(RTCManager_getCurrentTimestamp(), buff);
		strcat(url, (char*)buff);
	}
	
	return FUNCTION_SUCCESS;
}

static void resetListenFailCount(){
	_failListenCount = 0;
}

static void incrementListenFailCount(){
	if(_failListenCount < 255){ // if _failCount = 256+1, _failCount = 0 because uint8_t
		_failListenCount++;
	}
}

void messageListener_clearAlarmListenerRetryIfSet(){
	if(RTCManager_removeAlarm(&_listenerRetryAlarm)){
		USARTManager_printUsbWait("Canceled Listener Retry Alarm\r\n");
	}
}

static void setAlarmListenerRetryDelay(int delaySec){
	messageListener_clearAlarmListenerRetryIfSet();
	USARTManager_printUsbWait("Listener retry alarm : ");
	RTCManager_setDelayedCallback(delaySec, &_listenerRetryAlarm, listenerRetryCallback);
}

static void listenerRetryCallback(struct calendar_descriptor *const calendar){
	maskUtilities_setRequestMaskBits(REQUEST_BIT_LISTENER);
	UNUSED(calendar);
}

void messageListener_printListenerRetryAlarm()
{
	USARTManager_printUsbWait("Listener retry : ");
	RTCManager_printTimeFromTimestamp(_listenerRetryAlarm.data.timestamp);
	USARTManager_printUsbWait("\r\n");
}

bool messageListener_isListenerRetryAlarmScheduled()
{
	return RTCManager_isAlarmScheduled(&_listenerRetryAlarm);
}

/* Depending on the number of time getting the instruction failed, we compute a time to wait for the next instruction */
static int computeWaitListenRetryTime(){
	switch(_failListenCount){
		case 1:
		return LISTENER_FAIL_WAIT_1;
		break;
		case 2:
		return LISTENER_FAIL_WAIT_2;
		break;
		case 3:
		return LISTENER_FAIL_WAIT_3;
		break;
		case 4:
		return LISTENER_FAIL_WAIT_4;
		break;
		case 5:
		return LISTENER_FAIL_WAIT_1;
		break;
		case 6:
		return LISTENER_FAIL_WAIT_6;
		break;
		case 7:
		return LISTENER_FAIL_WAIT_7;
		break;
		case 8:
		return LISTENER_FAIL_WAIT_8;
		break;
		case 9:
		return LISTENER_FAIL_WAIT_9;
		break;
		default:
		return LISTENER_FAIL_WAIT_9;
		break;
	}
}

/* This function sets the time for the next retry to listen to the HTTP message
If the calendar functionalities are initialized, we delay the retry to the future.
If those functinalities are not set yet, we retry immediately */
static void setRetryListener(){
	setAlarmListenerRetryDelay(computeWaitListenRetryTime());
}

static uint8_t processMessages(char* httpMessage){
	USARTManager_printUsbWait("GETTING INSTRUCTION SUCCESS.\r\n  >> Instruction : ");
	USARTManager_printUsbWait(httpMessage);
	USARTManager_printUsbWait("\r\n");
	// suppression des [" et "] (début et fin de chaine)
	if(strlen(httpMessage) < 4)
	{
		return FUNCTION_FAILURE;
	}
	else
	{
		httpMessage = httpMessage+2;
		httpMessage[strlen(httpMessage) - 2] = '\0';
	}
	char* separator = "\",\"";
	char* messageAnchor = NULL;
	char* token = strtok_r(httpMessage, separator, &messageAnchor);

	if(token == NULL){
		return FUNCTION_FAILURE;
	}
	
	while(token != NULL)
	{
		statusManager_processMessage(token);
		token = strtok_r(NULL, separator, &messageAnchor);
	}

	return FUNCTION_SUCCESS;
}

