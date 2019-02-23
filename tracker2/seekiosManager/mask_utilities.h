#ifndef MASK_UTILITIES_H_
#define MASK_UTILITIES_H_

#include <stdint.h>
#include <FreeRTOS.h>
#include <event_groups.h>
#include <stdbool.h>

#define INTERRUPT_BIT_MOTION_DETECTED		(1 << 0)
#define INTERRUPT_BIT_TAP_DETECTED			(1 << 1)
#define INTERRUPT_BIT_DOUBLE_TAP_DETECTED	(1 << 2)
#define INTERRUPT_BIT_RING_INDICATOR		(1 << 3)
#define INTERRUPT_BIT_BUTTON				(1 << 4)
#define INTERRUPT_BIT_CALENDAR_MODE_WAKEUP	(1 << 5)
#define INTERRUPT_BIT_CALENDAR_BATTERY_LOG	(1 << 6)
#define INTERRUPT_BIT_GPS_WAKEUP			(1 << 7)

// Limité à 24
#define REQUEST_BIT_START_MODE_FROM_LPC				(1 << 0)// start mode using last parsed config
#define REQUEST_BIT_START_MODE_FROM_RC				(1 << 1)// start mode using running config
#define REQUEST_BIT_ON_DEMAND						(1 << 2)
#define REQUEST_BIT_SOS								(1 << 3)
#define REQUEST_BIT_SENDER							(1 << 4)
#define REQUEST_BIT_GPS_MODE						(1 << 5)
#define REQUEST_BIT_GPS_ON_DEMAND					(1 << 6)
#define REQUEST_BIT_GPS_SOS							(1 << 7)
#define REQUEST_BIT_GPS_EXPIRED						(1 << 8)
#define REQUEST_BIT_GSM_NEW_INSTRUCTION				(1 << 10)
#define REQUEST_BIT_SEEKIOS_TURN_OFF				(1 << 11)
#define REQUEST_BIT_SEEKIOS_TURN_ON					(1 << 12)
#define REQUEST_BIT_BATTERY_LEDS					(1 << 13)
#define REQUEST_BIT_CHECK_BATTERY					(1 << 14)
#define REQUEST_BIT_TEST_FUNCTIONALITIES			(1 << 15)
#define REQUEST_BIT_HTTP_SESSION_EXPIRED			(1 << 16)
#define REQUEST_BIT_CHECK_NETWORK_STATUS			(1 << 17)
#define REQUEST_BIT_UPDATE_CALENDAR_PROCESSES		(1 << 18)
#define REQUEST_BIT_LISTENER						(1 << 19)
#define REQUEST_BIT_SIGNIFICANT_MOTION_DETECTED		(1 << 20)
#define REQUEST_BIT_USB_DEBUGGING					(1 << 21) // USB Monitor + USB output on COM port

#define RUNNING_BIT_LISTENER						(1 << 0)
#define RUNNING_BIT_MODE							(1 << 1)
#define RUNNING_BIT_ON_DEMAND						(1 << 2)
#define RUNNING_BIT_SOS								(1 << 3)
#define RUNNING_BIT_SENDER							(1 << 4)
#define RUNNING_BIT_GPS_MANAGER						(1 << 5)
#define RUNNING_BIT_BUTTON_ACTION					(1 << 6)
#define RUNNING_BIT_GSM_TASK						(1 << 7)
#define RUNNING_BIT_LED_INSTRUCTION					(1 << 8)
#define RUNNING_BIT_HTTP_SESSION_TASK				(1 << 10)
#define RUNNING_BIT_TEST_BLE						(1 << 11)
#define RUNNING_BIT_TEST_GPS						(1 << 12)
#define RUNNING_BIT_TEST_CALENDARS					(1 << 13)
#define RUNNING_BIT_TEST_GSM						(1 << 14)
#define RUNNING_BIT_CHECK_NETWORK_STATUS			(1 << 16)
#define RUNNING_BIT_RING_INDICATOR_HANDLE			(1 << 17)
#define RUNNING_BIT_TEST_FUNCTIONALITIES			(1 << 18)
#define RUNNING_BIT_CHECK_BATTERY					(1 << 19)
#define RUNNING_BIT_SGNIFICANT_MOVE_DETECTION		(1 << 20)
#define RUNNING_BIT_USB_MONITOR						(1 << 22)

void maskUtilities_init(void);
void maskUtilities_setInterruptMaskBitsFromISR(uint32_t bitsToSet);
void maskUtilities_setRequestMaskBitsFromISR(uint32_t bitsToSet);
EventBits_t maskUtilities_setRequestMaskBits(uint32_t bitsToSet);
EventBits_t maskUtilities_setRunningMaskBits(uint32_t bitsToSet);
EventBits_t maskUtilities_setInterruptMaskBits(uint32_t bitsToSet);
EventBits_t maskUtilities_clearRequestMaskBits(uint32_t bitsToClear);
EventBits_t maskUtilities_clearRunningMaskBits(uint32_t bitsToClear);
EventBits_t maskUtilities_clearInterruptMaskBits(uint32_t bitsToClear);
EventBits_t maskUtilities_clearInterruptMask(void);
EventBits_t maskUtilities_clearRunningMask(void);
EventBits_t maskUtilities_clearRequestMask(void);
EventBits_t maskUtilities_getRequestMask(void);
EventBits_t maskUtilities_getRunningMask(void);
EventBits_t maskUtilities_getRunningMaskFromISR(void);
EventBits_t maskUtilities_getInterruptMask(void);
EventBits_t maskUtilities_requestHttpSessionFromSender(void);
bool maskUtilities_areAllMaskCleared(void);
bool maskUtilities_isRunningMaskCleared(void);
bool maskUtilities_isGSMRequested(EventBits_t requestMask);
bool maskUtilities_isHttpSessionRequested(void);
bool maskUtilities_isHttpSessionUnused(void);
void maskUtilities_clearHttpRequestsBits(void);
void maskUtilities_requestHttpSessionListener(void);
void maskUtilities_requestHttpSessionSender(void);
void maskUtilities_setHttpSessionListenerUnused(void);
void maskUtilities_setHttpSessionSenderUnused(void);
EventBits_t maskUtilities_requestGPSFromMode(void);
EventBits_t maskUtilities_stopRequestGPSFromMode(void);
bool maskUtilities_isGpsRequested(void);
bool maskUtilities_useLastParsedConfig(void);
bool maskUtilities_useRunningConfig(void);
bool maskUtilities_isRunningBitCleared(EventBits_t runningBits);
void maskUtilities_printRequestMask(void);
void maskUtilities_printRunningMask(void);
void maskUtilities_printInterruptMask(void);
void maskUtilities_setRequestTurnOffSeekios(void);
void maskUtilities_setRequestTurnOnSeekios(void);

#endif /* MASK_UTILITIES_H_ */