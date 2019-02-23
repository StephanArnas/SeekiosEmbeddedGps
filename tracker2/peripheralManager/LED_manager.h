#ifndef LED_MANAGER_H_
#define LED_MANAGER_H_

#include <seekiosManager/mask_utilities.h>
#include <stdint.h>
#include <sgs/port_sgs.h>
#include <queue.h>
#include <seekiosCore/seekios.h>
#include <string.h>

#define MAX_LEDS_ACTIONS 3
#define MAX_LEDS_ACTION_RECEIVE_TIME 1000
#define LED_BLIKING_PERIOD_MS 500
#define INFINITE_RUNS	255 // if we place this value in the "nbRuns" variable, it performs an infinite number of runs.

typedef enum{
	LED_STATUS_NONE,
	LED_STATUS_STATIC,
	LED_STATUS_BLINKING
}E_LED_STATUS;

typedef enum{
	LED_PRIORITY_LOW = 0,
	LED_PRIORITY_MEDIUM = 1,
	LED_PRIORITY_HIGH = 2,
	LED_PRIORITY_VERY_HIGH = 3,
}E_LED_INSTRUCTION_PRIORITY;

typedef struct{
	E_LED_STATUS	rdLEDStatus;
	E_LED_STATUS	gnLEDStatus;
	E_LED_STATUS	yeLEDStatus;
	uint32_t				actionDuration; // duration in MILISECONDS
}LEDAction;

typedef struct {
	LEDAction	actions[MAX_LEDS_ACTIONS];
	uint8_t		nbActions;
	uint8_t		nbRuns;
	uint32_t	instructionId;
	uint8_t		priority;
}LEDInstruction;


void LEDManager_init(void);
void task_LEDManager(void* param);
void LEDManager_blinkGreenLED(int countOfBlink);
void LEDManager_blinkRedLED(int countOfBlink);
void LEDManager_blinkYellowLED(int countOfBlink);
void LEDManager_turnOnGreenLED(void);
void LEDManager_turnOffGreenLED(void);
void LEDManager_turnOnRedLED(void);
void LEDManager_turnOffRedLED(void);
void LEDManager_turnOnYellowLED(void);
void LEDManager_turnOffYellowLED(void);
bool LEDManager_stopCurrentlyRunningInstructionId(uint32_t instructionId);
bool LEDManager_runLEDInstruction(LEDInstruction *instructionPtr);
void LEDManager_addInstructionID(LEDInstruction *instructionPtr);
bool LEDManager_isInstructionRunning(uint32_t instructionID);
void LEDManager_stopCurrentlyRunningInstruction(void);

#endif /* LED_MANAGER_H_ */