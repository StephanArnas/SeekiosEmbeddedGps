/*
 * FreeRTOS_overlay.c
 *
 * Created: 3/28/2017 12:12:49 PM
 *  Author: acer-hola
 * Content : An overlay of some FreeRTOS functionalities
 */ 

#include <seekiosCore/FreeRTOS_overlay.h>

void FreeRTOSOverlay_taskDelete(TaskHandle_t taskToDelete)
{
	char* taskName = pcTaskGetTaskName(taskToDelete);
	USARTManager_printUsbWait("\t<<< Deleting task ");
	USARTManager_printUsbWait(taskName);
	USARTManager_printUsbWait("\r\n");
	vTaskDelete(taskToDelete);
}
