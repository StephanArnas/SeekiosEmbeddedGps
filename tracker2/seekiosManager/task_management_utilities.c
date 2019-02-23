/*
* task_management_utilities.c
*
* Created: 03/08/2016 14:54:23
*  Author: Luc pfffffffff
*/

#include <seekiosManager/task_management_utilities.h>

static TaskHandle_t _messageListenerHandle;
static TaskHandle_t _GPSManagerHandle;
static TaskHandle_t _messageSenderHandle;
static TaskHandle_t _gsmTaskHandle;
static TaskHandle_t _ringIndicatorTaskHandle;
static TaskHandle_t	_currentlyRunningModeTaskHandle;
static TaskHandle_t _onDemandTaskHandle;
static TaskHandle_t _SOSTaskHandle;
static TaskHandle_t _httpSessionTaskHandle;
static TaskHandle_t _functionalitiesTestTaskHandle;
static TaskHandle_t _fakeNMEAFramesGenerator;
static TaskHandle_t _checkNetworkStatusTaskHandle;
static TaskHandle_t _checkBatteryTaskHandle;
static TaskHandle_t _significantMoveDetectionTaskHandle;
static TaskHandle_t _gsmTestTaskHandle;
static TaskHandle_t _gpsTestTaskHandle;
static TaskHandle_t _calendarTestTaskHandle;
static TaskHandle_t _usbMonitorTaskHandle;

BaseType_t taskManagementUtilities_suspendTasksAndCreateTask(TaskFunction_t pxTaskCode, const char * const pcName, const uint16_t usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask){
	taskManagementUtilities_suspendRunningTasks();
	volatile UBaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
	UNUSED(wm);
	vTaskDelay(200);
	taskManagementUtilities_resumeRunningTasks();
	volatile int fh = xPortGetFreeHeapSize();
	char buff[16];
	stringHelper_intToString(fh, (unsigned char*)buff);
	USARTManager_printUsbWait("Remaining HEAP Before creating ");
	USARTManager_printUsbWait(pcName);
	USARTManager_printUsbWait(" : ");
	USARTManager_printUsbWait(buff);
	USARTManager_printUsbWait("\r\n");
	return xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
}

void taskManagementUtilities_suspendRunningTasks(){
	EventBits_t runningMask = maskUtilities_getRunningMask();

	if(runningMask & RUNNING_BIT_GPS_MANAGER){
		if(eTaskGetState(_GPSManagerHandle) != eBlocked){ // we don't suspend the task if it is already in the blocked state
			vTaskSuspend(_GPSManagerHandle);
		}
	}

	if(runningMask & RUNNING_BIT_GSM_TASK){
		if(eTaskGetState(_gsmTaskHandle) != eBlocked){
			vTaskSuspend(_gsmTaskHandle);
		}
	}

	if(runningMask & RUNNING_BIT_LISTENER){
		if(eTaskGetState(_messageListenerHandle) != eBlocked){
			vTaskSuspend(_messageListenerHandle);
		}
	}

	if(runningMask & RUNNING_BIT_ON_DEMAND){
		if(eTaskGetState(_onDemandTaskHandle) != eBlocked){
			vTaskSuspend(_onDemandTaskHandle);
		}
	}

	if(runningMask & RUNNING_BIT_SENDER){
		if(eTaskGetState(_messageSenderHandle) != eBlocked){
			vTaskSuspend(_messageSenderHandle);
		}
	}

	if(runningMask & RUNNING_BIT_SOS){
		if(eTaskGetState(_SOSTaskHandle) != eBlocked){
			vTaskSuspend(_SOSTaskHandle);
		}
	}

	if(runningMask & RUNNING_BIT_MODE){
		if(eTaskGetState(_currentlyRunningModeTaskHandle) != eBlocked){
			vTaskSuspend(_currentlyRunningModeTaskHandle);
		}
	}

	if(runningMask & RUNNING_BIT_CHECK_NETWORK_STATUS){
		if(eTaskGetState(_checkNetworkStatusTaskHandle) != eBlocked){
			vTaskSuspend(_checkNetworkStatusTaskHandle);
		}
	}

	if(runningMask & RUNNING_BIT_CHECK_BATTERY){
		if(eTaskGetState(_checkBatteryTaskHandle) != eBlocked){
			vTaskSuspend(_checkBatteryTaskHandle);
		}
	}

	if(runningMask & RUNNING_BIT_RING_INDICATOR_HANDLE){
		if(eTaskGetState(_ringIndicatorTaskHandle) != eBlocked){
			vTaskSuspend(_ringIndicatorTaskHandle);
		}
	}

	if(runningMask & RUNNING_BIT_HTTP_SESSION_TASK){
		if(eTaskGetState(_httpSessionTaskHandle) != eBlocked){
			vTaskSuspend(_httpSessionTaskHandle);
		}
	}

	if(runningMask & RUNNING_BIT_SGNIFICANT_MOVE_DETECTION){
		if(eTaskGetState(_significantMoveDetectionTaskHandle) != eBlocked){
			vTaskSuspend(_significantMoveDetectionTaskHandle);
		}
	}
}

void taskManagementUtilities_resumeRunningTasks(){
	EventBits_t runningMask = maskUtilities_getRunningMask();
	if(runningMask & RUNNING_BIT_GPS_MANAGER){
		vTaskResume(_GPSManagerHandle);
	}

	if(runningMask & RUNNING_BIT_GSM_TASK){
		vTaskResume(_gsmTaskHandle);
	}

	if(runningMask & RUNNING_BIT_LISTENER){
		vTaskResume(_messageListenerHandle);
	}

	if(runningMask & RUNNING_BIT_ON_DEMAND){
		vTaskResume(_onDemandTaskHandle);
	}

	if(runningMask & RUNNING_BIT_SENDER){
		vTaskResume(_messageSenderHandle);
	}

	if(runningMask & RUNNING_BIT_SOS){
		vTaskResume(_SOSTaskHandle);
	}

	if(runningMask & RUNNING_BIT_MODE){
		vTaskResume(_currentlyRunningModeTaskHandle);
	}

	if(runningMask & RUNNING_BIT_CHECK_NETWORK_STATUS){
		vTaskResume(_checkNetworkStatusTaskHandle);
	}

	if(runningMask & RUNNING_BIT_CHECK_BATTERY){
		vTaskResume(_checkBatteryTaskHandle);
	}

	if(runningMask & RUNNING_BIT_RING_INDICATOR_HANDLE){
		vTaskResume(_ringIndicatorTaskHandle);
	}

	if(runningMask & RUNNING_BIT_HTTP_SESSION_TASK){
		vTaskResume(_httpSessionTaskHandle);
	}

	if(runningMask & RUNNING_BIT_SGNIFICANT_MOVE_DETECTION){
		vTaskResume(_significantMoveDetectionTaskHandle);
	}
}

BaseType_t taskManagementUtilities_startOnDemandTask(){
	return taskManagementUtilities_suspendTasksAndCreateTask(modes_onDemand, "m-OD", STACK_SIZE_ONDEMAND_TASK, NULL, TASK_DEFAULT_PRIORITY, &_onDemandTaskHandle);
}

BaseType_t taskManagementUtilities_startSOSTask(){
	return taskManagementUtilities_suspendTasksAndCreateTask(modes_SOS, "m-SOS", STACK_SIZE_SOS, NULL, TASK_DEFAULT_PRIORITY, &_SOSTaskHandle);
}

BaseType_t taskManagementUtilities_startTrackingModeTask(){
	return taskManagementUtilities_suspendTasksAndCreateTask(modes_trackingMode, "m-track", STACK_SIZE_TRACKING_TASK, NULL, TASK_DEFAULT_PRIORITY, &_currentlyRunningModeTaskHandle);
}

BaseType_t taskManagementUtilities_startDontMoveModeMotionDetectedTask(){
	return taskManagementUtilities_suspendTasksAndCreateTask(modes_dontMoveModeMotionDetected, "m-DM", STACK_SIZE_DONT_MOVE_TASK, NULL, TASK_DEFAULT_PRIORITY, &_currentlyRunningModeTaskHandle);
}

BaseType_t taskManagementUtilities_startZoneModeTask(){
	return taskManagementUtilities_suspendTasksAndCreateTask(modes_zoneMode, "m-zone", STACK_SIZE_ZONE_TASK, NULL, TASK_DEFAULT_PRIORITY, &_currentlyRunningModeTaskHandle);
}

BaseType_t taskManagementUtilities_startDailyTrackingModeTask(){
	return taskManagementUtilities_suspendTasksAndCreateTask(modes_dailyTrackingMode, "m-DT", STACK_SIZE_DAILY_TRACKING_TASK, NULL, TASK_DEFAULT_PRIORITY, &_currentlyRunningModeTaskHandle);
}

BaseType_t taskManagementUtilities_startGPSTask(){
	return taskManagementUtilities_suspendTasksAndCreateTask(task_GPSManager,"u-GPS",STACK_SIZE_GPS_TASK,NULL,TASK_DEFAULT_PRIORITY,&_GPSManagerHandle	);
}

BaseType_t taskManagementUtilities_startSenderTask(){
	return taskManagementUtilities_suspendTasksAndCreateTask(task_messageSender,"u-send",STACK_SIZE_SENDING_TASK,NULL,TASK_DEFAULT_PRIORITY,&_messageSenderHandle);
}

BaseType_t taskManagementUtilities_startListenerTask(){
	return taskManagementUtilities_suspendTasksAndCreateTask(task_messageListener,"u-list",STACK_SIZE_LISTENING_TASK,NULL,TASK_DEFAULT_PRIORITY,&_messageListenerHandle);
}

BaseType_t taskManagementUtilities_startGSMTask(E_GSM_TASK_PARAM gsmTaskParam){
	if (gsmTaskParam == GSM_TASK_PARAM_POWER_ON_NORM_FUNCT)
	{
		return taskManagementUtilities_suspendTasksAndCreateTask(task_GSMTask,"u-pong",STACK_SIZE_MODULE_TASK,"PON-NORMAL",TASK_DEFAULT_PRIORITY,&_gsmTaskHandle);
	}
	else if(gsmTaskParam == GSM_TASK_PARAM_POWER_OFF)
	{
		return taskManagementUtilities_suspendTasksAndCreateTask(task_GSMTask,"u-pofg",STACK_SIZE_MODULE_TASK,"POFF",TASK_DEFAULT_PRIORITY,&_gsmTaskHandle);
	}
	return pdFAIL;
}

void taskManagementUtilities_terminateGSMTestTask()
{
	vTaskSuspend(_gsmTestTaskHandle);
	FreeRTOSOverlay_taskDelete(_gsmTestTaskHandle);
	_gsmTestTaskHandle = NULL;
	
}

void taskManagementUtilities_terminateGPSTestTask()
{
	vTaskSuspend(_gpsTestTaskHandle);
	FreeRTOSOverlay_taskDelete(_gpsTestTaskHandle);
	_gpsTestTaskHandle = NULL;
	
}

void taskManagementUtilities_terminateCalendarTestTask()
{
	vTaskSuspend(_calendarTestTaskHandle);
	FreeRTOSOverlay_taskDelete(_calendarTestTaskHandle);
	_calendarTestTaskHandle = NULL;
}



void taskManagementUtilities_terminateRunningTaskMode(){
	vTaskSuspend(_currentlyRunningModeTaskHandle);
	FreeRTOSOverlay_taskDelete(_currentlyRunningModeTaskHandle);
	_currentlyRunningModeTaskHandle = NULL;
	testMonitor_raiseEvent(EVENT_TASK_MODE_STOPPED);
}

void taskManagementUtilities_terminateUsbMonitorTask(){
	vTaskSuspend(_usbMonitorTaskHandle);
	FreeRTOSOverlay_taskDelete(_usbMonitorTaskHandle);
	_usbMonitorTaskHandle = NULL;
}

void taskManagementUtilities_startButtonManagerTask(){
	if(taskManagementUtilities_suspendTasksAndCreateTask(task_buttonManager,"u-but",STACK_SIZE_BUTTON_TASK,NULL,TASK_DEFAULT_PRIORITY,NULL)==pdPASS){
		USARTManager_printUsbWait("Button manager ready.\r\n");
	}
}

void taskManagementUtilities_startLedManagerTask(){
	if(taskManagementUtilities_suspendTasksAndCreateTask(task_LEDManager,"u-led",STACK_SIZE_LED_TASK,NULL,TASK_DEFAULT_PRIORITY,NULL)==pdPASS){
		USARTManager_printUsbWait("LED manager ready.\r\n");
	}
}

void taskManagementUtilities_startHttpSessionExpiredTask(){
	taskManagementUtilities_suspendTasksAndCreateTask(task_HttpSession,"http-e",STACK_SIZE_HTTP_SESSION_TASK,"EXPIRED",TASK_DEFAULT_PRIORITY,&_httpSessionTaskHandle);
}

void taskManagementUtilities_startGPSTestTask()
{
	taskManagementUtilities_suspendTasksAndCreateTask(task_testGPS,"u-gpst", STACK_SIZE_TEST_GPS_TASK,NULL, TASK_DEFAULT_PRIORITY, &_gpsTestTaskHandle);
}

void taskManagementUtilities_startCalendarsTestTask()
{
	taskManagementUtilities_suspendTasksAndCreateTask(task_testCalendars,"u-calt", STACK_SIZE_TEST_CALENDARS_TASK,NULL, TASK_DEFAULT_PRIORITY, &_calendarTestTaskHandle);
}

void taskManagementUtilities_startGSMTestTask()
{
	taskManagementUtilities_suspendTasksAndCreateTask(task_testGSM,"u-gsmt", STACK_SIZE_TEST_GSM_TASK,NULL, TASK_DEFAULT_PRIORITY, &_gsmTestTaskHandle);
}

void taskManagementUtilities_startPowerTestTask(){
	if(xTaskCreate(
	task_powerTest,
	"u-pt",
	STACK_SIZE_POWER_TESTS_TASK,
	NULL,
	TASK_DEFAULT_PRIORITY,
	NULL
	) == pdPASS){
		vTaskStartScheduler();
	}
	else
	{
		send_debug("Failure starting power tests.\r\n");
	}
}

void taskManagementUtilities_startCheckNetworkStatusTask()
{
	taskManagementUtilities_suspendTasksAndCreateTask(task_checkNetworkStatus,"u-nets", STACK_SIZE_CHECK_NETWORK_STATUS_TASK,NULL, TASK_DEFAULT_PRIORITY, &_checkNetworkStatusTaskHandle);
}

BaseType_t taskManagementUtilities_startRingInterruptTask()
{
	return taskManagementUtilities_suspendTasksAndCreateTask(task_handleRingInterrupt,"u-hrii",STACK_SIZE_RING_INTERRUPT_TASK,NULL,TASK_DEFAULT_PRIORITY,&_ringIndicatorTaskHandle);
}

BaseType_t taskManagementUtilities_startCheckBatteryValueTask()
{
	return taskManagementUtilities_suspendTasksAndCreateTask(task_checkBattery,"u-cbl",STACK_SIZE_CHECK_BATTERY_LEVEL,NULL,TASK_DEFAULT_PRIORITY,&_checkBatteryTaskHandle);
}

BaseType_t taskManagementUtilities_startSignificantMoveDetectionTask()
{
	return taskManagementUtilities_suspendTasksAndCreateTask(task_detectSignificantMotion, "u-dsm", STACK_SIZE_SIGNIFICANT_MOTION_DETECTION, NULL, TASK_DEFAULT_PRIORITY, &_significantMoveDetectionTaskHandle);
}

BaseType_t taskManagementUtilities_startTestMonitorTask()
{
	return taskManagementUtilities_suspendTasksAndCreateTask(task_testMonitor, "t-tm", STACK_SIZE_TEST_MONITOR, NULL, TASK_DEFAULT_PRIORITY, &_usbMonitorTaskHandle);
}