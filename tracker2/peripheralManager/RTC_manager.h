/*
 * RTC_manager.h
 *
 * Created: 22/04/2016 13:44:05
 *  Author: Luc
 */ 


#ifndef RTC_MANAGER_H_
#define RTC_MANAGER_H_

#include <peripheralManager/USART_manager.h>
#include <seekiosCore/seekios.h>
#include <stdbool.h>
#include <FreeRTOS.h>
#include <task.h>
#include <sgs/port_sgs.h>
#include <time.h>

void RTCManager_init(void);
void RTCManager_setAlarm(struct calendar_alarm *const alarm, calendar_cb_alarm_t callback);
bool RTCManager_removeAlarm(struct calendar_alarm *const alarm);
void RTCManager_setAlarmModeDelay(int delayMs);
bool RTCManager_setCalendarFromMessage(char* message);
void RTCManager_setCalendarInitialized(void);
void RTCManager_initDateTime(struct tm* datePtr);
void RTCManager_copyDateTime(struct tm* resultPtr, struct tm toCopy);
bool RTCManager_isCalendarInitialized(void);
void RTCManager_example(void);
bool RTCManager_setDelayedCallback(int delaySec, struct calendar_alarm* alarm, calendar_cb_alarm_t callbackFunc);
void RTCManager_getCurrentTm(struct tm* resultPtr);
bool RTCManager_getCurrentDateTime(struct calendar_date_time *currentDateTime);
void RTCManager_setAlarmFromTm(struct tm* tmDatePtr,struct calendar_alarm *alarm, calendar_cb_alarm_t callback);
void RTCManager_removeSecondsFromTm(struct tm* sourcePtr, struct tm* resultPtr, uint32_t secondsToRemove);
time_t RTCManager_getCurrentTimestamp(void);
void RTCManager_printTime(struct calendar_date_time *dt);
void RTCManager_printTimeFromTimestamp(uint32_t timestamp);
void RTCManager_calendarAlarmsExample1(void);
void RTCManager_calendarAlarmsExample2(void);
bool RTCManager_isAlarmScheduled(struct calendar_alarm* alarmPtr);
void RTCManager_updateSeekiosDatetime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);

#endif /* RTC_MANAGER_H_ */