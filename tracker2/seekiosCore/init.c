#include <seekiosCore/init.h>

/*
In this file, the whole system is instanciated : usart, system, delays, freertos, interrupts
we also configure the peripherals
*/

void init_init(){
	USARTManager_init();
	maskUtilities_init();
	TRNGManager_init();
	#if (BLE_ACTIVATED == 1 && POWER_TESTS_ACTIVATED == 0)
	BLEManager_init();
	#endif
	#if	(SIMULATE_SENDING_FAILURES == 1)
	USARTManager_printUsbWait("Sending failure simulation : press button to toggle.\r\n");
	#endif
	#if (WDT_ACTIVATED==1)
	WDTManager_init();
	#endif
	authentication_init();
	statusManager_initStatusManager();
	functionalitiesTest_init();
	messageListener_init();
	messageSender_init();
	buttonManager_init();
	GPSManager_init();
	LEDManager_init();
	dontMoveBle_init();
	dataflashManager_init();
	ledUtilities_init();
	IMUManager_init();
	USBManager_init();
	dontMove_init();
	RTCManager_init();
	batteryLevel_init();
	GSMManager_init();
	send_debug("Initialization Seekios done.\r\n");
}
