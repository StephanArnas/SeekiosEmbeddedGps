#include <modesManager/modes.h>
static void raiseSOSLocationSentEvent(void);
static void raiseSOSSentEvent(void);

void modes_SOS(void* param)
{
	UNUSED(param);
	maskUtilities_setRunningMaskBits(RUNNING_BIT_SOS);
	
	USARTManager_printUsbWait("-=[ START SOS ]=-\r\n");
	
	modesToolkit_wrapMessageAndSend(NULL, MESSAGE_TYPE_SOS, raiseSOSSentEvent, 0);//, RTCManager_getCurrentTimestamp());

	maskUtilities_setRequestMaskBits(REQUEST_BIT_GPS_SOS);

	SatelliteCoordinate gpsData;
	bool locationFound = false;
	GPSManager_askCurrentPositionFromSOS();
	locationFound = GPSManager_waitCurrentPositionFromSOS(&gpsData, GET_GPS_POSITION_TIMEOUT);
	GPSManager_stopAskingCurrentPositionFromSOS();

	maskUtilities_clearRequestMaskBits(REQUEST_BIT_GPS_SOS);

	E_MESSAGE_TYPE msgType = MESSAGE_TYPE_SOS_LOCATION;
	SatelliteCoordinate *msgData = NULL;
	//time_t fixTimestamp = 0;
	if(locationFound)
	{
		msgData = &gpsData;
		msgType = MESSAGE_TYPE_SOS_LOCATION;
		//fixTimestamp = gpsData.fixTimestamp;
	}
	else
	{
		msgData = NULL;
		msgType = MESSAGE_TYPE_SOS_LOCATION_TRIANGULATION;
		//fixTimestamp = RTCManager_getCurrentTimestamp();
	}

	modesToolkit_wrapMessageAndSend(msgData, msgType, raiseSOSLocationSentEvent, 0);//, fixTimestamp);
	volatile UBaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
	UNUSED(wm);
	USARTManager_printUsbWait("-=[ END SOS ]=-\r\n");
	
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_SOS);
	FreeRTOSOverlay_taskDelete(NULL);
}

static void raiseSOSLocationSentEvent()
{
	testMonitor_raiseEvent(EVENT_SOS_LOCATION_SENT);
}

static void raiseSOSSentEvent()
{
	testMonitor_raiseEvent(EVENT_SOS_SENT);
}