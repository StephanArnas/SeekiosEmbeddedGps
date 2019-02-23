/*
 * task_management_utilities.h
 *
 * Created: 03/08/2016 14:54:38
 *  Author: Luc pfffffffff
 */ 


#ifndef TASK_MANAGEMENT_UTILITIES_H_
#define TASK_MANAGEMENT_UTILITIES_H_

#include <seekiosManager/mask_utilities.h>
#include <FreeRTOS.h>
#include <stdint.h>
#include <event_groups.h>
#include <task.h>
#include <messageListener/message_listener.h>
#include <peripheralManager/button_manager.h>
#include <peripheralManager/GSMManager.h>
#include <modesManager/modes.h>
#include <peripheralManager/LED_manager.h>
#include <tests/powerTests.h>
#include <peripheralManager/BLE_manager_adapted.h>
#include <seekiosManager/seekios_info_manager.h>

#define TASK_DEFAULT_PRIORITY	tskIDLE_PRIORITY+1

/* Modes tasks */
#define STACK_SIZE_ONDEMAND_TASK			120 //(configMINIMAL_STACK_SIZE*5)
#define STACK_SIZE_SOS						110 //(configMINIMAL_STACK_SIZE*5)
#define STACK_SIZE_TRACKING_TASK			226 //(configMINIMAL_STACK_SIZE*7)
#define STACK_SIZE_ZONE_TASK				260 //(configMINIMAL_STACK_SIZE*9)
#define STACK_SIZE_IN_TIME_TASK				300 //(configMINIMAL_STACK_SIZE*7)
#define STACK_SIZE_DAILY_TRACKING_TASK		300
#if (BLE_ACTIVATED==1)
	#define STACK_SIZE_DONT_MOVE_TASK			395 //(configMINIMAL_STACK_SIZE*7) // What consumes : the BLE Verification and HASH
#else
	#define STACK_SIZE_DONT_MOVE_TASK			140 //(configMINIMAL_STACK_SIZE*7)
#endif

/* Peripherals/Utilities tasks */
#define STACK_SIZE_SEEKIOS_MANAGER_TASK			190
#define STACK_SIZE_POWER_TESTS_TASK				500
#define STACK_SIZE_LISTENING_TASK				290
#define STACK_SIZE_GPS_TASK						180
#define STACK_SIZE_SENDING_TASK					264
#define STACK_SIZE_BUTTON_TASK					100
#define STACK_SIZE_LED_TASK						70
#define STACK_SIZE_MODULE_TASK					120
#define STACK_SIZE_RING_INTERRUPT_TASK			95
#define STACK_SIZE_CS_TASK						95
#define STACK_SIZE_HTTP_SESSION_TASK			110
#define STACK_SIZE_CHECK_NETWORK_STATUS_TASK	120
#define STACK_SIZE_CHECK_BATTERY_LEVEL			130
#define STACK_SIZE_SIGNIFICANT_MOTION_DETECTION	110

/* Testing tasks */
#define STACK_SIZE_FUNCTIONALITIES_TEST	200
#define STACK_SIZE_TEST_BLE_TASK		300
#define STACK_SIZE_TEST_GSM_TASK		150
#define STACK_SIZE_TEST_GPS_TASK		150
#define STACK_SIZE_TEST_CALENDARS_TASK	100
#define STACK_SIZE_TEST_MONITOR			500


BaseType_t taskManagementUtilities_suspendTasksAndCreateTask(TaskFunction_t pxTaskCode, const char * const pcName, const uint16_t usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask);
void taskManagementUtilities_suspendRunningTasks(void);
void taskManagementUtilities_resumeRunningTasks(void);
void FreeRTOSOverlay_taskDelete(TaskHandle_t taskToDelete);
BaseType_t taskManagementUtilities_startOnDemandTask(void);
BaseType_t taskManagementUtilities_startSOSTask(void);
BaseType_t taskManagementUtilities_startInTimeModeTask(void);
BaseType_t taskManagementUtilities_startFollowMeModeTask(void);
BaseType_t taskManagementUtilities_startTrackingModeTask(void);
BaseType_t taskManagementUtilities_startDontMoveModeMotionDetectedTask(void);
BaseType_t taskManagementUtilities_startZoneModeTask(void);
BaseType_t taskManagementUtilities_startDailyTrackingModeTask(void);
BaseType_t taskManagementUtilities_startGPSTask(void);
BaseType_t taskManagementUtilities_startSenderTask(void);
BaseType_t taskManagementUtilities_startListenerTask(void);
BaseType_t taskManagementUtilities_startGSMTask(E_GSM_TASK_PARAM gsmTaskParam);
void taskManagementUtilities_terminateRunningTaskMode(void);
void taskManagementUtilities_terminateUsbMonitorTask(void);
void taskManagementUtilities_startButtonManagerTask(void);
void taskManagementUtilities_startLedManagerTask(void);
void taskManagementUtilities_startHttpSessionOpenerTask(void);
void taskManagementUtilities_startHttpSessionExpiredTask(void);
#if (POWER_TESTS_ACTIVATED==1)
void taskManagementUtilities_startPowerTestTask(void);
#endif
void taskManagementUtilities_startBLETestTask(void);
void taskManagementUtilities_startGPSTestTask(void);
void taskManagementUtilities_startCalendarsTestTask(void);
void taskManagementUtilities_startGSMTestTask(void);
void taskManagementUtilities_startCheckNetworkStatusTask(void);
BaseType_t taskManagementUtilities_startRingInterruptTask(void);
BaseType_t taskManagementUtilities_startCheckBatteryValueTask(void);
BaseType_t taskManagementUtilities_startSignificantMoveDetectionTask(void);
void taskManagementUtilities_terminateGSMTestTask(void);
void taskManagementUtilities_terminateGPSTestTask(void);
void taskManagementUtilities_terminateCalendarTestTask(void);
BaseType_t taskManagementUtilities_startTestMonitorTask(void);

#endif /* TASK_MANAGEMENT_UTILITIES_H_ */