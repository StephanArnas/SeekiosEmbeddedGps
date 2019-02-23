/*
 * functionnalities_test.h
 *
 * Created: 05/08/2016 14:14:40
 *  Author: Luc
 */ 


#ifndef FUNCTIONNALITIES_TEST_H_
#define FUNCTIONNALITIES_TEST_H_

#include <FreeRTOS.h>
#include <event_groups.h>
#include <seekiosManager/mask_utilities.h>
#include <peripheralManager/GPS_manager.h>
#include <seekiosCore/seekios.h>
#include <sgs/serial_sgs.h>
#include <peripheralManager/LED_manager.h>
#include <peripheralManager/GSMManager.h>
#include <peripheralManager/GPS_manager.h>
#include <peripheralManager/bma222_adapted.h>
#include <peripheralManager/BLE_manager_adapted.h>
#include <seekiosManager/mask_utilities.h>
#include <seekiosManager/task_management_utilities.h>
#include <tools/led_utilities.h>
#include <seekiosBLE/customProfiles/testBLE.h>
#include <sgs/port_sgs.h>
#include <hpl_calendar.h>
#include <peripheralManager/USB_manager.h>

/* Bit orders for the SHR */
// FUNCTIONALITY_BIT_GSM_USART				0 // GSM to MCU USART communication
// FUNCTIONALITY_BIT_GSM_GPRS				1 // GSM GPRS Functionality
// FUNCTIONALITY_BIT_GPS_USART				2 // GPS Usart communication
// FUNCTIONALITY_BIT_GPS_FRAMES				3 // GPS frames received correctly (GPGGA etc)
// FUNCTIONALITY_BIT_GPS_POSITION			4 // GPS positions received well
// FUNCTIONALITY_BIT_BLE_CONFIGURED			5 // BLE Working and configured
// FUNCTIONALITY_BIT_BLE_ADVERTISING		6 // BLE Advertising successful
// FUNCTIONALITY_BIT_BLE_CONNECTION			7 // BLE Connection successful
// FUNCTIONALITY_BIT_IMU_GET_ACCELERATIONS	8 // IMU Getting acceleration data
// FUNCTIONALITY_BIT_IMU_INTERRUPTION		9 // IMU Generating slope interruptions
// FUNCTIONALITY_BIT_DATAFLASH_READ			10 // DF Success reading from
// FUNCTIONALITY_BIT_DATAFLASH_WRITE		11 // DF Success writing to
// FUNCTIONALITY_BIT_LEDS					12 // LEDs working right
// FUNCTIONALITY_BIT_BUTTON					13 // Button interrupt working
// FUNCTIONALITY_BIT_CALENDAR_TIME			14 // Calendar time working
// FUNCTIONALITY_BIT_CALENDAR_INTERRUPT		15 // Calendar alarm interrupt working
// FUNCTIONALITY_BIT_ADC_USB				16 // USB Charge working
// FUNCTIONALITY_BIT_ADC_BATTERY			17 // USB Charge working

typedef struct{
	uint8_t UID[10];
	uint8_t versionName[10];
}SeekiosTestReport;

#define RSSI_SAMPLES_COUNT 10

typedef struct{
	bool isGSMPoweringOn;
	bool isNetworkWorking;
	bool isGPRSWorking;
	uint8_t RSSISamples[RSSI_SAMPLES_COUNT];
	uint8_t IMEI[20];
	uint8_t IMSI[20];
	uint16_t batteryVoltage;
}GSMTestReport;

typedef struct{
	bool isUSARTWorking;
	bool isFramesWorking;
	bool isPositionWorking;
}GPSTestReport;

typedef struct{
	bool isBLEConfigured;
	bool isAdvertising;
	bool isBLEConnectionWorking;
	uint8_t MACAddress[15];
}BLETestReport;

typedef struct{
	bool isInterruptionWorking;
	int8_t accelerations[3];
	// TODO : put accel samples ?
}IMUTestReport;

typedef struct{
	bool isLEDsWorking;
}LEDTestReport;

typedef struct{
	bool isButtonWorking;
}ButtonTestReport;

typedef struct{
	bool isReadWorking;
	bool isWriteWorking;
}DFTestReport;

typedef struct{
	bool isTimeWorking;
	bool isAlarmWorking;
}CalendarTestReport;

typedef struct{
	int ADCUSBVoltage;
	int ADCBatVoltage;
	uint16_t voltageMin;
	uint16_t voltageMax;
}ADCTestReport; 


typedef struct{
	SeekiosTestReport	seekiosTestReport;
	GSMTestReport		gsmTestReport;
	GPSTestReport		gpsTestReport;
	BLETestReport		bleTestReport;
	IMUTestReport		imuTestReport;
	LEDTestReport		ledTestReport;
	ButtonTestReport	buttonTestReport;
	DFTestReport		dfTestReport;
	CalendarTestReport	calendarTestReport;
	ADCTestReport		adcTestReport;
}TestReport;

void functionalitiesTest_setFunctionnalitiesMaskBitsFromISR(EventBits_t bits);
void functionalitiesTest_init(void);
void functionalitiesTest_runTests(bool isProductionTest);
void functionalitiesTest_testBLE(void);
void task_testBLE(void);
void task_testGSM(void);
void task_testGPS(void);
void task_testCalendars(void);

#define PRODUCTION_TEST_MAX_TIME_MS 60000

#endif /* FUNCTIONNALITIES_TEST_H_ */