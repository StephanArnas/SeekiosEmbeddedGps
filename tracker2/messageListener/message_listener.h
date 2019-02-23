#ifndef MESSAGE_LISTENER_H_
#define MESSAGE_LISTENER_H_

#include <string.h>
#include <seekiosCore/seekios.h>
#include <peripheralManager/GPS_manager.h>
#include <peripheralManager/battery_manager.h>
#include <statusManager/status_manager.h>
#include <seekiosManager/seekios_info_manager.h>
#include <peripheralManager/GSMManager.h.>
#include <peripheralManager/RTC_manager.h>
#include <tools/led_utilities.h>
#include <FreeRTOS.h>
#include <task.h>

#define MAX_NUMBER_STORED_SMS 10 // TODO : définir la valeur réelle de cette variable

/* Wait time for the next listening if getting the instructions from the cloud failed */
/* Wait time IN SECONDS */
#define LISTENER_FAIL_WAIT_1 15
#define LISTENER_FAIL_WAIT_2 15
#define LISTENER_FAIL_WAIT_3 15
#define LISTENER_FAIL_WAIT_4 60
#define LISTENER_FAIL_WAIT_5 60
#define LISTENER_FAIL_WAIT_6 60
#define LISTENER_FAIL_WAIT_7 180
#define LISTENER_FAIL_WAIT_8 180
#define LISTENER_FAIL_WAIT_9 180

void task_messageListener(void* param);
void messageListener_init(void);
void messageListener_clearAlarmListenerRetryIfSet(void);
void messageListener_printListenerRetryAlarm(void);
bool messageListener_isListenerRetryAlarmScheduled(void);

#endif /* MESSAGE_LISTENER_H_ */