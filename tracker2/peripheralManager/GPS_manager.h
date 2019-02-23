#ifndef GPS_MANAGER_H_
#define GPS_MANAGER_H_

#include <stdlib.h>
#include <string.h>
#include <port_sgs.h>
#include <tools/string_helper.h>
#include <tools/utils.h>
#include <peripheralManager/USART_manager.h>
#include <seekiosManager/mask_utilities.h>
#include <sgs/helper_sgs.h>
#include <FreeRTOS.h>
#include <event_groups.h>
#include <stdbool.h>
#include <peripheralManager/RTC_manager.h>
#include <peripheralManager/TRNG_Manager.h>
#include <tests/test_monitor.h>

#define FAKE_NMEA_FRAME_1 "$GPGGA,144841.000,4329.3827,N,00132.0499,W,1,13,0.7,1111,M,50.8,M,,*62\r\n$GNRMC,020911.000,A,4327.7170,N,00128.8516,W,14.42,114.61,280117,,,A*56\r\n" // precise, anglet
#define FAKE_NMEA_FRAME_2 "$GPGGA,144841.000,4310.2827,N,00131.1499,W,1,5,1.77,-25.6,M,50.8,M,,*58\r\n$GNRMC,020911.000,A,4327.7170,N,00128.8516,W,14.42,114.61,280117,,,A*56\r\n" // very far from anglet
//#define FAKE_NMEA_FRAME_3 "$GPGGA,144841.000,4329.3827,N,00132.0496,W,1,5,1.66,-25.6,M,50.8,M,,*51\n$GNRMC,020911.000,A,4327.7170,N,00128.8516,W,14.42,114.61,280117,,,A*56\n" // fail checksum
#define FAKE_NMEA_FRAME_4 "$GPGGA,143841.000,4315.2827,N,00131.1499,W,1,5,6.45,2222,M,50.8,M,,*6E\r\n$GNRMC,020911.000,A,4327.7170,N,00128.8516,W,14.42,114.61,280117,,,A*56\r\n" // bad quality
//#define FAKE_NMEA_FRAME_1   "$GPGGA,172004.000,4327.9000,N,00129.5348,W,1,9,1.21,53.9,M,50.8,M,,*7C\n$GNRMC,020911.000,A,4327.7170,N,00128.8516,W,14.42,114.61,280117,,,A*56\n"
//#define FAKE_NMEA_FRAME_2   "$GPGGA,172005.000,4327.9001,N,00129.5348,W,1,9,1.21,53.9,M,50.8,M,,*7C\n$GNRMC,020911.000,A,4327.7170,N,00128.8516,W,14.42,114.61,280117,,,A*56\n"
//#define FAKE_NMEA_FRAME_3   "$GPGGA,172006.000,4327.9003,N,00129.5349,W,1,9,1.21,53.9,M,50.8,M,,*7C\n$GNRMC,020911.000,A,4327.7170,N,00128.8516,W,14.42,114.61,280117,,,A*56\n"
// #define FAKE_NMEA_FRAME_4   "$GPGGA,172124.000,4327.8983,N,00129.5376,W,1,9,1.21,50.4,M,50.8,M,,*7F"
// #define FAKE_NMEA_FRAME_5   "$GPGGA,172409.000,4327.8974,N,00129.5469,W,1,8,1.54,51.6,M,50.8,M,,*74"
// #define FAKE_NMEA_FRAME_6   "$GPGGA,172619.000,4327.8960,N,00129.5469,W,1,9,1.22,52.9,M,50.8,M,,*7E"
// #define FAKE_NMEA_FRAME_7   "$GPGGA,173229.000,4327.8946,N,00129.5424,W,1,9,1.24,51.2,M,50.8,M,,*7B"
// #define FAKE_NMEA_FRAME_8   "$GPGGA,173733.000,4327.8948,N,00129.5483,W,1,9,1.25,51.8,M,50.8,M,,*7D"
// #define FAKE_NMEA_FRAME_9   "$GPGGA,173734.000,4327.8947,N,00129.5486,W,1,9,1.25,51.9,M,50.8,M,,*71"
// #define FAKE_NMEA_FRAME_10  "$GPGGA,173735.000,4327.8947,N,00129.5484,W,1,9,1.25,51.9,M,50.8,M,,*72"
// #define FAKE_NMEA_FRAME_11  "$GPGGA,173950.000,4327.8934,N,00129.5325,W,1,6,1.72,51.7,M,50.8,M,,*74"
// #define FAKE_NMEA_FRAME_12  "$GPGGA,174142.000,4327.8957,N,00129.5280,W,1,9,1.26,52.9,M,50.8,M,,*70"
// #define FAKE_NMEA_FRAME_13  "$GPGGA,174359.000,4327.8970,N,00129.5432,W,1,8,1.37,52.6,M,50.8,M,,*7C"
// #define FAKE_NMEA_FRAME_14  "$GPGGA,174400.000,4327.8971,N,00129.5434,W,1,8,1.37,52.6,M,50.8,M,,*70"
//#define FAKE_NMEA_FRAME_15  "$GPGGA,174837.000,4327.8960,N,00129.5489,W,1,8,1.37,45.6,M,50.8,M,,*78\n$GNRMC,020911.000,A,4327.7170,N,00128.8516,W,14.42,114.61,280117,,,A*56\n" // technocité, milieu escaliers

#define GPS_NMEA_NUMBER 2  //nombre de sentences nmea que l'on utilise (GGA & RMC)

#define GPS_WARMUP_TIME			60000
#define GPS_MAX_FIXTIME			60000
#define GPS_MAX_FIXTIME_ZONE	120000
#define GPS_READING_TIMEOUT		30000
#define GPS_MAX_OCCURRENCE		50
#define GPS_EXPIRATION_TIME		5 // After we stop using the GPS, expiration time (in min) before we shut down the GPS

#define GPS_BIT_CURPOS_REQ_BIT_ON_DEMAND	(1 << 0)
#define GPS_BIT_CURPOS_REQ_BIT_MODES		(1 << 1)
#define GPS_BIT_CURPOS_REQ_BIT_SOS			(1 << 2)
#define GPS_BIT_CURPOS_ANS_BIT_ON_DEMAND	(1 << 3)
#define GPS_BIT_CURPOS_ANS_BIT_MODES		(1 << 4)
#define GPS_BIT_CURPOS_ANS_BIT_SOS			(1 << 5)
#define GPS_BIT_NMEA_AVAILABLE				(1 << 6)
#define GPS_BIT_REQ_GPS_USART_TEST			(1 << 7)
#define GPS_BIT_ANS_GPS_USART_TEST			(1 << 8)

typedef enum {
	GPS_TYPE_NONE,
	GPS_TYPE_GPRMC,
	GPS_TYPE_GPGGA,
	GPS_TYPE_GPGLL,
} E_GPS_TYPE;

typedef struct {
	double lat;
	double lon;
	float alt;	// in meter
} Coordinate;

typedef enum{
	FIX_QUALITY_INVALID = 0,
	FIX_QUALITY_GPS		= 1,
	FIX_QUALITY_DGPS	= 2
}E_FIX_QUALITY;

typedef struct{
	Coordinate coordinate;
	float hDOP;
	uint8_t satellitesNumber;
	E_FIX_QUALITY fixQuality;
	float speed;
	float trackAngle;
} SatelliteCoordinate;

typedef enum
{
	GPS_RESPONSE_STATUS_NONE = 0,
	GPS_RESPONSE_STATUS_FRAME_RECEIVED = 1, // Valid GXGGA frame received (no checksum)
	GPS_RESPONSE_STATUS_FRAME_PARSED = 2, // Valid GXGGA frame received with position data
}E_GPS_RESPONSE_STATUS;

#if (ACTIVATE_GPS_LOGS == 1)
typedef struct  
{
	uint32_t logId;
	uint32_t timestamp;
	SatelliteCoordinate satCoordinate;
}GPSLog;
#endif

typedef struct vector{
	double x;
	double y;
} Vector;

typedef struct {
	double maxHdop;
	double minSatNum;
}QualityCriterias;

typedef struct {
	TickType_t timeFrameToGetBestPosition;
	bool respectsQualityCriteria;
	bool continueAfterTimeframeIfPositionFound;
	bool returnFirstPositionFound;
	QualityCriterias qualityCriterias;
}GetPositionParameters;

void GPSManager_init(void);
void GPSManager_getGPSData(void);
void task_GPSManager(void* param);
uint8_t GPSManager_powerOnGPS(void);
uint8_t GPSManager_powerOffGPS(void);
bool check_sentence_validity(char *sentence,uint16_t sentenceSIZE);
double convertNMEA(char * param, char * orientation);
int parse_sentence(char *sentencebuffer,SatelliteCoordinate *currentlocationPtr);
void GPSManager_requestCurrentPosition(uint8_t requestBit);
bool GPSManager_isCurrentPositionAvailable(uint8_t requestBit);
void GPSManager_askCurrentPositionFromMode(void);
void GPSManager_askCurrentPositionFromOnDemand(void);
void GPSManager_askCurrentPositionFromSOS(void);
void GPSManager_askGPSUSARTTest(void);
void GPSManager_stopAskingGPSUSARTTest(void);
void GPSManager_stopAskingCurrentPositionFromMode(void);
void GPSManager_stopAskingCurrentPositionFromOnDemand(void);
void GPSManager_stopAskingCurrentPositionFromSOS(void);
bool GPSManager_waitCurrentPositionFromMode(SatelliteCoordinate *result, TickType_t timeout);
bool GPSManager_waitCurrentPositionFromOnDemand(SatelliteCoordinate *result, TickType_t timeout);
bool GPSManager_waitCurrentPositionFromSOS(SatelliteCoordinate *result, TickType_t timeout);
void GPSManager_copySatelliteCoordinate(SatelliteCoordinate *dest, SatelliteCoordinate *src);
bool GPSManager_getPositionFromMode(SatelliteCoordinate* result, GetPositionParameters* criterias);
bool catch_GGA_Data(char *sentencebuffer,int dollarINDEX);
bool GPSManager_isGPSOn(void);
bool GPSManager_isGPSExpirationAlarmScheduled(void);
void GPSManager_removeGPSExpirationAlarm(void);
void GPSManager_scheduleGPSExpirationAlarm(void);
bool GPSManager_isGPSBufferEmpty();
void GPSManager_printGPSExpirationAlarm(void);
void GPSManager_printGPSLogs(void);
void GPSManager_addGPSLog(SatelliteCoordinate* satCoordinatePtr);
void GPSManager_printWrongNMEALog(void);
void GPSManager_test(void);
void GPSManager_setGPSBitsFromISR(EventBits_t bitsToSet);
EventBits_t GPSManager_getGPSBitsFromISR(void);
bool GPSManager_isPositionBetter(SatelliteCoordinate *positionPtr1, SatelliteCoordinate *positionPtr2);
E_GPS_RESPONSE_STATUS GPSManager_waitCoordinate(SatelliteCoordinate *resultCoordinate, uint32_t timeout);
bool GPSManager_respectsQualityCriteria(SatelliteCoordinate *coordinatePtr, QualityCriterias *criterias);
void GPSManager_askGPSUSARTTest(void);
bool GPSManager_testGPSUSART(uint32_t timeout);
void GPSManager_showAllGPSData(void);

#endif /* GPS_MANAGER_H_ */