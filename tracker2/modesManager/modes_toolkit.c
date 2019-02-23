#include <modesManager/modes_toolkit.h>
static void wakeUpModeCallback(struct calendar_descriptor *const calendar);
static void wakeUpGpsCallback(struct calendar_descriptor *const calendar);
static uint32_t convertMsToSec(int delayMs);

static struct calendar_alarm _wakeUpModeAlarm;		// used to wake-up modes (refresh rates, in time etc)
static struct calendar_alarm _wakeUpGpsAlarm;		// used to wake-up modes (refresh rates, in time etc)
static bool _seekiosMovedSinceLastCycle = true;
static SatelliteCoordinate _lastTrackingCoordinate;

void modesToolkit_wrapMessageAndSend(SatelliteCoordinate *gpsDataPtr, E_MESSAGE_TYPE messageType, void (*callbackFunction)(void), uint32_t modeId){//, time_t timestamp){
	OutputMessage message;
	message.messageType = messageType;
	message.timestamp = RTCManager_getCurrentTimestamp(); // TODO : what happens if there is no timestamp ?
	message.onSendSuccess = callbackFunction;
	message.modeId = modeId;
	if(gpsDataPtr != NULL){
		message.content.satelliteCoordinate.coordinate.lat = gpsDataPtr->coordinate.lat;
		message.content.satelliteCoordinate.coordinate.lon = gpsDataPtr->coordinate.lon;
		message.content.satelliteCoordinate.coordinate.alt = gpsDataPtr->coordinate.alt;
		message.content.satelliteCoordinate.hDOP = gpsDataPtr->hDOP;
	}
	messageSender_addToSendList(&message);
}

void modesToolkit_sleepModeUntilNextCycle(uint32_t timeToWaitMs){
	USARTManager_printUsbWait("MODE NEXT CYCLE : ");
	modesToolkit_setAlarmModeDelay(timeToWaitMs);
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_MODE);
	FreeRTOSOverlay_taskDelete(NULL);
}

void modesToolkit_sleepGpsUntilNextCycle(uint32_t timeToWaitms)
{
	USARTManager_printUsbWait("GPS NEXT WAKEUP : ");
	modesToolkit_setAlarmGpsDelay(timeToWaitms);
	maskUtilities_stopRequestGPSFromMode();
}


void test_setAlarmModeDelay(int delayMs){
	modesToolkit_setAlarmModeDelay(delayMs);
}

void modesToolkit_removeTrackingPeriodAlarm(void){
	RTCManager_removeAlarm(&_wakeUpModeAlarm);
}

void modesToolkit_removeGpsAlarm()
{
	RTCManager_removeAlarm(&_wakeUpGpsAlarm);
}

bool modesUtilities_isTrackingPeriodAlarmScheduled()
{
	return RTCManager_isAlarmScheduled(&_wakeUpModeAlarm);
}

bool modesUtilities_isGpsAlarmScheduled()
{
	return RTCManager_isAlarmScheduled(&_wakeUpGpsAlarm);
}

void modesToolkit_printTrackingPeriodAlarm()
{
	USARTManager_printUsbWait("Tracking period next : ");
	RTCManager_printTimeFromTimestamp(_wakeUpModeAlarm.data.timestamp);
	USARTManager_printUsbWait("\r\n");
}

void modesToolkit_printGpsAlarm()
{
	USARTManager_printUsbWait("GPS alarm next : ");
	RTCManager_printTimeFromTimestamp(_wakeUpGpsAlarm.data.timestamp);
	USARTManager_printUsbWait("\r\n");
}

/* Set alarm interrupt to be triggered in [delayMs] ms to wake_up modes*/
void modesToolkit_setAlarmModeDelay(uint32_t delayMs){
	uint32_t delaySec = convertMsToSec(delayMs);
	RTCManager_setDelayedCallback(delaySec, &_wakeUpModeAlarm, wakeUpModeCallback);
}

void modesToolkit_setAlarmGpsDelay(uint32_t delayMs)
{
	uint32_t delaySec = convertMsToSec(delayMs);
	RTCManager_setDelayedCallback(delaySec, &_wakeUpGpsAlarm, wakeUpGpsCallback);
}

static void wakeUpModeCallback(struct calendar_descriptor *const calendar){
	maskUtilities_setInterruptMaskBitsFromISR(INTERRUPT_BIT_CALENDAR_MODE_WAKEUP);
	UNUSED(calendar);
}

static void wakeUpGpsCallback(struct calendar_descriptor *const calendar)
{
	maskUtilities_setRequestMaskBits(REQUEST_BIT_GPS_MODE);
	UNUSED(calendar);
}

static uint32_t convertMsToSec(int delayMs){
	if(delayMs > 0){
		return delayMs / 1000;
		} else {
		return 0;
	}
}

void modesToolkit_startTrackingSlopeDetection()
{
	IMUManager_powerModeLowPower();
	IMUManager_startSlopeDetection();
	_seekiosMovedSinceLastCycle = false;
}

void modesToolkit_setSeekiosMovedSinceLastCycle()
{
	_seekiosMovedSinceLastCycle = true;
	IMUManager_stopSlopeDetection();
}

void modesToolkit_stopTrackingSlopeDetection()
{
	IMUManager_stopSlopeDetection();
	IMUManager_suspend();
	_seekiosMovedSinceLastCycle = true;
}

bool modesToolkit_didSeekiosMovedSinceLastCycle()
{
	return _seekiosMovedSinceLastCycle;
}

bool modesToolkit_isPositionBetterThanPreviousOne(SatelliteCoordinate *gpsDataPtr)
{
	return GPSManager_isPositionBetter(gpsDataPtr, &_lastTrackingCoordinate);
}

void modesToolkit_getPreviousPosition(SatelliteCoordinate *gpsDataPtr)
{
	gpsDataPtr->coordinate.alt = _lastTrackingCoordinate.coordinate.alt;
	gpsDataPtr->coordinate.lat = _lastTrackingCoordinate.coordinate.lat;
	gpsDataPtr->coordinate.lon = _lastTrackingCoordinate.coordinate.lon;
	gpsDataPtr->fixQuality = _lastTrackingCoordinate.fixQuality;
	gpsDataPtr->hDOP = _lastTrackingCoordinate.hDOP;
	gpsDataPtr->satellitesNumber = _lastTrackingCoordinate.satellitesNumber;
}

void modesToolkit_setPreviousPosition(SatelliteCoordinate *gpsDataPtr)
{
	_lastTrackingCoordinate.coordinate.alt = gpsDataPtr->coordinate.alt;
	_lastTrackingCoordinate.coordinate.lat = gpsDataPtr->coordinate.lat;
	_lastTrackingCoordinate.coordinate.lon = gpsDataPtr->coordinate.lon;
	_lastTrackingCoordinate.fixQuality = gpsDataPtr->fixQuality;
	_lastTrackingCoordinate.hDOP = gpsDataPtr->hDOP;
	_lastTrackingCoordinate.satellitesNumber = gpsDataPtr->satellitesNumber;
}