/*
 * bma222_tos.h
 *
 * Created: 22/06/2016 10:39:13
 *  Author: Luc
 */ 


#ifndef BMA222_TOS_H_
#define BMA222_TOS_H_

#include <stdint.h>
#include <sgs/bma222_sgs.h>
#include <stdio.h>
#include <tools/string_helper.h>
#include <peripheralManager/USART_manager.h>
#include <FreeRTOS.h>
#include <task.h>
#include <seekiosManager/mask_utilities.h>

void IMUManager_init();
void IMUManager_startSlopeDetection(void);
void IMUManager_powerModeLowPower(void);
void IMUManager_suspend(void);
void IMUManager_printSleepDuration(void);
void IMUManager_stopSlopeDetection(void);
void task_detectSignificantMotion(void* param);
bool IMUManager_detectSignificantMove(void);
void IMUManager_powerModeNormal(void);
bool IMUManager_testNormalModeAccelerations(void);
//void BMA222_powerModeNormal(void);

#define SLOPE_DETECTION_SENSITIVITY 3
#define BMA_POWERMODE_NORMAL		0
#define BMA_POWERMODE_LOW_POWER		1
#define BMA_POWERMODE_SUSPEND		2

#define MOTION_DETECTION_TIMEOUT				10000
#define MOTION_DETECTION_SAMPLES				16			// the number of samples that we use to compute movement detection
#define MOTION_DETECTION_REFRESH_RATE_MS		20			// the refresh rate at which we take the samples. // TODO : use BMA interrupt that notifies new data
#define MOTION_DETECTION_THRESHOLD				35			// the threshold is the percentage of values that equals in the buffer for "stable" state to be detected. If the threshold is not met, then it's a motion
#define MOTION_DETECTION_NB_SIMILAR_VALUES		MOTION_DETECTION_SAMPLES*MOTION_DETECTION_THRESHOLD/100

#endif /* BMA222_TOS_H_ */