#include <seekiosManager/mask_utilities.h>

static EventGroupHandle_t _runningMaskHandle;
static EventGroupHandle_t _requestMaskHandle;
static EventGroupHandle_t _interruptMaskHandle;

void maskUtilities_init(){
	_interruptMaskHandle		= xEventGroupCreate();
	_runningMaskHandle			= xEventGroupCreate();
	_requestMaskHandle			= xEventGroupCreate();

	maskUtilities_clearRunningMask();
	maskUtilities_clearRequestMask();
	maskUtilities_clearInterruptMask();
}

EventBits_t maskUtilities_setRequestMaskBits(uint32_t bitsToSet){
	return xEventGroupSetBits(
	_requestMaskHandle,
	bitsToSet
	);
}

EventBits_t maskUtilities_setRunningMaskBits(uint32_t bitsToSet){
	return xEventGroupSetBits(
	_runningMaskHandle,
	bitsToSet
	);
}

void maskUtilities_setInterruptMaskBitsFromISR(uint32_t bitsToSet){
	BaseType_t xHigherPriorityTaskWoken, xResult;

	xHigherPriorityTaskWoken = pdFALSE;
	xResult = xEventGroupSetBitsFromISR(
	_interruptMaskHandle,
	bitsToSet,
	&xHigherPriorityTaskWoken
	);

	if( xResult != pdFAIL )
	{
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}

void maskUtilities_setRequestMaskBitsFromISR(uint32_t bitsToSet){
	BaseType_t xHigherPriorityTaskWoken, xResult;

	xHigherPriorityTaskWoken = pdFALSE;
	xResult = xEventGroupSetBitsFromISR(
	_requestMaskHandle,
	bitsToSet,
	&xHigherPriorityTaskWoken
	);

	if( xResult != pdFAIL )
	{
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}

EventBits_t maskUtilities_setInterruptMaskBits(uint32_t bitsToSet){
	return xEventGroupSetBits(
	_interruptMaskHandle,
	bitsToSet
	);
}

EventBits_t maskUtilities_clearRequestMaskBits(uint32_t bitsToClear){
	return xEventGroupClearBits(
	_requestMaskHandle,
	bitsToClear
	);
}

EventBits_t maskUtilities_clearRunningMaskBits(uint32_t bitsToClear){
	return xEventGroupClearBits(
	_runningMaskHandle,
	bitsToClear
	);
}

EventBits_t maskUtilities_clearInterruptMaskBits(uint32_t bitsToClear){
	return xEventGroupClearBits(
	_interruptMaskHandle,
	bitsToClear
	);
}

EventBits_t maskUtilities_clearInterruptMask(){
	return xEventGroupClearBits(
	_interruptMaskHandle,
	INTERRUPT_BIT_MOTION_DETECTED
	| INTERRUPT_BIT_TAP_DETECTED
	| INTERRUPT_BIT_DOUBLE_TAP_DETECTED
	| INTERRUPT_BIT_RING_INDICATOR
	| INTERRUPT_BIT_BUTTON
	| INTERRUPT_BIT_CALENDAR_MODE_WAKEUP
	);
}

EventBits_t maskUtilities_clearRunningMask(){
	return xEventGroupClearBits(
	_runningMaskHandle,
	RUNNING_BIT_LISTENER
	| RUNNING_BIT_MODE
	| RUNNING_BIT_ON_DEMAND
	| RUNNING_BIT_SENDER
	| RUNNING_BIT_GPS_MANAGER
	| RUNNING_BIT_LED_INSTRUCTION
	| RUNNING_BIT_GSM_TASK
	);
}


EventBits_t maskUtilities_clearRequestMask(){
	return xEventGroupClearBits(
	_requestMaskHandle,
	REQUEST_BIT_START_MODE_FROM_LPC
	| REQUEST_BIT_START_MODE_FROM_RC
	| REQUEST_BIT_ON_DEMAND
	| REQUEST_BIT_SENDER
	| REQUEST_BIT_GPS_MODE
	| REQUEST_BIT_GPS_ON_DEMAND
	| REQUEST_BIT_GPS_SOS
	| REQUEST_BIT_GPS_EXPIRED
	| REQUEST_BIT_SEEKIOS_TURN_OFF
	| REQUEST_BIT_SEEKIOS_TURN_ON
	);
}

EventBits_t maskUtilities_requestGPSFromMode(){
	return maskUtilities_setRequestMaskBits(REQUEST_BIT_GPS_MODE);
}

EventBits_t maskUtilities_stopRequestGPSFromMode(){
	return maskUtilities_clearRequestMaskBits(REQUEST_BIT_GPS_MODE);
}

EventBits_t maskUtilities_getRequestMask(){
	return xEventGroupGetBits(_requestMaskHandle );
}

EventBits_t maskUtilities_getRunningMask(){
	return xEventGroupGetBits(_runningMaskHandle);
}

EventBits_t maskUtilities_getRunningMaskFromISR(){
	return xEventGroupGetBitsFromISR(_runningMaskHandle);
}

EventBits_t maskUtilities_getInterruptMask(){
	return xEventGroupGetBits(_interruptMaskHandle );
}

bool maskUtilities_isRunningMaskCleared(){
	EventBits_t runningMask = maskUtilities_getRunningMask();
	return !((runningMask & RUNNING_BIT_ON_DEMAND)\
	|| (runningMask & RUNNING_BIT_LISTENER)\
	|| (runningMask & RUNNING_BIT_SENDER)\
	|| (runningMask & RUNNING_BIT_SOS)\
	|| (runningMask & RUNNING_BIT_GSM_TASK)\
	|| (runningMask & RUNNING_BIT_MODE)\
	|| (runningMask & RUNNING_BIT_GPS_MANAGER)\
	|| (runningMask & RUNNING_BIT_LED_INSTRUCTION)\
	|| (runningMask & RUNNING_BIT_BUTTON_ACTION)
	|| (runningMask & RUNNING_BIT_HTTP_SESSION_TASK));
}

bool maskUtilities_isRunningBitCleared(EventBits_t runningBits){
	EventBits_t runningMask = maskUtilities_getRunningMask();
	return !(runningMask & runningBits);
}

bool maskUtilities_isGSMRequested(EventBits_t requestMask){
	return (requestMask & REQUEST_BIT_GSM_NEW_INSTRUCTION);
}

/* For the GPS Manager : returns true if one of the GPS request flag is set */
bool maskUtilities_isGpsRequested(){
	EventBits_t requestMask = maskUtilities_getRequestMask();
	return (requestMask & (REQUEST_BIT_GPS_ON_DEMAND | REQUEST_BIT_GPS_MODE | REQUEST_BIT_GPS_SOS)) != 0;
}

bool maskUtilities_areAllMaskCleared(){
	uint32_t requestMask = maskUtilities_getRequestMask();
	uint32_t runningMask = maskUtilities_getRunningMask();
	uint32_t interruptMask = maskUtilities_getInterruptMask();
	return requestMask == 0 && interruptMask == 0 && runningMask== 0;
}

bool maskUtilities_useLastParsedConfig(){
	EventBits_t requestMask = maskUtilities_getRequestMask();
	if(requestMask & REQUEST_BIT_START_MODE_FROM_LPC){
		return true;
	}
	return false;
}

bool maskUtilities_useRunningConfig(){
	EventBits_t requestMask = maskUtilities_getRequestMask();
	if(requestMask & REQUEST_BIT_START_MODE_FROM_RC){
		return true;
	}
	return false;
}

void maskUtilities_printRequestMask()
{
	EventBits_t requestMask = maskUtilities_getRequestMask();
	USARTManager_printUsbWait("RequestMask :\r\n");
	if(requestMask & REQUEST_BIT_START_MODE_FROM_LPC	 ) USARTManager_printUsbWait("\tREQUEST_BIT_START_MODE_FROM_LPC	\r\n");
	if(requestMask & REQUEST_BIT_START_MODE_FROM_RC		 ) USARTManager_printUsbWait("\tREQUEST_BIT_START_MODE_FROM_RC	\r\n");
	if(requestMask & REQUEST_BIT_ON_DEMAND				 ) USARTManager_printUsbWait("\tREQUEST_BIT_ON_DEMAND				\r\n");
	if(requestMask & REQUEST_BIT_SOS					 ) USARTManager_printUsbWait("\tREQUEST_BIT_SOS					\r\n");
	if(requestMask & REQUEST_BIT_SENDER					 ) USARTManager_printUsbWait("\tREQUEST_BIT_SENDER				\r\n");
	if(requestMask & REQUEST_BIT_GPS_MODE				 ) USARTManager_printUsbWait("\tREQUEST_BIT_GPS_MODE				\r\n");
	if(requestMask & REQUEST_BIT_GPS_ON_DEMAND			 ) USARTManager_printUsbWait("\tREQUEST_BIT_GPS_ON_DEMAND			\r\n");
	if(requestMask & REQUEST_BIT_GPS_SOS				 ) USARTManager_printUsbWait("\tREQUEST_BIT_GPS_SOS				\r\n");
	if(requestMask & REQUEST_BIT_GPS_EXPIRED			 ) USARTManager_printUsbWait("\tREQUEST_BIT_GPS_EXPIRED			\r\n");
	if(requestMask & REQUEST_BIT_GSM_NEW_INSTRUCTION	 ) USARTManager_printUsbWait("\tREQUEST_BIT_GSM_NEW_INSTRUCTION	\r\n");
	if(requestMask & REQUEST_BIT_SEEKIOS_TURN_OFF		 ) USARTManager_printUsbWait("\tREQUEST_BIT_SEEKIOS_TURN_OFF		\r\n");
	if(requestMask & REQUEST_BIT_SEEKIOS_TURN_ON		 ) USARTManager_printUsbWait("\tREQUEST_BIT_SEEKIOS_TURN_ON		\r\n");
	if(requestMask & REQUEST_BIT_BATTERY_LEDS			 ) USARTManager_printUsbWait("\tREQUEST_BIT_BATTERY_LEDS			\r\n");
	if(requestMask & REQUEST_BIT_CHECK_BATTERY	 ) USARTManager_printUsbWait("\tREQUEST_BIT_CHECK_BATTERY_LEVEL	\r\n");
	if(requestMask & REQUEST_BIT_TEST_FUNCTIONALITIES	 ) USARTManager_printUsbWait("\tREQUEST_BIT_TEST_FUNCTIONALITIES	\r\n");
	if(requestMask & REQUEST_BIT_HTTP_SESSION_EXPIRED	 ) USARTManager_printUsbWait("\tREQUEST_BIT_HTTP_SESSION_EXPIRED	\r\n");
}

void maskUtilities_printRunningMask()
{
	EventBits_t runningMask = maskUtilities_getRunningMask();
	USARTManager_printUsbWait("RunningMask :\r\n");
	if(runningMask & RUNNING_BIT_LISTENER				) USARTManager_printUsbWait("\tRUNNING_BIT_LISTENER				\r\n");
	if(runningMask & RUNNING_BIT_MODE					) USARTManager_printUsbWait("\tRUNNING_BIT_MODE					\r\n");
	if(runningMask & RUNNING_BIT_ON_DEMAND				) USARTManager_printUsbWait("\tRUNNING_BIT_ON_DEMAND				\r\n");
	if(runningMask & RUNNING_BIT_SOS					) USARTManager_printUsbWait("\tRUNNING_BIT_SOS					\r\n");
	if(runningMask & RUNNING_BIT_SENDER				) USARTManager_printUsbWait("\tRUNNING_BIT_SENDER					\r\n");
	if(runningMask & RUNNING_BIT_GPS_MANAGER			) USARTManager_printUsbWait("\tRUNNING_BIT_GPS_MANAGER			\r\n");
	if(runningMask & RUNNING_BIT_BUTTON_ACTION		) USARTManager_printUsbWait("\tRUNNING_BIT_BUTTON_MANAGER			\r\n");
	if(runningMask & RUNNING_BIT_GSM_TASK		) USARTManager_printUsbWait("\tRUNNING_BIT_GSM_POWERING_ON		\r\n");
	if(runningMask & RUNNING_BIT_LED_INSTRUCTION		) USARTManager_printUsbWait("\tRUNNING_BIT_LED_INSTRUCTION		\r\n");
	if(runningMask & RUNNING_BIT_TEST_FUNCTIONALITIES	) USARTManager_printUsbWait("\tRUNNING_BIT_TEST_FUNCTIONALITIES	\r\n");
	if(runningMask & RUNNING_BIT_HTTP_SESSION_TASK		) USARTManager_printUsbWait("\tRUNNING_BIT_HTTP_SESSION_TASK		\r\n");
	if(runningMask & RUNNING_BIT_TEST_BLE				) USARTManager_printUsbWait("\tRUNNING_BIT_TEST_BLE				\r\n");
	if(runningMask & RUNNING_BIT_TEST_GPS				) USARTManager_printUsbWait("\tRUNNING_BIT_TEST_GPS				\r\n");
	if(runningMask & RUNNING_BIT_TEST_CALENDARS		) USARTManager_printUsbWait("\tRUNNING_BIT_TEST_CALENDARS			\r\n");
	if(runningMask & RUNNING_BIT_TEST_GSM				) USARTManager_printUsbWait("\tRUNNING_BIT_TEST_GSM				\r\n");
}

void maskUtilities_printInterruptMask()
{
	EventBits_t interruptMask = maskUtilities_getInterruptMask();
	USARTManager_printUsbWait("InterruptMask :\r\n");
	if(interruptMask & INTERRUPT_BIT_MOTION_DETECTED		) USARTManager_printUsbWait("\tINTERRUPT_BIT_MOTION_DETECTED		\r\n");
	if(interruptMask & INTERRUPT_BIT_TAP_DETECTED			) USARTManager_printUsbWait("\tINTERRUPT_BIT_TAP_DETECTED			\r\n");
	if(interruptMask & INTERRUPT_BIT_DOUBLE_TAP_DETECTED	) USARTManager_printUsbWait("\tINTERRUPT_BIT_DOUBLE_TAP_DETECTED	\r\n");
	if(interruptMask & INTERRUPT_BIT_RING_INDICATOR			) USARTManager_printUsbWait("\tINTERRUPT_BIT_SMS_WAKE_UP			\r\n");
	if(interruptMask & INTERRUPT_BIT_BUTTON					) USARTManager_printUsbWait("\tINTERRUPT_BIT_BUTTON				\r\n");
	if(interruptMask & INTERRUPT_BIT_CALENDAR_MODE_WAKEUP	) USARTManager_printUsbWait("\tINTERRUPT_BIT_CALENDAR_MODE_WAKEUP	\r\n");
}

/* Sets the request bit turn seekios off, and also clears the turn on seekios request bit */
void maskUtilities_setRequestTurnOffSeekios()
{
	maskUtilities_setRequestMaskBits(REQUEST_BIT_SEEKIOS_TURN_OFF);
	maskUtilities_clearRequestMaskBits(REQUEST_BIT_SEEKIOS_TURN_ON);
}

/* Sets the request bit turn seekios off, and also clears the turn on seekios request bit */
void maskUtilities_setRequestTurnOnSeekios()
{
	maskUtilities_setRequestMaskBits(REQUEST_BIT_SEEKIOS_TURN_ON);
	maskUtilities_clearRequestMaskBits(REQUEST_BIT_SEEKIOS_TURN_OFF);
}