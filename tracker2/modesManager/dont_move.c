#include <modesManager/modes.h>
static void app_seekiosauth_keyreceived(uint8_t* key);
static void app_seekiosauthwaiter_state_changed(bool state);
static void app_seekiosauthwaiter_adv_report(at_ble_status_t status);
static void dontMoveConnectionTimeoutCallback(TimerHandle_t xTimer);
static bool checkUserProximity(void);
static void setDontMoveRASState(void);

static TimerHandle_t _dontMoveBLEConnectionTimeoutTimer;

static bool _dontMoveAdvertisingTimeout;
static bool _dontMoveConnectionTimeout;
static bool _isUserDisconnected;
static bool _isUserAuthenticated;

void dontMove_init(){
	_dontMoveAdvertisingTimeout = false;
	_isUserDisconnected = false;
	_isUserAuthenticated = false;
	_dontMoveBLEConnectionTimeoutTimer = xTimerCreate("Dont move BLE Connection timer timeout", DONT_MOVE_BLE_CONNECTION_TIMEOUT, pdFALSE, (void*)0, dontMoveConnectionTimeoutCallback);
}

void modes_dontMoveModeMotionDetected(void* param){
	UNUSED(param);
	maskUtilities_setRunningMaskBits(RUNNING_BIT_MODE);

	/* Get mode params */
	vTaskDelay(500);
	uint8_t state =			statusManager_getRunningStatusState();
	uint8_t refreshRate =	statusManager_getRunningRefreshRate();

	if(state == DONT_MOVE_STATE_MOTION_DETECTED){
		USARTManager_printUsbWait("Seekios moved !\r\n");
		#if (ADDITIONAL_LED_INSTRUCTIONS==1)
		ledUtilities_runMovementSuccessLedInstruction();
		#endif

		bool notifySeekiosMoved = true;

		#if (BLE_ACTIVATED == 1)
		notifySeekiosMoved = !checkUserProximity(); // TODO : we should check proximity if the user decided to activate this extra-option
		#endif // BLE_ACTIVATED==1

		if(notifySeekiosMoved)
		{
			powerStateManager_disablePowerSavingIfEnabled();
			modesToolkit_wrapMessageAndSend(NULL, MESSAGE_TYPE_DONT_MOVE, NULL, statusManager_getRunningConfigModeId());//, RTCManager_getCurrentTimestamp());
			volatile BaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
			UNUSED(wm);
			if(refreshRate > 0)
			{
				state = DONT_MOVE_STATE_TRACKING;
				statusManager_setRunningConfigStatusState(state);
			}
			else
			{
				state = DONT_MOVE_STATE_SUSPEND;
				statusManager_setRunningConfigStatusState(state);
			}
		}
		else
		{
			volatile BaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
			UNUSED(wm);
			USARTManager_printUsbWait("User close to the Seekios. Canceling alert.\r\n");
			setDontMoveRASState();
		}
	}


	if(state == DONT_MOVE_STATE_TRACKING){
		tracking_infiniteTracking(MESSAGE_TYPE_DONT_MOVE_TRACKING, refreshRate);
	}

	maskUtilities_clearRunningMaskBits(RUNNING_BIT_MODE);
	FreeRTOSOverlay_taskDelete(NULL);
}

static void setDontMoveRASState()
{
	statusManager_setRunningConfigStatusState(DONT_MOVE_STATE_RAS);
	IMUManager_powerModeLowPower();
	IMUManager_startSlopeDetection();
}

bool checkUserProximity(){
	
	BLEManager_wakeUp();

	dontmoveble_configure(NULL);

	dmb_register_seekiosauth_handler(app_seekiosauth_keyreceived);
	dmb_register_state_handler(app_seekiosauthwaiter_state_changed);
	dmb_register_adv_report_handler(app_seekiosauthwaiter_adv_report);

	_dontMoveAdvertisingTimeout = false;
	_isUserDisconnected = false;
	_isUserAuthenticated = false;
	_dontMoveConnectionTimeout = false;
	while(!_dontMoveAdvertisingTimeout && !_isUserDisconnected)
	{
		ble_event_task();

		if(_dontMoveConnectionTimeout)
		{
			dontmoveble_disconnect();
		}
		vTaskDelay(1000);
	}

	volatile BaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
	UNUSED(wm);
	if(xTimerIsTimerActive(_dontMoveBLEConnectionTimeoutTimer))
	{
		xTimerStop(_dontMoveBLEConnectionTimeoutTimer, 0);
	}
	vTaskDelay(1000);
	dontmoveble_deconfigure();
	BLEManager_sleep();
	return _isUserAuthenticated;
}

static void app_seekiosauth_keyreceived(uint8_t* key){
	USARTManager_printUsbWait("Key received : ");
	for(uint8_t i=0;i<AUTH_KEY_LENGTH;i++)
	{
		USARTManager_printByte(key[i]);
	}
	USARTManager_printUsbWait("\r\n");
	_isUserAuthenticated = authentication_authenticateUser(key);
	if(_isUserAuthenticated){
		USARTManager_printUsbWait("Authentication SUCCESSFUL !\r\n");
	}
	else
	{
		USARTManager_printUsbWait("Authentication FAILURE !!!\r\n");
	}
	dontmoveble_disconnect();
}

static void app_seekiosauthwaiter_state_changed(bool state){
	if(state == true) // connected
	{
		xTimerStart(_dontMoveBLEConnectionTimeoutTimer, 0);
	}
	else if (state == false) // disconnected
	{
		_isUserDisconnected = true;
	}
}

static void app_seekiosauthwaiter_adv_report(at_ble_status_t status){
	if(status == AT_BLE_GAP_TIMEOUT){ // adv timeout stops the advertisement automatically
		USARTManager_printUsbWait("Advertising timeout !");
		_dontMoveAdvertisingTimeout = true;
	}
}

static void dontMoveConnectionTimeoutCallback(TimerHandle_t xTimer){
	_dontMoveConnectionTimeout = true;
	at_ble_event_user_defined_post(NULL);
	UNUSED(xTimer);
}