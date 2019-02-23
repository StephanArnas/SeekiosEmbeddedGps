#ifndef MODES_H_
#define MODES_H_

#include <string.h>
#include <seekiosCore/seekios.h>
#include <seekiosManager/mask_utilities.h>
#include <modesManager/modes_toolkit.h>
#include <peripheralManager/GPS_manager.h>
#include <peripheralManager/RTC_manager.h>
#include <FreeRTOS.h>
#include <task.h>
#include <peripheralManager/RTC_manager.h>
#include <thirdparty/wireless/ble_sdk/ble_services/ble_mgr/ble_manager.h>
#include <peripheralManager/BLE_manager_adapted.h>
#include <tools/geolocation_tools.h>
#include <peripheralManager/bma222_adapted.h>
#include <seekiosBLE/customProfiles/dontMoveBLE.h>
#include <tools/led_utilities.h>
#include <authentication/authentication.h>
#include <math.h>
#include <tests/test_monitor.h>

#define DONT_MOVE_BLE_CONNECTION_TIMEOUT			15000 // in milliseconds

#define GET_GPS_POSITION_TIMEOUT					60000
#define GET_GPS_POSITION_LONG_TIMEOUT				120000
#define LONG_REFRESHRATE_LIMIT						90000
#define MAX_REFRESH_RATE_ACCELEROMETER_IMPROVEMENT	60 // The maximum refresh rate where we will perform an accelerometer improvement on the positions

/* Zone mode parameters */
#define OUT_OF_ZONE_THRESHOLD		60
#define MAX_DISTANCE_FROM_ZONE		20
#define FAR_DISTANCE_FROM_ZONE		500
#define SPEED_THRESHOLD_HIGH		20
#define SPEED_THRESHOLD_VERY_SLOW	1.5

void modes_SOS(void* param);
void modes_waitingMode(void* param);
void modes_onDemand(void* param);
void modes_dontMoveModeMotionDetected(void* param);
void modes_trackingMode(void* param);
void modes_zoneMode(void* param);
void modes_inTimeMode(void* param);
void modes_followMeMode(void* param);
void modes_dailyTrackingMode(void* param);
void dontMove_startSlopeDetection(void);
void dontMove_testDontMoveMotionDetection(void);
void dontMove_init(void);
void tracking_infiniteTracking(E_MESSAGE_TYPE trackingMessageType, int refreshRateMin);

typedef struct{
	struct tm nextInTimeDate;
	uint8_t nextInTimeMargin;
}NextInTimeData;

#endif /* MODES_H_ */