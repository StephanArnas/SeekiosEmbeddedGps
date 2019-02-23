#include <modesManager/modes.h>

static void raiseMessageSentOnDemand();

void modes_onDemand(void* param)
{
	UNUSED(param);
	maskUtilities_setRunningMaskBits(RUNNING_BIT_ON_DEMAND);
	maskUtilities_setRequestMaskBits(REQUEST_BIT_GPS_ON_DEMAND);
	
	USARTManager_printUsbWait("-=[ START ON DEMAND ]=-\r\n");
	
	
	SatelliteCoordinate gpsData;
	bool locationFound = false;
	GPSManager_askCurrentPositionFromOnDemand();
	locationFound = GPSManager_waitCurrentPositionFromOnDemand(&gpsData, GET_GPS_POSITION_LONG_TIMEOUT);
	GPSManager_stopAskingCurrentPositionFromOnDemand();

	E_MESSAGE_TYPE msgType = MESSAGE_TYPE_ON_DEMAND_TRIANGULATION;
	SatelliteCoordinate *msgData = NULL;

	if(locationFound)
	{
		msgData = &gpsData;
		msgType = MESSAGE_TYPE_ON_DEMAND;
	}

	volatile UBaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
	UNUSED(wm);
	modesToolkit_wrapMessageAndSend(msgData, msgType, raiseMessageSentOnDemand, 0);//, fixTimestamp);

	USARTManager_printUsbWait("-=[ END ON DEMAND ]=-\r\n");
	
	wm = uxTaskGetStackHighWaterMark(NULL);
	maskUtilities_clearRequestMaskBits(REQUEST_BIT_GPS_ON_DEMAND);
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_ON_DEMAND);
	FreeRTOSOverlay_taskDelete(NULL);
}

static void raiseMessageSentOnDemand()
{
	testMonitor_raiseEvent(EVENT_MESSAGE_SENT_ON_DEMAND);
}