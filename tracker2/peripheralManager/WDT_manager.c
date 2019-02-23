#include <peripheralManager/WDT_manager.h>
static void softwareWatchdogTimerCallback(xTimerHandle timer);
static xTimerHandle softwareWatchdogTimerHandle;

extern struct wdt_descriptor WDT_;
static struct calendar_alarm _dogKickingAlarm;

void WDTManager_init(){	

	// config software resetting timer - if the timer fails to kick the dog, there will be a software reset
	softwareWatchdogTimerHandle = xTimerCreate("Software watchdog timer", SOFTWARE_WATCHDOG_PERIOD, pdTRUE, (void*)0, softwareWatchdogTimerCallback);
	xTimerStart(softwareWatchdogTimerHandle, 10000);
	// config WDT clock
	WDT__init();

	// config WDT itself
// 	WDT->CTRLA.bit.ENABLE = 0;
// 	WDT->CTRLA.bit.WEN = 0;
// 	WDT->CTRLA.bit.ALWAYSON = 0;
// 
// 	//WDT->CONFIG.bit.PER = 0x7 // 1 sec
// 	//WDT->CONFIG.bit.PER = 0x8 // 2 sec
// 	WDT->CONFIG.bit.PER = 0x9; // 4 Sec
// 	//WDT->CONFIG.bit.PER = 0xA // 8 sec
// 	//WDT->CONFIG.bit.PER = 0xB // 16 sec
// 
// 	WDT->CTRLA.bit.ENABLE = 0x1;
// 	while(WDT->SYNCBUSY.bit.ENABLE);

	uint32_t clk_rate;
	uint16_t timeout_period;

	clk_rate = 1000;
	timeout_period = 16384;
	//NVIC_EnableIRQ(WDT_IRQn);
	wdt_set_timeout_period(&WDT_, clk_rate, timeout_period);
	//hri_wdt_set_INTEN_EW_bit(WDT);
	//hri_wdt_write_EWCTRL_EWOFFSET_bf(WDT, 0x0A);
	wdt_enable(&WDT_);
	hri_wdt_write_CTRLA_ALWAYSON_bit(WDT, true);
}

void WDTManager_turnOffWDT(){
	wdt_disable(&WDT_);
}

void WDTManager_turnOnWDT(){
	wdt_enable(&WDT_);
}


void kickTheDog(){
  	WDT->CLEAR.reg = 0xA5;
  	while(WDT->SYNCBUSY.bit.CLEAR);
}

static void softwareWatchdogTimerCallback(xTimerHandle timer){
	// todo : check sanity of the program
	kickTheDog();
}

static void dogKickingAlarmCallback()
{
	kickTheDog();
}

bool WDTManager_isDogKickingAlarmScheduled()
{
	return RTCManager_isAlarmScheduled(&_dogKickingAlarm);
}

void WDTManager_setDogKickingAlarm()
{
	if(!WDTManager_isDogKickingAlarmScheduled())
	{
		USARTManager_printUsbWait("Setting dog kicking alarm : ");
		RTCManager_setDelayedCallback(12, &_dogKickingAlarm, dogKickingAlarmCallback);
	}
}

void WDTManager_removeDogKickingAlarm()
{
	RTCManager_removeAlarm(&_dogKickingAlarm);
}