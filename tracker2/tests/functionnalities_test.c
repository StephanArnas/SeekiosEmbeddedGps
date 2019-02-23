/* This file is designed to test the functionnalities of the Seekios :

X LEDs
X BLE
X> Config
X> Advertisement working
X> Connection working
X GSM
X> USART
X> GPRS
X GPS
X> USART
X> Frames reception working
X> Positions working
X IMU
X> Get accelerations working
X> Interruptions working
X Button
X Leds
X Dataflash
X Calendars ?

*/

#include <tests/functionnalities_test.h>

static void testLEDS(void);
static void testDataflash(void);
static void testIMU(void);
static void exportUSBLastResultsTests(void);
static void testBleTimeoutCallback(TimerHandle_t xTimer);
static void testADC(void);
static void waitUSBUnplugging(void);
static int waitTestStart(void);
static void initLastTestReport(void);
static void serializeSeekiosTestReport(uint8_t *res, SeekiosTestReport *seekTestReport);
static void serializeGSMTestReport(uint8_t* res, GSMTestReport* gsmTestReport);
static void serializeGPSTestReport(uint8_t* res, GPSTestReport* gpsTestReport);
static void serializeBLETestReport(uint8_t* res, BLETestReport* bleTestReport);
static void serializeIMUTestReport(uint8_t* res, IMUTestReport* imuTestReport);
static void serializeLEDTestReport(uint8_t* res, LEDTestReport* ledTestReport);
static void serializeButtonTestReport(uint8_t* res, ButtonTestReport* butTestReport);
static void serializeDataFlashTestReport(uint8_t* res, DFTestReport* dfTestReport);
static void serializeCalendarTestReport(uint8_t* res, CalendarTestReport* calTestReport);
static void serializeADCTestReport(uint8_t* res, ADCTestReport* adcTestReport);
static bool updateLastTestResults(TickType_t timeout);
static void exportGPRSLastResultsTests(void);
static uint8_t buildSeekiosHardwareReportURL(char* url);
static void catTestsResults(char* url);

#define GPS_TEST_PERIOD				10000
#define BLE_TEST_CONNECTION_TIMEOUT 20000
#define FUNCT_TEST_BUF_SIZE			128

static uint8_t _functTestBuf[FUNCT_TEST_BUF_SIZE];

static struct calendar_alarm _testAlarm;
static xTimerHandle _testBLEConnectionTimeoutTimer;

static bool _bleAdvTimeout = false;
static bool _bleDisconnected = false;
static bool _testBleConnectionTimeout = false;

TestReport _lastTestReport;

void functionalitiesTest_init(){
	_testBLEConnectionTimeoutTimer = xTimerCreate("BLE test connection timeout timer", BLE_TEST_CONNECTION_TIMEOUT, pdFALSE, (void*)0, testBleTimeoutCallback);
	initLastTestReport();
}

/*
Runs the Seekios full software and hardware test.
This task can be called in two situation. This situation should be passed as a param of the task.
- Situation ADMIN		: This task is triggered by an admin message and performs a full checkup, then reports it to the server
- Situation PRODUCTION	: This task is triggered in production, when the Seekios runs for the first time. It performs a full
checkup with specific settings, then reports it to the server and to an USB Serial connection
Notes : - the test of the GSM functionalities is made at the same time than we send the message to the server
- the test of the BLE Connection is made with an app that sends the char "test" to the Seekios
- you have to trigger the button and the IMU interrupt to validate their tests
*/
void functionalitiesTest_runTests(bool isProductionTest){
	
	maskUtilities_setRunningMaskBits(RUNNING_BIT_TEST_FUNCTIONALITIES);
	uint32_t instructionId = ledUtilities_runTestOverLedInstruction();
	if(isProductionTest)
	{
		USBManager_start();
		volatile int testsDutationSec = 60;
		while((testsDutationSec = waitTestStart()) > 0)
		{
			LEDManager_stopCurrentlyRunningInstruction();
			instructionId = ledUtilities_runTestFunctionalitiesFastLedInstruction();
			updateLastTestResults(testsDutationSec*1000); // Convert to MS
			exportUSBLastResultsTests();
			ledUtilities_runTestOverLedInstruction();
		}
		USBManager_stop();
	}
	else
	{
		// TODO : perform tests then send thru the SHR
		LEDManager_stopCurrentlyRunningInstructionId(instructionId);
		instructionId = ledUtilities_runTestFunctionalitiesFastLedInstruction();
		updateLastTestResults(PRODUCTION_TEST_MAX_TIME_MS);
		exportGPRSLastResultsTests();
	}
	LEDManager_stopCurrentlyRunningInstruction();

	volatile UBaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
	UNUSED(wm);
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_TEST_FUNCTIONALITIES);
}

static void exportGPRSLastResultsTests()
{
	// also send by USART
	USARTManager_printUsbWait("Sending data to server...\r\n");

	if(GSMManager_isModuleStarted())
	{
		if(GSMManager_startHTTP()==FUNCTION_SUCCESS)
		{
			char url[150];
			char result[10];
			if(buildSeekiosHardwareReportURL(url)==FUNCTION_SUCCESS)
			{
				if(GSMManager_httpGET(url, result) == HTTP_REQUEST_STATUS_OK)
				{
					USARTManager_printUsbWait("Sending test results to cloud success.\r\n");
				}
			}
		}
	}
}

static uint8_t buildSeekiosHardwareReportURL(char* url)
{
	strcpy(url, SEEKIOS_EMBEDDED_SERVICE_URL"SHR/");

	strcat(url,_lastTestReport.gsmTestReport.IMEI);
	strcat(url, "/");

	strcat(url,_lastTestReport.seekiosTestReport.UID);
	strcat(url,"/");

	strcat(url,_lastTestReport.gsmTestReport.IMSI);
	strcat(url,"/");

	strcat(url, _lastTestReport.bleTestReport.MACAddress);
	strcat(url, "/");

	char battery[4] = "";
	batteryManager_getBatteryPercentageStr(battery);
	strcat(url, battery);
	strcat(url,"/");

	if(RTCManager_isCalendarInitialized()){
		char buff[12] = "";
		stringHelper_intToString(RTCManager_getCurrentTimestamp(), (unsigned char*)buff);
		strcat(url, buff);
		strcat(url, "/");
	}
	else
	{
		strcat(url, "-1/");
	}

	catTestsResults(url);

	seekiosInfoManager_catSeekiosVersion(url);

	return FUNCTION_SUCCESS;
}

static void catTestsResults(char* url)
{
	_lastTestReport.gsmTestReport.isGSMPoweringOn									?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.gsmTestReport.isGPRSWorking										?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.gpsTestReport.isUSARTWorking									?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.gpsTestReport.isFramesWorking									?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.gpsTestReport.isPositionWorking									?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.bleTestReport.isBLEConfigured									?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.bleTestReport.isAdvertising										?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.bleTestReport.isBLEConnectionWorking							?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.imuTestReport.accelerations[0] != 0 || _lastTestReport.imuTestReport.accelerations[1] != 0 || _lastTestReport.imuTestReport.accelerations[2] != 0 ?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.imuTestReport.isInterruptionWorking								?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.dfTestReport.isReadWorking										?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.dfTestReport.isWriteWorking										?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.ledTestReport.isLEDsWorking										?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.buttonTestReport.isButtonWorking								?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.calendarTestReport.isTimeWorking								?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.calendarTestReport.isAlarmWorking								?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.adcTestReport.ADCBatVoltage > BATTERY_LEVEL_0_VOLTAGE			?	strcat(url,"1") : strcat(url,"0");
	_lastTestReport.adcTestReport.ADCUSBVoltage > USB_LEVEL_MINIMUM_CHARGE_VOLTAGE	?	strcat(url,"1") : strcat(url,"0");
}

/* Updates the lastTestReport. If the test wasn't completed before the timeout, then the updating ends anyway.
If the GSM fails to be turned on, then the test finishes anyway.*/
static bool updateLastTestResults(TickType_t timeout)
{
	TickType_t startingTick = xTaskGetTickCount();

	initLastTestReport();
	testLEDS();
	USARTManager_printUsbWait("Full test of the Seekios.\r\n START TESTS WHEN USB PLUGGED\r\nTest button by clicking it\r\nTest imu by tapping firmly\r\nTest BLE by connecting to it");
	taskManagementUtilities_startGSMTestTask();
	testIMU();
	#if (BLE_ACTIVATED == 1)
	taskManagementUtilities_startBLETestTask();
	#endif

	taskManagementUtilities_startGPSTestTask();
	taskManagementUtilities_startCalendarsTestTask();
	testDataflash();
	testADC();
	
	seekiosInfoManager_seekiosVersionToString(_lastTestReport.seekiosTestReport.versionName);
	bool timedOut = false;
	
	/* Waiting for the test to be over. The conditions for the test to be over are : 
		- the GSM doesn't turn ON (gsm task over and gsm USART test result to false)
		- the timeout is reached
		- all result gathering are over (all running test tasks are over)
	*/
	volatile EventBits_t runningMask;
	volatile uint32_t elapsedTime = 0;
	do 
	{
		TickType_t currentTime = xTaskGetTickCount();
		elapsedTime = currentTime - startingTick;
		timedOut = elapsedTime > timeout;
		runningMask = maskUtilities_getRunningMask();
	} while (!timedOut
	&& !((runningMask & RUNNING_BIT_TEST_GSM) == 0 && (_lastTestReport.gsmTestReport.isGSMPoweringOn == false)) /* GSM test over but GSM usart not working */
	&& ((runningMask & RUNNING_BIT_TEST_GPS)!= 0 || (runningMask & RUNNING_BIT_TEST_GSM) != 0 || (runningMask & RUNNING_BIT_TEST_CALENDARS)!=0));

	// /!/ If at this point, some tasks might still be active, however they should end by themselves - GSM, GPS, CALENDARS /!/

	volatile EventBits_t interruptMask = maskUtilities_getInterruptMask();
	batteryManager_updateChargeStatus();

	if((interruptMask & INTERRUPT_BIT_MOTION_DETECTED)>0)
	{
		_lastTestReport.imuTestReport.isInterruptionWorking = true;
		maskUtilities_clearInterruptMaskBits(INTERRUPT_BIT_MOTION_DETECTED);
	}

	if((interruptMask & INTERRUPT_BIT_BUTTON) > 0)
	{
		_lastTestReport.buttonTestReport.isButtonWorking = true;
		maskUtilities_clearInterruptMaskBits(INTERRUPT_BIT_BUTTON);
	}

	// TODO : check if charging
	uint16_t voltages[2];
	ADC_read_BAT_USB(voltages);
	_lastTestReport.adcTestReport.ADCUSBVoltage = voltages[1];
	_lastTestReport.adcTestReport.ADCBatVoltage = voltages[0];

	USARTManager_printUsbWait("Tests over.\r\n");
	uint8_t buff[10];
	stringHelper_intToString(elapsedTime, buff);
	USARTManager_printUsbWait("Test duration : ");
	USARTManager_printUsbWait(buff);
	USARTManager_printUsbWait("\r\n");
	IMUManager_stopSlopeDetection();
	IMUManager_suspend();

	return true;
}

static void waitUSBUnplugging()
{
	uint16_t voltages[2];
	do
	{
		ADC_read_BAT_USB(voltages);
		vTaskDelay(500);
	}while(voltages[1] > USB_LEVEL_MINIMUM_CHARGE_VOLTAGE);
}


static void serializeSeekiosTestReport(uint8_t *res, SeekiosTestReport *seekTestReport)
{
	strcpy(res, TEST_RES_SKS);
	strcat(res,":");
	strcat(res,seekTestReport->UID);
	strcat(res, ",");
	strcat(res,seekTestReport->versionName);
	strcat(res, "\r\n");
}

static void serializeGSMTestReport(uint8_t* res, GSMTestReport* gsmTestReport)
{
	strcpy(res, TEST_RES_GSM);
	strcat(res,":");
	gsmTestReport->isGSMPoweringOn == 1 ? strcat(res,"1,") : strcat(res, "0,");
	gsmTestReport->isNetworkWorking == 1 ? strcat(res,"1,") : strcat(res, "0,");
	gsmTestReport->isGPRSWorking == 1 ? strcat(res,"1,") : strcat(res, "0,");
	uint8_t buf[6];
	
	stringHelper_intToString(RSSI_SAMPLES_COUNT, buf);
	strcat(res, buf);
	strcat(res, ",");
	
	for(uint8_t i=0;i<RSSI_SAMPLES_COUNT; i++)
	{
		stringHelper_intToString(gsmTestReport->RSSISamples[i], buf);
		strcat(res, buf);
		strcat(res, ",");
	}
	
	strcat(res, gsmTestReport->IMEI);
	strcat(res, ",");
	strcat(res, gsmTestReport->IMSI);
	
	strcat(res,",");
	stringHelper_intToString(gsmTestReport->batteryVoltage, buf);
	strcat(res,buf);
	strcat(res,"\r\n");
}

static void serializeGPSTestReport(uint8_t* res, GPSTestReport* gpsTestReport)
{
	strcpy(res, TEST_RES_GPS);
	strcat(res,":");
	gpsTestReport->isUSARTWorking == 1 ? strcat(res,"1,") : strcat(res, "0,");
	gpsTestReport->isFramesWorking == 1 ? strcat(res,"1,") : strcat(res, "0,");
	gpsTestReport->isPositionWorking == 1 ? strcat(res,"1") : strcat(res, "0");
	strcat(res,"\r\n");

}

static void serializeBLETestReport(uint8_t* res, BLETestReport* bleTestReport)
{
	strcpy(res, TEST_RES_BLE);
	strcat(res,":");
	bleTestReport->isBLEConfigured == 1 ? strcat(res,"1,") : strcat(res, "0,");
	bleTestReport->isAdvertising == 1 ? strcat(res,"1,") : strcat(res, "0,");
	bleTestReport->isBLEConnectionWorking == 1 ? strcat(res,"1,") : strcat(res, "0,");
	strcat(res,bleTestReport->MACAddress);
	strcat(res,"\r\n");

}

static void serializeIMUTestReport(uint8_t* res, IMUTestReport* imuTestReport)
{
	strcpy(res, TEST_RES_IMU);
	strcat(res,":");
	imuTestReport->isInterruptionWorking == 1 ? strcat(res,"1") : strcat(res, "0");
	
	for(uint8_t i = 0;i<3;i++)
	{
		uint8_t buf[6];
		strcat(res,",");
		stringHelper_intToString(imuTestReport->accelerations[i], buf);
		strcat(res,buf);
	}
	
	strcat(res,"\r\n");

}

static void serializeLEDTestReport(uint8_t* res, LEDTestReport* ledTestReport)
{
	strcpy(res, TEST_RES_LED);
	strcat(res,":");
	ledTestReport->isLEDsWorking == 1 ? strcat(res,"1") : strcat(res, "0");
	strcat(res,"\r\n");

}

static void serializeButtonTestReport(uint8_t* res, ButtonTestReport* butTestReport)
{
	strcpy(res, TEST_RES_BUT);
	strcat(res,":");
	butTestReport->isButtonWorking == 1 ? strcat(res,"1") : strcat(res, "0");
	strcat(res,"\r\n");

}

static void serializeDataFlashTestReport(uint8_t* res, DFTestReport* dfTestReport)
{
	strcpy(res, TEST_RES_DF);
	strcat(res,":");
	dfTestReport->isReadWorking == 1 ? strcat(res,"1,") : strcat(res, "0,");
	dfTestReport->isWriteWorking == 1 ? strcat(res,"1") : strcat(res, "0");
	strcat(res,"\r\n");

}

static void serializeCalendarTestReport(uint8_t* res, CalendarTestReport* calTestReport)
{
	strcpy(res, TEST_RES_CAL);
	strcat(res,":");
	calTestReport->isTimeWorking == 1 ? strcat(res,"1,") : strcat(res, "0,");
	calTestReport->isAlarmWorking == 1 ? strcat(res,"1") : strcat(res, "0");
	strcat(res,"\r\n");

}

static void serializeADCTestReport(uint8_t* res, ADCTestReport* adcTestReport)
{
	strcpy(res, TEST_RES_ADC);
	strcat(res,":");
	uint8_t buf[10];
	stringHelper_intToString(adcTestReport->ADCUSBVoltage, buf);
	strcat(res,buf);
	strcat(res,",");
	stringHelper_intToString(adcTestReport->ADCBatVoltage, buf);
	strcat(res,buf);
	strcat(res,",");
	stringHelper_intToString(adcTestReport->voltageMin, buf);
	strcat(res,buf);
	strcat(res,",");
	stringHelper_intToString(adcTestReport->voltageMax, buf);
	strcat(res,buf);
	strcat(res,"\r\n");
}

/* tests the GSM powering on */
void task_testGSM()
{
	maskUtilities_setRunningMaskBits(RUNNING_BIT_TEST_GSM);
	USARTManager_printUsbWait("Starting testing GSM...");
	GSMBatData batData;
	batData.batteryPercentage = 0;
	batData.batteryVoltage = 0;
	if(GSMManager_powerOnGSM()==FUNCTION_SUCCESS){
		_lastTestReport.gsmTestReport.isGSMPoweringOn = true;
		GSMManager_getIMEI(_lastTestReport.gsmTestReport.IMEI);
		GSMManager_getIMSI(_lastTestReport.gsmTestReport.IMSI);
		seekiosInfoManager_updateSeekiosUID();
		seekiosInfoManager_getSeekiosUID(_lastTestReport.seekiosTestReport.UID);
		seekiosInfoManager_checkIfSeekiosVersionNew();
		GSMManager_getGSMBatteryData(&batData);
		if(GSMManager_attachNetworkServiceManually(LONG_WAIT)==FUNCTION_SUCCESS)
		{
			_lastTestReport.gsmTestReport.isNetworkWorking = true;
			for(uint8_t i=0;i<RSSI_SAMPLES_COUNT;i++)
			{
				_lastTestReport.gsmTestReport.RSSISamples[i] =	GSMManager_getRSSIInt();
				vTaskDelay(1000);
			}
			
			if(GSMManager_startHTTP())
			{
				_lastTestReport.gsmTestReport.isGPRSWorking = true;
				GSMManager_stopHTTP();
			}
		}
	}
	USARTManager_printUsbWait("...GSM Testing over !\r\n");
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_TEST_GSM);
	FreeRTOSOverlay_taskDelete(NULL);
}

/* Waits for the USB cable to be plugged and detect voltage + checks battery voltage adc */
static void testADC()
{
	uint16_t voltages[2];
	ADC_read_BAT_USB(voltages);

	if(voltages[0] > BATTERY_LEVEL_0_VOLTAGE)
	{

	}
	
	if(voltages[1] > USB_LEVEL_MINIMUM_CHARGE_VOLTAGE)
	{

	}
}

/* Exports the tests results on the USART debug and over the USB */
static void exportUSBLastResultsTests(){

	serializeSeekiosTestReport(_functTestBuf, &(_lastTestReport.seekiosTestReport));
	USBManager_print(_functTestBuf);
	USARTManager_printUsbWait(_functTestBuf);

	serializeGSMTestReport(_functTestBuf, &(_lastTestReport.gsmTestReport));
	USBManager_print(_functTestBuf);
	USARTManager_printUsbWait(_functTestBuf);

	serializeGPSTestReport(_functTestBuf, &(_lastTestReport.gpsTestReport));
	USBManager_print(_functTestBuf);
	USARTManager_printUsbWait(_functTestBuf);

	serializeBLETestReport(_functTestBuf, &(_lastTestReport.bleTestReport));
	USBManager_print(_functTestBuf);
	USARTManager_printUsbWait(_functTestBuf);

	serializeIMUTestReport(_functTestBuf, &(_lastTestReport.imuTestReport));
	USBManager_print(_functTestBuf);
	USARTManager_printUsbWait(_functTestBuf);

	serializeLEDTestReport(_functTestBuf, &(_lastTestReport.ledTestReport));
	USBManager_print(_functTestBuf);
	USARTManager_printUsbWait(_functTestBuf);

	serializeButtonTestReport(_functTestBuf, &(_lastTestReport.buttonTestReport));
	USBManager_print(_functTestBuf);
	USARTManager_printUsbWait(_functTestBuf);

	serializeDataFlashTestReport(_functTestBuf, &(_lastTestReport.dfTestReport));
	USBManager_print(_functTestBuf);
	USARTManager_printUsbWait(_functTestBuf);

	serializeCalendarTestReport(_functTestBuf, &(_lastTestReport.calendarTestReport));
	USBManager_print(_functTestBuf);
	USARTManager_printUsbWait(_functTestBuf);

	serializeADCTestReport(_functTestBuf, &(_lastTestReport.adcTestReport));
	USBManager_print(_functTestBuf);
	USARTManager_printUsbWait(_functTestBuf);

	/* Notify tests over */
	strcpy(_functTestBuf, FUNC_TEST_OVER);
	strcat(_functTestBuf, "\n");
	USBManager_print(_functTestBuf);
}

static void functionalities_test_ble_state(bool state)
{
	if(state)
	{
		xTimerStart(_testBLEConnectionTimeoutTimer, 0);
	}
	else
	{
		_bleDisconnected = true;
	}
}

static void functionalities_test_adv_report()
{
	_bleAdvTimeout = true;
}

static void functionalities_test_char_changed(uint8_t* test_char_value)
{
	if(memcmp(test_char_value, "TEST", 4)==0)
	{
		_lastTestReport.bleTestReport.isBLEConnectionWorking = true;
		testble_disconnect();
	}
}

static void testBleTimeoutCallback(TimerHandle_t xTimer)
{
	_testBleConnectionTimeout = true;
	at_ble_event_user_defined_post(NULL);
	UNUSED(xTimer);
}

void task_testBLE(){
	maskUtilities_setRunningMaskBits(RUNNING_BIT_TEST_BLE);

	// check config
	BLEManager_wakeUp();

	at_ble_addr_t address = {AT_BLE_ADDRESS_PUBLIC, {0xAB, 0xCD, 0xEF, 0xAB, 0xCD, 0xEF}};
	if(at_ble_addr_get(&address) == AT_BLE_SUCCESS)
	{
		BLEManager_convertMacAddressByteToString(address.addr, _lastTestReport.bleTestReport.MACAddress);
		_lastTestReport.bleTestReport.isBLEConfigured = true;
		if(testble_configure(NULL))
		{
			_lastTestReport.bleTestReport.isAdvertising = true;
		}

		tb_register_char_changed_callback(functionalities_test_char_changed);
		tb_register_state_callback(functionalities_test_ble_state);
		tb_register_adv_report_callback(functionalities_test_adv_report);

		_bleAdvTimeout = false;
		_bleDisconnected = false;
		_testBleConnectionTimeout= false;
		while(!_bleAdvTimeout && !_bleDisconnected)
		{
			ble_event_task();
			
			if(_testBleConnectionTimeout)
			{
				testble_disconnect();
			}
		}
		
		if(xTimerIsTimerActive(_testBLEConnectionTimeoutTimer))
		{
			xTimerStop(_testBLEConnectionTimeoutTimer, 0);
		}
		testble_deconfigure();
	}
	BLEManager_sleep();

	maskUtilities_clearRunningMaskBits(RUNNING_BIT_TEST_BLE);
	FreeRTOSOverlay_taskDelete(NULL);
}

static void testAlarmCallback()
{
	_lastTestReport.calendarTestReport.isAlarmWorking = true;
}

void task_testCalendars(){

	maskUtilities_setRunningMaskBits(RUNNING_BIT_TEST_CALENDARS);
	USARTManager_printUsbWait("Starting testing Calendar...");
	struct calendar_date_time currentDateTime;
	calendar_get_date_time(&CALENDAR_X, &currentDateTime);
	struct calendar_time time;
	time.hour = currentDateTime.time.hour;
	time.min = currentDateTime.time.min;
	time.sec = 00; // this will create a tiny offset in the seekios's datetime
	calendar_set_time(&CALENDAR_X, &time);

	_testAlarm.data.datetime.time.sec = 5;
	_testAlarm.option = CALENDAR_ALARM_MATCH_SEC;
	_testAlarm.mode = ONESHOT;
	calendar_set_alarm(&CALENDAR_X, &_testAlarm, testAlarmCallback);

	vTaskDelay(10000);
	calendar_get_date_time(&CALENDAR_X, &currentDateTime);
	if(currentDateTime.time.sec>=9)
	{
		_lastTestReport.calendarTestReport.isTimeWorking = true;
	}
	else
	{
		USARTManager_printUsbWait("Expected 10sec, got ");
		USARTManager_printInt(currentDateTime.time.sec);
		USARTManager_printUsbWait("sec\r\n");
	}
	USARTManager_printUsbWait("...Calendar testing ok !\r\n");
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_TEST_CALENDARS);
	FreeRTOSOverlay_taskDelete(NULL);
}

static void testLEDS()
{
	LEDManager_turnOnGreenLED();
	LEDManager_turnOnYellowLED();
	LEDManager_turnOnRedLED();
	vTaskDelay(1000);
	
	volatile bool gnLevel = gpio_get_pin_level(LED_0_gn);
	volatile bool rdLevel = gpio_get_pin_level(LED_1_rd);
	volatile bool yeLevel = gpio_get_pin_level(LED_2_ye);

	if(gnLevel && rdLevel && yeLevel)
	{
		_lastTestReport.ledTestReport.isLEDsWorking = true;
	}
}

void task_testGPS(){
	maskUtilities_setRunningMaskBits(RUNNING_BIT_TEST_GPS);
	USARTManager_printUsbWait("Starting testing GPS...");
	GPSManager_powerOnGPS();
	SatelliteCoordinate resultCoordinate;

	// We try to get a frame for X seconds (GPS_TEST_PERIOD)
	E_GPS_RESPONSE_STATUS gpsResponse = GPS_RESPONSE_STATUS_NONE;
	TickType_t startTicks = xTaskGetTickCount();
	uint32_t elapsedTime = 0;
	while(elapsedTime  < GPS_TEST_PERIOD && gpsResponse == GPS_RESPONSE_STATUS_NONE)
	{
		uint32_t remainingTime = GPS_TEST_PERIOD - elapsedTime;
		gpsResponse = GPSManager_waitCoordinate(&resultCoordinate, remainingTime);
		elapsedTime = xTaskGetTickCount() - startTicks;
	}

	if(gpsResponse == GPS_RESPONSE_STATUS_FRAME_PARSED)
	{
		_lastTestReport.gpsTestReport.isPositionWorking = true;
		_lastTestReport.gpsTestReport.isUSARTWorking = true;
		_lastTestReport.gpsTestReport.isFramesWorking = true;
	}
	else if(gpsResponse == GPS_RESPONSE_STATUS_FRAME_RECEIVED)
	{
		_lastTestReport.gpsTestReport.isUSARTWorking = true;
		_lastTestReport.gpsTestReport.isFramesWorking = true;
	}
	else
	{
		if(GPSManager_testGPSUSART(GPS_TEST_PERIOD)) // Try to get a full GPS coordinate
		{
			_lastTestReport.gpsTestReport.isUSARTWorking = true;
		}
	}
	
	GPSManager_powerOffGPS();
	USARTManager_printUsbWait("...GPS Testing done !\r\n");
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_TEST_GPS);
	FreeRTOSOverlay_taskDelete(NULL);
}

static void testIMU(){

	IMUManager_powerModeLowPower();
	IMUManager_startSlopeDetection();

	int8_t acc[3] = {0};
	bma222_get_acceleration(acc);
	_lastTestReport.imuTestReport.accelerations[0] = acc[0];
	_lastTestReport.imuTestReport.accelerations[1] = acc[1];
	_lastTestReport.imuTestReport.accelerations[2] = acc[2];
}

static void testDataflash(){
	char* dataToWrite  = "TEST_DATAFLASH";
	dataflashManager_writeToPage(PAGE_INDEX_TEST_DATAFLASH, strlen(dataToWrite)+1, dataToWrite);
	char resultBuff[20]={0};
	dataflashManager_readPage(PAGE_INDEX_TEST_DATAFLASH, strlen(dataToWrite)+1, (unsigned char*)resultBuff);
	if(strcmp(dataToWrite, resultBuff)==0)
	{
		_lastTestReport.dfTestReport.isReadWorking = true;
		_lastTestReport.dfTestReport.isWriteWorking = true;
	}
}

/* Waits for the TESTS_START instruction from the testing program. Returns a value over*/
static int waitTestStart()
{
	while(1)
	{
		int32_t read = cdcdf_acm_read(_functTestBuf, FUNCT_TEST_BUF_SIZE);
		
		if(read == USB_OK)
		{
			if(strstr(_functTestBuf,RX_GET_STATUS)) // Return current status of the Seekios
			{
				strcpy(_functTestBuf, TX_SEND_STATUS_FUNCTIONALITIES_TEST_PROD);
				strcat(_functTestBuf, "\r\n");
				USBManager_print(_functTestBuf);
			}
			else if(strstr(_functTestBuf, RX_TESTS_START)) // Orders test starting
			{
				strtok(_functTestBuf,":");
				char* refreshTimeStr = strtok(NULL,"\r\n");
				USARTManager_printUsbWait("START Tests with duration: ");
				USARTManager_printUsbWait(refreshTimeStr);
				USARTManager_printUsbWait("\r\n");
				volatile int refreshTime = atoi(refreshTimeStr);
				return refreshTime;
			}
			else if(strstr(_functTestBuf, RX_TESTS_STOP))
			{
				USARTManager_printUsbWait("STOP Tests\r\n");
				return 0;
			}
		}

		vTaskDelay(100);
	}
}


/* Inits all the values of the last test report */
static void initLastTestReport()
{
	/* Init general Seekios report */
	strcpy(_lastTestReport.seekiosTestReport.UID, "");
	strcpy(_lastTestReport.seekiosTestReport.versionName, "");

	/* Init GSM Test report */
	_lastTestReport.gsmTestReport.isGSMPoweringOn = false;
	_lastTestReport.gsmTestReport.isNetworkWorking = false;
	_lastTestReport.gsmTestReport.isGPRSWorking = false;
	
	for(uint8_t i=0;i<RSSI_SAMPLES_COUNT;i++)
	{
		_lastTestReport.gsmTestReport.RSSISamples[i] = 0;
	}
	
	strcpy(_lastTestReport.gsmTestReport.IMEI, "");
	strcpy(_lastTestReport.gsmTestReport.IMSI, "");
	_lastTestReport.gsmTestReport.batteryVoltage = 0;

	/* Init GPS Test report */
	_lastTestReport.gpsTestReport.isUSARTWorking = false;
	_lastTestReport.gpsTestReport.isFramesWorking = false;
	_lastTestReport.gpsTestReport.isPositionWorking = false;

	/* Init BLE Test report */
	_lastTestReport.bleTestReport.isBLEConfigured = false;
	_lastTestReport.bleTestReport.isBLEConnectionWorking = false;
	strcpy(_lastTestReport.bleTestReport.MACAddress, "FFFFFFFFFFFF");
	
	/* Init IMU Test report */
	_lastTestReport.imuTestReport.isInterruptionWorking= false;
	_lastTestReport.imuTestReport.accelerations[0] = 0;
	_lastTestReport.imuTestReport.accelerations[1] = 0;
	_lastTestReport.imuTestReport.accelerations[2] = 0;

	/* Init LED Test report */
	_lastTestReport.ledTestReport.isLEDsWorking = false;

	/* Button test report */
	_lastTestReport.buttonTestReport.isButtonWorking = false;

	/* DF test report */
	_lastTestReport.dfTestReport.isReadWorking = false;
	_lastTestReport.dfTestReport.isWriteWorking = false;

	/* Calendar test report */
	_lastTestReport.calendarTestReport.isAlarmWorking = false;
	_lastTestReport.calendarTestReport.isTimeWorking = false;

	/* ADC test report */
	_lastTestReport.adcTestReport.ADCUSBVoltage = 0;
	_lastTestReport.adcTestReport.ADCBatVoltage = 0;
	_lastTestReport.adcTestReport.voltageMin = BATTERY_LEVEL_0_VOLTAGE;
	_lastTestReport.adcTestReport.voltageMax = BATTERY_LEVEL_100_VOLTAGE;
}
