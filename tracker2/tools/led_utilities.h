/*
 * led_utilities.h
 *
 * Created: 28/07/2016 11:27:00
 *  Author: Luc
 */ 


#ifndef LED_UTILITIES_H_
#define LED_UTILITIES_H_

#include <peripheralManager/LED_manager.h>
#include <semphr.h>
#include <string.h>

#define TURNING_OFF_LED_PERIOD		3000
#define TURNING_ON_LED_PERIOD		3000
#define FAST_TURNING_LEDS_INTERVAL	111
#define SLOW_TURNING_LEDS_INTERVAL	333

typedef enum{
	LED_BATTERY_TIER_NONE,
	LED_BATTERY_TIER_LOW,
	LED_BATTERY_TIER_MEDIUM,
	LED_BATTERY_TIER_HIGH,
	LED_BATTERY_TIER_FULL,
}E_LED_BATTERY_TIER;


void ledUtilities_init(void);
uint32_t ledUtilities_runVerifySeekiosIdentityLedInstruction(void);
uint32_t ledUtilities_runTestFunctionalitiesFastLedInstruction(void);
uint32_t ledUtilities_runTestFunctionalitiesSlowLedInstruction(void);
uint32_t ledUtilities_runMovementSuccessLedInstruction(void);
uint32_t ledUtilities_runTurnOffSeekiosLedInstruction(void);
uint32_t ledUtilities_runTurnOnSeekiosLedInstruction(void);
uint32_t ledUtilities_runFastTurningLedInstruction(void);
uint32_t ledUtilities_runSmsWakeupHandlingInstruction(void);
uint32_t ledUtilities_runSOSLedInstructions(void);
uint32_t ledUtilities_runBatteryLedInstruction(E_LED_BATTERY_TIER batteryPercentageTier);
uint32_t ledUtilities_runTestFailedLedInstruction(void);
uint32_t ledUtilities_runTestOverLedInstruction(void);
uint32_t ledUtilities_runCurrentlyTurningOffLedInstruction(void);
uint32_t ledUtilities_runBatteryChargeInstruction(E_LED_BATTERY_TIER batteryPercentageTier);
uint32_t ledUtilities_runResetSeekiosLedInstruction(void);
uint32_t ledUtilities_instructionReceivedLedInstruction(void);
uint32_t ledUtilities_runExitModeLedInstruction(void);

#endif /* LED_UTILITIES_H_ */