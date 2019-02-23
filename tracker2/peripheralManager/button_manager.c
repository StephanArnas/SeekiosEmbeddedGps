#include <peripheralManager/button_manager.h>

static void triplePressTimerCallback(TimerHandle_t xTimer );
static void shortPressTimerCallback(TimerHandle_t xTimer);
static void oneSecPressTimerCallback(TimerHandle_t xTimer);
static void checkOneSecPeriodicTimerCount(void);
static void requestSeekiosUpdate(void);

#define SEEKIOS_EXTINCTION_PERIOD	4000
#define SEEKIOS_TRIPLE_PRESS_PERIOD 1500
#define SEEKIOS_INSTRUCTION_PERIOD	2000
#define SEEKIOS_SHORT_PRESS_PERIOD	1000
#define SEEKIOS_1_SEC_PRESSURE		1000

#define BUTTON_DOWN_STATE false
#define BUTTON_UP_STATE true

static xTimerHandle _triplePressTimerHandle;
static xTimerHandle _1secPeriodicTimerHandle;
static xTimerHandle _shortPressTimerHandle;

static uint8_t _falls;
static uint8_t _rises;

static bool _triplePressTimerActive;
static bool _shortPressTimerActive;
static bool _oneSecPeriodicTimerActive;

static uint8_t _oneSecPeriodicTimerCount;

static uint32_t _turnOffLedInstructionID = 0;
static uint32_t _turnOnLedInstructionID = 0;
static uint32_t _turningLedInstructionID = 0;

static E_BUTTON_ACTION _buttonAction = BUTTON_ACTION_NONE;
static E_BUTTON_ACTION _tempButtonAction = BUTTON_ACTION_NONE;

void buttonManager_init(){
	_triplePressTimerHandle = xTimerCreate("Triple press timer handle", SEEKIOS_TRIPLE_PRESS_PERIOD, pdFALSE, (void*) 0, triplePressTimerCallback);
	_1secPeriodicTimerHandle = xTimerCreate("1 sec periodic timer", SEEKIOS_1_SEC_PRESSURE, pdTRUE, (void*) 0, oneSecPressTimerCallback);
	_shortPressTimerHandle = xTimerCreate("Short press timer", SEEKIOS_1_SEC_PRESSURE, pdFALSE, (void*) 0, shortPressTimerCallback);
}

void task_buttonManager(void* params){
	UNUSED(params);
	_oneSecPeriodicTimerActive = false;
	_triplePressTimerActive = false;
	while(1){
		_rises = 0;
		_falls = 0;
		_oneSecPeriodicTimerCount = 0;
		bool currentPinState = true;// false = down, true = up
		_buttonAction = BUTTON_ACTION_NONE;
		_tempButtonAction = BUTTON_ACTION_NONE;
		while(_buttonAction == BUTTON_ACTION_NONE){

			checkOneSecPeriodicTimerCount();

			bool newPinState = gpio_get_pin_level(PIN_PA13);
			if(newPinState != currentPinState){
				if(newPinState == BUTTON_UP_STATE){
					_rises++;
				}
				else if (newPinState == BUTTON_DOWN_STATE)
				{
					_falls++;
				}
			}
			if(_falls >= 1)
			{
				maskUtilities_setRunningMaskBits(RUNNING_BIT_BUTTON_ACTION);
			}


			if( _falls == 1  && _rises == 0 && _tempButtonAction == BUTTON_ACTION_NONE){

				if(!_shortPressTimerActive){
					xTimerStart(_shortPressTimerHandle, 0);
					_shortPressTimerActive = true;
				}

				if(!_oneSecPeriodicTimerActive){
					xTimerStart(_1secPeriodicTimerHandle, 0);
					_oneSecPeriodicTimerActive = true;
				}
			}

			if(_rises > 0){
				
				if(_oneSecPeriodicTimerActive == true){
					xTimerStop(_1secPeriodicTimerHandle, 0);
					_oneSecPeriodicTimerActive = false;
				} 

				if(powerStateManager_isSeekiosOn()){
					LEDManager_stopCurrentlyRunningInstructionId(_turningLedInstructionID);	
				}

				if(_tempButtonAction == BUTTON_ACTION_SHORT_PRESS
				|| _tempButtonAction == BUTTON_ACTION_2_SEC_PRESS
				|| _tempButtonAction == BUTTON_ACTION_3_SEC_PRESS
				|| _tempButtonAction == BUTTON_ACTION_6_SEC_PRESS){
					_buttonAction = _tempButtonAction;
				}
			}

			if(_falls == 1 && _rises == 1 && !_triplePressTimerActive && _buttonAction == BUTTON_ACTION_NONE){
				xTimerStart(_triplePressTimerHandle, 0);
				_triplePressTimerActive = true;
			}

			if(_rises == 3 && _falls == 3){
				if(_triplePressTimerActive){
					xTimerStop(_triplePressTimerHandle, 0);
					_triplePressTimerActive = false;
				}
				_buttonAction = BUTTON_ACTION_SOS;
			}
			
			currentPinState = newPinState;
			vTaskDelay(1);
		}

		if(_shortPressTimerActive){
			xTimerStop(_shortPressTimerHandle, 0);
			_shortPressTimerActive = false;
		}

		if(_oneSecPeriodicTimerActive){
			xTimerStop(_1secPeriodicTimerHandle, 0);
			_oneSecPeriodicTimerActive = false;
		}

		if(_triplePressTimerActive){
			xTimerStop(_triplePressTimerHandle, 0);
			_triplePressTimerActive = false;
		}

		volatile BaseType_t wm = uxTaskGetStackHighWaterMark(NULL);

		switch(_buttonAction){
			case BUTTON_ACTION_SHORT_PRESS:
			USARTManager_printUsbWait("BUTTON INSTRUCTION : Get Battery\r\n");
			maskUtilities_setRequestMaskBits(REQUEST_BIT_BATTERY_LEDS);
			break;
			case BUTTON_ACTION_6_SEC_PRESS:
			USARTManager_printUsbWait("BUTTON INSTRUCTION : Reset Seekios\r\n");
			requestSeekiosUpdate();
			NVIC_SystemReset();
			break;
			case BUTTON_ACTION_3_SEC_PRESS:
			if(powerStateManager_isSeekiosOn()){
				USARTManager_printUsbWait("BUTTON INSTRUCTION : Turn OFF Seekios\r\n");
				maskUtilities_setRequestTurnOffSeekios();
			}
			else if(!powerStateManager_isSeekiosOn()) {
				USARTManager_printUsbWait("BUTTON INSTRUCTION : Turn ON Seekios\r\n");
				maskUtilities_setRequestTurnOnSeekios();
			}
			break;
			case BUTTON_ACTION_2_SEC_PRESS:
			// Useless. Was there only to put a button combination to deactivate power saving functionality
			break;
			case BUTTON_ACTION_SOS:
			USARTManager_printUsbWait("BUTTON INSTRUCTION : SOS !!\r\n");
			maskUtilities_setRequestMaskBits(REQUEST_BIT_SOS);
			ledUtilities_runSOSLedInstructions();
			break;
			default:
			break;
		}
		
		UNUSED(wm);
		vTaskDelay(1);
		maskUtilities_clearRunningMaskBits(RUNNING_BIT_BUTTON_ACTION);
	}
	
	FreeRTOSOverlay_taskDelete(NULL);
}

/* Changes the value of the boot byte to request an update */
static void requestSeekiosUpdate()
{
	nvm_erase_row((uint32_t)FUSES_START_ADDRESS);
	uint8_t buff[FUSES_SIZE] = {0xff};
	buff[0]		= 0xBB;
	buff[FUSES_INDEX_VERSION]		= SEEKIOS_VERSION;
	buff[FUSES_INDEX_SUBVERSION]	= SEEKIOS_SUBVERSION;
	nvm_write_buffer(FUSES_START_ADDRESS, buff, FUSES_SIZE);
	vTaskDelay(1000);
}

void buttonManager_testRequestSeekiosUpdate()
{
	requestSeekiosUpdate();
}

static void triplePressTimerCallback(TimerHandle_t xTimer ){
	// fail double press : we have to exit the button manager, because no button case exists after that case
	USARTManager_printUsbWait("CANCELED : triple tap\r\n");
	_buttonAction = BUTTON_ACTION_FAIL;
	_triplePressTimerActive = false;
	UNUSED(xTimer);
}

static void shortPressTimerCallback(TimerHandle_t xTimer){
	if(_rises == 1 && _falls == 1){
		_tempButtonAction = BUTTON_ACTION_SHORT_PRESS;
	}
	else
	{
		_tempButtonAction = BUTTON_ACTION_SHORT_PRESS_FAIL;
	}	
	_shortPressTimerActive = false;
	UNUSED(xTimer);
}

static void oneSecPressTimerCallback(TimerHandle_t xTimer){
	_oneSecPeriodicTimerCount++;
	UNUSED(xTimer);
}

/* This will look at the oneSecPeriodicTimerCount incremented by the oneSecPressTimerCallback, and checks the button/led action depending on its value.
This function will also check if the timerCount is different from the last time the function was called. It not, it will do nothing.*/
static void checkOneSecPeriodicTimerCount()
{
	static uint8_t lastSecPeriodicTimerCount = 0;

	if(lastSecPeriodicTimerCount == _oneSecPeriodicTimerCount)
	{
		return;
	}

	lastSecPeriodicTimerCount = _oneSecPeriodicTimerCount;

	if(_oneSecPeriodicTimerCount==2)
	{
		_tempButtonAction = BUTTON_ACTION_2_SEC_PRESS;
		if(powerStateManager_isPowerSavingEnabled())
		{
			LEDManager_stopCurrentlyRunningInstructionId(_turningLedInstructionID);
		}
	}
	else if(_oneSecPeriodicTimerCount==3)
	{
		_tempButtonAction = BUTTON_ACTION_3_SEC_PRESS;
	}
	else if(_oneSecPeriodicTimerCount==6)
	{
		_tempButtonAction = BUTTON_ACTION_6_SEC_PRESS;
	}

	if(_oneSecPeriodicTimerCount == 3)
	{
		if(powerStateManager_isSeekiosOn())
		{
			_turnOffLedInstructionID = ledUtilities_runTurnOffSeekiosLedInstruction();
		}
		else
		{
			_turnOnLedInstructionID = ledUtilities_runTurnOnSeekiosLedInstruction();
		}
	}
	else if(_oneSecPeriodicTimerCount == 6)
	{
		ledUtilities_runResetSeekiosLedInstruction();
	}
}