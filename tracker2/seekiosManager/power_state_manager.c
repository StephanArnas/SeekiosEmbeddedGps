#include <seekiosManager/power_state_manager.h>

static void startPowerSavingAlarm(int8_t powerSavingHourOffset);
static void stopPowerSavingAlarm(void);
static void addPowerSavingDisabledMessage(uint32_t runningConfigModeId);
static void setPowerSavingDisabledFlash(void);
static void setPowerSavingEnabledFlash(void);
static bool isPowerSavingEnabledFromFlash(void);

static bool _isSeekiosOn;
static struct calendar_alarm _powerSavingInstructionAlarm;
static EventGroupHandle_t _pendingActionsMask; // When the Seekios is powering down, this mask is used to remember the actions that the Seekios will have to perform when he wakes up
static bool _isPowerSavingEnabled = false;

#define ALARM_WAKE_GSM_PERIOD 60000

void powerStateManager_init(bool firstRunSeekios){
	_isSeekiosOn = false;
	_isPowerSavingEnabled = isPowerSavingEnabledFromFlash();
	if(firstRunSeekios)
	{
		maskUtilities_setRequestTurnOffSeekios();
	}
	else
	{
		maskUtilities_setRequestTurnOnSeekios();
		ledUtilities_runTurnOnSeekiosLedInstruction();
	}

	_pendingActionsMask = xEventGroupCreate();
}

void powerStateManager_setPendingActionBits(EventBits_t bitsToSet)
{
	xEventGroupSetBits(_pendingActionsMask, bitsToSet);
}

void powerStateManager_clearPendingActionBits(EventBits_t bitsToSet)
{
	xEventGroupClearBits(_pendingActionsMask, bitsToSet);
}

EventBits_t powerStateManager_getPendingActionMask()
{
	return xEventGroupGetBits(_pendingActionsMask);
}

void powerStateManager_clearPendingActionsMask()
{
	xEventGroupClearBits(_pendingActionsMask,
	PENDING_ACTION_BIT_RING_INDICATOR_INTERRUPT | 
	PENDING_ACTION_BIT_CHECK_LPC	| 
	PENDING_ACTION_BIT_SENDER		| 
	PENDING_ACTION_BIT_LISTENER);
}

void powerStateManager_setSeekiosOff(){
	_isSeekiosOn = false;
	testMonitor_raiseEvent(EVENT_SEEKIOS_STATE_OFF);
}

bool powerStateManager_isSeekiosOn(){
	return _isSeekiosOn;
}

void powerStateManager_setSeekiosOn(){
	_isSeekiosOn = true;
	testMonitor_raiseEvent(EVENT_SEEKIOS_STATE_ON);
}


void powerStateManager_enablePowerSaving(int8_t cultureHoursOffset){
	USARTManager_printUsbWait("-= POWER SAVING ENABLED =-\r\n");
	_isPowerSavingEnabled = true;
	setPowerSavingEnabledFlash();
	startPowerSavingAlarm(cultureHoursOffset);
}

void powerStateManager_disablePowerSavingIfEnabled(){
	
	if(powerStateManager_isPowerSavingEnabled())
	{
		_isPowerSavingEnabled = false;
		setPowerSavingDisabledFlash();
		stopPowerSavingAlarm();
		statusManager_setRunningIsPowerSavingEnabled(false);
		addPowerSavingDisabledMessage(statusManager_getRunningConfigModeId());
		maskUtilities_setRequestMaskBits(REQUEST_BIT_LISTENER);
		USARTManager_printUsbWait("-= POWER SAVING DISABLED =-\r\n");
	}
}

static void powerSavingInstructionAlarmCallBack(struct calendar_descriptor *const calendar){
	maskUtilities_setRequestMaskBitsFromISR(REQUEST_BIT_LISTENER);
	UNUSED(calendar);
}

static void setPowerSavingDisabledFlash()
{
	char writeBuf[2] = "0";
	dataflashManager_writeToPage(PAGE_INDEX_SEEKIOS_IN_POWER_SAVING,20,(unsigned char*)writeBuf);
}

static void setPowerSavingEnabledFlash()
{
	char writeBuf[2] = "1";
	dataflashManager_writeToPage(PAGE_INDEX_SEEKIOS_IN_POWER_SAVING,20,(unsigned char*)writeBuf);
}

static bool isPowerSavingEnabledFromFlash(){
	char readChar;
	dataflashManager_readPage(PAGE_INDEX_SEEKIOS_IN_POWER_SAVING,1,(unsigned char*)&readChar);
	return readChar == '1';
}

bool powerStateManager_isPowerSavingEnabled(){
	return _isPowerSavingEnabled;
}

static void addPowerSavingDisabledMessage(uint32_t runningConfigModeId)
{
	OutputMessage message;
	message.messageType = MESSAGE_TYPE_POWER_SAVING_DISABLED;
	message.timestamp = RTCManager_getCurrentTimestamp();
	message.modeId = runningConfigModeId;
	message.onSendSuccess = NULL;
	messageSender_addToSendList(&message);
}

static void startPowerSavingAlarm(int8_t powerSavingHourOffset){
	_powerSavingInstructionAlarm.data.datetime.time.hour = 12 - powerSavingHourOffset;
	_powerSavingInstructionAlarm.data.datetime.time.min = 0;
	_powerSavingInstructionAlarm.data.datetime.time.sec = 0;
	
	//Repeat each day
	_powerSavingInstructionAlarm.mode = REPEAT;
	_powerSavingInstructionAlarm.option = CALENDAR_ALARM_MATCH_HOUR;
	
	RTCManager_setAlarm(&_powerSavingInstructionAlarm, powerSavingInstructionAlarmCallBack);
}

static void stopPowerSavingAlarm(){
	if(RTCManager_removeAlarm(&_powerSavingInstructionAlarm)){
		USARTManager_printUsbWait("Canceled Power Saving Alarm\r\n");
	}
}