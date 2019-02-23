/*
 * FreeRTOS_overlay.h
 *
 * Created: 3/28/2017 12:13:25 PM
 *  Author: acer-hola
 */ 


#ifndef FREERTOS_OVERLAY_H_
#define FREERTOS_OVERLAY_H_

#include <FreeRTOS.h>
#include <task.h>

void FreeRTOSOverlay_taskDelete(TaskHandle_t taskToDelete);


#endif /* FREERTOS_OVERLAY_H_ */