#include <peripheralManager/GPS_manager.h>

static void clearAllcurPosAnswerFlag();
static void clearAllcurPosRequestFlag();
static void initCoordinate(SatelliteCoordinate *coordinatePtr);
static void setCurrentPosition(SatelliteCoordinate *coordinatePtr);
static bool waitCurrentPosition(SatelliteCoordinate *result, TickType_t timeout, EventBits_t bit);
static void setAnswerCurrentPositionFlags(void);
static int computeChecksumFromNMEA(char *sentence,uint8_t numberOfNMEA);
static int validateChecksum(char * nmea,uint8_t numberOfNMEA);
static uint8_t getStarIndex(char* nmea,uint8_t numberOfNMEA);
static bool waitGGAFrame(uint8_t* resultBuf, uint32_t timeout);
static void clearNMEAAvailableBit(void);

static EventGroupHandle_t _gpsMaskhandle;
static SatelliteCoordinate _currentPosition;
static struct calendar_alarm _gpsExpirationAlarm;
static bool _isGpsOn;

#if (ACTIVATE_GPS_LOGS == 1)
#define GPS_LOG_COUNT 64
static GPSLog	_GPSLogs[GPS_LOG_COUNT];
static uint32_t _GPSLogIndex = 0;
static uint32_t _GPSLogId = 0;
#endif

#if (ACTIVATE_WRONG_NMEA_FRAME_LOGS == 1)
static uint8_t wrongNMEABuffer[100]={0};
#endif

void GPSManager_init(){
	_gpsMaskhandle = xEventGroupCreate();
	_isGpsOn= false;
	clearAllcurPosRequestFlag();
	clearAllcurPosAnswerFlag();
}

bool GPSManager_isGPSOn(){
	return _isGpsOn;
}

void task_GPSManager(void* param){
	UNUSED(param);
	maskUtilities_setRunningMaskBits(RUNNING_BIT_GPS_MANAGER);
	USARTManager_printUsbWait("GPS Manager started !\r\n");
	
	GPSManager_getGPSData();
	
	volatile UBaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
	UNUSED(wm);
	USARTManager_printUsbWait("GPS Manager stopped !\r\n");
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_GPS_MANAGER);
	FreeRTOSOverlay_taskDelete(NULL);
}

static void GPSSessionExpirationCallback(){
	maskUtilities_setRequestMaskBitsFromISR(REQUEST_BIT_GPS_EXPIRED);
}

bool GPSManager_isGPSExpirationAlarmScheduled(){
	return RTCManager_isAlarmScheduled(&_gpsExpirationAlarm);
}

void GPSManager_scheduleGPSExpirationAlarm(){
	GPSManager_removeGPSExpirationAlarm();
	USARTManager_printUsbWait("GPS EXPIRATION : ");
	RTCManager_setDelayedCallback(GPS_EXPIRATION_TIME*60, & _gpsExpirationAlarm, GPSSessionExpirationCallback);
}

void GPSManager_removeGPSExpirationAlarm(){
	testMonitor_raiseEvent(EVENT_GPS_EXPIRATION_ALARM_OFF);
	if(RTCManager_removeAlarm(&_gpsExpirationAlarm))
	{
		USARTManager_printUsbWait("Removed GPS expired alarm !\r\n");
	}
}

void GPSManager_printGPSExpirationAlarm()
{
	USARTManager_printUsbWait("GPS Expiration : ");
	RTCManager_printTimeFromTimestamp(_gpsExpirationAlarm.data.timestamp);
	USARTManager_printUsbWait("\r\n");
}

/* Copies the value of _currentPosition insite the result variable */
void GPSManager_getCurrentPosition(SatelliteCoordinate *result){
	GPSManager_copySatelliteCoordinate(result, &_currentPosition);
}

/* Met à jour en permanence la _currentPosition
Frames received :
GPGGA --> Fix data (velocity, position, time, sat num, dop, fix type)
GLGSV --> Satellites info (azimuth, CNR...)
GPRMC --> Minimum recommended data (position, velocity, time), NO precision info ! (sat num, dop...)
GPVTG --> Velocity info
GPGSA --> Active satellites (fix, HDOP, VDOP)

Reminder  :
GL = Glonass data
GP = GPS data*/
void GPSManager_getGPSData()
{
	SatelliteCoordinate tempCoordinate;
	initCoordinate(&tempCoordinate);
	do
	{
		if(GPSManager_waitCoordinate(&tempCoordinate, GPS_READING_TIMEOUT) == GPS_RESPONSE_STATUS_FRAME_PARSED)
		{
			setCurrentPosition(&tempCoordinate);
			memset(gps_buf, 0, GPS_BUF_SIZE);
			setAnswerCurrentPositionFlags();
			initCoordinate(&tempCoordinate);
		}
		vTaskDelay(10);

	} while (maskUtilities_isGpsRequested());

	volatile UBaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
	UNUSED(wm);
	clearAllcurPosRequestFlag();
	clearAllcurPosAnswerFlag();
}

/* Reads the GPS buffer, waits for a valid coordinate. (Including checksum verification and parsing) */
E_GPS_RESPONSE_STATUS GPSManager_waitCoordinate(SatelliteCoordinate *resultCoordinatePtr, uint32_t timeout)
{
	E_GPS_RESPONSE_STATUS responseStatus = GPS_RESPONSE_STATUS_NONE;
	//volatile char sentenceBuffer[256];
	if(waitGGAFrame(gps_buf, timeout))
	{
		responseStatus = GPS_RESPONSE_STATUS_FRAME_RECEIVED;
		#if (ACTIVATE_NMEA_FRAME_PRINT==1)
		USARTManager_printUsbWait(gps_buf);
		#endif
		if (parse_sentence(gps_buf,resultCoordinatePtr) == FUNCTION_SUCCESS)
		{
			//volatile UBaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
			
			responseStatus = GPS_RESPONSE_STATUS_FRAME_PARSED;
		}
	}
	return responseStatus;
}

/* Computes the checksum of the NMEA frame, then compares it to what was contained in the frame */
static int validateChecksum( char * nmea, uint8_t numberOfNMEA)
{
	uint8_t nmeaIndex=0;
	while (nmeaIndex<numberOfNMEA)
	{
		volatile uint8_t computedCheckSum = computeChecksumFromNMEA(nmea,nmeaIndex);
		volatile uint8_t starIndex = getStarIndex(nmea,nmeaIndex);
		if(starIndex == 0)
		{
			return false;
		}
		char gotSum[3];
		//On récupère le checksum de la trame nmea
		gotSum[0] = nmea[starIndex + 1];
		gotSum[1] = nmea[starIndex + 2];
		gotSum[2] = '\0';
		
		//On passe du format "HEX string" à int
		int givenChecksum = (int)strtol(gotSum, NULL, 16);
		//On compare le checksum calculé avec celui de la trame
		if(givenChecksum != computedCheckSum)
		{
			USARTManager_printUsbWait("GPS MANAGER: checksum FAILED\r\n");
			USARTManager_printUsbWait("Compu CS :");
			USARTManager_printByte(computedCheckSum);
			USARTManager_printUsbWait("\r\nGiven CS :");
			USARTManager_printByte(givenChecksum);
			USARTManager_printUsbWait("\r\n");
			return false;
		}
		nmeaIndex++;
	}
	return true;
}

static uint8_t getStarIndex(char* nmea,uint8_t nmeaIndex)
{
	uint8_t nmeaLength = strlen(nmea);
	uint8_t starCounter=0;
	
	for(uint8_t i=0;i< nmeaLength - 2; i++)
	{
		if(nmea[i] == '*')
		{
			if (starCounter==nmeaIndex)
			{
				return i;
			}
			starCounter++;
		}
	}
	return 0;
}

static int computeChecksumFromNMEA(char *sentence,uint8_t nmeaIndex)
{
	uint8_t i;
	uint8_t XOR;
	uint8_t c;
	uint8_t starCounter=0;
	bool dollar=true;
	
	for (XOR = 0, i = 0; i < strlen(sentence); i++)
	{
		c = (unsigned char)sentence[i];
		if (c == '$')
		{
			dollar=true;
		}
		if (c == '*')
		{
			dollar=false;
			if (starCounter==nmeaIndex)
			{
				break;
			}
			else
			{
				starCounter++;
				XOR=0;
			}
		}
		if (c != '$' && dollar) XOR ^= c;
	}
	return XOR;
}

static void initCoordinate(SatelliteCoordinate *coordinate){
	coordinate->hDOP=5;
	coordinate->speed=0;
	coordinate->coordinate.lat=90;
	coordinate->coordinate.lon=180;
	coordinate->coordinate.alt=0.0;
	coordinate->satellitesNumber=0;
	coordinate->fixQuality=0;
}

static void setCurrentPosition(SatelliteCoordinate *coordinatePtr){
	GPSManager_copySatelliteCoordinate(&_currentPosition, coordinatePtr);
}

bool GPSManager_isGPSBufferEmpty(){

	for(uint16_t i=0; i<GPS_BUF_SIZE;i++){
		if(gps_buf[i] != 0){
			return false;
		}
	}

	return true;
}

void GPSManager_copySatelliteCoordinate(SatelliteCoordinate *dest, SatelliteCoordinate *src){
	dest->coordinate.lat	= src->coordinate.lat;
	dest->coordinate.lon	= src->coordinate.lon;
	//dest->coordinate.latDMS	= src->coordinate.latDMS;
	//dest->coordinate.lonDMS	= src->coordinate.lonDMS;
	dest->coordinate.alt	= src->coordinate.alt;
	dest->hDOP				= src->hDOP;
	dest->satellitesNumber	= src->satellitesNumber;
	dest->fixQuality		= src->fixQuality;
	dest->speed				= src->speed;
	dest->trackAngle		=src->trackAngle;
}

int parse_sentence(char *sentenceBuffer, SatelliteCoordinate *currentlocationPtr)
{
	int paramTokenCounter=0;
	char *token=NULL;
	char orientationLatLong[4];
	char tempLatLongAlt[16];

	currentlocationPtr->coordinate.lat=90;
	currentlocationPtr->coordinate.lon=180;

	for (token = strtok_single(sentenceBuffer, ","); token != NULL; token = strtok_single(NULL, ","))
	{
		//$GPGGA,064036.289,4836.5375,N,00740.9373,E,1,04,3.2,200.2,M,,,,0000*0E\r\n$GPRMC,181403.000,A,4327.8864,N,00129.5422,W,0.11,0.00,080317,,,A*75
		// Pour chaque valeur de paramTokenCounter :
		//	0 - $GPGGA       : Type de trame
		//	1 - 064036.289   : Trame envoyée à 06h 40m 36,289s (heure UTC)
		//	2 - 4836.5375	 : Latitude 48,608958°	    }==>
		//  3 - N			 : Nord					    }==> = 48°36'32.25" Nord
		//	4 - 00740.9373	 : Longitude 7,682288°	    }==>
		//  5 - E			 : Est						}==> = 7°40'56.238" Est
		//	6 - 1            : Fix quality : 0 invalid, 1 GPS fix, 2 DGPS fix, 3 PPS Fix, 4 Real time Kinetic, 5 float RTK, 6 estimated, 7 Manual input mode, 8 Simulation mode
		//	7 - 04           : Nombre de satellites utilisés pour calculer les coordonnées
		//	8 - 3.2          : Précision horizontale ou HDOP (Horizontal dilution of precision)
		//	9 - 200.2		 : Altitude 200,2
		// 10 - M			 : en mètres
		//11+ -	,,,,,0000    : D'autres informations peuvent être inscrites dans ces champs
		//	  - *0E          : Somme de contrôle de parité, un simple XOR sur les caractères entre $ et *3

		switch (paramTokenCounter)
		{
			case 0:break;
			case 1:break;
			//	2 - 4836.5375	 : Latitude 48,608958°
			case 2:
			if(token[0]==0){return FUNCTION_FAILURE;}
			strcpy(tempLatLongAlt,token);
			break;
			//  3 - N			 : Nord
			case 3:
			if(token[0]==0){return FUNCTION_FAILURE;}
			strcpy(orientationLatLong,token);
			currentlocationPtr->coordinate.lat=convertNMEA(tempLatLongAlt,orientationLatLong);
			//currentlocationPtr->coordinate.latDMS=strtod(tempLatLongAlt,NULL);
			break;
			//	4 - 00740.9373	 : Longitude 7,682288°
			case 4:
			if(token[0]==0){return FUNCTION_FAILURE;}
			strcpy(tempLatLongAlt,token);
			break;
			//  5 - E			 : Est
			case 5:
			if(token[0]==0){return FUNCTION_FAILURE;}
			strcpy(orientationLatLong,token);
			currentlocationPtr->coordinate.lon=convertNMEA(tempLatLongAlt,orientationLatLong);
			//currentlocationPtr->coordinate.lonDMS=strtod(tempLatLongAlt,NULL);
			break;
			//	6 - 1            : Type de positionnement (le 1 est un positionnement GPS)
			case 6:
			if (atoi(token) != FIX_QUALITY_GPS 
			&& atoi(token) != FIX_QUALITY_DGPS)
			{
				#if (ACTIVATE_WRONG_NMEA_FRAME_LOGS == 1)
				memcpy(wrongNMEABuffer, sentenceBuffer, 100);
				#endif
				return FUNCTION_FAILURE;
			}
			currentlocationPtr->fixQuality = atoi(token);
			break;
			//	7 - 04           : Nombre de satellites utilisés pour calculer les coordonnées
			case 7:
			if(token[0]==0){return FUNCTION_FAILURE;}
			currentlocationPtr->satellitesNumber=atoi(token);
			break;
			//	8 - 3.2          : Précision horizontale ou HDOP (Horizontal dilution of precision)
			case 8:
			if(token[0]==0){return FUNCTION_FAILURE;}
			currentlocationPtr->hDOP=strtod(token,NULL);
			break;
			//	9 - 200.2		 : Altitude 200,2
			case 9:
			strcpy(tempLatLongAlt,token);
			break;
			// 10 - M			 : en mètres
			case 10:
			if (token[0]=='M'){
				currentlocationPtr->coordinate.alt=strtod(tempLatLongAlt,NULL);
			}
			break;
			case 11:break;
			case 12:break;
			case 13:break;
			case 14:break;
			case 15:break;
			case 16:break;
			case 17:break;
			case 18:break;
			case 19:break;
			case 20:break;
			case 21:
			currentlocationPtr->speed=strtod(token,NULL);
			currentlocationPtr->speed*=1.852;//noeuds->km/h
			break;
			case 22:
			currentlocationPtr->trackAngle=strtod(token,NULL);
			break;
			default:break;
		}
		paramTokenCounter++;
	}
	
	if (currentlocationPtr->coordinate.lon>-180 && currentlocationPtr->coordinate.lon<180
	&& currentlocationPtr->coordinate.lat>-90 && currentlocationPtr->coordinate.lat<90)
	{
		//Get actual date to set fix acquisition date
		//currentlocationPtr->fixTimestamp = RTCManager_getCurrentTimestamp();
		return FUNCTION_SUCCESS;
	}
	else return FUNCTION_FAILURE;
}

/* Returns true if GGA data was successfully taken, false if it failed */
bool catch_GGA_Data(char *sentencebuffer,int dollarINDEX)
{
	int charCursor=dollarINDEX;
	int sentenceSize=0;
	
	do
	{
		charCursor++;
		
		if (charCursor>=256)
		{
			charCursor=0;
		}
		sentencebuffer[sentenceSize]=gps_buf[charCursor];
		sentenceSize++;
		if(sentenceSize >= 128){
			sentenceSize = 0;
		}
	}
	while (gps_buf[charCursor]!='\n');
	
	sentencebuffer[sentenceSize]='\0';

	// should be GNGGA ou GPGGA
	if ((sentencebuffer[1]=='N' || sentencebuffer[1]=='P')
	&& sentencebuffer[3]=='G'&& sentencebuffer[4]=='A')
	{
		return true;
	}
	return false;
}

/* Allume le GPS et set le flag correspondant au mode appelant */
uint8_t GPSManager_powerOnGPS()
{
	_isGpsOn = true;
	USARTManager_printUsbWait("GPS Powered ON!\r\n");
	if (gpio_get_pin_level(GPS_power_enable))
	{
		return FUNCTION_FAILURE;
	}
	else
	{
		gpio_set_pin_level(GPS_power_enable, true);
		vTaskDelay(2000);
		return FUNCTION_SUCCESS;
	}
}

/* Eteint le GPS et clear le flag correspondant au mode appelant */
uint8_t GPSManager_powerOffGPS()
{
	_isGpsOn = false;
	testMonitor_raiseEvent(EVENT_GPS_STATE_OFF);
	USARTManager_printUsbWait("GPS Powered OFF!\r\n");
	if  (!gpio_get_pin_level(GPS_power_enable))
	{
		return FUNCTION_FAILURE;
	}
	else
	{
		gpio_set_pin_level(GPS_power_enable, false);
		vTaskDelay(200);
		send_gps("at\n\r\0");
		vTaskDelay(2000);
		return FUNCTION_SUCCESS;
	}
}

double convertNMEA(char * param, char * orientation)
{
	double f = strtod(param,NULL);
	double deg, min, sec, res;

	deg = (int)(f / 100.0);
	min = (int)(f - deg*100.0);
	sec = 60.0*(f - deg*100.0 - min);

	res = deg + min / 60 + sec / 3600;

	if (strlen(orientation) > 0 && (orientation[0] == 'S' || orientation[0] == 'W'))
	{
		res = res*-1;
	}
	return res;
}

bool check_sentence_validity(char *sentence,uint16_t sentenceSIZE)
{
	uint16_t commaCounter=0;
	uint16_t i=0;
	
	for (i=0;i<sentenceSIZE;i++)
	{
		if (sentence[i]==',')
		{
			commaCounter++;
		}
		if (commaCounter==6)
		{
			break;
		}
	}
	if (sentence[i+1]!='0')
	{
		return 1;
	}
	else return 0;
}

/* Sets the answer flags in the answer mask for the bits that we requesting, and clears the request mask*/
static void setAnswerCurrentPositionFlags(){
	EventBits_t requestsMask = xEventGroupGetBits( _gpsMaskhandle );
	EventBits_t answerMask = 0;
	if(requestsMask & GPS_BIT_CURPOS_REQ_BIT_MODES)
	{
		answerMask |= GPS_BIT_CURPOS_ANS_BIT_MODES;
	}
	
	if(requestsMask & GPS_BIT_CURPOS_REQ_BIT_ON_DEMAND)
	{
		answerMask |= GPS_BIT_CURPOS_ANS_BIT_ON_DEMAND;
	}
	
	if(requestsMask & GPS_BIT_CURPOS_REQ_BIT_SOS)
	{
		answerMask |= GPS_BIT_CURPOS_ANS_BIT_SOS;
	}
	xEventGroupSetBits(_gpsMaskhandle, answerMask);
}

/*
Demande une position au gps
*/
void GPSManager_askCurrentPositionFromMode(){
	xEventGroupClearBits(_gpsMaskhandle, GPS_BIT_CURPOS_ANS_BIT_MODES);
	xEventGroupSetBits(_gpsMaskhandle, GPS_BIT_CURPOS_REQ_BIT_MODES);

}

void GPSManager_askCurrentPositionFromOnDemand(){
	xEventGroupClearBits(_gpsMaskhandle, GPS_BIT_CURPOS_ANS_BIT_ON_DEMAND);
	xEventGroupSetBits(_gpsMaskhandle, GPS_BIT_CURPOS_REQ_BIT_ON_DEMAND);
}

void GPSManager_askCurrentPositionFromSOS(){
	xEventGroupClearBits(_gpsMaskhandle, GPS_BIT_CURPOS_ANS_BIT_SOS);
	xEventGroupSetBits(_gpsMaskhandle, GPS_BIT_CURPOS_REQ_BIT_SOS);
}

void GPSManager_askGPSUSARTTest()
{
	xEventGroupSetBits(_gpsMaskhandle, GPS_BIT_ANS_GPS_USART_TEST);
	xEventGroupSetBits(_gpsMaskhandle, GPS_BIT_REQ_GPS_USART_TEST);
}

void GPSManager_stopAskingGPSUSARTTest()
{
	xEventGroupClearBits(_gpsMaskhandle, GPS_BIT_REQ_GPS_USART_TEST);
}

/*
Stop la demande de position
*/
void GPSManager_stopAskingCurrentPositionFromMode(){
	xEventGroupClearBits(_gpsMaskhandle, GPS_BIT_CURPOS_REQ_BIT_MODES);
}

void GPSManager_stopAskingCurrentPositionFromOnDemand(){
	xEventGroupClearBits(_gpsMaskhandle, GPS_BIT_CURPOS_REQ_BIT_ON_DEMAND);
}

void GPSManager_stopAskingCurrentPositionFromSOS(){
	xEventGroupClearBits(_gpsMaskhandle, GPS_BIT_CURPOS_REQ_BIT_SOS);
}

/*
Demande une position au GPS de la part d'un mode et attend qu'il la donne jusqu'au timeout.
Si aucune position n'est donnée, retourne faux
*/
bool GPSManager_waitCurrentPositionFromMode(SatelliteCoordinate *resultPtr, TickType_t timeout){
	return waitCurrentPosition(resultPtr, timeout, GPS_BIT_CURPOS_ANS_BIT_MODES);
}

bool GPSManager_waitCurrentPositionFromOnDemand(SatelliteCoordinate *result, TickType_t timeout){
	return waitCurrentPosition(result, timeout, GPS_BIT_CURPOS_ANS_BIT_ON_DEMAND);
}

bool GPSManager_waitCurrentPositionFromSOS(SatelliteCoordinate *result, TickType_t timeout){
	return waitCurrentPosition(result, timeout, GPS_BIT_CURPOS_ANS_BIT_SOS);
}

/*
Demande une position au GPS et attend qu'il la donne jusqu'au timeout.
Si aucune position n'est donnée, retourne faux
*/
static bool waitCurrentPosition(SatelliteCoordinate *resultPtr, TickType_t timeout, EventBits_t bit){
	//Attend que le bit de réponse passe à 1 et le remet à 0 en suivant.
	EventBits_t uxBits;
	uxBits = xEventGroupWaitBits(_gpsMaskhandle, bit, pdTRUE, pdFALSE, timeout);
	//Si on a bien eu la réponse
	if(uxBits & bit){
		//On récupère la current position
		GPSManager_getCurrentPosition(resultPtr);
		return true;
	}
	//Si on a eu un timeout
	return false;
}

/*
Récupère la position GPS du seekios. Prend en paramètre un GetPositionParameters
- timeFrameToGetBestPosition : temps à attendre pour essayer de récupérer la meilleure position possible
- respectsQualityCriteria : vrai si la position doit respecter les critères minimum de qualité et de précision
- continueAfterTimeFrameIfPositionNotFound : vrai si la fonction doit continuer si le GPS n'a pas trouvé de position
dans le temps imparti
- returnFirstPositionFound : vrai si la fonction doit retourner la première Position trouvée sans plus attendre
*/
bool GPSManager_getPositionFromMode(SatelliteCoordinate* resultPtr, GetPositionParameters* params){

	GPSManager_askCurrentPositionFromMode();
	bool atLeastOneLocationFound = false;
	TickType_t startTime = xTaskGetTickCount();
	//On continue pour essayer d'améliorer la position tant qu'il nous reste du temps
	//Ou si aucune position n'a été trouvé et que continueAfterTimeFrameIfPositionNotFound est vrai
	while((xTaskGetTickCount() - startTime < params->timeFrameToGetBestPosition)
	|| (!atLeastOneLocationFound && params->continueAfterTimeframeIfPositionFound)){

		SatelliteCoordinate position;

		//donne le bon timer au timeout
		TickType_t timeout = 300000; // 5 min
		if(atLeastOneLocationFound || !params->continueAfterTimeframeIfPositionFound)
		{
			timeout = params->timeFrameToGetBestPosition - (xTaskGetTickCount() - startTime);
		}
		
		//On attend la réponse
		bool locationFound = GPSManager_waitCurrentPositionFromMode(&position, timeout);
		if(!locationFound) continue;
		
		if(!params->respectsQualityCriteria || GPSManager_respectsQualityCriteria(&position,&(params->qualityCriterias))) // Si les critères de qualité sont demandés, on les regarde
		{
			if(!atLeastOneLocationFound) //Si on n'avait toujours pas de position on la prend
			{
				GPSManager_copySatelliteCoordinate(resultPtr, &position);
				atLeastOneLocationFound = true;
				//USARTManager_printUsbWait("Valid Position found !\r\n");
				if(params->returnFirstPositionFound)
				{
					break;
				}
			}
			else if(GPSManager_isPositionBetter(&position, resultPtr))
			{
				GPSManager_copySatelliteCoordinate(resultPtr, &position);
				USARTManager_printUsbWait("Better valid Position found !\r\n");
			}
		}
	}
	GPSManager_stopAskingCurrentPositionFromMode();

	return atLeastOneLocationFound;
}


#if (ACTIVATE_GPS_LOGS == 1)
void GPSManager_addGPSLog(SatelliteCoordinate* satCoordinatePtr)
{
	USARTManager_printUsbWait("Adding new GPS Log.\r\n");
	if(_GPSLogIndex >= GPS_LOG_COUNT)
	{
		_GPSLogIndex = 0;
	}

	_GPSLogs[_GPSLogIndex].logId = _GPSLogId;
	_GPSLogs[_GPSLogIndex].timestamp = RTCManager_getCurrentTimestamp();
	GPSManager_copySatelliteCoordinate(&(_GPSLogs[_GPSLogIndex].satCoordinate), satCoordinatePtr);

	_GPSLogIndex++;
	_GPSLogId++;
}

void GPSManager_printGPSLogs()
{
	char buff[16];
	USARTManager_printUsbWait("####### GPS LOGS #######################################################\r\n");
	USARTManager_printUsbWait("logId, timestamp, lat, lon, alt, fixQuality, satellitesNumber, hdop\r\n");
	for(uint8_t i = 0; i<min(_GPSLogId, GPS_LOG_COUNT);i++)
	{
		USARTManager_printUsbWait("\t");
		stringHelper_intToString(_GPSLogs[i].logId, (unsigned char*)buff);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait(",");
		stringHelper_intToString(_GPSLogs[i].timestamp, (unsigned char*)buff);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait(",");
		stringHelper_doubleToString(_GPSLogs[i].satCoordinate.coordinate.lat, (char*)buff, 9);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait(",");
		stringHelper_doubleToString(_GPSLogs[i].satCoordinate.coordinate.lon, (char*)buff, 9);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait(",");
		stringHelper_doubleToString(_GPSLogs[i].satCoordinate.coordinate.alt, (char*)buff, 2);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait(",");
		stringHelper_intToString(_GPSLogs[i].satCoordinate.fixQuality, (unsigned char*)buff);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait(",");
		stringHelper_intToString(_GPSLogs[i].satCoordinate.satellitesNumber, (unsigned char*)buff);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait(",");
		stringHelper_doubleToString(_GPSLogs[i].satCoordinate.hDOP, (char*)buff, 1);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait("\r\n");
	}
	USARTManager_printUsbWait("########################################################################\r\n");
}

#endif

/* Returns true if position 1 better than position 2. Returns false otherwise. */
bool GPSManager_isPositionBetter(SatelliteCoordinate *positionPtr1, SatelliteCoordinate *positionPtr2)
{
	if(positionPtr1->fixQuality > positionPtr2->fixQuality) // DPGS prefered over simple GPS, over all the other values
	{
		return true;
	}
	else if(positionPtr1->fixQuality == positionPtr2->fixQuality
	&& positionPtr1->satellitesNumber > positionPtr2->satellitesNumber)
	{
		return true;
	}
	else if(positionPtr1->fixQuality == positionPtr2->fixQuality
	&& positionPtr1->satellitesNumber == positionPtr2->satellitesNumber
	&& positionPtr1->hDOP < positionPtr2->hDOP)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/* Returns true if the coordinate respects the quality criterias, false otherwise */
bool GPSManager_respectsQualityCriteria(SatelliteCoordinate *coordinatePtr, QualityCriterias *criterias)
{
	return coordinatePtr->hDOP <= criterias->maxHdop && coordinatePtr->satellitesNumber >= criterias->minSatNum;
}

static void clearAllcurPosRequestFlag(){
	xEventGroupClearBits(
	_gpsMaskhandle,
	GPS_BIT_CURPOS_REQ_BIT_ON_DEMAND | GPS_BIT_CURPOS_REQ_BIT_MODES | GPS_BIT_CURPOS_REQ_BIT_SOS
	);
}

static void clearAllcurPosAnswerFlag(){
	xEventGroupClearBits(
	_gpsMaskhandle,
	GPS_BIT_CURPOS_ANS_BIT_ON_DEMAND | GPS_BIT_CURPOS_ANS_BIT_MODES | GPS_BIT_CURPOS_ANS_BIT_SOS
	);
}

void GPSManager_printWrongNMEALog()
{
	#if (ACTIVATE_WRONG_NMEA_FRAME_LOGS == 1)
	// Restore comas
	uint8_t curChar = wrongNMEABuffer[0];
	uint8_t i=0;
	while(curChar != '*' && i<100)
	{
		curChar = wrongNMEABuffer[i];
		if(curChar == 0)
		{
			wrongNMEABuffer[i]=',';
		}
		i++;
	}

	USARTManager_printUsbWaitN((char*)wrongNMEABuffer, 100);
	#endif
}

/* Waits for a valid GXGGA frame (with checksum validation) */
static bool waitGGAFrame(uint8_t* resultBuf, uint32_t timeout)
{
	bool nmeaFound = false;
	#if (FAKE_POSITIONS_ACTIVATED==1)
	UNUSED(timeout);
	vTaskDelay(1000);
	uint32_t randNum = TRNGManager_getTRN(); // Use randnum num to randomly pick a frame

	//strcpy(resultBuf, FAKE_NMEA_FRAME_15);
	
	if(randNum % 3 == 0)
	{
		strcpy(resultBuf, FAKE_NMEA_FRAME_1);
	}
	else if(randNum % 3 == 1)
	{
		strcpy(resultBuf, FAKE_NMEA_FRAME_4);
	}
	else if(randNum % 3 == 2)
	{
		strcpy(resultBuf, FAKE_NMEA_FRAME_2);
	}
	
	nmeaFound = true;

	#else

	clearNMEAAvailableBit();
	EventBits_t gpsMask = xEventGroupWaitBits(_gpsMaskhandle, GPS_BIT_NMEA_AVAILABLE, pdFALSE, pdTRUE, timeout); // we do NOT clear on exit to avoid overwrite until we copied the data
	if(gpsMask & GPS_BIT_NMEA_AVAILABLE)
	{
		nmeaFound = true;
	}
	#endif

	if(nmeaFound)
	{
		
		if(validateChecksum(gps_buf, GPS_NMEA_NUMBER))
		{
			//
			strcpy(resultBuf, gps_buf);
			return true;
		}
	}

	return false;
}

static void clearNMEAAvailableBit()
{
	xEventGroupClearBits(_gpsMaskhandle, GPS_BIT_NMEA_AVAILABLE);
}


void GPSManager_test()
{
	//GPSManager_powerOnGPS();
	while(1)
	{
		USARTManager_printUsbWait("Waiting for GGA data...\r\n");
		char result[128];
		if(waitGGAFrame(result, 10000))
		{
			USARTManager_printUsbWait(gps_buf);
		}
	}
}

void GPSManager_setGPSBitsFromISR(EventBits_t bitsToSet)
{
	BaseType_t xHigherPriorityTaskWoken, xResult;

	/* xHigherPriorityTaskWoken must be initialised to pdFALSE. */
	xHigherPriorityTaskWoken = pdFALSE;

	xResult = xEventGroupSetBitsFromISR(_gpsMaskhandle,bitsToSet,&xHigherPriorityTaskWoken);

	if( xResult != pdFAIL )
	{
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}

EventBits_t GPSManager_getGPSBitsFromISR()
{
	return xEventGroupGetBitsFromISR(_gpsMaskhandle);
}

bool GPSManager_testGPSUSART(uint32_t timeout)
{
	GPSManager_askGPSUSARTTest();
	EventBits_t result = xEventGroupWaitBits(_gpsMaskhandle, GPS_BIT_ANS_GPS_USART_TEST, pdTRUE, pdTRUE, timeout);
	GPSManager_stopAskingGPSUSARTTest();
	if(result & GPS_BIT_ANS_GPS_USART_TEST)
	{
		return true;
	}
	return false;
}