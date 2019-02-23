#include <peripheralManager/LED_manager.h>

static void toggleYellowLedShining(TimerHandle_t xTimer);
static void toggleRedLedShining(TimerHandle_t xTimer);
static void toggleGreenLedShining(TimerHandle_t xTimer);
static void stopLedAndTimers(void);
static bool isLedActionRunning(void);
static void copyLEDInstruction(LEDInstruction *dest, LEDInstruction *src);
static void launchAction(LEDAction *actionPtr);
static void stopLeds(void);

static TimerHandle_t _greenLedTimerHandle;
static int _actualGreenLedBlink;
static int _countOfGreenLedBlink;
static bool _greenTimerRunning;

static TimerHandle_t _redLedTimerHandle;
static int _actualRedLedBlink;
static int _countOfRedLedBlink;
static bool _redTimerRunning;

static TimerHandle_t _yellowLedTimerHandle;
static int _actualYellowLedBlink;
static int _countOfYellowLedBlink;
static bool _yellowTimerRunning;

static LEDInstruction	_currentlyRunningLedInstruction;
static int				_currentlyRunningInstructionRepeatCount;
static uint8_t			_currentlyRunningInstructionAction;
static bool				_isInstructionRunning;

void LEDManager_init(){

	gpio_set_pin_direction(LED_0_gn, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LED_0_gn, GPIO_PIN_FUNCTION_OFF);
	gpio_set_pin_direction(LED_1_rd, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LED_1_rd, GPIO_PIN_FUNCTION_OFF);
	gpio_set_pin_direction(LED_2_ye, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LED_2_ye, GPIO_PIN_FUNCTION_OFF);

	_currentlyRunningInstructionRepeatCount = 0;
	_currentlyRunningInstructionAction = 0;

	_greenTimerRunning = false;
	_redTimerRunning = false;
	_yellowTimerRunning = false;

	_greenLedTimerHandle = xTimerCreate("Green Led Timer", LED_BLIKING_PERIOD_MS, pdTRUE, (void*)0, toggleGreenLedShining);
	_redLedTimerHandle = xTimerCreate("Red Led Timer", LED_BLIKING_PERIOD_MS, pdTRUE, (void*)0, toggleRedLedShining);
	_yellowLedTimerHandle = xTimerCreate("Yellow Led Timer", LED_BLIKING_PERIOD_MS, pdTRUE, (void*)0, toggleYellowLedShining);
}

void LEDManager_addInstructionID(LEDInstruction *instructionPtr){
	static uint32_t ID = 1;
	instructionPtr->instructionId = ID;
	ID++;
}

void LEDManager_stopCurrentlyRunningInstruction()
{
	stopLedAndTimers();
	_isInstructionRunning = false;
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_LED_INSTRUCTION);
}

bool LEDManager_stopCurrentlyRunningInstructionId(uint32_t instructionId){
	if(_currentlyRunningLedInstruction.instructionId == instructionId && _isInstructionRunning){
		stopLedAndTimers();
		_isInstructionRunning = false;
		maskUtilities_clearRunningMaskBits(RUNNING_BIT_LED_INSTRUCTION);
		return true;
	}
	else
	{
		return false;
	}
};

/* Changes the running LED instruction to the one passed as a parameter. Returns true if the instruction successfully replaced the previous one, false otherwise */
bool LEDManager_runLEDInstruction(LEDInstruction *instructionPtr){
	
	if(!_isInstructionRunning || (_isInstructionRunning && instructionPtr->priority >= _currentlyRunningLedInstruction.priority)){
		stopLedAndTimers();
		_currentlyRunningInstructionRepeatCount = 0;
		_currentlyRunningInstructionAction = 0;
		copyLEDInstruction(&_currentlyRunningLedInstruction, instructionPtr);
		_isInstructionRunning = true;
		maskUtilities_setRunningMaskBits(RUNNING_BIT_LED_INSTRUCTION);
		return true;
	}
	else
	{
		return false;
	}
}

static void copyLEDInstruction(LEDInstruction *dest, LEDInstruction *src)
{
	uint8_t i = 0;
	for(i=0; i<MAX_LEDS_ACTIONS; i++)
	{
		dest->actions[i].actionDuration = src->actions[i].actionDuration;
		dest->actions[i].gnLEDStatus = src->actions[i].gnLEDStatus;
		dest->actions[i].yeLEDStatus = src->actions[i].yeLEDStatus;
		dest->actions[i].rdLEDStatus = src->actions[i].rdLEDStatus;
	}

	dest->nbActions		= src->nbActions;
	dest->nbRuns		= src->nbRuns;
	dest->instructionId = src->instructionId;
	dest->priority		= src->priority;
}

volatile uint32_t wm;

void task_LEDManager(void* param){
	UNUSED(param);
	while(1){
		if(_isInstructionRunning){
			if(!isLedActionRunning()){
				_isInstructionRunning = (_currentlyRunningInstructionRepeatCount < _currentlyRunningLedInstruction.nbRuns) || (_currentlyRunningLedInstruction.nbRuns == INFINITE_RUNS);
				if(!_isInstructionRunning){
					maskUtilities_clearRunningMaskBits(RUNNING_BIT_LED_INSTRUCTION);
				}
				else
				{
					launchAction(&(_currentlyRunningLedInstruction.actions[_currentlyRunningInstructionAction]));
					_currentlyRunningInstructionAction++;
					if(_currentlyRunningInstructionAction >= _currentlyRunningLedInstruction.nbActions){
						_currentlyRunningInstructionAction = 0;
						_currentlyRunningInstructionRepeatCount++;
					}
				}
				wm = uxTaskGetStackHighWaterMark(NULL);
			}
		}
		vTaskDelay(10);
	}
}

static void launchAction(LEDAction *actionPtr){
	uint8_t numberOfBlinks = 0;

	if(actionPtr->gnLEDStatus == LED_STATUS_BLINKING){
		xTimerChangePeriod(_greenLedTimerHandle, LED_BLIKING_PERIOD_MS, 0);
		numberOfBlinks = actionPtr->actionDuration/LED_BLIKING_PERIOD_MS;
		LEDManager_blinkGreenLED(numberOfBlinks);
	}
	else if(actionPtr->gnLEDStatus == LED_STATUS_STATIC)
	{
		xTimerChangePeriod(_greenLedTimerHandle, actionPtr->actionDuration, 0);
		LEDManager_blinkGreenLED(1);
	}
	else if(actionPtr->gnLEDStatus == LED_STATUS_NONE)
	{
		xTimerChangePeriod(_greenLedTimerHandle, actionPtr->actionDuration, 0);
		LEDManager_blinkGreenLED(0);
	}

	if(actionPtr->rdLEDStatus == LED_STATUS_BLINKING){
		xTimerChangePeriod(_redLedTimerHandle, LED_BLIKING_PERIOD_MS, 0);
		numberOfBlinks = actionPtr->actionDuration/LED_BLIKING_PERIOD_MS;
		LEDManager_blinkRedLED(numberOfBlinks);
	}
	else if(actionPtr->rdLEDStatus == LED_STATUS_STATIC)
	{
		xTimerChangePeriod(_redLedTimerHandle, actionPtr->actionDuration, 0);
		LEDManager_blinkRedLED(1);
	}
	else if(actionPtr->rdLEDStatus == LED_STATUS_NONE)
	{
		xTimerChangePeriod(_redLedTimerHandle, actionPtr->actionDuration, 0);
		LEDManager_blinkRedLED(0);
	}

	if(actionPtr->yeLEDStatus == LED_STATUS_BLINKING)
	{
		xTimerChangePeriod(_yellowLedTimerHandle, LED_BLIKING_PERIOD_MS, 0);
		numberOfBlinks = actionPtr->actionDuration/LED_BLIKING_PERIOD_MS;
		LEDManager_blinkYellowLED(numberOfBlinks);
	}
	else if(actionPtr->yeLEDStatus == LED_STATUS_STATIC)
	{
		xTimerChangePeriod(_yellowLedTimerHandle, actionPtr->actionDuration, 0);
		LEDManager_blinkYellowLED(1);
	}
	else if(actionPtr->yeLEDStatus == LED_STATUS_NONE)
	{
		xTimerChangePeriod(_yellowLedTimerHandle, actionPtr->actionDuration, 0);
		LEDManager_blinkYellowLED(0);
	}
}

static bool isLedActionRunning(){
	return xTimerIsTimerActive(_greenLedTimerHandle) != pdFALSE || xTimerIsTimerActive(_yellowLedTimerHandle) != pdFALSE || xTimerIsTimerActive(_redLedTimerHandle) != pdFALSE;
}

static void stopLedAndTimers(){
	if(xTimerIsTimerActive(_greenLedTimerHandle)){
		xTimerStop(_greenLedTimerHandle, 0);
	}
	if(xTimerIsTimerActive(_yellowLedTimerHandle)){
		xTimerStop(_yellowLedTimerHandle, 0);
	}
	if(xTimerIsTimerActive(_redLedTimerHandle)){
		xTimerStop(_redLedTimerHandle, 0);
	}
	stopLeds();
}

static void stopLeds(){
	gpio_set_pin_level(LED_0_gn, false);
	gpio_set_pin_level(LED_1_rd, false);
	gpio_set_pin_level(LED_2_ye, false);
}

void LEDManager_turnOnGreenLED(){
	gpio_set_pin_level(LED_0_gn, true);
}

void LEDManager_turnOffGreenLED(){
	gpio_set_pin_level(LED_0_gn, false);
}

void LEDManager_turnOnRedLED(){
	gpio_set_pin_level(LED_1_rd, true);
}

void LEDManager_turnOffRedLED(){
	gpio_set_pin_level(LED_1_rd, false);
}

void LEDManager_turnOnYellowLED(){
	gpio_set_pin_level(LED_2_ye, true);
}

void LEDManager_turnOffYellowLED(){
	gpio_set_pin_level(LED_2_ye, false);
}

/*0 : no blink, always off 1 : static. More : will blink several times, the timer runs anyway to notify the Seekios of the action*/
void LEDManager_blinkGreenLED(int countOfBlink) {
	_countOfGreenLedBlink = countOfBlink;
	_actualGreenLedBlink = 0;
	if(_countOfGreenLedBlink == 0)
	{
		gpio_set_pin_level(LED_0_gn, false);
	}
	else
	{
		gpio_set_pin_level(LED_0_gn, true);
	}
	
	xTimerStart(_greenLedTimerHandle, 0);
}

/*0 : no blink, always off 1 : static. More : will blink several times, the timer runs anyway to notify the Seekios of the action*/
void LEDManager_blinkRedLED(int countOfBlink) {
	_countOfRedLedBlink = countOfBlink;
	_actualRedLedBlink = 0;
	if(_countOfRedLedBlink == 0)
	{
		gpio_set_pin_level(LED_1_rd, false);
	}
	else
	{
		gpio_set_pin_level(LED_1_rd, true);
	}
	xTimerStart(_redLedTimerHandle, 0);
}

/*0 : no blink, always off 1 : static. More : will blink several times, the timer runs anyway to notify the Seekios of the action*/
void LEDManager_blinkYellowLED(int countOfBlink) {
	_countOfYellowLedBlink = countOfBlink;
	_actualYellowLedBlink = 0;
	if(_countOfYellowLedBlink == 0)
	{
		gpio_set_pin_level(LED_2_ye, false);
	}
	else
	{
		gpio_set_pin_level(LED_2_ye, true);
	}
	
	xTimerStart(_yellowLedTimerHandle, 0);
}

static void toggleGreenLedShining(TimerHandle_t xTimer){
	
	gpio_toggle_pin_level(LED_0_gn);
	_actualGreenLedBlink++;

	if( _actualGreenLedBlink >= _countOfGreenLedBlink)
	{
		gpio_set_pin_level(LED_0_gn, false);
		xTimerStop( _greenLedTimerHandle, 0 );
	}
	UNUSED(xTimer);
}

static void toggleRedLedShining(TimerHandle_t xTimer){
	
	gpio_toggle_pin_level(LED_1_rd);
	_actualRedLedBlink++;

	if( _actualRedLedBlink >= _countOfRedLedBlink)
	{
		gpio_set_pin_level(LED_1_rd, false);
		xTimerStop( _redLedTimerHandle, 0 );
	}
	UNUSED(xTimer);
}

static void toggleYellowLedShining(TimerHandle_t xTimer){
	
	gpio_toggle_pin_level(LED_2_ye);
	_actualYellowLedBlink++;

	if( _actualYellowLedBlink >= _countOfYellowLedBlink)
	{
		gpio_set_pin_level(LED_2_ye, false);
		xTimerStop( _yellowLedTimerHandle, 0 );
	}
	UNUSED(xTimer);
}

bool LEDManager_isInstructionRunning(uint32_t instructionID)
{
	if(_isInstructionRunning && _currentlyRunningLedInstruction.instructionId == instructionID)
	{
		return true;
	}
	else
	{
		return false;
	}
}