#ifndef MODES_TOOLKIT_H_
#define MODES_TOOLKIT_H_

#include <string.h>
#include <peripheralManager/USART_manager.h>
#include <statusManager/status_manager.h>
#include <messageSender/message_sender.h>
#include <tools/string_helper.h>
#include <peripheralManager/GPS_manager.h>
#include <peripheralManager/bma222_adapted.h>

void modesToolkit_wrapMessageAndSend(SatelliteCoordinate *gpsDataPtr, E_MESSAGE_TYPE messageType, void (*callbackFunction)(void), uint32_t modeId);//, time_t timestamp);
void test_setAlarmModeDelay(int delayMs);
void modesToolkit_removeTrackingPeriodAlarm(void);
void modesToolkit_removeGpsAlarm(void);
void modesToolkit_setAlarmModeDelay(uint32_t delayMs);
void modesToolkit_setAlarmGpsDelay(uint32_t delayMs);
void modesToolkit_startModeSlopeDetection(void);
void modesToolkit_printTrackingPeriodAlarm(void);
bool modesUtilities_isTrackingPeriodAlarmScheduled(void);
bool modesUtilities_isGpsAlarmScheduled(void);
bool modesToolkit_didSeekiosMovedSinceLastTrackig(void);
void modesToolkit_setSeekiosMovedSinceLastCycle(void);
void modesToolkit_startTrackingSlopeDetection(void);
void modesToolkit_setSeekiosMovedSinceLastCycle(void);
void modesToolkit_stopTrackingSlopeDetection(void);
bool modesToolkit_didSeekiosMovedSinceLastCycle(void);
bool modesToolkit_isPositionBetterThanPreviousOne(SatelliteCoordinate *gpsDataPtr);
void modesToolkit_getPreviousPosition(SatelliteCoordinate *gpsDataPtr);
void modesToolkit_setPreviousPosition(SatelliteCoordinate *gpsDataPtr);
void modesToolkit_sleepGpsUntilNextCycle(uint32_t timeToWaitms);
void modesToolkit_sleepModeUntilNextCycle(uint32_t timeToWaitMs);
void modesToolkit_printGpsAlarm(void);
#endif /* MODES_TOOLKIT_H_ */