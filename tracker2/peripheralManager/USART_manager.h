/*
 * USART_manager.h
 *
 * Created: 09/03/2016 11:38:58
 *  Author: Luc
 */ 


#ifndef USART_MANAGER_H_
#define USART_MANAGER_H_

#include <string.h>
#include <tools/string_helper.h>
#include <FreeRTOS.h>
#include <serial_sgs.h>
#include <semphr.h>
#include <task.h>
#include <stdarg.h>
#include <stdint.h>
#include <FreeRTOS.h>
#include <event_groups.h>
#include <stdbool.h>
#include <seekiosCore/seekios.h>
#include <peripheralManager/USB_manager.h>
#include <sgs/helper_sgs.h>

typedef enum{
	USART_DEVICES_ARDUINO			=	0,
	USART_DEVICES_USB				=	1,
	USART_DEVICES_GSM_GPS_SHIELD	=	2,
}E_USART_DEVICES;

typedef enum{
	SERIAL_ANSWER_ERROR		=	0,
	SERIAL_ANSWER_OK		=	1,
	SERIAL_ANSWER_TIMEOUT	=	2,
	SERIAL_ANSWER_NONE		=	3,
} E_SERIAL_ANSWER;

typedef enum{
	ANSWER_VALIDATION_ERROR		=	0,
	ANSWER_VALIDATION_OK		=	1,
	ANSWER_VALIDATION_RAS	=	2,
} E_ANSWER_VALIDATION;

#define GSM_USART_BIT_TRANSFER_COMPLETE (1 << 0)

E_SERIAL_ANSWER serialAnswerFlag;
E_SERIAL_ANSWER USARTManager_sendATCommand(unsigned int timeout, uint8_t nbStrings, ...);
E_SERIAL_ANSWER USARTManager_readAnswerAndValidate(unsigned int timeout, void (*validatationCallback)(void));
void USARTManager_printUsbWait(const char* string);
void USARTManager_printByte(const uint8_t byte);
void USARTManager_printInt(const int integer);
void USARTManager_gsmUsartMaskSetBitsFromISR(EventBits_t bitsToSet);
void USARTManager_gsmUsartMaskClearBits(EventBits_t bitsToClear);
void USARTManager_init(void);
void USARTManager_printUsbWaitN(const char* text, uint16_t length);
bool USARTManager_repeatATCommandUntilSuccess(uint8_t numberTries, char* ATcommand, unsigned int timeout);
void USARTManager_activateUSBOutput(void);
void USARTManager_deactivateUSBOutput(void);

#endif /* USART_MANAGER_H_ */