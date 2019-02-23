#include <modesManager/modes.h>
static double getMinDistanceFromEdges(Coordinate *seekiosLocationPtr,int nbEdges, Coordinate *zonePointsPtr);
static bool checkIfSeekiosInZone(Coordinate zoneCoordinates[NB_MAX_COORDINATES], int nbCoordinates, SatelliteCoordinate *outOfZoneCoordinate);
static double getMinDistanceFromZone(Coordinate *seekiosLocationPtr, int nbCoordinates, Coordinate* coordinates);
static void setZoneWaitSignificantMotitionState(void);
static void raiseMessageSentOutOfZone(void);

void modes_zoneMode(void* param){
	UNUSED(param);
	maskUtilities_setRunningMaskBits(RUNNING_BIT_MODE);
	maskUtilities_requestGPSFromMode();

	vTaskDelay(500);
	Coordinate coordinates[NB_MAX_COORDINATES];
	int nbCoordinates = statusManager_getRunningConfigCoordinates(coordinates);
	uint16_t refreshRate = statusManager_getRunningRefreshRate();
	uint8_t state = statusManager_getRunningStatusState();
	bool trackingWhenSeekiosOutOfZone = refreshRate > 0;
	SatelliteCoordinate outOfZoneCoordinate;
	if(state == ZONE_STATE_CHECK_POSITION)
	{
		if(checkIfSeekiosInZone(coordinates, nbCoordinates, &outOfZoneCoordinate))
		{
			// seekios in zone
			setZoneWaitSignificantMotitionState();
		}
		else
		{
			powerStateManager_disablePowerSavingIfEnabled();
			// out of zone : send message and start tracking
			modesToolkit_wrapMessageAndSend(&outOfZoneCoordinate, MESSAGE_TYPE_OUT_OF_ZONE, raiseMessageSentOutOfZone, statusManager_getRunningConfigModeId());
			if(trackingWhenSeekiosOutOfZone)
			{
				state = ZONE_STATE_TRACKING;
			}
			else
			{
				state = ZONE_STATE_SUSPEND;
			}
			volatile uint32_t zoneWm = uxTaskGetStackHighWaterMark(NULL);
			UNUSED(zoneWm);
			statusManager_setRunningConfigStatusState(state);
		}
	}

	if(state == ZONE_STATE_TRACKING)
	{
		tracking_infiniteTracking(MESSAGE_TYPE_ZONE_TRACKING, refreshRate);
	}

	maskUtilities_stopRequestGPSFromMode();
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_MODE);
	FreeRTOSOverlay_taskDelete(NULL);
}

/*Traitement du mode zone dans son état initiale : seekios dans la zone, attente de la sortie
returns true if seekios out of zone, nothing if nothing detected
Algorithme de détection de sortie Zone :
- récupère la position
- si c'est dans la zone, on return true
- si c'est hors de la zone, on incrément le counterOutOfZone et on continue jusqu'à ce que la limite du compteur soit dépassée, ou que le seekios soit de retour dans la zone
Note : le counterOutOfZone est incrémenté proportionellement à la vitesse
returns true if the seekios is in zone
returns false otherwise
*/
static bool checkIfSeekiosInZone(Coordinate zoneCoordinates[NB_MAX_COORDINATES], int nbZoneCoordinates, SatelliteCoordinate *outOfZoneCoordinatePtr)
{	
	volatile SatelliteCoordinate lastSatelliteData;
	GetPositionParameters params;
	params.continueAfterTimeframeIfPositionFound=true;
	params.qualityCriterias.maxHdop=2;
	params.qualityCriterias.minSatNum=4;
	params.respectsQualityCriteria=true;
	params.returnFirstPositionFound=true;
	params.timeFrameToGetBestPosition=60000;
	uint8_t counterOutOfZone=0;
	
	uint8_t buf[15];
	
	volatile double minDistanceFromZone;
	do
	{
		GPSManager_getPositionFromMode(&lastSatelliteData, &params);
		minDistanceFromZone=getMinDistanceFromZone(&(lastSatelliteData.coordinate), nbZoneCoordinates, zoneCoordinates);
		if (minDistanceFromZone<MAX_DISTANCE_FROM_ZONE)
		{
			USARTManager_printUsbWait("[zone ras] Seekios in or close to zone.\r\n");
			counterOutOfZone = 0;
			return true;
		}
		else 
		{
			//We increment the counterOutOfZone depending on the speed of the seekios. The faster the seekios is, the more we increment the counterOutOfZone
			if (minDistanceFromZone>FAR_DISTANCE_FROM_ZONE)
			{
				counterOutOfZone+=OUT_OF_ZONE_THRESHOLD/6;
			}
				
			if (lastSatelliteData.speed>SPEED_THRESHOLD_HIGH)
			{
				counterOutOfZone+=OUT_OF_ZONE_THRESHOLD/3;
			}
			else if (lastSatelliteData.speed>SPEED_THRESHOLD_VERY_SLOW)
			{
				counterOutOfZone+=(int)lastSatelliteData.speed;
			}
			else if (lastSatelliteData.speed<=SPEED_THRESHOLD_VERY_SLOW)
			{
				counterOutOfZone++;
			}
		}		
		USARTManager_printUsbWait("[zone ras] Position out of zone !\r\n");
		USARTManager_printUsbWait("Counter: ");
		stringHelper_intToString(counterOutOfZone,buf);
		strcat(buf,"\r\n");
		USARTManager_printUsbWait(buf);
		USARTManager_printUsbWait("Distance: ");
		stringHelper_doubleToString(minDistanceFromZone,buf,2);
		strcat(buf,"\r\n");
		USARTManager_printUsbWait(buf);
		USARTManager_printUsbWait("Speed: ");
		stringHelper_doubleToString(lastSatelliteData.speed,buf,2);
		strcat(buf,"\r\n");
		USARTManager_printUsbWait(buf);		
		if (counterOutOfZone>=OUT_OF_ZONE_THRESHOLD)//on attend 60 itérations OutOfZone d'affilée
		{
			USARTManager_printUsbWait("[zone ooz] Seekios out of zone.\r\n");
			GPSManager_copySatelliteCoordinate(outOfZoneCoordinatePtr, &lastSatelliteData);
			return false;
		}
		
	} while (1);
}

/* Sets the seekios in the ZONE_STATE_WAIT_SIGNIFICANT_MOTION state. */
static void setZoneWaitSignificantMotitionState()
{
	statusManager_setRunningConfigStatusState(ZONE_STATE_WAIT_SIGNIFICANT_MOTION);
	IMUManager_powerModeLowPower();
	IMUManager_startSlopeDetection();
}

/* Returns 0 if the seekios is in the zone, or the minimum distance from the zone if out of the zone */
static double getMinDistanceFromZone(Coordinate *seekiosLocationPtr, int nbCoordinates, Coordinate* zoneCoordinates){
	double minDistance;
	
	if(geolocationTools_isPositionInZone(seekiosLocationPtr,nbCoordinates,zoneCoordinates))
	{
		return 0;
	}
	
	minDistance=getMinDistanceFromEdges(seekiosLocationPtr, nbCoordinates, zoneCoordinates);
		
	return minDistance;
}

//On calcule la distance entre le Seekios et les segments de la zone (hauteur)
static double getMinDistanceFromEdges(Coordinate *seekiosLocationPtr,int nbEdges, Coordinate *zonePointsPtr)
{
	double minDistance = geolocationTools_getShortestDistanceFromEdge(
	zonePointsPtr[0].lat,
	zonePointsPtr[0].lon,
	zonePointsPtr[(1)%nbEdges].lat,
	zonePointsPtr[(1)%nbEdges].lon,
	seekiosLocationPtr);

	for(int i=1; i<nbEdges; i++){

		double tempDistance = geolocationTools_getShortestDistanceFromEdge(
		zonePointsPtr[i].lat,
		zonePointsPtr[i].lon,
		zonePointsPtr[(i+1)%nbEdges].lat,
		zonePointsPtr[(i+1)%nbEdges].lon,
		seekiosLocationPtr);

		if(tempDistance < minDistance){
			minDistance = tempDistance;
		}
	}

	char buff[16];
	stringHelper_doubleToString(minDistance, buff, 0);
	USARTManager_printUsbWait("Shortest distance to zone : ");
	USARTManager_printUsbWait(buff);
	USARTManager_printUsbWait("m\r\n");

	return minDistance;
}

static void raiseMessageSentOutOfZone()
{
	testMonitor_raiseEvent(EVENT_MESSAGE_SENT_OUT_OF_ZONE);
}