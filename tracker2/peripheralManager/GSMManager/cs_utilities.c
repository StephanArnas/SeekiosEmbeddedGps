#include <peripheralManager/GSMManager/cs_manager.h>

/* The cs manager contains all the functions to manager the connexion to the gsm network (not including packet network functions.*/

static bool isNetworkAttached();
static bool isNetworkDetached();
static uint8_t parseNetworkRegistration(char* responseToParse);

bool csManager_attachNetworkService(uint16_t timeout)
{
	TickType_t startingTime = xTaskGetTickCount();
	while(!isNetworkAttached())
	{
		TickType_t currentTime = xTaskGetTickCount();
		uint16_t elapsedTime = currentTime - startingTime;
		if(elapsedTime >= timeout)
		{
			return false;
		}
		USARTManager_sendATCommand(timeout - elapsedTime, 1, "AT+COPS=0\r\n");
	}
	return true;
}

bool csManager_detachNetworkService(uint16_t timeout)
{
	TickType_t startingTime = xTaskGetTickCount();
	while(!isNetworkDetached())
	{
		TickType_t currentTime = xTaskGetTickCount();
		uint16_t elapsedTime = currentTime - startingTime;
		if(elapsedTime >= timeout)
		{
			return false;
		}
		USARTManager_sendATCommand(timeout - elapsedTime, 1, "AT+COPS=2\r\n");
	}
	return true;
}


static bool isNetworkAttached()
{
	if(USARTManager_sendATCommand(VERY_LONG_WAIT, 1, "AT+CREG?\r\n") == SERIAL_ANSWER_OK)
	{
		volatile E_NETWORK_REGISTRATION_STATUS networkRegistrationStatus = parseNetworkRegistration(gsm_buf);
		if(networkRegistrationStatus == NETWORK_REGISTRATION_REGISTERED_HOME
		|| networkRegistrationStatus == NETWORK_REGISTRATION_REGISTERED_ROAMING)
		{
			return true;
		}
	}
	return false;
}

static bool isNetworkDetached()
{
	if(USARTManager_sendATCommand(VERY_LONG_WAIT, 1, "AT+CREG?\r\n") == SERIAL_ANSWER_OK)
	{
		volatile E_NETWORK_REGISTRATION_STATUS networkRegistrationStatus = parseNetworkRegistration(gsm_buf);
		if(networkRegistrationStatus == NETWORK_REGISTRATION_NOT_REGISTERED
		|| networkRegistrationStatus == NETWORK_REGISTRATION_DENIED)
		{
			return true;
		}
	}
	return false;
}

static uint8_t parseNetworkRegistration(char* responseToParse)
{
	char* start = strstr(responseToParse, "+CREG:");
	strtok(start, ": ,\r\n");
	strtok(NULL, ": ,\r\n");
	char* networkRegistrationStatusStr = strtok(NULL, ": ,\r\n");
	uint8_t networkRegistrationStatus = atoi(networkRegistrationStatusStr);
	return networkRegistrationStatus;
}