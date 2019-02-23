#include <tests/test_monitor.h>

static void triggerTest(char* triggerName);

#define MONITOR_BUF_LENGTH 512

void task_testMonitor(void* param)
{
	UNUSED(param);
	uint8_t monitorBuf[MONITOR_BUF_LENGTH];
	maskUtilities_setRunningMaskBits(RUNNING_BIT_USB_MONITOR);
	/* Listens to the messages left by the testing program */
	while(1)
	{
		int32_t read = cdcdf_acm_read(monitorBuf, MONITOR_BUF_LENGTH);
				
		if(read == USB_OK)
		{
			if(strstr(monitorBuf,RX_GET_STATUS)) // Return current status of the Seekios
			{
				strcpy(monitorBuf, TX_SEND_STATUS_FUNCTIONALITIES_TEST_SCENARIO);
				strcat(monitorBuf, "\r\n");
				USBManager_print(monitorBuf);
			}
			else if(strstr(monitorBuf, RX_TEST_TRIGGER))
			{
				USARTManager_printUsbWait("Test trigger received : ");
				USARTManager_printUsbWait(monitorBuf);
				USARTManager_printUsbWait("\r\n");
				triggerTest(monitorBuf);
			}
		}

		vTaskDelay(100);
	}
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_USB_MONITOR);
	vTaskDelete(NULL);
}

static void triggerTest(char* triggerFrame)
{
	if(triggerFrame == NULL)
	{
		return;
	}

	if(strstr(triggerFrame, TRIGGER_BUTTON_SOS))
	{
		maskUtilities_setRequestMaskBits(REQUEST_BIT_SOS);
	}
	else if(strstr(triggerFrame, TRIGGER_BUTTON_ACTION_OFF))
	{
		maskUtilities_setRequestMaskBits(REQUEST_BIT_SEEKIOS_TURN_OFF);
	}
	else if(strstr(triggerFrame, TRIGGER_BUTTON_ACTION_ON))
	{
		maskUtilities_setRequestMaskBits(REQUEST_BIT_SEEKIOS_TURN_ON);
	}
	else if(strstr(triggerFrame, TRIGGER_INSTRUCTION))
	{
		strtok(triggerFrame, ":");
		char* instruction = strtok(NULL, "\0\r\n");
		if(instruction != NULL)
		{
			statusManager_processMessage(instruction);
		}
	}

	return;
}

void testMonitor_raiseEvent(const char* eventName)
{
	if(maskUtilities_getRequestMask() & REQUEST_BIT_USB_DEBUGGING)
	{
		USBManager_println(eventName);
	}
}