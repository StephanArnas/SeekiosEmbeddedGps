#include <messageSender/message_sender.h>

static void drainSendList(void);
static BaseType_t addToOptimizedListBack(PrioritizedOutputMessage *msgPtr);
static E_HTTP_REQUEST_STATUS sendMessage(OutputMessage msg, char* serverResponse);
/* Concatène l'ID du seekios à la chaine dest passée en paramètre*/
static void catSeekiosId(uint8_t* dest);
static uint8_t saveMessageToFlash(PrioritizedOutputMessage* messagePtr);
static void setRetrySendAlarm(void);
static void resetSendFailCount(void);
static void incrementSendFailCount(void);
static uint32_t computeWaitSendRetryTime(void);
static void initSendList(void);
static void senderRetryCallback(struct calendar_descriptor *const calendar);
static void setAlarmSenderRetryDelay(int delayMin);
static void senderRetryCallback(struct calendar_descriptor *const calendar);
static void catAlertMessage(char* resultBuf, uint8_t* battery, uint8_t* signal, OutputMessage* msgPtr);
static void catCellsDataMessage(char* resultBuf, uint8_t* battery, uint8_t* signal, OutputMessage* msgPtr);
static void catCoordinateMessage(char* resultBuf, uint8_t* battery, uint8_t* signal, OutputMessage* msgPtr);
static void catSeekiosSignalAndBattery(char* resultBuf, uint8_t* battery, uint8_t* signal);
static void catCoordinate(char* resultBuf,OutputMessage* msgPtr);
static void catTimestamp(char* dest, time_t timestamp);
static E_MESSAGE_PRIORITY computeMessagePriorityFromMessageType(E_MESSAGE_TYPE type);
static void popOptimizedListFirstMessage(void);
static void addToOptimizedList(PrioritizedOutputMessage *pMsgPtr);
static void sortOptimizedSendList(void);
static uint8_t replaceLowestPriorityMessage(PrioritizedOutputMessage* pMsgPtr);
static uint8_t addToOptimizedListAtIndex(PrioritizedOutputMessage *pMsgPtr, uint8_t index);
static void serializeMessageUrl(OutputMessage msg, char* msgString);
static bool getFirstMessage(OutputMessage *msgPtr);
static uint8_t replaceAlertMessage(PrioritizedOutputMessage *pMsgPtr);
static E_MESSAGE_CATEGORY computeMessageCategoryFromMessageType(E_MESSAGE_TYPE type);
static void extractFrontOutputMessageFromFlash(PrioritizedOutputMessage* result);
static bool isValidMessage(PrioritizedOutputMessage *pMsgPtr);
static void catVersionMessage(char* resultBuf, uint8_t* battery, uint8_t* signal, OutputMessage* msgPtr);
static void catModeId(char* resultBuf, uint32_t modeId);
static void catAlertMessageWithMode(char* resultBuf, uint8_t* battery, uint8_t* signal, OutputMessage* msgPtr);
static void catCoordinateMessageWithMode(char* resultBuf, uint8_t* battery, uint8_t* signal, OutputMessage* msgPtr);

static struct calendar_alarm _senderRetryAlarm;
static OutputMessageArray	_optimizedSendList;
static SemaphoreHandle_t	_optimizedSendListLock;
static QueueHandle_t		_sendList;
static uint8_t				_failSendCount;
static uint8_t				_nbSavedMessagesInFlash;
static uint8_t				_prioritizedOutputMessageSize;
static uint8_t				_nbMaxSavedOutputMessagesInFlash;

void messageSender_init(){
	_failSendCount = 0;
	_nbSavedMessagesInFlash = 0;
	_nbMaxSavedOutputMessagesInFlash = EXT_FLASH_PAGE_SIZE/sizeof(PrioritizedOutputMessage);
	_prioritizedOutputMessageSize = sizeof(PrioritizedOutputMessage);
	_optimizedSendList.nbMessages = 0;
	_optimizedSendListLock = xSemaphoreCreateRecursiveMutex();
	fakeSendingFailure = false;
	initSendList();
}

/* créé la liste et son sémaphore associé */
static void initSendList(){
	_sendList = xQueueCreate(SENDLIST_SIZE, sizeof(OutputMessage));
}

void task_messageSender(void *param){
	UNUSED(param);
	messageSender_clearAlarmSenderRetry();
	maskUtilities_setRunningMaskBits(RUNNING_BIT_SENDER);
	maskUtilities_clearRequestMaskBits(REQUEST_BIT_SENDER);

	bool httpOpen = GSMManager_useHTTP(statusManager_getGPRSExpirationTime());
	volatile E_HTTP_REQUEST_STATUS httpRequestStatus = HTTP_REQUEST_STATUS_UNKNOWN_ERROR;
	uint8_t serverResponse[16];

	OutputMessage msg;
	if(httpOpen)
	{
		drainSendList();
		if(getFirstMessage(&msg) == FUNCTION_SUCCESS)
		{
			httpRequestStatus = sendMessage(msg, serverResponse); // TODO : tant qu'on est en mode retry d'envoi, on ne retente pas d'envoyer les messages
		}
		vTaskDelay(2000);
	}

	if(!httpOpen // Case where we retry to send the message
	|| httpRequestStatus == HTTP_REQUEST_STATUS_NETWORK_ERROR
	|| httpRequestStatus == HTTP_REQUEST_STATUS_REQUEST_TIMEOUT
	|| httpRequestStatus == HTTP_REQUEST_STATUS_NONE)
	{
		USARTManager_printUsbWait("Sending message FAILED : retrying\r\n");
		incrementSendFailCount();
		setRetrySendAlarm();
	}
	else // Success or other error : we delete it
	{
		if(httpRequestStatus == HTTP_REQUEST_STATUS_OK)
		{
			USARTManager_printUsbWait("Sending message SUCCESS\r\n");
			if(msg.onSendSuccess != NULL)
			{
				msg.onSendSuccess();
			}
		}
		USARTManager_printUsbWait("Deleting message\r\n");
		popOptimizedListFirstMessage();
		resetSendFailCount();
		reorganizeOptimizedSendList();
		if(_optimizedSendList.nbMessages > 0)
		{
			maskUtilities_setRequestMaskBits(REQUEST_BIT_SENDER);
		}
	}

	volatile BaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
	UNUSED(wm);
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_SENDER);
	FreeRTOSOverlay_taskDelete(NULL);
}

static bool getFirstMessage(OutputMessage *msgPtr){
	if(_optimizedSendList.nbMessages > 0)
	{
		if(xSemaphoreTakeRecursive(_optimizedSendListLock, 0)==pdPASS){
			*msgPtr = _optimizedSendList.messages[0].outputMessage;
			xSemaphoreGiveRecursive(_optimizedSendListLock);
			return true;
		}
	}
	return false;
}

static void popOptimizedListFirstMessage(){
	if(xSemaphoreTakeRecursive(_optimizedSendListLock, 0)==pdPASS){
		
		if(_optimizedSendList.nbMessages > 0)
		{
			for(uint8_t i = 1; i<_optimizedSendList.nbMessages;i++){
				_optimizedSendList.messages[i-1] = _optimizedSendList.messages[i];
			}
			_optimizedSendList.nbMessages--;
		}
		xSemaphoreGiveRecursive(_optimizedSendListLock);
	}
}

static void resetSendFailCount(){
	_failSendCount = 0;
}

static void incrementSendFailCount(){
	if(_failSendCount < 255){ // if _failCount = 256+1, _failCount = 0 because uint8_t
		_failSendCount++;
	}
}

/* TODO : include RSSI in retry calculation ? */
static void setRetrySendAlarm(){
	setAlarmSenderRetryDelay(computeWaitSendRetryTime());
}

static void setAlarmSenderRetryDelay(int delayMin){
	uint32_t delaySec = delayMin*60;
	USARTManager_printUsbWait("Sender retry : ");
	RTCManager_setDelayedCallback(delaySec, &_senderRetryAlarm, senderRetryCallback);
}

void messageSender_printSenderRetryAlarm()
{
	USARTManager_printUsbWait("Sender retry : ");
	RTCManager_printTimeFromTimestamp(_senderRetryAlarm.data.timestamp);
	USARTManager_printUsbWait("\r\n");
}

void messageSender_clearAlarmSenderRetry(){
	if(RTCManager_removeAlarm(&_senderRetryAlarm)){
		USARTManager_printUsbWait("Canceled Sender Retry Alarm\r\n");
	}
}

static void senderRetryCallback(struct calendar_descriptor *const calendar){
	maskUtilities_setRequestMaskBitsFromISR(REQUEST_BIT_SENDER);
	UNUSED(calendar);
}

bool messageSender_isSenderRetryAlarmScheduled()
{
	return RTCManager_isAlarmScheduled(&_senderRetryAlarm);
}

static uint32_t computeWaitSendRetryTime(){
	switch(_failSendCount){
		case 1:
		return SENDER_FAIL_WAIT_1;
		break;
		case 2:
		return SENDER_FAIL_WAIT_2;
		break;
		case 3:
		return SENDER_FAIL_WAIT_3;
		break;
		case 4:
		return SENDER_FAIL_WAIT_4;
		break;
		case 5:
		return SENDER_FAIL_WAIT_5;
		break;
		case 6:
		return SENDER_FAIL_WAIT_6;
		break;
		case 7:
		return SENDER_FAIL_WAIT_7;
		break;
		case 8:
		return SENDER_FAIL_WAIT_8;
		break;
		case 9:
		return SENDER_FAIL_WAIT_9;
		break;
		default:
		return SENDER_FAIL_WAIT_9;
		break;
	}
}

static void extractFrontOutputMessageFromFlash(PrioritizedOutputMessage* result){
	dataflashManager_readPage(PAGE_INDEX_SAVE_MESSAGES, EXT_FLASH_PAGE_SIZE, genericBuf);
	int startIndex = _prioritizedOutputMessageSize*(_nbSavedMessagesInFlash-1);
	memcpy(result, genericBuf+startIndex, _prioritizedOutputMessageSize);
	_nbSavedMessagesInFlash--;
}

/* If data is in flash, take the last message from the flash and put it in the optimizedSendList */
void reorganizeOptimizedSendList(){
	if(xSemaphoreTakeRecursive(_optimizedSendListLock, 0)==pdPASS){
		while(_nbSavedMessagesInFlash > 0 && _optimizedSendList.nbMessages < OPTIMIZED_SENDLIST_SIZE){
			PrioritizedOutputMessage msg;
			extractFrontOutputMessageFromFlash(&msg);
			addToOptimizedList(&msg);
		}
		xSemaphoreGiveRecursive(_optimizedSendListLock);
	}
}

bool messageSender_outboxHasMessages(){
	return _optimizedSendList.nbMessages > 0;
}

/* This function encapsulates the action of adding elements to the sendList */
int messageSender_addToSendList(OutputMessage* messagePtr){
	int result = xQueueSendToBack(_sendList, messagePtr, MAX_QUEUE_SEND_TIME);
	if(result == pdTRUE){
		maskUtilities_setRequestMaskBits(REQUEST_BIT_SENDER);
		return FUNCTION_SUCCESS;
	}
	else
	{
		return FUNCTION_FAILURE;
	}
}

/* Cette fonction envoie le message au module GSM */
static E_HTTP_REQUEST_STATUS sendMessage(OutputMessage msg, char* serverResponse){
	uint8_t serializedMessage[TX_BUFF_SIZE];
	USARTManager_printUsbWait("START sending message...\r\n");
	serializeMessageUrl(msg, (char*) serializedMessage);
	#if (SIMULATE_SENDING_FAILURES)
	if(fakeSendingFailure){
		USARTManager_printUsbWait("SIMULATE SENDING FAILURES : ON\r\n");
	}else
	{
		USARTManager_printUsbWait("SIMULATE SENDING FAILURES : OFF\r\n");
	}
	#endif
	E_HTTP_REQUEST_STATUS requestStatus;
	if(fakeSendingFailure)
	{
		requestStatus = HTTP_REQUEST_STATUS_UNKNOWN_ERROR;
	}
	else
	{
		requestStatus = GSMManager_httpGET((char*)serializedMessage, (char*)serverResponse);
	}

	if((requestStatus == HTTP_REQUEST_STATUS_OK)){
		USARTManager_printUsbWait("MESSAGE SENT\r\n  >> url : ");
		USARTManager_printUsbWait((char*)serializedMessage);
		USARTManager_printUsbWait("\r\n");
	}
	else
	{
		USARTManager_printUsbWait("MESSAGE NOT SENT  >> FAIL !!!\r\n");
		
	}
	return requestStatus;
}

/* Sérialise le message msg dans la chaine de caractère msgString SOUS FORMAT URL pour le renvoyer dans une requête GET*/
/* FORMAT ON DEMAND: http://seekios.cloudapp.net/SeekiosEmbeddedService.svc/<API_function_name>/<Seekios_ID>/<battery_level>/<signal_RSSI>/<lat>/<lon>*/
static void serializeMessageUrl(OutputMessage msg, char* msgString){
	msgString[0] = '\0';
	uint8_t battery[4] = "";
	uint8_t signal[4] = "";
	batteryManager_getBatteryPercentageStr(battery);
	GSMManager_getSignalLevelPctage(signal);
	strcpy(msgString, SEEKIOS_EMBEDDED_SERVICE_URL);
	switch (msg.messageType)
	{
		case MESSAGE_TYPE_ON_DEMAND :
		strcat(msgString, "RODR/");
		catCoordinateMessage(msgString, battery, signal, &msg);
		break;
		case MESSAGE_TYPE_ON_DEMAND_TRIANGULATION:
		strcat(msgString,"RODRBCD/");
		catCellsDataMessage(msgString, battery, signal, &msg);
		break;
		case MESSAGE_TYPE_SOS :
		strcat(msgString, "SSOS/");
		catAlertMessage(msgString, battery, signal, &msg);
		break;
		case MESSAGE_TYPE_CRITICAL_POWER_EXTINCTION:
		// TODO : build URL
		break;
		case MESSAGE_TYPE_SOS_LOCATION :
		strcat(msgString, "SSOSL/");
		catCoordinateMessage(msgString, battery, signal, &msg);
		break;
		case MESSAGE_TYPE_SOS_LOCATION_TRIANGULATION:
		strcat(msgString,"SSOSLBCD/");
		catCellsDataMessage(msgString, battery, signal, &msg);
		break;
		case MESSAGE_TYPE_TRACKING :
		strcat(msgString, "ANTL/");
		catCoordinateMessageWithMode(msgString, battery, signal, &msg);
		break;
		case MESSAGE_TYPE_OUT_OF_ZONE :
		strcat(msgString, "NSOOZ/");
		catCoordinateMessageWithMode(msgString, battery, signal, &msg);
		break;
		case MESSAGE_TYPE_ZONE_TRACKING :
		strcat(msgString, "ANZTL/");
		catCoordinateMessageWithMode(msgString, battery, signal, &msg);
		break;
		case MESSAGE_TYPE_DONT_MOVE:
		strcat(msgString, "NSM/");
		catAlertMessageWithMode(msgString, battery, signal, &msg);
		break;
		case MESSAGE_TYPE_DONT_MOVE_TRACKING:
		strcat(msgString, "ANDMTL/");
		catCoordinateMessageWithMode(msgString, battery, signal, &msg);
		break;
		case MESSAGE_TYPE_SEEKIOS_VERSION_UPDATED:
		strcat(msgString,"USV/");
		catVersionMessage(msgString, battery, signal, &msg);
		break;
		case MESSAGE_TYPE_LOW_BATTERY:
		strcat(msgString, "CBA/");
		catAlertMessage(msgString, battery, signal, &msg);
		break;
		case MESSAGE_TYPE_POWER_SAVING_DISABLED:
		strcat(msgString, "PSD/");
		catAlertMessageWithMode(msgString, battery, signal, &msg);
		break;
		default:
		break;
	}
}

static void catVersionMessage(char* resultBuf, uint8_t* battery, uint8_t* signal, OutputMessage* msgPtr)
{
	// TODO : implement
	catSeekiosId((unsigned char*)resultBuf);
	catSeekiosSignalAndBattery(resultBuf, battery, signal);
	seekiosInfoManager_catSeekiosVersion(resultBuf);
	catTimestamp(resultBuf, msgPtr->timestamp);
}

static void catAlertMessage(char* resultBuf, uint8_t* battery, uint8_t* signal, OutputMessage* msgPtr)
{
	catSeekiosId((unsigned char*)resultBuf);
	catSeekiosSignalAndBattery(resultBuf, battery, signal);
	catTimestamp(resultBuf, msgPtr->timestamp);
}

static void catAlertMessageWithMode(char* resultBuf, uint8_t* battery, uint8_t* signal, OutputMessage* msgPtr)
{
	catSeekiosId((unsigned char*)resultBuf);
	catSeekiosSignalAndBattery(resultBuf, battery, signal);
	catTimestamp(resultBuf, msgPtr->timestamp);
	catModeId(resultBuf, msgPtr->modeId);
}


static void catCellsDataMessage(char* resultBuf, uint8_t* battery, uint8_t* signal, OutputMessage* msgPtr){
	catSeekiosId((unsigned char*)resultBuf);
	catSeekiosSignalAndBattery(resultBuf, battery, signal);
	GSMManager_catCellsData(resultBuf);
	catTimestamp(resultBuf, msgPtr->timestamp);
}

static void catCoordinateMessage(char* resultBuf, uint8_t* battery, uint8_t* signal, OutputMessage* msgPtr){
	catSeekiosId((unsigned char*)resultBuf);
	catSeekiosSignalAndBattery(resultBuf, battery, signal);
	catCoordinate(resultBuf, msgPtr);
	catTimestamp(resultBuf, msgPtr->timestamp);
}

static void catCoordinateMessageWithMode(char* resultBuf, uint8_t* battery, uint8_t* signal, OutputMessage* msgPtr){
	catSeekiosId((unsigned char*)resultBuf);
	catSeekiosSignalAndBattery(resultBuf, battery, signal);
	catCoordinate(resultBuf, msgPtr);
	catTimestamp(resultBuf, msgPtr->timestamp);
	catModeId(resultBuf, msgPtr->modeId);
}

static void catSeekiosSignalAndBattery(char* resultBuf, uint8_t* battery, uint8_t* signal){
	strcat(resultBuf,"/");
	strcat(resultBuf, (char*)battery);
	strcat(resultBuf,"/");
	strcat(resultBuf, (char*)signal);
}

static void catCoordinate(char* resultBuf,OutputMessage* msgPtr){
	char buff[20] = "";
	strcat(resultBuf,"/");
	stringHelper_doubleToString(msgPtr->content.satelliteCoordinate.coordinate.lat,buff,11);
	strcat(resultBuf,buff);
	strcat(resultBuf,"/");
	stringHelper_doubleToString(msgPtr->content.satelliteCoordinate.coordinate.lon,buff,11);
	strcat(resultBuf,buff);
	strcat(resultBuf,"/");
	stringHelper_doubleToString(msgPtr->content.satelliteCoordinate.coordinate.alt,buff,2);
	strcat(resultBuf,buff);
	strcat(resultBuf,"/");
	stringHelper_doubleToString(msgPtr->content.satelliteCoordinate.hDOP,buff,2);
	strcat(resultBuf,buff);
}

static void catModeId(char* resultBuf, uint32_t modeId)
{
	char buff[12];
	stringHelper_intToString(modeId, buff);
	strcat(resultBuf, "/");
	strcat(resultBuf, buff);
}

/* Concatène l'ID du seekios à la chaine dest passée en paramètre*/
static void catSeekiosId(uint8_t* dest){
	uint8_t seekiosId[9] = "";
	seekiosInfoManager_getSeekiosUID(seekiosId);
	strcat((char*)dest, (char*)seekiosId);
}

/* Concatène l'ID du seekios à la chaine dest passée en paramètre*/
static void catTimestamp(char* dest, time_t timestamp){
	uint8_t buff[12] = "";
	stringHelper_longToString(timestamp, buff);
	strcat(dest, "/");
	strcat(dest, (char*)buff);
}

/* TODO : faire un algorithme qui va déterminer les messages à mettre ou non dans la optimizedSendList
Pours the content of the sendList into the optimizedList or the flash (if OL full) or erase it if both full*/
static void drainSendList(){
	while(uxQueueMessagesWaiting(_sendList) > 0){
		OutputMessage msg;
		xQueueReceive(_sendList, &msg, (TickType_t) MAX_QUEUE_RECEIVE_TIME);
		PrioritizedOutputMessage pMsg;
		memcpy(&(pMsg.outputMessage), &msg, sizeof(OutputMessage));
		pMsg.messagePriority = computeMessagePriorityFromMessageType(msg.messageType);
		pMsg.messageCategory = computeMessageCategoryFromMessageType(msg.messageType);
		addToOptimizedList(&pMsg);
	}
}

/* Inserts a Prioritized Output Message at the right position in the optimized sendList */
static void addToOptimizedList(PrioritizedOutputMessage *pMsgPtr){
	
	if(!isValidMessage(pMsgPtr))
	{
		return;
	}

	if(xSemaphoreTakeRecursive(_optimizedSendListLock, 0)==pdPASS){
		if(replaceAlertMessage(pMsgPtr) == FUNCTION_FAILURE)
		{
			if(addToOptimizedListBack(pMsgPtr) == errQUEUE_FULL){
				if(replaceLowestPriorityMessage(pMsgPtr) == FUNCTION_FAILURE){
					saveMessageToFlash(pMsgPtr);
				}
			}
			sortOptimizedSendList();
		}
		xSemaphoreGiveRecursive(_optimizedSendListLock);
	}
}

/* Check message validity : Existing type */
static bool isValidMessage(PrioritizedOutputMessage *pMsgPtr)
{
	if(pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_CRITICAL_POWER_EXTINCTION
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_ALERT
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_ON_DEMAND
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_ON_DEMAND_TRIANGULATION
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_SOS
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_SOS_LOCATION
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_SOS_LOCATION_TRIANGULATION
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_TRACKING
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_DONT_MOVE
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_DONT_MOVE_TRACKING
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_OUT_OF_ZONE
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_ZONE_TRACKING
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_ZONE_BACK_IN_ZONE
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_IN_TIME
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_FOLLOW_ME
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_FOLLOW_ME_TRACKING
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_DAILY_TRACK
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_DAILY_TRACK_TRIANGULATION
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_SEEKIOS_VERSION_UPDATED
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_LOW_BATTERY
	|| pMsgPtr->outputMessage.messageType == MESSAGE_TYPE_POWER_SAVING_DISABLED)
	{
		return true;
	}
	return false;
}

static uint8_t replaceAlertMessage(PrioritizedOutputMessage *pMsgPtr)
{
	if(pMsgPtr->messageCategory == MESSAGE_CATEGORY_TRACKING)
	{
		return FUNCTION_FAILURE;
	}

	for (uint8_t i = 0 ; i < _optimizedSendList.nbMessages; i++)
	{
		if (_optimizedSendList.messages[i].outputMessage.messageType == pMsgPtr->outputMessage.messageType) /* Decreasing order */
		{
			memcpy(&_optimizedSendList.messages[i], pMsgPtr, sizeof(PrioritizedOutputMessage));
			return FUNCTION_SUCCESS;
		}
	}

	return FUNCTION_FAILURE;
}

/* Orders the optimized sendlist follow the rules :
- we put the messages that have the highest priorities at the lowest indexes of the list
- we group messages of the same types
- within those groups of messages of the same priorities, we put the ones with the lowest timestamp at the lowest indexes in those groups of messages
*/
static void sortOptimizedSendList(){
	
	/* Order by priority */
	PrioritizedOutputMessage swap;
	uint8_t n = _optimizedSendList.nbMessages;
	uint8_t i;
	uint8_t j;

	for (i = 0 ; i < ( n - 1 ); i++)
	{
		for (j = 0 ; j < n - i - 1; j++)
		{
			if (_optimizedSendList.messages[j].messagePriority < _optimizedSendList.messages[j+1].messagePriority) /* Decreasing order */
			{
				swap = _optimizedSendList.messages[j];
				_optimizedSendList.messages[j] = _optimizedSendList.messages[j+1];
				_optimizedSendList.messages[j+1] = swap;
			}
		}
	}

	/* Order by message types (with the same priority) */
	for (i = 0 ; i < ( n - 1 ); i++)
	{
		for (j = 0 ; j < n - i - 1; j++)
		{
			if ((_optimizedSendList.messages[j].messagePriority == _optimizedSendList.messages[j+1].messagePriority)
			&&
			(_optimizedSendList.messages[j].outputMessage.messageType > _optimizedSendList.messages[j+1].outputMessage.messageType)) /* Increasing order */
			{
				swap = _optimizedSendList.messages[j];
				_optimizedSendList.messages[j] = _optimizedSendList.messages[j+1];
				_optimizedSendList.messages[j+1] = swap;
			}
		}
	}

	/* Order by timestamps (with the same priority) */
	for (i = 0 ; i < ( n - 1 ); i++)
	{
		for (j = 0 ; j < n - i - 1; j++)
		{
			if ((_optimizedSendList.messages[j].messagePriority == _optimizedSendList.messages[j+1].messagePriority)
			&&
			(_optimizedSendList.messages[j].outputMessage.messageType == _optimizedSendList.messages[j+1].outputMessage.messageType)
			&&
			(_optimizedSendList.messages[j].outputMessage.timestamp > _optimizedSendList.messages[j+1].outputMessage.timestamp)) /* Increasing order */
			{
				swap = _optimizedSendList.messages[j];
				_optimizedSendList.messages[j] = _optimizedSendList.messages[j+1];
				_optimizedSendList.messages[j+1] = swap;
			}
		}
	}
}

static uint8_t replaceLowestPriorityMessage(PrioritizedOutputMessage* pMsgPtr){
	if(_optimizedSendList.nbMessages > 0){
		uint8_t lowestPriorityMessageIndex = 0;
		PrioritizedOutputMessage lowestPriorityMessage = _optimizedSendList.messages[lowestPriorityMessageIndex];
		for(uint8_t i=1; i<_optimizedSendList.nbMessages;++i){
			PrioritizedOutputMessage tempMsg = _optimizedSendList.messages[i];
			if(tempMsg.messagePriority < lowestPriorityMessage.messagePriority
			|| (tempMsg.messagePriority == lowestPriorityMessage.messagePriority && tempMsg.outputMessage.timestamp < lowestPriorityMessage.outputMessage.timestamp))
			{
				lowestPriorityMessageIndex = i;
				lowestPriorityMessage = tempMsg;
			}
		}
		
		if(lowestPriorityMessage.messagePriority < pMsgPtr->messagePriority
		|| (lowestPriorityMessage.messagePriority == pMsgPtr->messagePriority && lowestPriorityMessage.outputMessage.timestamp < pMsgPtr->outputMessage.timestamp))
		{
			addToOptimizedListAtIndex(pMsgPtr, lowestPriorityMessageIndex);
			saveMessageToFlash(&lowestPriorityMessage);
			return FUNCTION_SUCCESS;
		}
	}
	return FUNCTION_FAILURE;
}

/* Fonction appellée depuis le list_for_each de flushSendList */
static BaseType_t addToOptimizedListBack(PrioritizedOutputMessage *pMsgPtr){
	if(_optimizedSendList.nbMessages >= OPTIMIZED_SENDLIST_SIZE)
	{
		return errQUEUE_FULL;
	}
	else
	{
		memcpy(&_optimizedSendList.messages[_optimizedSendList.nbMessages], pMsgPtr, sizeof(PrioritizedOutputMessage));
		_optimizedSendList.nbMessages++;
		return pdPASS;
	}
}

/* Replaces a message in the optmizedList at a certain index. Index must be lower than the amout of saved message */
static uint8_t addToOptimizedListAtIndex(PrioritizedOutputMessage *pMsgPtr, uint8_t index){
	if(index < _optimizedSendList.nbMessages){
		memcpy(&_optimizedSendList.messages[index], pMsgPtr, sizeof(PrioritizedOutputMessage));
		return FUNCTION_SUCCESS;
	}
	else
	{
		return FUNCTION_FAILURE;
	}
}

static E_MESSAGE_CATEGORY computeMessageCategoryFromMessageType(E_MESSAGE_TYPE type)
{
	switch(type){
		case MESSAGE_TYPE_ALERT:
		case MESSAGE_TYPE_CRITICAL_POWER_EXTINCTION:
		case MESSAGE_TYPE_SOS:
		case MESSAGE_TYPE_SOS_LOCATION:
		case MESSAGE_TYPE_SOS_LOCATION_TRIANGULATION:
		case MESSAGE_TYPE_DONT_MOVE:
		case MESSAGE_TYPE_FOLLOW_ME:
		case MESSAGE_TYPE_IN_TIME:
		case MESSAGE_TYPE_OUT_OF_ZONE:
		case MESSAGE_TYPE_ZONE_BACK_IN_ZONE:
		case MESSAGE_TYPE_ON_DEMAND:
		case MESSAGE_TYPE_ON_DEMAND_TRIANGULATION:
		case MESSAGE_TYPE_NONE:
		case MESSAGE_TYPE_DAILY_TRACK:
		case MESSAGE_TYPE_DAILY_TRACK_TRIANGULATION:
		case MESSAGE_TYPE_SEEKIOS_VERSION_UPDATED:
		case MESSAGE_TYPE_LOW_BATTERY:
		case MESSAGE_TYPE_POWER_SAVING_DISABLED:
		return MESSAGE_CATEGORY_ALERT;
		break;
		
		case MESSAGE_TYPE_TRACKING:
		case MESSAGE_TYPE_ZONE_TRACKING:
		case MESSAGE_TYPE_DONT_MOVE_TRACKING:
		case MESSAGE_TYPE_FOLLOW_ME_TRACKING:
		return MESSAGE_CATEGORY_TRACKING;
		break;
	}
	return MESSAGE_CATEGORY_ALERT;
}

static E_MESSAGE_PRIORITY computeMessagePriorityFromMessageType(E_MESSAGE_TYPE type){
	switch(type){
		case MESSAGE_TYPE_ALERT:
		case MESSAGE_TYPE_CRITICAL_POWER_EXTINCTION:
		case MESSAGE_TYPE_SOS:
		case MESSAGE_TYPE_SOS_LOCATION:
		case MESSAGE_TYPE_SOS_LOCATION_TRIANGULATION:
		case MESSAGE_TYPE_DONT_MOVE:
		case MESSAGE_TYPE_FOLLOW_ME:
		case MESSAGE_TYPE_IN_TIME:
		case MESSAGE_TYPE_OUT_OF_ZONE:
		case MESSAGE_TYPE_ZONE_BACK_IN_ZONE:
		case MESSAGE_TYPE_SEEKIOS_VERSION_UPDATED:
		case MESSAGE_TYPE_LOW_BATTERY:
		case MESSAGE_TYPE_POWER_SAVING_DISABLED:
		return MESSAGE_PRIORITY_MEDIUM;
		break;
		
		case MESSAGE_TYPE_TRACKING:
		case MESSAGE_TYPE_ZONE_TRACKING:
		case MESSAGE_TYPE_DAILY_TRACK:
		case MESSAGE_TYPE_DAILY_TRACK_TRIANGULATION:
		case MESSAGE_TYPE_DONT_MOVE_TRACKING:
		case MESSAGE_TYPE_FOLLOW_ME_TRACKING:
		case MESSAGE_TYPE_ON_DEMAND:
		case MESSAGE_TYPE_ON_DEMAND_TRIANGULATION:
		case MESSAGE_TYPE_NONE:
		return MESSAGE_PRIORITY_LOW;
		break;
	}
	return MESSAGE_PRIORITY_LOW;
}

void messageSender_testWriteOutputMessageInFlash(){
	PrioritizedOutputMessage msg1;
	msg1.messagePriority = MESSAGE_PRIORITY_LOW;
	msg1.outputMessage.content.satelliteCoordinate.coordinate.lat	= 43.293829;
	msg1.outputMessage.content.satelliteCoordinate.coordinate.lon	= -1.5393948;
	msg1.outputMessage.messageType			= MESSAGE_TYPE_ON_DEMAND;
	msg1.outputMessage.timestamp			= 1467733153;

	PrioritizedOutputMessage msg2;
	msg2.messagePriority = MESSAGE_PIORITY_HIGH;
	msg2.outputMessage.content.satelliteCoordinate.coordinate.lat	= 35.293829;
	msg2.outputMessage.content.satelliteCoordinate.coordinate.lon	= -2.5393948;
	msg2.outputMessage.messageType			= MESSAGE_TYPE_SOS_LOCATION;
	msg2.outputMessage.timestamp			= 1467733186;

	saveMessageToFlash(&msg1);
	saveMessageToFlash(&msg2);
	PrioritizedOutputMessage result;
	extractFrontOutputMessageFromFlash(&result);

}

static uint8_t saveMessageToFlash(PrioritizedOutputMessage* messagePtr){
	if(_nbSavedMessagesInFlash < _nbMaxSavedOutputMessagesInFlash){
		dataflashManager_readPage(PAGE_INDEX_SAVE_MESSAGES, EXT_FLASH_PAGE_SIZE, genericBuf);
		int startIndex = _prioritizedOutputMessageSize*_nbSavedMessagesInFlash;
		if(startIndex+_prioritizedOutputMessageSize<EXT_FLASH_PAGE_SIZE)
		{
			memcpy(genericBuf+startIndex, messagePtr, _prioritizedOutputMessageSize);
			dataflashManager_writeToPage(PAGE_INDEX_SAVE_MESSAGES, EXT_FLASH_PAGE_SIZE, genericBuf);
			_nbSavedMessagesInFlash++;
			return FUNCTION_SUCCESS;
		}
	}
	
	return FUNCTION_FAILURE;
}

// TODO : à supprimer. Utilisé pour debug.
void messageSender_testOptimizedSendList()
{
	PrioritizedOutputMessage msg1;
	msg1.messagePriority = MESSAGE_PRIORITY_LOW;
	msg1.messageCategory = MESSAGE_CATEGORY_TRACKING;
	msg1.outputMessage.content.satelliteCoordinate.coordinate.lat	= 43.293829;
	msg1.outputMessage.content.satelliteCoordinate.coordinate.lon	= -1.5393948;
	msg1.outputMessage.messageType			= MESSAGE_TYPE_ON_DEMAND;
	msg1.outputMessage.timestamp			= 1;

	PrioritizedOutputMessage msg2;
	msg2.messagePriority = MESSAGE_PRIORITY_LOW;
	msg2.messageCategory = MESSAGE_CATEGORY_TRACKING;
	msg2.outputMessage.content.satelliteCoordinate.coordinate.lat	= 35.293829;
	msg2.outputMessage.content.satelliteCoordinate.coordinate.lon	= -2.5393948;
	msg2.outputMessage.messageType			= MESSAGE_TYPE_SOS_LOCATION;
	msg2.outputMessage.timestamp			= 2;

	PrioritizedOutputMessage msg3;
	msg3.messagePriority = MESSAGE_PRIORITY_LOW;
	msg3.messageCategory = MESSAGE_CATEGORY_TRACKING;
	msg3.outputMessage.content.satelliteCoordinate.coordinate.lat	= 35.293829;
	msg3.outputMessage.content.satelliteCoordinate.coordinate.lon	= -2.5393948;
	msg3.outputMessage.messageType			= MESSAGE_TYPE_SOS_LOCATION;
	msg3.outputMessage.timestamp			= 3;

	OutputMessage readMsg;

	addToOptimizedList(&msg1);
	addToOptimizedList(&msg2);
	addToOptimizedList(&msg3);
	addToOptimizedList(&msg1);
	addToOptimizedList(&msg2);
	addToOptimizedList(&msg3);
	addToOptimizedList(&msg1);
	addToOptimizedList(&msg2);
	addToOptimizedList(&msg3);
	popOptimizedListFirstMessage();
	reorganizeOptimizedSendList();
	getFirstMessage(&readMsg);
	popOptimizedListFirstMessage();
	reorganizeOptimizedSendList();
	getFirstMessage(&readMsg);
	popOptimizedListFirstMessage();
	reorganizeOptimizedSendList();
	getFirstMessage(&readMsg);
	popOptimizedListFirstMessage();
	reorganizeOptimizedSendList();
	getFirstMessage(&readMsg);
	popOptimizedListFirstMessage();
	reorganizeOptimizedSendList();
	getFirstMessage(&readMsg);
	popOptimizedListFirstMessage();
	reorganizeOptimizedSendList();
	getFirstMessage(&readMsg);
	popOptimizedListFirstMessage();
	reorganizeOptimizedSendList();
	getFirstMessage(&readMsg);
	popOptimizedListFirstMessage();
	reorganizeOptimizedSendList();
	getFirstMessage(&readMsg);
	popOptimizedListFirstMessage();
	reorganizeOptimizedSendList();
	getFirstMessage(&readMsg);
}