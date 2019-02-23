#include <tools/led_utilities.h>

static LEDInstruction _genericInstruction;
static SemaphoreHandle_t _genericInstructionLock;

void ledUtilities_init()
{
	_genericInstructionLock = xSemaphoreCreateMutex();
}

uint32_t ledUtilities_runTurnOnSeekiosLedInstruction(){
	if(xSemaphoreTake(_genericInstructionLock, 0) == pdPASS)
	{
		
		_genericInstruction.actions[0].actionDuration = FAST_TURNING_LEDS_INTERVAL;
		_genericInstruction.actions[0].gnLEDStatus = LED_STATUS_NONE;
		_genericInstruction.actions[0].yeLEDStatus = LED_STATUS_NONE;
		_genericInstruction.actions[0].rdLEDStatus = LED_STATUS_STATIC;

		_genericInstruction.actions[1].actionDuration = FAST_TURNING_LEDS_INTERVAL;
		_genericInstruction.actions[1].gnLEDStatus = LED_STATUS_NONE;
		_genericInstruction.actions[1].yeLEDStatus = LED_STATUS_STATIC;
		_genericInstruction.actions[1].rdLEDStatus = LED_STATUS_NONE;

		_genericInstruction.actions[2].actionDuration = FAST_TURNING_LEDS_INTERVAL;
		_genericInstruction.actions[2].gnLEDStatus = LED_STATUS_STATIC;
		_genericInstruction.actions[2].yeLEDStatus = LED_STATUS_NONE;
		_genericInstruction.actions[2].rdLEDStatus = LED_STATUS_NONE;

		LEDManager_addInstructionID(&_genericInstruction);
		_genericInstruction.nbActions = 3;
		_genericInstruction.nbRuns = 3;
		_genericInstruction.priority = LED_PRIORITY_LOW;

		LEDManager_runLEDInstruction(&_genericInstruction);

		xSemaphoreGive(_genericInstructionLock);
		return _genericInstruction.instructionId;
	}
}




uint32_t ledUtilities_runResetSeekiosLedInstruction()
{
	if(xSemaphoreTake(_genericInstructionLock, 0) == pdPASS)
	{
		_genericInstruction.actions[0].actionDuration = FAST_TURNING_LEDS_INTERVAL;
		_genericInstruction.actions[0].gnLEDStatus = LED_STATUS_NONE;
		_genericInstruction.actions[0].yeLEDStatus = LED_STATUS_STATIC;
		_genericInstruction.actions[0].rdLEDStatus = LED_STATUS_NONE;

		_genericInstruction.actions[1].actionDuration = FAST_TURNING_LEDS_INTERVAL;
		_genericInstruction.actions[1].gnLEDStatus = LED_STATUS_NONE;
		_genericInstruction.actions[1].yeLEDStatus = LED_STATUS_NONE;
		_genericInstruction.actions[1].rdLEDStatus = LED_STATUS_NONE;

		LEDManager_addInstructionID(&_genericInstruction);
		_genericInstruction.nbActions = 2;
		_genericInstruction.nbRuns = 255;
		_genericInstruction.priority = LED_PRIORITY_VERY_HIGH;

		LEDManager_runLEDInstruction(&_genericInstruction);
		xSemaphoreGive(_genericInstructionLock);
		return _genericInstruction.instructionId;
	}
	return 0;
}

/* Runs the LED display on the Seekios when the USB is plugged :
- If < 33% : red led bliking
- If >= 33% && < 66 % : red led static, yellow led blinking
- If >= 66% : yellow and red led static, green led bliking 
- If >= 100% : red, yellow, green leds static*/ 
uint32_t ledUtilities_runBatteryChargeInstruction(E_LED_BATTERY_TIER batteryPercentageTier)
{
		LEDInstruction batteryChargeInstruction;
		LEDAction actions[3];
		uint32_t actionDuration = 10000;
		switch(batteryPercentageTier)
		{
			case LED_BATTERY_TIER_NONE:
				actions[0].actionDuration = actionDuration;
				actions[0].gnLEDStatus = LED_STATUS_NONE;
				actions[0].yeLEDStatus = LED_STATUS_NONE;
				actions[0].rdLEDStatus = LED_STATUS_NONE;
			break;
			case LED_BATTERY_TIER_LOW:
				actions[0].actionDuration = actionDuration;
				actions[0].gnLEDStatus = LED_STATUS_NONE;
				actions[0].yeLEDStatus = LED_STATUS_NONE;
				actions[0].rdLEDStatus = LED_STATUS_BLINKING;
			break;
			case LED_BATTERY_TIER_MEDIUM:
				actions[0].actionDuration = actionDuration;
				actions[0].gnLEDStatus = LED_STATUS_NONE;
				actions[0].yeLEDStatus = LED_STATUS_BLINKING;
				actions[0].rdLEDStatus = LED_STATUS_STATIC;
			break;
			case LED_BATTERY_TIER_HIGH:
				actions[0].actionDuration = actionDuration;
				actions[0].gnLEDStatus = LED_STATUS_BLINKING;
				actions[0].yeLEDStatus = LED_STATUS_STATIC;
				actions[0].rdLEDStatus = LED_STATUS_STATIC;
			break;
			case LED_BATTERY_TIER_FULL:
				actions[0].actionDuration = actionDuration;
				actions[0].gnLEDStatus = LED_STATUS_STATIC;
				actions[0].yeLEDStatus = LED_STATUS_STATIC;
				actions[0].rdLEDStatus = LED_STATUS_STATIC;
			break;
		}

		memcpy(batteryChargeInstruction.actions, actions, sizeof(LEDAction)*MAX_LEDS_ACTIONS);
		LEDManager_addInstructionID(&batteryChargeInstruction);
		batteryChargeInstruction.nbActions = 1;
		batteryChargeInstruction.nbRuns = 255;
		batteryChargeInstruction.priority = LED_PRIORITY_LOW;

		LEDManager_runLEDInstruction(&batteryChargeInstruction);

		return batteryChargeInstruction.instructionId;
}

uint32_t ledUtilities_runTestFunctionalitiesFastLedInstruction(){
	LEDInstruction testFunctionalitiesFastLedInstruction;
	LEDAction actions[3];
	actions[0].actionDuration = FAST_TURNING_LEDS_INTERVAL;
	actions[0].gnLEDStatus = LED_STATUS_NONE;
	actions[0].yeLEDStatus = LED_STATUS_NONE;
	actions[0].rdLEDStatus = LED_STATUS_STATIC;

	actions[1].actionDuration = FAST_TURNING_LEDS_INTERVAL;
	actions[1].gnLEDStatus = LED_STATUS_NONE;
	actions[1].yeLEDStatus = LED_STATUS_STATIC;
	actions[1].rdLEDStatus = LED_STATUS_NONE;

	actions[2].actionDuration = FAST_TURNING_LEDS_INTERVAL;
	actions[2].gnLEDStatus = LED_STATUS_STATIC;
	actions[2].yeLEDStatus = LED_STATUS_NONE;
	actions[2].rdLEDStatus = LED_STATUS_NONE;

	memcpy(testFunctionalitiesFastLedInstruction.actions, actions, sizeof(LEDAction)*MAX_LEDS_ACTIONS);
	LEDManager_addInstructionID(&testFunctionalitiesFastLedInstruction);
	testFunctionalitiesFastLedInstruction.nbActions = 3;
	testFunctionalitiesFastLedInstruction.nbRuns = 255; 
	testFunctionalitiesFastLedInstruction.priority = LED_PRIORITY_LOW;

	LEDManager_runLEDInstruction(&testFunctionalitiesFastLedInstruction);

	return testFunctionalitiesFastLedInstruction.instructionId;
}

uint32_t ledUtilities_runTestFunctionalitiesSlowLedInstruction(){
	LEDInstruction testFunctionalitiesSlowLedInstruction;
	LEDAction actions[3];
	actions[0].actionDuration = SLOW_TURNING_LEDS_INTERVAL;
	actions[0].gnLEDStatus = LED_STATUS_NONE;
	actions[0].yeLEDStatus = LED_STATUS_NONE;
	actions[0].rdLEDStatus = LED_STATUS_STATIC;

	actions[1].actionDuration = SLOW_TURNING_LEDS_INTERVAL;
	actions[1].gnLEDStatus = LED_STATUS_NONE;
	actions[1].yeLEDStatus = LED_STATUS_STATIC;
	actions[1].rdLEDStatus = LED_STATUS_NONE;

	actions[2].actionDuration = SLOW_TURNING_LEDS_INTERVAL;
	actions[2].gnLEDStatus = LED_STATUS_STATIC;
	actions[2].yeLEDStatus = LED_STATUS_NONE;
	actions[2].rdLEDStatus = LED_STATUS_NONE;

	memcpy(testFunctionalitiesSlowLedInstruction.actions, actions, sizeof(LEDAction)*MAX_LEDS_ACTIONS);
	LEDManager_addInstructionID(&testFunctionalitiesSlowLedInstruction);
	testFunctionalitiesSlowLedInstruction.nbActions = 3;
	testFunctionalitiesSlowLedInstruction.nbRuns = 255;
	testFunctionalitiesSlowLedInstruction.priority = LED_PRIORITY_VERY_HIGH;

	LEDManager_runLEDInstruction(&testFunctionalitiesSlowLedInstruction);

	return testFunctionalitiesSlowLedInstruction.instructionId;
}

uint32_t ledUtilities_runTestOverLedInstruction()
{
	LEDInstruction testFailedLedInstruction;
	LEDAction actions[3];
	actions[0].actionDuration = 5000;
	actions[0].gnLEDStatus = LED_STATUS_STATIC;
	actions[0].yeLEDStatus = LED_STATUS_STATIC;
	actions[0].rdLEDStatus = LED_STATUS_STATIC;

	memcpy(testFailedLedInstruction.actions, actions, sizeof(LEDAction)*MAX_LEDS_ACTIONS);
	LEDManager_addInstructionID(&testFailedLedInstruction);
	testFailedLedInstruction.nbActions = 1;
	testFailedLedInstruction.nbRuns = 255;
	testFailedLedInstruction.priority = LED_PRIORITY_VERY_HIGH;

	LEDManager_runLEDInstruction(&testFailedLedInstruction);

	return testFailedLedInstruction.instructionId;
}

uint32_t ledUtilities_runTestFailedLedInstruction()
{
		LEDInstruction testFailedLedInstruction;
		LEDAction actions[3];
		actions[0].actionDuration = 5000;
		actions[0].gnLEDStatus = LED_STATUS_NONE;
		actions[0].yeLEDStatus = LED_STATUS_NONE;
		actions[0].rdLEDStatus = LED_STATUS_STATIC;

		memcpy(testFailedLedInstruction.actions, actions, sizeof(LEDAction)*MAX_LEDS_ACTIONS);
		LEDManager_addInstructionID(&testFailedLedInstruction);
		testFailedLedInstruction.nbActions = 1;
		testFailedLedInstruction.nbRuns = 255;
		testFailedLedInstruction.priority = LED_PRIORITY_VERY_HIGH;

		LEDManager_runLEDInstruction(&testFailedLedInstruction);

		return testFailedLedInstruction.instructionId;
}

/* TODO : pour debug, a supprimer */
uint32_t ledUtilities_runMovementSuccessLedInstruction(){
	LEDInstruction instruction;
	instruction.actions[0].actionDuration = 2000;
	instruction.actions[0].gnLEDStatus = LED_STATUS_BLINKING;
	instruction.actions[0].yeLEDStatus = LED_STATUS_BLINKING;
	instruction.actions[0].rdLEDStatus = LED_STATUS_BLINKING;
	LEDManager_addInstructionID(&instruction);
	instruction.nbActions = 1;
	instruction.nbRuns = 1;
	instruction.priority = LED_PRIORITY_MEDIUM;
	LEDManager_runLEDInstruction(&instruction);
	return instruction.instructionId;
}

uint32_t ledUtilities_runTurnOffSeekiosLedInstruction(){

	if(xSemaphoreTake(_genericInstructionLock, 0) == pdPASS)
	{
		_genericInstruction.actions[0].actionDuration = TURNING_OFF_LED_PERIOD;
		_genericInstruction.actions[0].gnLEDStatus = LED_STATUS_NONE;
		_genericInstruction.actions[0].yeLEDStatus = LED_STATUS_NONE;
		_genericInstruction.actions[0].rdLEDStatus = LED_STATUS_STATIC;

		LEDManager_addInstructionID(&_genericInstruction);
		_genericInstruction.nbActions =	1;
		_genericInstruction.nbRuns = 1;
		_genericInstruction.priority = LED_PRIORITY_VERY_HIGH;
		LEDManager_runLEDInstruction(&_genericInstruction);
		xSemaphoreGive(_genericInstructionLock);
		return _genericInstruction.instructionId;
	}
	
	return 0;
}

uint32_t ledUtilities_runFastTurningLedInstruction(){
	LEDInstruction turningLedInstruction;
	LEDAction actions[3];
	actions[0].actionDuration = FAST_TURNING_LEDS_INTERVAL;
	actions[0].gnLEDStatus = LED_STATUS_NONE;
	actions[0].yeLEDStatus = LED_STATUS_NONE;
	actions[0].rdLEDStatus = LED_STATUS_STATIC;

	actions[1].actionDuration = FAST_TURNING_LEDS_INTERVAL;
	actions[1].gnLEDStatus = LED_STATUS_NONE;
	actions[1].yeLEDStatus = LED_STATUS_STATIC;
	actions[1].rdLEDStatus = LED_STATUS_NONE;

	actions[2].actionDuration = FAST_TURNING_LEDS_INTERVAL;
	actions[2].gnLEDStatus = LED_STATUS_STATIC;
	actions[2].yeLEDStatus = LED_STATUS_NONE;
	actions[2].rdLEDStatus = LED_STATUS_NONE;

	memcpy(turningLedInstruction.actions, actions, sizeof(LEDAction)*MAX_LEDS_ACTIONS);
	LEDManager_addInstructionID(&turningLedInstruction);
	turningLedInstruction.nbActions = 3;
	turningLedInstruction.nbRuns = 5;
	turningLedInstruction.priority = LED_PRIORITY_MEDIUM;

	LEDManager_runLEDInstruction(&turningLedInstruction);

	return turningLedInstruction.instructionId;
}


uint32_t ledUtilities_runSOSLedInstructions(){
	LEDInstruction SOSInstruction;
	
	LEDAction SOSLedAction;
	SOSLedAction.actionDuration = 3000;
	SOSLedAction.gnLEDStatus = LED_STATUS_BLINKING;
	SOSLedAction.rdLEDStatus = LED_STATUS_BLINKING;
	SOSLedAction.yeLEDStatus = LED_STATUS_BLINKING;
	
	SOSInstruction.actions[0] = SOSLedAction;
	SOSInstruction.nbActions = 1;
	SOSInstruction.nbRuns = 1;
	SOSInstruction.priority = LED_PRIORITY_VERY_HIGH;
	LEDManager_addInstructionID(&SOSInstruction);

	LEDManager_runLEDInstruction(&SOSInstruction);
	return SOSInstruction.instructionId; 
}

uint32_t ledUtilities_runBatteryLedInstruction(E_LED_BATTERY_TIER batteryPercentageTier){

	LEDInstruction batteryLedInstruction;

	batteryLedInstruction.actions[0].actionDuration = 500;
	batteryLedInstruction.actions[0].gnLEDStatus = LED_STATUS_NONE;
	batteryLedInstruction.actions[0].yeLEDStatus = LED_STATUS_NONE;
	batteryLedInstruction.actions[0].rdLEDStatus = LED_STATUS_NONE;

	if(batteryPercentageTier  == LED_BATTERY_TIER_LOW){
		batteryLedInstruction.actions[0].rdLEDStatus = LED_STATUS_STATIC;
	}
	else if (batteryPercentageTier  == LED_BATTERY_TIER_MEDIUM)
	{
		batteryLedInstruction.actions[0].yeLEDStatus = LED_STATUS_STATIC;
	}
	else if(batteryPercentageTier  == LED_BATTERY_TIER_HIGH)
	{
		batteryLedInstruction.actions[0].gnLEDStatus = LED_STATUS_STATIC;
	}
	else
	{
		batteryLedInstruction.actions[0].gnLEDStatus = LED_STATUS_STATIC;
	}

	LEDManager_addInstructionID(&batteryLedInstruction);
	batteryLedInstruction.nbActions = 1;
	batteryLedInstruction.nbRuns = 1;
	batteryLedInstruction.priority = LED_PRIORITY_MEDIUM;
	LEDManager_runLEDInstruction(&batteryLedInstruction);

	return batteryLedInstruction.instructionId;
}

uint32_t ledUtilities_runSmsWakeupHandlingInstruction(){
	LEDInstruction wakeupHandlingInstruction;
	
	LEDAction actions[3];
	actions[0].actionDuration = 1000;
	actions[0].gnLEDStatus = LED_STATUS_STATIC;
	actions[0].yeLEDStatus = LED_STATUS_STATIC;
	actions[0].rdLEDStatus = LED_STATUS_STATIC;

	memcpy(wakeupHandlingInstruction.actions, actions, sizeof(LEDAction)*MAX_LEDS_ACTIONS);
	LEDManager_addInstructionID(&wakeupHandlingInstruction);
	wakeupHandlingInstruction.nbActions = 1;
	wakeupHandlingInstruction.nbRuns = 1;
	wakeupHandlingInstruction.priority = LED_PRIORITY_MEDIUM;

	LEDManager_runLEDInstruction(&wakeupHandlingInstruction);

	return wakeupHandlingInstruction.instructionId;
}

uint32_t ledUtilities_instructionReceivedLedInstruction(){
	LEDInstruction instructionReceivedInstruction;

	instructionReceivedInstruction.actions[0].actionDuration = 1500;
	instructionReceivedInstruction.actions[0].gnLEDStatus = LED_STATUS_STATIC;
	instructionReceivedInstruction.actions[0].yeLEDStatus = LED_STATUS_STATIC;
	instructionReceivedInstruction.actions[0].rdLEDStatus = LED_STATUS_STATIC;

	LEDManager_addInstructionID(&instructionReceivedInstruction);
	instructionReceivedInstruction.nbActions = 1;
	instructionReceivedInstruction.nbRuns = 1;
	instructionReceivedInstruction.priority = LED_PRIORITY_MEDIUM;

	LEDManager_runLEDInstruction(&instructionReceivedInstruction);
	return instructionReceivedInstruction.instructionId;
}

uint32_t ledUtilities_runExitModeLedInstruction()
{
	LEDInstruction exitModeLedInstruction;

	exitModeLedInstruction.actions[0].actionDuration = 2000;
	exitModeLedInstruction.actions[0].gnLEDStatus = LED_STATUS_NONE;
	exitModeLedInstruction.actions[0].yeLEDStatus = LED_STATUS_BLINKING;
	exitModeLedInstruction.actions[0].rdLEDStatus = LED_STATUS_BLINKING;

	LEDManager_addInstructionID(&exitModeLedInstruction);
	exitModeLedInstruction.nbActions = 1;
	exitModeLedInstruction.nbRuns = 1;
	exitModeLedInstruction.priority = LED_PRIORITY_MEDIUM;

	LEDManager_runLEDInstruction(&exitModeLedInstruction);
	return exitModeLedInstruction.instructionId;
}

uint32_t ledUtilities_runCurrentlyTurningOffLedInstruction()
{
	LEDInstruction turningOffLedInstruction;

	turningOffLedInstruction.actions[0].actionDuration = 1000;
	turningOffLedInstruction.actions[0].gnLEDStatus = LED_STATUS_NONE;
	turningOffLedInstruction.actions[0].yeLEDStatus = LED_STATUS_NONE;
	turningOffLedInstruction.actions[0].rdLEDStatus = LED_STATUS_BLINKING;

	LEDManager_addInstructionID(&turningOffLedInstruction);
	turningOffLedInstruction.nbActions = 1;
	turningOffLedInstruction.nbRuns = 255;
	turningOffLedInstruction.priority = LED_PRIORITY_VERY_HIGH;

	LEDManager_runLEDInstruction(&turningOffLedInstruction);
	return turningOffLedInstruction.instructionId;
}