#ifndef USB_MANAGER_H_
#define USB_MANAGER_H_

#include <seekiosCore/seekios.h>
#include <usb_start.h>
#include <FreeRTOS.h>
#include <event_groups.h>
#include <semphr.h>

bool USBManager_start(void);
bool USBManager_stop(void);
void USBManager_print(const char* string);
void USBManager_println(const char* string);
void USBManager_test(void);
void USBManager_init(void);

#define USB_BIT_TRANSFER_COMPLETE (1 << 0)


/* All USB Commands that can be sent/received to/from the Seekios for several purposes */

/* Generic commands */
	/* Recieved commands */
	#define RX_GET_STATUS						"GET_STATUS"
	#define RX_TESTS_START						"TESTS_START"
	#define RX_TESTS_STOP						"TESTS_STOP"
	#define RX_TEST_TRIGGER						"TRIGGER"

	/* Sent commands */
	#define TX_SEND_STATUS_FUNCTIONALITIES_TEST_PROD		"STATUS_FUNC_TEST_PROD"
	#define TX_SEND_STATUS_FUNCTIONALITIES_TEST_SCENARIO	"STATUS_FUNC_TEST_SCENARIO"

/* Functionalities test commands */
	/* Recieved commands */

	/* Sent commands */
	#define TEST_RES_SKS "TEST_RES_SKS"
	#define TEST_RES_GSM "TEST_RES_GSM"
	#define TEST_RES_GPS "TEST_RES_GPS"
	#define TEST_RES_BLE "TEST_RES_BLE"
	#define TEST_RES_IMU "TEST_RES_IMU"
	#define TEST_RES_LED "TEST_RES_LED"
	#define TEST_RES_BUT "TEST_RES_BUT"
	#define TEST_RES_DF	 "TEST_RES_DF"
	#define TEST_RES_CAL "TEST_RES_CAL"
	#define TEST_RES_ADC "TEST_RES_ADC"
	#define FUNC_TEST_OVER	"FUNC_TEST_OVER"


#endif /* USB_MANAGER_H_ */