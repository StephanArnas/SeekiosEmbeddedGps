#include <modesManager/modes.h>

static void raiseMessageSentTracking();

/* Tracking mode with refresh rate une seconds
TODO : Shut down GPS (clear use flag too) between big refresh times */
void modes_trackingMode(void* param)
{
	UNUSED(param);
	maskUtilities_setRunningMaskBits(RUNNING_BIT_MODE);
	vTaskDelay(500);
	int refreshRateMin = statusManager_getRunningRefreshRate();
	tracking_infiniteTracking(MESSAGE_TYPE_TRACKING, refreshRateMin);
	
}

void tracking_infiniteTracking(E_MESSAGE_TYPE trackingMessageType, int refreshRateMin)
{
	maskUtilities_requestGPSFromMode();
	int refreshRateMs = 60000*refreshRateMin;

	SatelliteCoordinate gpsData;

	while(1){		
		GetPositionParameters params;
		params.timeFrameToGetBestPosition = GPS_MAX_FIXTIME;
		params.respectsQualityCriteria = true;
		params.continueAfterTimeframeIfPositionFound = true;
		params.returnFirstPositionFound = false;
		params.qualityCriterias.maxHdop = 2;
		params.qualityCriterias.minSatNum = 6;
		GPSManager_getPositionFromMode(&gpsData, &params);
		// We keep the previous position if the seekios didn't move and if the new position is worse than the previous one
		if(!modesToolkit_didSeekiosMovedSinceLastCycle() && !modesToolkit_isPositionBetterThanPreviousOne(&gpsData))
		{
			USARTManager_printUsbWait("Seekios didn't move and worse position : keeping the previous position.\r\n");
			modesToolkit_getPreviousPosition(&gpsData);
		}
		else
		{
			USARTManager_printUsbWait("Seekios moved or better position : taking the new position.\r\n");
			modesToolkit_setPreviousPosition(&gpsData);
		}

		#if (ACTIVATE_GPS_LOGS == 1)
		GPSManager_addGPSLog(&gpsData);
		#endif

		modesToolkit_wrapMessageAndSend(&gpsData, trackingMessageType, raiseMessageSentTracking, statusManager_getRunningConfigModeId());//, gpsData.fixTimestamp);
		
		modesToolkit_startTrackingSlopeDetection();

		// On attends RefreshRate - GPS_MAX_FIXTIME pour redémarrer le mode
		// On "chauffe" le GPS en avance en l'allumant RefreshRate - GPS_MAX_FIXTIME - GPS_WARMUP_TIME
		// Le temps d'expiration du GPS n'est pas pris en compte
		int32_t timeToWait = refreshRateMs - GPS_MAX_FIXTIME;
		if(timeToWait > 0){
			USARTManager_printUsbWait("Wait : ");
			USARTManager_printInt(timeToWait);
			USARTManager_printUsbWait("\r\n");
			
			bool gpsShouldSleep = timeToWait > GPS_WARMUP_TIME;
			
			if(gpsShouldSleep)
			{
				USARTManager_printUsbWait("Putting GPS to sleep.\r\n");
				modesToolkit_sleepGpsUntilNextCycle(timeToWait - GPS_WARMUP_TIME);
			}
			
			USARTManager_printUsbWait("Putting mode to sleep.\r\n");
			modesToolkit_sleepModeUntilNextCycle(timeToWait);
			
			vTaskDelay(timeToWait);
		}
		
		volatile UBaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
		UNUSED(wm);
	}
}

static void raiseMessageSentTracking()
{
	testMonitor_raiseEvent(EVENT_MESSAGE_SENT_TRACKING);
}