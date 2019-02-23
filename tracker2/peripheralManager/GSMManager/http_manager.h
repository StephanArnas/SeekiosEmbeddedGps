#ifndef GPRS_MANAGER_H_
#define GPRS_MANAGER_H_

#include <FreeRTOS.h>
#include <task.h>
#include <seekiosManager/mask_utilities.h>
#include <seekiosCore/seekios.h>
#include <peripheralManager/RTC_manager.h>

#define GPRS_STATUS_BIT_HTTP_CONFIGURED			(1 << 0) // http config process worked
#define GPRS_STATUS_BIT_HTTP_INITIALIZED		(1 << 1) // http init successful

#define GPRS_EXPIRATION_TIME_5_MIN		5
#define GPRS_EXPIRATION_TIME_16_MIN		16

typedef enum{
	HTTP_REQUEST_STATUS_NONE,						// the code that executes the http request couldn't be executed
	HTTP_REQUEST_STATUS_UNKNOWN_ERROR,				// error source unknown
	HTTP_REQUEST_STATUS_OK					= 200,	// no error
	HTTP_REQUEST_STATUS_BAD_REQUEST			= 400,
	HTTP_REQUEST_STATUS_FORBIDDEN			= 403,
	HTTP_REQUEST_STATUS_NOT_FOUND			= 404,
	HTTP_REQUEST_STATUS_REQUEST_TIMEOUT		= 408,
	HTTP_REQUEST_STATUS_SERVER_ERROR		= 500,	// error source server
	HTTP_REQUEST_STATUS_NETWORK_ERROR		= 601,
}E_HTTP_REQUEST_STATUS;

typedef enum{
	BEARER_STATUS_CONNECTING	= 0,
	BEARER_STATUS_CONNECTED		= 1,
	BEARER_STATUS_CLOSING		= 2,
	BEARER_STATUS_CLOSED		= 3,
}E_BEARER_STATUS;

typedef enum{
	GPRS_REGISTRATION_NOT_REGISTERED = 				0,
	GPRS_REGISTRATION_REGISTERED_HOME = 			1,
	GPRS_REGISTRATION_NOT_REGISTERED_SEARCHING =	2,
	GPRS_REGISTRATION_DENIED = 						3,
	GPRS_REGISTRATION_UNKNOWN = 					4,
	GPRS_REGISTRATION_REGISTERED_ROAMING = 			5,
}E_GPRS_REGISTRATION_STATUS;

typedef struct{
	E_HTTP_REQUEST_STATUS (*httpGET)(char* url, char* httpMessage);
	uint8_t (*startHTTP)(void);
	uint8_t (*stopHTTP)(void);
	void (*task_HttpSession)(void* param);
	bool (*isSessionExpirationAlarmScheduled)(void);
	void (*removeHttpSessionExpirationAlarm)(void);
	void (*scheduleHttpSessionExpirationAlarm)(uint8_t expirationTime);
	void (*printSessionExpirationAlarm)(void);
}HttpService;

void httpManager_init(HttpService *httpServicePtr);

#endif /* GPRS_MANAGER_H_ */