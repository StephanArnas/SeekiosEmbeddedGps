#include <seekiosManager/seekios_manager.h>

static void init(void);
static void handleOnDemandRequest(void);
static void printEndMessage(void);
static void handleGPSRequest(void);
static void handleMotionDetection(void);
static void handleOutputMessageRequest(void);
static void handleRingIndicatorInterrupt(void);
static void handlePowerOnGSM(void);
static void handleSosRequest(void);
static void hibernateIfInactive(void);
static void handleModeWakeUpCalendarInterrupt(void);
static bool terminateRunningMode();
static void turnOnSeekios(void);
static void turnOffSeekios(void);
static void	waitForTasksTermination(void);
static void showBatteryLevel(void);
static void handleCheckBatteryValueRequest(void);
static void handleHttpSessionExpiration(void);
static void handleGPSUnused(void);
static void handleGPSReused(void);
static void handleGPSExpired(void);
static void runProductionTests(void);
static void terminateGPSUse(void);
static void showNewModeStarting(E_MODE_STATUS newStatus);
static void handleUpdateCalendarProcesses(void);
static void handleCheckNetworkStatusRequest(void);
static void handleUSBPlug(void);
static void handleCheckLPC(void);
static void handleRunModeFromRC(void);
static bool runMode(ModeStatus *modeStatusPtr);
static void purgeMasks(void);
static void restoreMasks(void);
static void handleListenerRequest(void);
static void handleTestFunctionalitiesRequest(void);
static void printConfiguredAlarmTimes(void);
static void handlePowerOnGSM(void);
static void handlePowerOffGSM(void);
static void showDebugData(void);
static void handleRequestSignificantMotionDetection(void);
static void handleUSBDebuggingRequest(void);
static void terminateUSBDebugging(void);
static void raiseRunningModeEvent(E_MODE_STATUS statusToRun, uint8_t statusStateToRun);
static void gsmLifeCycle(void);
static void gpsLifeCycle(void);


static bool _allMaskClearedDoubleCheck;

static size_t fh;
static BaseType_t wm;
/*TODO: delete line*/volatile char sentenceBuffer[128];
static void init(){
	seekiosManagerStarted = true;

	//#if (DEBUG_MODE == 1)
	//while(1) // Test du watchdog
	//{
		//USARTManager_printUsbWait("Sleeping !\r\n");
		//sleep_deep();
		//kickTheDog();
	//}
	//#endif


	dataflashManager_eraseUsedPages();
	taskManagementUtilities_startButtonManagerTask();
	taskManagementUtilities_startLedManagerTask();
	bool isSeekiosFirstRun = seekiosInfoManager_isSeekiosFirstRun();
	powerStateManager_init(isSeekiosFirstRun);

	if(isSeekiosFirstRun)
	{
		runProductionTests();
	}
	seekiosInfoManager_initSeekiosInfo();
	USARTManager_printUsbWait("--- SEEKIOS MANAGER STARTING ---\r\n");

	#if (USB_ACTIVATED == 1)
	maskUtilities_setRequestMaskBits(REQUEST_BIT_USB_DEBUGGING);
	#endif

	_allMaskClearedDoubleCheck = false;
}

void task_seekiosManager(void* param){
	UNUSED(param);
	init();

	while(1){
		
		volatile EventBits_t requestMask		 = maskUtilities_getRequestMask();
		volatile EventBits_t runningMask		 = maskUtilities_getRunningMask();
		volatile EventBits_t interruptMask		 = maskUtilities_getInterruptMask();
		
		batteryManager_updateChargeStatus();

		if(requestMask & REQUEST_BIT_CHECK_BATTERY && !(runningMask & RUNNING_BIT_CHECK_BATTERY)){
			USARTManager_printUsbWait("Battery Check Task will RUN SOON\r\n");
			handleCheckBatteryValueRequest();
		}

		if(maskUtilities_getRequestMask() & REQUEST_BIT_SEEKIOS_TURN_OFF){
			turnOffSeekios();
		} else if(maskUtilities_getRequestMask() & REQUEST_BIT_SEEKIOS_TURN_ON){
			turnOnSeekios();
		}

		if((requestMask & REQUEST_BIT_USB_DEBUGGING) && !(runningMask & RUNNING_BIT_USB_MONITOR))
		{
			handleUSBDebuggingRequest();
		}
		else if(!(requestMask & REQUEST_BIT_USB_DEBUGGING) && (runningMask & RUNNING_BIT_USB_MONITOR))
		{
			terminateUSBDebugging();
		}

		if(requestMask & REQUEST_BIT_UPDATE_CALENDAR_PROCESSES)
		{
			handleUpdateCalendarProcesses();
		}

		#if (DEBUG_MODE==1)
		if(requestMask & REQUEST_BIT_BATTERY_LEDS){
			showDebugData();
		}
		#endif

		if(interruptMask & INTERRUPT_BIT_RING_INDICATOR && !(runningMask & RUNNING_BIT_RING_INDICATOR_HANDLE))
		{
			handleRingIndicatorInterrupt();
		} 

		gsmLifeCycle();

		if(powerStateManager_isSeekiosOn()){

			if((requestMask & REQUEST_BIT_ON_DEMAND) && !(runningMask & RUNNING_BIT_ON_DEMAND)){
				handleOnDemandRequest();
			}

			if(((requestMask & REQUEST_BIT_LISTENER) || (requestMask & REQUEST_BIT_SENDER))
			&& !((runningMask & RUNNING_BIT_LISTENER) || (runningMask & RUNNING_BIT_SENDER)))
			{
				if(requestMask & REQUEST_BIT_LISTENER){
					handleListenerRequest();
				}
				else if(requestMask & REQUEST_BIT_SENDER)
				{
					handleOutputMessageRequest();
				}
				
			}
			
			if((requestMask & REQUEST_BIT_CHECK_NETWORK_STATUS) 
			&& !(runningMask & RUNNING_BIT_CHECK_NETWORK_STATUS))
			{
				handleCheckNetworkStatusRequest();
			}

			if(interruptMask & INTERRUPT_BIT_CALENDAR_MODE_WAKEUP){
				handleModeWakeUpCalendarInterrupt();
			}

			gpsLifeCycle();

			if(interruptMask & INTERRUPT_BIT_MOTION_DETECTED )
			{
				if(!(runningMask & RUNNING_BIT_SGNIFICANT_MOVE_DETECTION))
				{
					handleMotionDetection();
				}
				else
				{
					maskUtilities_clearInterruptMaskBits(INTERRUPT_BIT_MOTION_DETECTED);
				}
			}

			if(requestMask & REQUEST_BIT_SIGNIFICANT_MOTION_DETECTED)
			{
				handleRequestSignificantMotionDetection();
			}

			if(requestMask & REQUEST_BIT_START_MODE_FROM_LPC)
			{
				handleCheckLPC();
			}
			else if (requestMask & REQUEST_BIT_START_MODE_FROM_RC)
			{
				handleRunModeFromRC();
			}
			
			if ((requestMask & REQUEST_BIT_HTTP_SESSION_EXPIRED)
			&& !(runningMask & RUNNING_BIT_HTTP_SESSION_TASK)
			&& !(runningMask & RUNNING_BIT_LISTENER)
			&& !(runningMask & RUNNING_BIT_SENDER))
			{
				handleHttpSessionExpiration();
			}

			if(requestMask & REQUEST_BIT_BATTERY_LEDS){
				showBatteryLevel();
			}
		} // if SeekiosOn


		if((requestMask & REQUEST_BIT_SOS) && !(runningMask & RUNNING_BIT_SOS)){
			handleSosRequest();
		}

		if(requestMask & REQUEST_BIT_TEST_FUNCTIONALITIES){
			handleTestFunctionalitiesRequest(); // TODO : test and uncomment
		}
		
		handleUSBPlug();

		fh = xPortGetFreeHeapSize();
		wm = uxTaskGetStackHighWaterMark(NULL);

		if(maskUtilities_areAllMaskCleared() && !_allMaskClearedDoubleCheck){
			_allMaskClearedDoubleCheck = true;
			vTaskDelay(1000);
			continue;
		}
		else if(!maskUtilities_areAllMaskCleared())
		{
			_allMaskClearedDoubleCheck = false;
			vTaskDelay(1000);
			continue;
		}

		hibernateIfInactive();
	}
}

/* GSM Management :
- Seekios ON -> GSM ON
- Seekios Off USB plugged -> GSM ON
- Seekios Off USB unplugged -> GSM OFF
*/
static void gsmLifeCycle()
{
	EventBits_t runningMask = maskUtilities_getRunningMask();
	EventBits_t requestMask = maskUtilities_getRequestMask();
	
	if(!(runningMask & RUNNING_BIT_GSM_TASK))
	{
		if(powerStateManager_isSeekiosOn())
		{
			if(powerStateManager_isPowerSavingEnabled()) // TODO : check la batterie (donc allumer le GSM ?) plus régulièrement lorsque le seekios est en charge ?
			{
				bool isGsmNeeded = batteryManager_isBatteryCharging()\
				|| messageSender_outboxHasMessages()\
				|| (requestMask & REQUEST_BIT_LISTENER)\
				|| (runningMask & RUNNING_BIT_LISTENER)\
				|| (runningMask & RUNNING_BIT_SENDER)\
				|| (requestMask & REQUEST_BIT_CHECK_BATTERY);
								
				if(!GSMManager_isModuleStarted() && isGsmNeeded)
				{
					USARTManager_printUsbWait("GSM needed\r\n");
					handlePowerOnGSM();
				}
				else if(GSMManager_isModuleStarted() && !isGsmNeeded)
				{
					handlePowerOffGSM();
				}
			}
			else if(!GSMManager_isModuleStarted())
			{
				handlePowerOnGSM();
			}
		}
		else if(!powerStateManager_isSeekiosOn())
		{
			// TODO : allumer le GSM à chaque battery check, et diminuer le TDR du battery check, pour ne pas allumer le seekios tout le temps, et économiser de la batterie
			if(!GSMManager_isModuleStarted() && batteryManager_isBatteryCharging()) // Seekios off, but charging : powering on gsm
			{
				handlePowerOnGSM();
			}
			else if(GSMManager_isModuleStarted() && !batteryManager_isBatteryCharging()) // Seekios off and not charging : powering off gsm
			{
				handlePowerOffGSM();
			}
		}
	}
}

static void gpsLifeCycle()
{
	EventBits_t runningMask = maskUtilities_getRunningMask();
	EventBits_t requestMask = maskUtilities_getRequestMask();

	if( !GPSManager_isGPSOn()
	&& maskUtilities_isGpsRequested()
	&& !(runningMask & RUNNING_BIT_GPS_MANAGER))
	{
		handleGPSRequest();
	}
	else if(GPSManager_isGPSOn() && !(runningMask & RUNNING_BIT_GPS_MANAGER))
	{
		if (requestMask & REQUEST_BIT_GPS_EXPIRED)
		{
			handleGPSExpired();
		}
		else if(maskUtilities_isGpsRequested()
		&& !(runningMask & RUNNING_BIT_GPS_MANAGER)){
			handleGPSReused();
		}
		else if (!maskUtilities_isGpsRequested()
		&& !GPSManager_isGPSExpirationAlarmScheduled())
		{
			handleGPSUnused();
		}
	}
}

static void handleRequestSignificantMotionDetection()
{
	ModeStatus runningStatus;
	statusManager_getRunningStatus(&runningStatus);

	if(runningStatus.status == MODE_STATUS_DONT_MOVE && runningStatus.state == DONT_MOVE_STATE_RAS)
	{
		statusManager_setRunningConfigStatusState(DONT_MOVE_STATE_MOTION_DETECTED);
		maskUtilities_setRequestMaskBits(REQUEST_BIT_START_MODE_FROM_RC);
	}
	else if(runningStatus.status == MODE_STATUS_TRACKING
	|| (runningStatus.status == MODE_STATUS_DONT_MOVE && runningStatus.state == DONT_MOVE_STATE_TRACKING)
	|| (runningStatus.status == MODE_STATUS_ZONE && runningStatus.state == ZONE_STATE_TRACKING))
	{
		modesToolkit_setSeekiosMovedSinceLastCycle();
	}
	else if(runningStatus.status == MODE_STATUS_ZONE && runningStatus.state == ZONE_STATE_WAIT_SIGNIFICANT_MOTION)
	{
		statusManager_setRunningConfigStatusState(ZONE_STATE_CHECK_POSITION);
		maskUtilities_setRequestMaskBits(REQUEST_BIT_START_MODE_FROM_RC);
	}
	
	maskUtilities_clearRequestMaskBits(REQUEST_BIT_SIGNIFICANT_MOTION_DETECTED);
}

/* When the USB is plugged, we : show the current charge level.
We change the USB charge status every 30 seconds only : to avoid weird led showing when the battery is going up and down all the time
This function is called by the seekiosManager every seconds, so the secondsCount is updating to 1 each time the function is called.*/
static void handleUSBPlug()
{
	static uint32_t currentlyRunningChargeLevelInstructionID = 0;
	static uint32_t currentlyRunningChargeLevelPercentageTier = LED_BATTERY_TIER_NONE;

	if(batteryManager_isBatteryCharging())
	{
		E_LED_BATTERY_TIER batteryPercentageTier = batteryManager_getLedPercentageTier();
		if(!LEDManager_isInstructionRunning(currentlyRunningChargeLevelInstructionID)
		|| (batteryPercentageTier != currentlyRunningChargeLevelPercentageTier))
		{
			currentlyRunningChargeLevelInstructionID = ledUtilities_runBatteryChargeInstruction(batteryPercentageTier);
			currentlyRunningChargeLevelPercentageTier = batteryPercentageTier;
		}
	}
	else
	{	
		LEDManager_stopCurrentlyRunningInstructionId(currentlyRunningChargeLevelInstructionID);
	}
}

static void handleUSBDebuggingRequest()
{
	USARTManager_activateUSBOutput();
	USBManager_start();
	taskManagementUtilities_startTestMonitorTask();
}

static void terminateUSBDebugging()
{
	taskManagementUtilities_terminateUsbMonitorTask();
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_USB_MONITOR);
	USBManager_stop();
	USARTManager_deactivateUSBOutput();
}

/* This request is triggered once the Calendar is updated from the server.
Once the calendar is updated from the server, this handle has to sets the alarms
with the newly received calendar date.
It also disables the power sending, thus sending a PSD message */
static void handleUpdateCalendarProcesses()
{

	/* There is a bug with the RTC : when you change the calendar alarm, you can not set an alarm A and remove an old alarm B if the new Calendar date is in a time AFTER B.
	You have to remove B before setting the alarm A.*/
	bool rescheduleHttpExpiration = false;
	bool rescheduleNetworkCheck = false;
	bool rescheduleBatteryCheck = false;

	// Reschedule the http expiration
	
	if(WDTManager_isDogKickingAlarmScheduled())
	{
		WDTManager_removeDogKickingAlarm();
	}
	
	if(GSMManager_isSessionExpirationAlarmScheduled())
	{
		rescheduleHttpExpiration = true;
		GSMManager_removeHttpSessionExpirationAlarm();
	}

	if(GSMManager_isNetworkCheckAlarmScheduled())
	{
		rescheduleNetworkCheck = true;
		GSMManager_removeNetworkCheckAlarm();
	}

	if(batteryManager_isBatteryCheckAlarmScheduled())
	{
		rescheduleBatteryCheck = true;
		batteryManager_removeBatteryCheckAlarm();
	}

	if(rescheduleHttpExpiration)
	{
		GSMManager_scheduleHttpSessionExpirationAlarm(statusManager_getGPRSExpirationTime());
	}
	if(rescheduleNetworkCheck)
	{
		maskUtilities_setRequestMaskBits(REQUEST_BIT_CHECK_NETWORK_STATUS);
	}
	if(rescheduleBatteryCheck)
	{
		maskUtilities_setRequestMaskBits(REQUEST_BIT_CHECK_BATTERY);
	}

	maskUtilities_clearRequestMaskBits(REQUEST_BIT_UPDATE_CALENDAR_PROCESSES);
}

static void handleTestFunctionalitiesRequest()
{
	USARTManager_printUsbWait("Turning OFF Seekios before tests...");
	turnOffSeekios();
	functionalitiesTest_runTests(false);
	maskUtilities_clearRequestMaskBits(REQUEST_BIT_TEST_FUNCTIONALITIES);
}

static void handleHttpSessionExpiration(){
	USARTManager_printUsbWait("HTTP Session EXPIRED !\r\n");
	taskManagementUtilities_startHttpSessionExpiredTask();
}

/* Updates Checks if the battery level is critical. If yes, we try to turn the GSM on and send a message. */
static void handleCheckBatteryValueRequest(){
	taskManagementUtilities_startCheckBatteryValueTask();
}

/* Shows to the user the level of the battery with the leds */
static void showBatteryLevel(){
	E_LED_BATTERY_TIER batteryPercentageTier = batteryManager_getLedPercentageTier();
	ledUtilities_runBatteryLedInstruction(batteryPercentageTier);
	maskUtilities_clearRequestMaskBits(REQUEST_BIT_BATTERY_LEDS);
}

static void showDebugData()
{
	maskUtilities_printRequestMask();
	maskUtilities_printRunningMask();
	maskUtilities_printInterruptMask();
	USARTManager_printUsbWait("GSM : Started - Lastly checked network : ");
	char buff[10];
	GSMManager_getLastCheckNetwork(buff);
	USARTManager_printUsbWait(buff);
	USARTManager_printUsbWait("\r\n");
	maskUtilities_clearRequestMaskBits(REQUEST_BIT_BATTERY_LEDS);
	batterymanager_printLastSamples();
	struct calendar_date_time currentDateTime;
	calendar_get_date_time(&CALENDAR_X, &currentDateTime);
	RTCManager_printTime(&currentDateTime);
	USARTManager_printUsbWait("\r\n");
	printConfiguredAlarmTimes();
	USARTManager_printUsbWait("\r\n");
	#if (ACTIVATE_WRONG_NMEA_FRAME_LOGS == 1)
	GPSManager_printWrongNMEALog();
	#endif
	#if (ACTIVATE_BATTERY_LOG == 1)
	batteryManager_printBatteryLogs();
	#endif
	#if (ACTIVATE_GPS_LOGS == 1)
	GPSManager_printGPSLogs();
	#endif
}

static void printConfiguredAlarmTimes()
{	
	USARTManager_printUsbWait("Configured Alarms :\r\n");

	if(messageListener_isListenerRetryAlarmScheduled())
	{
		messageListener_printListenerRetryAlarm();
	}
	if(messageSender_isSenderRetryAlarmScheduled())
	{
		messageSender_printSenderRetryAlarm();
	}
	if( modesUtilities_isTrackingPeriodAlarmScheduled())
	{
		modesToolkit_printTrackingPeriodAlarm();
	}
	if(modesUtilities_isGpsAlarmScheduled())
	{
		modesToolkit_printGpsAlarm();	
	}
	if( GSMManager_isSessionExpirationAlarmScheduled())
	{
		GSMManager_printSessionExpirationAlarm();
	}
	if( GPSManager_isGPSExpirationAlarmScheduled())
	{
		GPSManager_printGPSExpirationAlarm();
	}
	if( batteryManager_isBatteryCheckAlarmScheduled() )
	{
		batteryManager_printBatteryCheckAlarm();
	}
	if( GSMManager_isNetworkCheckAlarmScheduled())
	{
		GSMManager_printNetworkCheckAlarm();
	}

}

static void turnOnSeekios(){
	
	/* Powering off the seekios disables the power saving */
	restoreMasks();
	USARTManager_printUsbWait("--- SEEKIOS POWERED ON---\r\n");
	powerStateManager_setSeekiosOn();
	maskUtilities_clearRequestMaskBits(REQUEST_BIT_SEEKIOS_TURN_ON);
}

static void turnOffSeekios(){
	uint32_t turningOffInstructionId = ledUtilities_runCurrentlyTurningOffLedInstruction();
	terminateRunningMode();
	waitForTasksTermination();
	terminateGPSUse();
	batteryManager_removeBatteryCheckAlarm();
	LEDManager_stopCurrentlyRunningInstructionId(turningOffInstructionId);
	powerStateManager_disablePowerSavingIfEnabled();
	maskUtilities_clearRequestMaskBits(REQUEST_BIT_SEEKIOS_TURN_OFF);
	purgeMasks();	
	USARTManager_printUsbWait("--- SEEKIOS POWERED OFF---\r\n");
	powerStateManager_setSeekiosOff();
}

/* Registers all the remaining pending actions that will have to be performed when the Seekios will wake up again, 
and removes all of them from the interrupt, request and configured alarm masks
When the Seekios wakes up again, it should call the function restoreMasks*/
static void purgeMasks()
{
	volatile EventBits_t requestMask		 = maskUtilities_getRequestMask();
	//volatile EventBits_t runningMask		 = maskUtilities_getRunningMask();
	volatile EventBits_t interruptMask		 = maskUtilities_getInterruptMask();


	if(messageListener_isListenerRetryAlarmScheduled()
	|| requestMask & REQUEST_BIT_LISTENER)
	{		
		USARTManager_printUsbWait("Storing PENDING_ACTION_BIT_LISTENER bit.\r\n");
		messageListener_clearAlarmListenerRetryIfSet();
		maskUtilities_clearRequestMaskBits(REQUEST_BIT_LISTENER);
		powerStateManager_setPendingActionBits(PENDING_ACTION_BIT_LISTENER);
	}

	if(interruptMask & INTERRUPT_BIT_RING_INDICATOR)
	{
		USARTManager_printUsbWait("Storing PENDING_ACTION_BIT_RING_INDICATOR_INTERRUPT bit.\r\n");
		maskUtilities_clearInterruptMaskBits(INTERRUPT_BIT_RING_INDICATOR);
		powerStateManager_setPendingActionBits(PENDING_ACTION_BIT_RING_INDICATOR_INTERRUPT);
	}
	
	if(requestMask & REQUEST_BIT_SENDER
	|| messageSender_isSenderRetryAlarmScheduled())
	{
		USARTManager_printUsbWait("Storing PENDING_ACTION_BIT_SENDER bit.\r\n");
		messageSender_clearAlarmSenderRetry();
		maskUtilities_clearRequestMaskBits(REQUEST_BIT_SENDER);
		powerStateManager_setPendingActionBits(PENDING_ACTION_BIT_SENDER);
	}

	if(requestMask & REQUEST_BIT_START_MODE_FROM_LPC)
	{
		USARTManager_printUsbWait("Storing PENDING_ACTION_BIT_CHECK_LPC bit.\r\n");
		maskUtilities_clearRequestMaskBits(REQUEST_BIT_START_MODE_FROM_LPC);
		powerStateManager_setPendingActionBits(PENDING_ACTION_BIT_CHECK_LPC);
	}

	if(requestMask & REQUEST_BIT_BATTERY_LEDS)
	{
		maskUtilities_clearRequestMaskBits(REQUEST_BIT_BATTERY_LEDS);
	}
	
	if(interruptMask & INTERRUPT_BIT_MOTION_DETECTED) // When we wake up again, we will do the motion detection again
	{
		maskUtilities_clearInterruptMaskBits(INTERRUPT_BIT_MOTION_DETECTED);
	}

}

/* Restores the masks that were purged with the function purgeMasks */
static void restoreMasks()
{
	volatile EventBits_t pendingActionsMask = powerStateManager_getPendingActionMask();

	if(pendingActionsMask & PENDING_ACTION_BIT_CHECK_LPC)
	{
		USARTManager_printUsbWait("Restoring PENDING_ACTION_BIT_CHECK_LPC bit.\r\n");
		maskUtilities_setRequestMaskBits(REQUEST_BIT_START_MODE_FROM_LPC);
	}
	else
	{
		USARTManager_printUsbWait("Restoring REQUEST_BIT_START_MODE_FROM_RC bit.\r\n");
		maskUtilities_setRequestMaskBits(REQUEST_BIT_START_MODE_FROM_RC);
	}

	if(pendingActionsMask & PENDING_ACTION_BIT_LISTENER)
	{
		USARTManager_printUsbWait("Restoring REQUEST_BIT_LISTENER bit.\r\n");
		maskUtilities_setRequestMaskBits(REQUEST_BIT_LISTENER);
	}

	if(pendingActionsMask & PENDING_ACTION_BIT_RING_INDICATOR_INTERRUPT)
	{
		USARTManager_printUsbWait("Restoring INTERRUPT_BIT_SMS_WAKE_UP bit.\r\n");
		maskUtilities_setInterruptMaskBits(INTERRUPT_BIT_RING_INDICATOR);
	}

	if(pendingActionsMask & PENDING_ACTION_BIT_SENDER)
	{
		USARTManager_printUsbWait("Restoring REQUEST_BIT_SENDER bit.\r\n");
		maskUtilities_setRequestMaskBits(REQUEST_BIT_SENDER);
	}

	powerStateManager_clearPendingActionsMask();
}

/* Terminates the GPS and all activity related to it */
static void terminateGPSUse()
{
	handleGPSExpired();
	GPSManager_removeGPSExpirationAlarm();
}

/* Attend la fin de l'exécution des tâches SOS, ON DEMAND, GSM powering on Sender, Http Session Task, Listener, GPS Manager, SOS*/
static void	waitForTasksTermination(){
	while(!maskUtilities_isRunningBitCleared(
	RUNNING_BIT_LISTENER
	| RUNNING_BIT_MODE
	| RUNNING_BIT_ON_DEMAND
	| RUNNING_BIT_SOS
	| RUNNING_BIT_SENDER
	| RUNNING_BIT_GPS_MANAGER
	| RUNNING_BIT_BUTTON_ACTION
	| RUNNING_BIT_GSM_TASK
	| RUNNING_BIT_HTTP_SESSION_TASK
	| RUNNING_BIT_CHECK_NETWORK_STATUS
	| RUNNING_BIT_RING_INDICATOR_HANDLE
	| RUNNING_BIT_TEST_FUNCTIONALITIES
	| RUNNING_BIT_CHECK_BATTERY
	| RUNNING_BIT_SGNIFICANT_MOVE_DETECTION
	));

	 testMonitor_raiseEvent(EVENT_TASKS_ACTIVES_STOPPED);
}

static void hibernateIfInactive(){
	if(maskUtilities_areAllMaskCleared() && !(powerStateManager_isPowerSavingEnabled() && GSMManager_isModuleStarted())){ // TODO : la condition sur le power saving pourrait être remplacée par une alarm
		while(maskUtilities_areAllMaskCleared())
		{
			USARTManager_printUsbWait("HIBERNATION...\r\n");
			WDTManager_setDogKickingAlarm();
			seekiosManager_hibernate();
			vTaskDelay(200);
		}
		USARTManager_printUsbWait("SEEKIOS WAKE FROM HIBERNATION !\r\n");
	}
}

/*
This function enters the MCU in deep sleep/hibernation state, deactivating many MCU functionnalities
*/
void seekiosManager_hibernate(){

	USARTManager_printUsbWait("deep sleep\r\n");
	sleep_deep(); // TODO : put deep sleep instead of normal sleep

	vTaskDelay(1000);
}

static void handleModeWakeUpCalendarInterrupt(){
	ModeStatus runningStatus;
	statusManager_getRunningStatus(&runningStatus);

	if(runningStatus.status == MODE_STATUS_TRACKING
	|| runningStatus.status == MODE_STATUS_ZONE
	|| (runningStatus.status == MODE_STATUS_DONT_MOVE && runningStatus.state == DONT_MOVE_STATE_TRACKING))
	{
		maskUtilities_setRequestMaskBits(REQUEST_BIT_START_MODE_FROM_RC);
	}

	maskUtilities_clearInterruptMaskBits(INTERRUPT_BIT_CALENDAR_MODE_WAKEUP);
}

static void handleOnDemandRequest(){
	if(taskManagementUtilities_startOnDemandTask() == pdPASS)
	{
		maskUtilities_clearRequestMaskBits(REQUEST_BIT_ON_DEMAND);
		raiseRunningModeEvent(MODE_STATUS_ON_DEMAND, 0);
	}
}

static void handleSosRequest(){
	
	if(!seekiosInfoManager_isSeekiosPeered() || !statusManager_isSOSAuthorized())
	{
		maskUtilities_clearRequestMaskBits(REQUEST_BIT_SOS);
	}
	else
	{
		powerStateManager_disablePowerSavingIfEnabled();
		
		if(!powerStateManager_isSeekiosOn())
		{
			turnOnSeekios();
		}
		
		if(powerStateManager_isSeekiosOn())
		{
			if(taskManagementUtilities_startSOSTask() == pdPASS){
				maskUtilities_clearRequestMaskBits(REQUEST_BIT_SOS);
				raiseRunningModeEvent(MODE_STATUS_SOS, 0);
			}
		}
	}
}

static void handleMotionDetection(){
	taskManagementUtilities_startSignificantMoveDetectionTask();
}

static void handleGPSRequest(){
	if(taskManagementUtilities_startGPSTask() == pdPASS){
		GPSManager_powerOnGPS();
	}
}

/* While the GPS manager stops itself because there is no GPS needs,
we set an alarm to wake up the seekios and determine that it has expired */
static void handleGPSUnused(){
	if(powerStateManager_isPowerSavingEnabled())
	{
		maskUtilities_setRequestMaskBits(REQUEST_BIT_GPS_EXPIRED);
	}
	else
	{
		GPSManager_scheduleGPSExpirationAlarm();
	}
}

static void handleGPSReused(){
	GPSManager_removeGPSExpirationAlarm();
	taskManagementUtilities_startGPSTask();
}

static void handleGPSExpired(){
	USARTManager_printUsbWait("GPS Expired !\r\n");
	GPSManager_powerOffGPS();
	maskUtilities_clearRequestMaskBits(REQUEST_BIT_GPS_EXPIRED);
}

static void handleOutputMessageRequest(){
	if(taskManagementUtilities_startSenderTask()==pdPASS){}
}

static void handleListenerRequest()
{
	if(taskManagementUtilities_startListenerTask() != pdPASS)
	{
		USARTManager_printUsbWait("Failure creating listener task....\r\n");
		UBaseType_t wm = xPortGetFreeHeapSize();
		USARTManager_printUsbWait("Remaining HEAP : ");
		char heapBuf[10];
		stringHelper_intToString(wm, (unsigned char*)heapBuf);
		USARTManager_printUsbWait(heapBuf);
		USARTManager_printUsbWait("\r\n");
	}
}

static void handleRingIndicatorInterrupt()
{
	
	if(powerStateManager_isPowerSavingEnabled() || !powerStateManager_isSeekiosOn())
	{			
		USARTManager_printUsbWait("Received SMS While off or power saving : storing it in pending.\r\n");
		USARTManager_printUsbWait("Storing PENDING_ACTION_BIT_RING_INDICATOR_INTERRUPT bit.\r\n");
		powerStateManager_setPendingActionBits(PENDING_ACTION_BIT_RING_INDICATOR_INTERRUPT);
		maskUtilities_clearInterruptMaskBits(INTERRUPT_BIT_RING_INDICATOR);
	}
	else
	{
		taskManagementUtilities_startRingInterruptTask();
	}
}

static void handlePowerOnGSM(){
	taskManagementUtilities_startGSMTask(GSM_TASK_PARAM_POWER_ON_NORM_FUNCT);
}

static void handlePowerOffGSM(){
	taskManagementUtilities_startGSMTask(GSM_TASK_PARAM_POWER_OFF);
}

static void handleCheckNetworkStatusRequest()
{
	if(!powerStateManager_isPowerSavingEnabled())
	{
		taskManagementUtilities_startCheckNetworkStatusTask();
	}
}

static void showNewModeStarting(E_MODE_STATUS newStatus)
{
	switch(newStatus)
	{
		case MODE_STATUS_DONT_MOVE:
		USARTManager_printUsbWait("-=[ START DONT MOVE ]=-\r\n");
		break;
		case MODE_STATUS_TRACKING:
		USARTManager_printUsbWait("-=[ START TRACKING ]=-\r\n");
		break;
		case MODE_STATUS_WAITING:
		USARTManager_printUsbWait("-=[ START WAITING ]=-\r\n");
		break;
		case MODE_STATUS_ZONE:
		USARTManager_printUsbWait("-=[ START ZONE ]=-\r\n");
		break;
		default:
		USARTManager_printUsbWait("-=[ UNKNOWN MODE STARTING ]=-\r\n");
		break;
	}
}

static void handleCheckLPC()
{
	if(statusManager_isLastParsedModeNew() || statusManager_isModeCanceled())
	{
		ModeStatus lastParsedSeekiosStatus;
		statusManager_getLastParsedStatus(&lastParsedSeekiosStatus);
		PowerSavingConfig powerSavingConfig;
		statusManager_getLastParsedPowerSavingConfig(&powerSavingConfig);
		
		if(terminateRunningMode()){
			printEndMessage();
		}
		showNewModeStarting(lastParsedSeekiosStatus.status);
		statusManager_initRunningMode();
		
		/* Run or disable power saving */
		if(powerSavingConfig.isPowerSavingEnabled)
		{
			powerStateManager_enablePowerSaving(powerSavingConfig.powerSavingCultureHoursOffset);
		}
		else {
			powerStateManager_disablePowerSavingIfEnabled();
		}
		
		runMode(&lastParsedSeekiosStatus);
	}
	else
	{
		maskUtilities_clearRequestMaskBits(REQUEST_BIT_START_MODE_FROM_LPC);
	}
}

static void handleRunModeFromRC()
{
	ModeStatus runningStatus;
	statusManager_getRunningStatus(&runningStatus);
	// Power saving state can only change  from a last parsed config or from a mode itself
	runMode(&runningStatus);
}

static bool runMode(ModeStatus *modeStatusPtr)
{
	/* The runModeTask boolean will be true if we have to either start a new mode (from the last parsed config, can be NONE/WAITING too)
	either start a task of a running mode */
	
	bool runSuccess = true;

	if(powerStateManager_isSeekiosOn())
	{
		if(modeStatusPtr->status == MODE_STATUS_TRACKING)
		{
			if(taskManagementUtilities_startTrackingModeTask() == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
			{
				runSuccess = false;
			}
		}
		else if(modeStatusPtr->status == MODE_STATUS_ZONE)
		{
			if(modeStatusPtr->state == ZONE_STATE_WAIT_SIGNIFICANT_MOTION)
			{
				IMUManager_powerModeLowPower();
				IMUManager_startSlopeDetection();
			}
			else if(modeStatusPtr->state == ZONE_STATE_CHECK_POSITION)
			{
				if(taskManagementUtilities_startZoneModeTask() == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
				{
					runSuccess = false;
				}
			}
			else if(modeStatusPtr->state == ZONE_STATE_TRACKING)
			{
				if(taskManagementUtilities_startZoneModeTask() == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
				{
					runSuccess = false;
				}
			}
		}
		else if(modeStatusPtr->status == MODE_STATUS_DONT_MOVE)
		{
			if(modeStatusPtr->state == DONT_MOVE_STATE_RAS)
			{
				IMUManager_powerModeLowPower();
				IMUManager_startSlopeDetection();
			}
			else if (modeStatusPtr->state == DONT_MOVE_STATE_MOTION_DETECTED)
			{
				if(taskManagementUtilities_startDontMoveModeMotionDetectedTask() == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
				{
					runSuccess = false;
				}
			}
			else if (modeStatusPtr->state == DONT_MOVE_STATE_TRACKING)
			{
				if(taskManagementUtilities_startDontMoveModeMotionDetectedTask() == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
				{
					runSuccess = false;
				}
			}
		}
		else if(modeStatusPtr->status == MODE_STATUS_NONE || modeStatusPtr->status == MODE_STATUS_WAITING)
		{
			runSuccess = true;
		}
	}

	if(runSuccess)
	{
		raiseRunningModeEvent(modeStatusPtr->status, modeStatusPtr->state);
		statusManager_setRunningConfigAndClearRequestModeConfigBits();
	}
	
	return runSuccess;
}

/* Raises the right event depending on the mode status/state that was put to run */
static void raiseRunningModeEvent(E_MODE_STATUS statusToRun, uint8_t statusStateToRun)
{
	UNUSED(statusStateToRun);
	switch(statusToRun)
	{
		case MODE_STATUS_DONT_MOVE:
		testMonitor_raiseEvent(EVENT_MODE_DONT_MODE_STARTED);
		break;
		case MODE_STATUS_TRACKING:
		testMonitor_raiseEvent(EVENT_MODE_TRACKING_STARTED);
		break;
		case MODE_STATUS_ZONE:
		testMonitor_raiseEvent(EVENT_MODE_ZONE_STARTED);
		break;
		case MODE_STATUS_WAITING:
		testMonitor_raiseEvent(EVENT_MODE_WAITING_STARTED);
		break;
		case MODE_STATUS_ON_DEMAND:
		testMonitor_raiseEvent(EVENT_MODE_ON_DEMAND_STARTED);
		break;
		case MODE_STATUS_SOS:
		testMonitor_raiseEvent(EVENT_MODE_SOS_STARTED);
		break;
		default:
		testMonitor_raiseEvent(EVENT_MODE_NONE_STARTED);
		break;
	}
}

/* Terminates the currently running mode : deletes the FreeRTOS task, removes the alarms
Returns true if a mode was stopped, false if there was no actual mode running (waiting or none)*/
static bool terminateRunningMode(){
	
	ModeStatus runningStatus;
	statusManager_getRunningStatus(&runningStatus);

	if(runningStatus.status == MODE_STATUS_NONE || runningStatus.status == MODE_STATUS_WAITING)
	{
		return false;
	}

	EventBits_t runningMask = maskUtilities_getRunningMask();

	if((runningMask & RUNNING_BIT_MODE)){
		taskManagementUtilities_terminateRunningTaskMode();
		maskUtilities_clearRunningMaskBits(RUNNING_BIT_MODE);
	}

	if(modesUtilities_isTrackingPeriodAlarmScheduled())
	{
		modesToolkit_removeTrackingPeriodAlarm();
	}
	
	if(modesUtilities_isGpsAlarmScheduled())
	{
		modesToolkit_removeGpsAlarm();
	}

	#if (BLE_ACTIVATED==1)
	BLEManager_cancelBleUse();
	#endif

	maskUtilities_clearRequestMaskBits(REQUEST_BIT_GPS_MODE);
	modesToolkit_stopTrackingSlopeDetection(); // Also stops the IMU functionalities
	return true;
}

static void printEndMessage(){
	char message[32];
	strcpy(message, "-=[ END ON ");
	ModeStatus runningStatus;
	statusManager_getRunningStatus(&runningStatus);
	switch(runningStatus.status){
		case MODE_STATUS_TRACKING:
		strcat(message, "TRACKING ]=-\r\n");
		break;
		case MODE_STATUS_ZONE:
		strcat(message, "ZONE ]=-\r\n");
		break;
		case MODE_STATUS_DONT_MOVE:
		strcat(message, "DONT MOVE ]=-\r\n");
		break;
		default:
		strcat(message,"NONE ]=-\r\n");
	}
	USARTManager_printUsbWait(message);
	//ledUtilities_runExitModeLedInstruction();
}

static void runProductionTests(){
	functionalitiesTest_runTests(true);
	seekiosInfoManager_setSeekiosNotFirstRun();
}


/* Tests the offset of the RTC :
To do such, we  : 
-get the time from the RTC
-sleep for 1 minute

Needs to be plugged in a computer with timestamp recording, to compare timestamps with the RTC results*/
void test_offset_clock()
{

	uint16_t year = 0;
	uint8_t month = 0;
	uint8_t day = 0;
	uint8_t hour = 0;
	uint8_t min = 0;
	uint8_t sec = 0;

	volatile uint8_t buff[50];

	strcpy(buff, __TIMESTAMP__);
	// buff like "Fri Dec 02 12:00:24 2016"

	char* token = strtok(buff," ");

	token = strtok(NULL," ");

	if(strstr(token, "Dec")!=NULL)
	{
		month = 12;
	}

	token = strtok(NULL," ");
	day = atoi(token);

	token = strtok(NULL,":");
	hour = atoi(token);

	token = strtok(NULL,":");
	min = atoi(token);

	token = strtok(NULL," ");
	sec = atoi(token);

	token = strtok(NULL,"\r\n ");
	year = atoi(token);
	
	RTCManager_updateSeekiosDatetime(year, month, day, hour, min, sec);
	
	uint32_t firstTimeStamp = RTCManager_getCurrentTimestamp();
	uint32_t newTimestamp = 0;
	while(1)
	{
		
		newTimestamp = RTCManager_getCurrentTimestamp();
		RTCManager_printTimeFromTimestamp(newTimestamp);
		USARTManager_printUsbWait(" - ");
		uint32_t elapsed = newTimestamp - firstTimeStamp;
		uint8_t buff[16];
		stringHelper_intToString(elapsed, buff);
		USARTManager_printUsbWait("Elapsed : ");
		USARTManager_printUsbWait((char*)buff);
		USARTManager_printUsbWait("\r\n");
		vTaskDelay(10000);
	}

}