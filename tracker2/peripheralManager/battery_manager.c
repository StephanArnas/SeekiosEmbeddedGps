#include <peripheralManager/battery_manager.h>

#define BATTERY_BUFFER_SIZE 3

static void batteryCheckTimerCallback(struct calendar_descriptor *const calendar);
static uint16_t ADCGetUSB(void);
static void analyzeBatteryLevel();
static uint16_t selectBatValue();
static void enqueueBatteryValue(uint16_t value);
static void resetBatteryCheckTimer(void);
static bool updateLastSampledBatteryVoltage(void);
static uint16_t getBatteryPercentage(void);
static void sendLowBatteryMessage(void);
static uint16_t convertConsumptionIndexToPeriod(uint8_t consumptionIndex);
static uint16_t buildBatteryCheckPeriod(void);

extern struct calendar_date_time _batteryLastCheckDateTime;
static struct calendar_alarm _checkBatteryValueTimerAlarm;
static E_CHARGE_STATUS _chargeStatus = CHARGE_STATUS_DISCHARGING;

/* The battery voltage is the voltage that will be shown to the user and used by the Seekios. It may be different from the regular battery voltage, because:
- We don't update _lastSampledbatteryVoltage if the seekios is in charge and the newly sampled voltage is lower than the previous _lastSampledbatteryVoltage
- We don't update _lastSampledbatteryVoltage if the seekios is in discharge and the newly sampled voltage is higher than the previous _lastSampledbatteryVoltage*/
static bool _lowBatteryAlertAlreadySent;
static uint16_t _lastBatVoltageToBeSent;
static uint16_t _batteryValues[BATTERY_BUFFER_SIZE];
static bool _batteryValuesFilled = false;

#if (ACTIVATE_BATTERY_LOG == 1)
static void addBatteryLog(void);
#define BATTERY_LOGS_COUNT 128
static BatteryLog batteryLogs[BATTERY_LOGS_COUNT];
static uint8_t	_logIndex = 0;
static uint32_t	_logId = 0;
#endif

void batteryLevel_init(){
	_batteryValuesFilled = false;
	_lowBatteryAlertAlreadySent = false;
	_lastBatVoltageToBeSent = 0;
	for (uint8_t i=0;i<BATTERY_BUFFER_SIZE;i++)
	{
		_batteryValues[i]=0;
	}
}

void batteryManager_updateChargeStatus()
{
	uint16_t usbVoltage = ADCGetUSB();
	//char buff[10];
	//stringHelper_intToString(usbVoltage, buff);
	//USARTManager_printUsbWait("USB ADC : ");
	//USARTManager_printUsbWait(buff);
	//USARTManager_printUsbWait("\r\n");
	
	if(usbVoltage >= USB_LEVEL_MINIMUM_CHARGE_VOLTAGE)
	{
		_chargeStatus = CHARGE_STATUS_CHARGING;
	}
	else
	{
		_chargeStatus = CHARGE_STATUS_DISCHARGING;
	}
}

static bool updateLastSampledBatteryVoltage()
{
	if(GSMManager_waitModuleUse())
	{
		GSMBatData gsmBatData;
		
		if(GSMManager_getGSMBatteryData(&gsmBatData))
		{
			enqueueBatteryValue(gsmBatData.batteryVoltage);
			
			if(!_batteryValuesFilled)
			{
				for(uint8_t i=1;i<BATTERY_BUFFER_SIZE;i++)
				{
					enqueueBatteryValue(gsmBatData.batteryVoltage);
				}
				_batteryValuesFilled = true;
			}
			
			volatile uint16_t selectedBatValue=selectBatValue();
			
			if ((batteryManager_isBatteryCharging() && selectedBatValue > _lastBatVoltageToBeSent)
			|| (!batteryManager_isBatteryCharging() && selectedBatValue < _lastBatVoltageToBeSent)
			|| _lastBatVoltageToBeSent == 0) //_batteryLevelToBeSent not initialized yet
			{
				_lastBatVoltageToBeSent = selectedBatValue;
			}
			
			char buff[10];//TODO: delete
			volatile uint32_t wm = uxTaskGetStackHighWaterMark(NULL);
			USARTManager_printUsbWait("Battery voltage to be sent: ");
			batteryManager_getBatteryPercentageStr(buff);
			USARTManager_printUsbWait(buff);
			USARTManager_printUsbWait("%\r\n");
			return true;
		}
	}
	return false;
}

/* Pops the first element of the list, and adds the new value at the end */
static void enqueueBatteryValue(uint16_t value)
{
	for (uint8_t i=0;i<BATTERY_BUFFER_SIZE-1;i++)
	{
		_batteryValues[i]=_batteryValues[i+1];
	}
	_batteryValues[BATTERY_BUFFER_SIZE-1]=value;
}

/* Selects the value that we will show to the user :
the middle value of the buffer of battery
values */
static uint16_t selectBatValue()
{
	if(BATTERY_BUFFER_SIZE <= 1)
	{
		return _batteryValues[0];
	}
	
	uint16_t tempBuff[BATTERY_BUFFER_SIZE]; // we create a temporary buff to not alter the original buff while after sorting
	memcpy(tempBuff, _batteryValues, BATTERY_BUFFER_SIZE*sizeof(uint16_t));
	
	utils_sortUint16ArrayAsc(tempBuff, BATTERY_BUFFER_SIZE);
	
	uint8_t middleIndex = BATTERY_BUFFER_SIZE/2;
	return tempBuff[middleIndex];
}

static uint16_t ADCGetUSB()
{
	ADC->CTRLA.bit.ENABLE = 0x00;					// off for parametering // ADC zur Parametrierung abschalten
	ADC->REFCTRL.reg = 0x05;						// reference voltage VDDANA //Referenzspannung VDDANA
	ADC->SAMPCTRL.bit.SAMPLEN = 0x5;				// more sampling time // AMPCTRL.SAMPLEN Sampling Zeit erhöhen
	ADC->AVGCTRL.bit.SAMPLENUM =0x7 ;				// more samples, add individual values //Sample-anzahl erhöhen, addiert die einzelnen Werte
	ADC->AVGCTRL.bit.ADJRES =0x4 ;					// Division coeff of the result (divided by 8) //Durchschnittsbildung durch oversampling
	ADC->CTRLC.bit.RESSEL =0x1  ;
	ADC->INPUTCTRL.reg = 0x0000;					// chose analog channel AI0 //Kanal AI0 wählen
	ADC->CTRLB.reg= 0x04;							//set Timer prescaler /128 - 6
	
	ADC->SWTRIG.reg = 0x2;							// set conversion start bit //Wandlung Startbit setzen  ADC->SWTRIG.bit.START = 1;

	ADC->CTRLA.bit.ENABLE = 0x00;
	ADC->INPUTCTRL.reg = 0x0001;                   //USB AIN0 //BAT AIN1
	ADC->CTRLA.bit.ENABLE = 0x01;
	ADC->SWTRIG.bit.START = 1;
	while(!ADC->INTFLAG.bit.RESRDY);
	return (uint16_t)(ADC->RESULT.reg*4.7)+1;	//start conversion
}

void task_checkBattery(void* param)
{
	UNUSED(param);
	maskUtilities_setRunningMaskBits(RUNNING_BIT_CHECK_BATTERY);
	batteryManager_updateAndAnalyzeBattery();
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_CHECK_BATTERY);
	FreeRTOSOverlay_taskDelete(NULL);
}

void batteryManager_updateAndAnalyzeBattery()
{
	if(updateLastSampledBatteryVoltage())
	{
		#if (ACTIVATE_BATTERY_LOG == 1)
		addBatteryLog();
		#endif
		if(powerStateManager_isSeekiosOn())
		{
			analyzeBatteryLevel();
		}
		resetBatteryCheckTimer();
		maskUtilities_clearRequestMaskBits(REQUEST_BIT_CHECK_BATTERY);
	}
}

void batteryManager_removeBatteryCheckAlarm()
{
	if(RTCManager_removeAlarm(&_checkBatteryValueTimerAlarm))
	{
		USARTManager_printUsbWait("Battery check alarm removed !\r\n");
	}
}

/* Schedules the next battery checking, depending on the current seekios status : on/off, powersaving on/off, current refresh rate, (maybe) current battery level...*/
static void resetBatteryCheckTimer(){
	batteryManager_removeBatteryCheckAlarm();
	USARTManager_printUsbWait("Battery Check Alarm  : ");
	
	uint16_t batteryCheckPeriodMin = buildBatteryCheckPeriod();

	uint8_t buf[5];
	stringHelper_intToString(batteryCheckPeriodMin, buf);
	USARTManager_printUsbWait("Battery check period : ");
	USARTManager_printUsbWait(buf);
	USARTManager_printUsbWait("\r\n");
	RTCManager_setDelayedCallback(batteryCheckPeriodMin*60, &_checkBatteryValueTimerAlarm, batteryCheckTimerCallback);
}

/* Builds the battery check period, in mins, depending on several parameters */
static uint16_t buildBatteryCheckPeriod()
{
	uint8_t percentage = getBatteryPercentage();
	uint8_t consumptionIndex = 0;
	uint8_t factor = 1;
	
	/* SMALL HACK : If battery is charging, be want to regularly update the battery Level */
	if(batteryManager_isBatteryCharging())
	{
		consumptionIndex = 20;
	}
	
	/* ADD TO CONSUMPTIONINDEX DEPENDING ON THE CURRENT BATTERY LEVEL */
	if(percentage <= 20)
	{
		factor = 2;
	}
	
	/* ADD TO CONSUMPTIONINDEX DEPENDING ON THE STATE */
	if(powerStateManager_isSeekiosOn() && !powerStateManager_isPowerSavingEnabled())
	{
		consumptionIndex += 5;
	}

	/* ADD TO CONSUMPTIONINDEX DEPENDING ON THE MODE */
	ModeStatus modeStatus;
	statusManager_getRunningStatus(&modeStatus);
	uint8_t modeRefreshRate = statusManager_getRunningRefreshRate();
	
	if(modeStatus.status == MODE_STATUS_DONT_MOVE && modeStatus.state != DONT_MOVE_STATE_TRACKING) // DM RAS
	{
		consumptionIndex += 1;
	}
	else if(modeStatus.status == MODE_STATUS_ZONE && modeStatus.state != ZONE_STATE_TRACKING) // Zone RAS
	{
		consumptionIndex += 8;
	}
	else if ((modeStatus.status == MODE_STATUS_DONT_MOVE && modeStatus.state == DONT_MOVE_STATE_TRACKING)
	|| (modeStatus.status == MODE_STATUS_ZONE && modeStatus.state == ZONE_STATE_TRACKING)
	|| (modeStatus.status == MODE_STATUS_TRACKING) // Trackings
	)
	{
		switch(modeRefreshRate)
		{
			case 1:		consumptionIndex += 15; break;
			case 2:		consumptionIndex += 15; break;
			case 3:		consumptionIndex += 15; break;
			case 4:		consumptionIndex += 15; break;
			case 5:		consumptionIndex += 15; break;
			case 10:	consumptionIndex += 12; break;
			case 15:	consumptionIndex += 12; break;
			case 30:	consumptionIndex += 10; break;
			case 60:	consumptionIndex += 10; break;
			case 120:	consumptionIndex += 6; break;
			case 180:	consumptionIndex += 6; break;
			case 240:	consumptionIndex += 5; break;
			case 300:	consumptionIndex += 5; break;
			case 360:	consumptionIndex += 5; break;
			case 720:	consumptionIndex += 3; break;
			case 1440:	consumptionIndex += 1; break;
		}
	}
	
	consumptionIndex = consumptionIndex * factor;
	
	return convertConsumptionIndexToPeriod(consumptionIndex);
}
// ALL POSSIBILITIES: // Computé à partir d'un programme python à demander
//('Tracking1440_Off_PS_100', 2)
//('DontMove_Off_PS_100', 2)
//('Tracking720_Off_PS_100', 4)
//('Tracking1440_Off_PS_20', 4)
//('DontMove_Off_PS_20', 4)
//('Tracking240_360_Off_PS_100', 6)
//('Tracking1440_On_100', 6)
//('DontMove_On_100', 6)
//('Tracking120_180_Off_PS_100', 7)
//('Tracking720_On_100', 8)
//('Tracking720_Off_PS_20', 8)
//('Zone_Off_PS_100', 9)
//('Tracking240_360_On_100', 10)
//('Tracking30_60_Off_PS_100', 11)
//('Tracking120_180_On_100', 11)
//('Tracking240_360_Off_PS_20', 12)
//('Tracking1440_On_20', 12)
//('DontMove_On_20', 12)
//('Tracking10_15_Off_PS_100', 13)
//('Zone_On_100', 13)
//('Tracking120_180_Off_PS_20', 14)
//('Tracking30_60_On_100', 15)
//('Tracking1_5_Off_PS_100', 16)
//('Tracking720_On_20', 16)
//('Tracking10_15_On_100', 17)
//('Zone_Off_PS_20', 18)
//('Tracking1_5_On_100', 20)
//('Tracking240_360_On_20', 20)
//('Tracking30_60_Off_PS_20', 22)
//('Tracking120_180_On_20', 22)
//('Tracking10_15_Off_PS_20', 26)
//('Zone_On_20', 26)
//('Tracking30_60_On_20', 30)
//('Tracking1_5_Off_PS_20', 32)
//('Tracking10_15_On_20', 34)
//('Tracking1_5_On_20', 40)

static uint16_t convertConsumptionIndexToPeriod(uint8_t consumptionIndex)
{
	if(consumptionIndex <= 2)
	{
		return 1440;
	}
	else if(consumptionIndex <= 5)
	{
		return 720;
	}
	else if(consumptionIndex <= 10)
	{
		return 360 ;
	}
	else if(consumptionIndex <= 15)
	{
		return 180;
	}
	else if(consumptionIndex <= 20)
	{
		return 60;
	}
	else
	{
		return 30;
	}
}

static void batteryCheckTimerCallback(struct calendar_descriptor *const calendar){
	maskUtilities_setRequestMaskBits(REQUEST_BIT_CHECK_BATTERY);
	USARTManager_printUsbWait("Battery check CALLBACK\r\n");
	UNUSED(calendar);
}

bool batteryManager_isBatteryCheckAlarmScheduled()
{
	return RTCManager_isAlarmScheduled(&_checkBatteryValueTimerAlarm);
}

static void analyzeBatteryLevel(){
	USARTManager_printUsbWait("Battery level : ");
	USARTManager_printInt(_lastBatVoltageToBeSent);
	USARTManager_printUsbWait("\r\n");
	
	if(_lastBatVoltageToBeSent <= BATTERY_LEVEL_0_VOLTAGE){ // extinction
		USARTManager_printUsbWait("Battery Empty Turning off !\r\n");
		maskUtilities_setRequestTurnOffSeekios();
	}
	else if(_lastBatVoltageToBeSent <= BATTERY_LEVEL_20_VOLTAGE
	&& !batteryManager_isBatteryCharging()
	&& !_lowBatteryAlertAlreadySent) // Critical battery voltage. We don't send an alert if it's charging, of if the alert was already sent  when we were below 20%
	{
		sendLowBatteryMessage();
		_lowBatteryAlertAlreadySent = true;
	}
	else if(_lastBatVoltageToBeSent > BATTERY_LEVEL_20_VOLTAGE)
	{
		_lowBatteryAlertAlreadySent = false;
	}
}

static void sendLowBatteryMessage()
{
	OutputMessage lowBatteryMessage;
	lowBatteryMessage.onSendSuccess = NULL;
	lowBatteryMessage.messageType = MESSAGE_TYPE_LOW_BATTERY;
	lowBatteryMessage.timestamp = RTCManager_getCurrentTimestamp();
	messageSender_addToSendList(&lowBatteryMessage); // TODO : fill this with the extinction message
}

static uint16_t getBatteryPercentage()
{
	volatile int batteryLevelPctage = ((float)(_lastBatVoltageToBeSent - BATTERY_LEVEL_0_VOLTAGE)/(float)(BATTERY_LEVEL_100_VOLTAGE - BATTERY_LEVEL_0_VOLTAGE))*100;
	batteryLevelPctage = max(min(batteryLevelPctage, 100),0);
	return batteryLevelPctage;
}

void batteryManager_getBatteryPercentageStr(uint8_t* buff)
{
	stringHelper_intToString(getBatteryPercentage(), buff);
}

bool batteryManager_isBatteryCharging()
{
	return _chargeStatus == CHARGE_STATUS_CHARGING;
}

void batteryManager_printBatteryCheckAlarm()
{
	USARTManager_printUsbWait("Next battery check : ");
	RTCManager_printTimeFromTimestamp(_checkBatteryValueTimerAlarm.data.timestamp);
	USARTManager_printUsbWait("\r\n");
}

E_LED_BATTERY_TIER batteryManager_getLedPercentageTier()
{
	if(_lastBatVoltageToBeSent < BATTERY_LEVEL_33_VOLTAGE)
	{
		return LED_BATTERY_TIER_LOW;
	}
	else if (_lastBatVoltageToBeSent >= BATTERY_LEVEL_33_VOLTAGE && _lastBatVoltageToBeSent < BATTERY_LEVEL_66_VOLTAGE)
	{
		return LED_BATTERY_TIER_MEDIUM;
	}
	else if(_lastBatVoltageToBeSent >= BATTERY_LEVEL_66_VOLTAGE && _lastBatVoltageToBeSent < BATTERY_LEVEL_100_VOLTAGE)
	{
		return LED_BATTERY_TIER_HIGH;
	}
	else
	{
		return LED_BATTERY_TIER_FULL;
	}
}

#if (ACTIVATE_BATTERY_LOG == 1)
static void addBatteryLog()
{
	// add log
	if(_logIndex >= BATTERY_LOGS_COUNT)
	{
		_logIndex = 0;
	}

	batteryLogs[_logIndex].logId = _logId;
	batteryLogs[_logIndex].timestamp = RTCManager_getCurrentTimestamp();
	batteryLogs[_logIndex].voltageShown = _lastBatVoltageToBeSent;
	GSMBatData gsmBatData = {0, 0};
	if(GSMManager_isModuleStarted())
	{
		GSMManager_getGSMBatteryData(&gsmBatData);
	}
	batteryLogs[_logIndex].voltageReal = gsmBatData.batteryVoltage;
	_logIndex++;
	_logId++;
}

void batterymanager_printLastSamples()
{
	char buff[10];
	stringHelper_intToString(_lastBatVoltageToBeSent, (unsigned char*)buff);
	USARTManager_printUsbWait("Battery level : ");
	USARTManager_printUsbWait(buff);
	stringHelper_intToString(ADCGetUSB(), (unsigned char*)buff);
	USARTManager_printUsbWait(" - USB Charge level : ");
	USARTManager_printUsbWait(buff);
	USARTManager_printUsbWait("\r\nCurrent dateTime : ");
}

void batteryManager_printBatteryLogs()
{
	char buff[16];
	USARTManager_printUsbWait("####### BATTERY LOGS #######\r\n");
	for(uint8_t i = 0; i<min(_logId, BATTERY_LOGS_COUNT);i++)
	{
		USARTManager_printUsbWait("\t");
		stringHelper_intToString(batteryLogs[i].logId, (unsigned char*)buff);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait(",");
		stringHelper_intToString(batteryLogs[i].timestamp, (unsigned char*)buff);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait(",");
		stringHelper_intToString(batteryLogs[i].voltageShown, (unsigned char*)buff);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait(",");
		stringHelper_intToString(batteryLogs[i].voltageReal, (unsigned char*)buff);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait("\r\n");
	}
	USARTManager_printUsbWait("############################\r\n");
}

#endif