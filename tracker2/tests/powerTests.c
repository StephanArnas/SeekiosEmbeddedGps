/*
* tests.c
*
* Created: 01/08/2016 13:58:33
*  Author: Luc
*/
#include <tests/powerTests.h>

#define RUNNING_BIT_GSM			(1 << 0)
#define RUNNING_BIT_GPRS		(1 << 1)
#define RUNNING_BIT_BLE			(1 << 2)
#define RUNNING_BIT_IMU			(1 << 3)
#define RUNNING_BIT_GPS			(1 << 4)
#define RUNNING_BIT_DATAFLASH	(1 << 5)

static void turnOnGSM(void);
static void turnOnGSM(void);
static void turnOffGSM(void);
static void turnOnGPRS(void);
static void turnOffGPRS(void);
static void turnOnBLE(void);
static void turnOffBLE(void);
static void normalIMU(void);
static void sleepIMU(void);
static void turnOnGPS(void);
static void turnOffGPS(void);
static void turnOnDataflash(void);
static void turnOffDataflash(void);
static void deepSleep(void);
static void printRunningPeripherals(void);
static void sleepBLE(void);
static void wakeBLE(void);
static void getSignalQuality(void);
static EventBits_t setRunningBit(EventBits_t bitToSet);
static EventBits_t clearRunningBit(EventBits_t bitToClear);
static void startPxpReporter(void);
static void stopPxpReporter(void);
static void startGPRSTest(void);
static void gprsTestAlarmCallback(struct calendar_descriptor *const calendar);
static void deepSleepV2(void);
static void setTapDetection(void);
static void suspendIMU(void);

static EventGroupHandle_t _runningPeripherals;
static struct calendar_alarm _gprsTestAlarm;

/* to add (maybe :
- Http request
- Peripherals use (To see consumption peaks)
- BLE Sleep ?
*/
#define POWERTEST_MENU "\r\n" \
"1 - Turn ON GSM\r\n" \
"2 - Turn OFF GSM\r\n" \
"3 - Turn ON GPRS\r\n" \
"4 - Turn OFF GPRS\r\n" \
"5 - Turn ON BLE\r\n" \
"6 - Turn OFF BLE\r\n"\
"7 - IMU Normal\r\n"\
"8 - IMU Sleep\r\n"\
"s - IMU Suspend\r\n"\
"9 - Turn ON GPS\r\n" \
"a - Turn OFF GPS\r\n"\
"b - Turn ON dataflash\r\n" \
"c - Turn OFF dataflash\r\n" \
"d - Start PXP Advertising\r\n" \
"e - Stop PXP Advertising\r\n" \
"f - Sleep BLE\r\n" \
"g - Wake  BLE\r\n" \
"h - Get Signal Quality\r\n"\
"i - Run GPRS Test\r\n"\
"t - Set tap detection\r\n"\
"X - Deep Sleep\r\n" \
"Y - Deep Sleep V2\r\n" \
"? - Get running peripherals\r\n"

void powerTests_init(){
	_runningPeripherals = xEventGroupCreate();
	seekiosInfoManager_initSeekiosInfo();
}

UBaseType_t wm;

void task_powerTest(void* param){
	seekiosManagerStarted = true;
	UNUSED(param);
	powerTests_init();
	maskUtilities_init();
	turnOffGSM();
	turnOffGPS();
	turnOffDataflash(); // working ?
	sleepIMU();
	//turnOffBLE();

	char choice = 0;

	wm = uxTaskGetStackHighWaterMark(NULL);

	while(1){
		USARTManager_printUsbWait(POWERTEST_MENU);
		uint8_t readBuf[2];
		io_read(&SER3_DEBUG, readBuf, 1);
		readBuf[1] = 0;
		USARTManager_printUsbWait("Choice : ");
		USARTManager_printUsbWait(readBuf);
		USARTManager_printUsbWait("\r\n");
		switch(readBuf[0]){
			case '1': // turn on GSM with sleep mode
			turnOnGSM();
			break;
			case '2': // turn off GSM
			turnOffGSM();
			break;
			case '3': // turn on GPRS (needs GSM on)
			turnOnGPRS();
			break;
			case '4':
			turnOffGPRS();
			break;
			case '5':
			turnOnBLE();
			break;
			case '6':
			turnOffBLE();
			break;
			case '7':
			normalIMU();
			break;
			case '8':
			sleepIMU();
			break;
			case 's':
			suspendIMU();
			break;
			case '9':
			turnOnGPS();
			break;
			case 'a':
			turnOffGPS();
			break;
			case 'b':
			turnOnDataflash();
			break;
			case 'c':
			turnOffDataflash();
			break;
			case 'd':
			startPxpReporter();
			break;
			case 'e':
			stopPxpReporter();
			break;
			case 'f':
			sleepBLE();
			break;
			case 'g':
			wakeBLE();
			break;
			case 'h':
			getSignalQuality();
			break;
			case 'i':
			startGPRSTest();
			break;
			case 'X':
			deepSleep();
			break;
			case 'Y':
			deepSleepV2();
			break;
			case '?':
			printRunningPeripherals();
			break;
			case 't':
			setTapDetection();
			break;
		}
	}
}

static void setTapDetection()
{
	USARTManager_printUsbWait("Starting slope detection...");
	IMUManager_startSlopeDetection();
	USARTManager_printUsbWait("...done !\r\n");
}

static EventBits_t setRunningBit(EventBits_t bitToSet){
	return xEventGroupSetBits(_runningPeripherals, bitToSet);
}

static EventBits_t clearRunningBit(EventBits_t bitToClear){
	return xEventGroupClearBits(_runningPeripherals, bitToClear);
}

static void turnOnGSM(){
	USARTManager_printUsbWait("Powering ON GSM...");
	GSMManager_powerOnGSM();
	setRunningBit(RUNNING_BIT_GSM);
	USARTManager_printUsbWait("...DONE\r\n");
}

static void turnOffGSM(){
	USARTManager_printUsbWait("Powering OFF GSM...");
	gpio_set_pin_level(GSM_power_enable, false);;
	clearRunningBit(RUNNING_BIT_GSM);
	USARTManager_printUsbWait("...DONE\r\n");
}

static void turnOnGPRS(){
	USARTManager_printUsbWait("Powering ON GPRS...");
	USARTManager_printUsbWait("...FAIL\r\n");
}

static void turnOffGPRS(){
	USARTManager_printUsbWait("Powering OFF GPRS...");
	USARTManager_printUsbWait("...FAIL\r\n");
}

static void sleepBLE(){
	USARTManager_printUsbWait("Make BLE Sleep...");
	//BLEManager_sleep();
	//	USARTManager_printUsbWait("...DONE\r\n");
	USARTManager_printUsbWait("... NOT AVAILABLE\r\n");
}

static void wakeBLE(){
	USARTManager_printUsbWait("Wake BLE...");
	//BLEManager_wakeUp();
	//	USARTManager_printUsbWait("...DONE\r\n");
	USARTManager_printUsbWait("... NOT AVAILABLE\r\n");
}

static void turnOnBLE(){
	static bool isFirstActivation = true;
	USARTManager_printUsbWait("Powering ON BLE...");
// 	#if (WDT_ACTIVATED==1)
// 	WDTManager_turnOffWDT();
// 	#endif
// 	if(isFirstActivation){
// 		BLEManager_init();
// 		isFirstActivation = false;
// 	}
// 	else
// 	{
// 		//BLEManager_powerOnBLE();
// 	}
// 	#if (WDT_ACTIVATED==1)
// 	WDTManager_turnOnWDT();
// 	#endif
// 	setRunningBit(RUNNING_BIT_BLE);
//	USARTManager_printUsbWait("...DONE\r\n");
	USARTManager_printUsbWait("... NOT AVAILABLE\r\n");
}

static void turnOffBLE(){
	USARTManager_printUsbWait("Powering OFF BLE...");
	//BLEManager_powerOffBLE();
	clearRunningBit(RUNNING_BIT_BLE);
	USARTManager_printUsbWait("...FAIL\r\n");
}

static void startPxpReporter(){
	USARTManager_printUsbWait("Testing PXP reporter BLE profile...");
	USARTManager_printUsbWait("...DONE\r\n");
}

static void stopPxpReporter(){
	USARTManager_printUsbWait("Stopping PXP reporter BLE profile...");
	//at_ble_adv_stop();
	//	USARTManager_printUsbWait("...DONE\r\n");
	USARTManager_printUsbWait("... NOT AVAILABLE\r\n");
}

static void normalIMU(){
	USARTManager_printUsbWait("Waking up IMU...");
	IMUManager_powerModeNormal();
	setRunningBit(RUNNING_BIT_IMU);
	USARTManager_printUsbWait("...DONE\r\n");
}

static void sleepIMU(){
	USARTManager_printUsbWait("Sleep IMU...");
	IMUManager_powerModeLowPower();
	clearRunningBit(RUNNING_BIT_IMU);
	USARTManager_printUsbWait("...DONE\r\n");
}

static void suspendIMU(){
	USARTManager_printUsbWait("Sleep IMU...");
	IMUManager_suspend();
	clearRunningBit(RUNNING_BIT_IMU);
	USARTManager_printUsbWait("...DONE\r\n");
}

static void turnOnGPS(){
	USARTManager_printUsbWait("Powering ON GPS...");
	GPSManager_powerOnGPS();
	setRunningBit(RUNNING_BIT_GPS);
	USARTManager_printUsbWait("...DONE\r\n");
}

static void turnOffGPS(){
	USARTManager_printUsbWait("Powering OFF GPS...");
	GPSManager_powerOffGPS();
	clearRunningBit(RUNNING_BIT_GPS);
	USARTManager_printUsbWait("...DONE\r\n");
}

static void turnOnDataflash(){
	USARTManager_printUsbWait("Powering ON DATAFLASH...");
	dataflashManager_powerUp();
	setRunningBit(RUNNING_BIT_DATAFLASH);
	USARTManager_printUsbWait("...DONE\r\n");
}

static void turnOffDataflash(){
	USARTManager_printUsbWait("Powering OFF DATAFLASH...");
	dataFlashManager_powerDown();
	clearRunningBit(RUNNING_BIT_DATAFLASH);
	USARTManager_printUsbWait("...DONE\r\n");
}

static void deepSleep(){
	USARTManager_printUsbWait("Entering deep sleep... V1 (Push button to leave)\r\n");
	seekiosManager_hibernate();
}

static void deepSleepV2(){
	USARTManager_printUsbWait("Entering deep sleep... V2 (Push button to leave)\r\n");
	sleep_deep();
}

static void printRunningPeripherals(){
	EventBits_t bits = xEventGroupGetBits(_runningPeripherals);
	
	USARTManager_printUsbWait("GSM : ");
	if(bits & RUNNING_BIT_GSM){
		USARTManager_printUsbWait("ON\r\n");
		} else {
		USARTManager_printUsbWait("OFF\r\n");
	}

	USARTManager_printUsbWait("GPRS : ");
	if(bits & RUNNING_BIT_GPRS){
		USARTManager_printUsbWait("ON\r\n");
		} else {
		USARTManager_printUsbWait("OFF\r\n");
	}

	USARTManager_printUsbWait("BLE : ");
	if(bits & RUNNING_BIT_BLE){
		USARTManager_printUsbWait("ON\r\n");
		} else {
		USARTManager_printUsbWait("OFF\r\n");
	}

	USARTManager_printUsbWait("IMU : ");
	if(bits & RUNNING_BIT_IMU){
		USARTManager_printUsbWait("ON\r\n");
		} else {
		USARTManager_printUsbWait("OFF\r\n");
	}

	USARTManager_printUsbWait("GPS : ");
	if(bits & RUNNING_BIT_GPS){
		USARTManager_printUsbWait("ON\r\n");
		} else {
		USARTManager_printUsbWait("OFF\r\n");
	}
	
	USARTManager_printUsbWait("DATAFLASH : ");
	if(bits & RUNNING_BIT_DATAFLASH){
		USARTManager_printUsbWait("ON\r\n");
		} else {
		USARTManager_printUsbWait("OFF\r\n");
	}
}

static void getSignalQuality(){
	char buff[5];
	GSMManager_getSignalLevelPctage((unsigned char*)buff);
}

static void gprsTestAlarmCallback(struct calendar_descriptor *const calendar)
{
	UNUSED(calendar);
}

#define GPRS_TEST_ALARM_PERIOD 60 // in sec

static void startGPRSTest(void)
{
	USARTManager_printUsbWait("Starting GPRS Test !\r\nSession : CLOSING EACH TIME\r\nRefresh : 1min\r\n");
	
// 	while(1)
// 	{
// 		startHTTP();	
// 
// 		uint8_t httpResult[512];
// 		UNUSED(httpResult);
// 		// perform HTTP request + get signal + get battery
// 		//GSMManager_getSeekiosMessages(httpResult);
// 
// 		// set alarm in 1 minute
// 		RTCManager_setDelayedCallback(GPRS_TEST_ALARM_PERIOD, &_gprsTestAlarm, gprsTestAlarmCallback);
// 
// 		stopHTTP();
// 
// 		// sleep
// 		seekiosManager_hibernate();
// 	}


}
