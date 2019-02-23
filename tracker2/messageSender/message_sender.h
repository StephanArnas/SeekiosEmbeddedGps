#ifndef MESSAGE_SENDER_H_
#define MESSAGE_SENDER_H_

#define TX_BUFF_SIZE 256

#include <main.h>
#include <peripheralManager/USART_manager.h>
#include <peripheralManager/GPS_manager.h>
#include <peripheralManager/dataflash_manager.h>
#include <peripheralManager/GSMManager.h>
#include <peripheralManager/RTC_manager.h>
#include <seekiosManager/seekios_info_manager.h>
#include <statusManager/status_manager.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <sgs/helper_sgs.h>

/* Wait time for the next listening if getting the instructions from the cloud failed */
/* Wait time IN MINUTES */
#define SENDER_FAIL_WAIT_1 1
#define SENDER_FAIL_WAIT_2 1
#define SENDER_FAIL_WAIT_3 1
#define SENDER_FAIL_WAIT_4 1
#define SENDER_FAIL_WAIT_5 1
#define SENDER_FAIL_WAIT_6 3
#define SENDER_FAIL_WAIT_7 3
#define SENDER_FAIL_WAIT_8 3
#define SENDER_FAIL_WAIT_9 3

#define OPTIMIZED_SENDLIST_SIZE 10 // TODO : for debug. Make bigger (10)
#define SENDLIST_SIZE			4
#define MAX_QUEUE_RECEIVE_TIME	500
#define MAX_QUEUE_SEND_TIME		500
#define SENDER_GSM_WAIT_TIME	120000

typedef union{
	SatelliteCoordinate satelliteCoordinate;
}MessageContent;

/* Message type, for the sender to know which cloud URL to send */
typedef enum{
	MESSAGE_TYPE_NONE =							1,
	MESSAGE_TYPE_CRITICAL_POWER_EXTINCTION =	2,
	MESSAGE_TYPE_ALERT =						3,
	MESSAGE_TYPE_ON_DEMAND =					4,
	MESSAGE_TYPE_ON_DEMAND_TRIANGULATION =		5,
	MESSAGE_TYPE_SOS =							6,
	MESSAGE_TYPE_SOS_LOCATION =					7,
	MESSAGE_TYPE_SOS_LOCATION_TRIANGULATION =	8,
	MESSAGE_TYPE_TRACKING =						9,
	MESSAGE_TYPE_DONT_MOVE =					10,
	MESSAGE_TYPE_DONT_MOVE_TRACKING =			11,
	MESSAGE_TYPE_OUT_OF_ZONE =					12,
	MESSAGE_TYPE_ZONE_TRACKING =				13,
	MESSAGE_TYPE_ZONE_BACK_IN_ZONE =			14,
	MESSAGE_TYPE_IN_TIME =						15,
	MESSAGE_TYPE_FOLLOW_ME =					16,
	MESSAGE_TYPE_FOLLOW_ME_TRACKING =			17,
	MESSAGE_TYPE_DAILY_TRACK =					18,
	MESSAGE_TYPE_DAILY_TRACK_TRIANGULATION =	19,
	MESSAGE_TYPE_SEEKIOS_VERSION_UPDATED =		20,
	MESSAGE_TYPE_LOW_BATTERY =					21,
	MESSAGE_TYPE_POWER_SAVING_DISABLED =		22,
} E_MESSAGE_TYPE;

// The message category
typedef enum{
	MESSAGE_CATEGORY_ALERT		=	0,
	MESSAGE_CATEGORY_TRACKING	=	1,
} E_MESSAGE_CATEGORY;

// The message priority
typedef enum{
	MESSAGE_PRIORITY_LOW		= 0,
	MESSAGE_PRIORITY_MEDIUM		= 1,
	MESSAGE_PIORITY_HIGH		= 2
}E_MESSAGE_PRIORITY;

typedef struct{
	E_MESSAGE_TYPE messageType;
	MessageContent content;
	time_t timestamp;
	uint32_t modeId;
	void (*onSendSuccess)(void); // callback to be called once the OutputMessage is successFully send
} OutputMessage;

typedef struct{
	OutputMessage outputMessage;
	E_MESSAGE_PRIORITY messagePriority;
	E_MESSAGE_CATEGORY messageCategory;
} PrioritizedOutputMessage;

typedef struct{
	PrioritizedOutputMessage messages[OPTIMIZED_SENDLIST_SIZE];
	uint8_t nbMessages;
}OutputMessageArray;

void messageSender_init(void);
void task_messageSender(void*);
int messageSender_addToSendList(OutputMessage *message);
void messageSender_testWriteOutputMessageInFlash(void);
void messageSender_clearAlarmSenderRetry(void);
void reorganizeOptimizedSendList(void);
bool messageSender_outboxHasMessages(void);
void messageSender_printSenderRetryAlarm(void);
bool messageSender_isSenderRetryAlarmScheduled(void);
bool fakeSendingFailure;

#endif /* MODES_H_ */