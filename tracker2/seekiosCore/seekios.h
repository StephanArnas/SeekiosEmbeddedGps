#ifndef GLOBAL_VAR_H_
#define GLOBAL_VAR_H_

#include <stdbool.h>
#include <hal_delay.h>
#include <seekiosCore/FreeRTOS_overlay.h>
#include <task.h>

#define UNUSED(x) (void)(x)

#define DEV_SES_URL				"http://52.169.118.29/SES.svc/"
#define PROD_SES_URL			"http://52.169.220.116/SES.svc/"

#define IS_STABLE_SEEKIOS_VER 1 // 1 : stable version - 0 : development/debug version

/*	1 to version and 1 to subversion means 1.001 
	1 to version and 100 to subversion means 1.1 */
#define SEEKIOS_VERSION		1
#define SEEKIOS_SUBVERSION	7

#if (IS_STABLE_SEEKIOS_VER == 0)
	/* Misc options */
	#define FAKE_POSITIONS_ACTIVATED						1	// 1 : activate fake random positions / 0 : use GPS positionsA Note : with this activated, the Seekios wont go into sleeping mode if the GPS is ON.
	#define ADDITIONAL_LED_INSTRUCTIONS						0	// 1 : add led instructions when sms wakeup processing / 0 : regular led instructions
	#define WDT_ACTIVATED									1   // Watch Dog Timer
	#define BLE_ACTIVATED									0
	#define POWER_TESTS_ACTIVATED							0   // c'était pour que le seekios ne fasse pas son run normal (un peu comme le menu de sgs)
	#define SIMULATE_SENDING_FAILURES						0	// tester du non-gsm avec appui sur le bouton
	#define POWER_SAVING_START								0
	#define DEBUG_MODE										1
	#define ACTIVATE_BATTERY_LOG							1
	#define ACTIVATE_NMEA_FRAME_PRINT						0
	#define ACTIVATE_WRONG_NMEA_FRAME_LOGS					0	// If the Seekios detects a frame with the right checksum but with weird values, it will store it here. Just for debug.
	#define ACTIVATE_DEBUG_LOGS								1
	#define ACTIVATE_GPS_LOGS								0	// GPS Logs : records all the position that the Seekios sent back to the user
	#define TESTING_ACTIVATED								0 // Use the scenario_tests program to run with this version
	#define USB_ACTIVATED									0 // Activate the USB from the beginning instead of waiting for an admin frame to authorize it

	/* Set to 1 to delete this part in flash */
	#define DELETE_FLASH_PAGE_INDEX_SEEKIOS_ID				0
	#define DELETE_FLASH_PAGE_INDEX_SAVE_MESSAGES			0
	#define DELETE_FLASH_PAGE_PAGE_INDEX_SEEKIOS_VERSION	0
	#define DELETE_FLASH_PAGE_PAGE_INDEX_FIRST_RUN			0
	#define DELETE_FLASH_PAGE_PAGE_INDEX_TEST_DATAFLASH		0
	#define DELETE_FLASH_PAGE_PAGE_INDEX_SEEKIOS_PEERED		0
	#define DELETE_FLASH_PAGE_INDEX_IN_POWER_SAVING			0

	/* When cloud not available (see addFakeFrames in message listener) */
	#define TEST_FAKE_DONT_MOVE								0
	#define TEST_FAKE_ZONE									0
	#define TEST_FAKE_TRACKING								0
	#define TEST_FAKE_ON_DEMAND								0
	#define TEST_FAKE_ADMIN_TEST							0
	#define TEST_FULL_FRAME									0 // Activating this will override the received frame with a hard-coded frame

	/* GSM/GPRS Options */
	#define SEEKIOS_EMBEDDED_SERVICE_URL					PROD_SES_URL // sinon utiliser DEV_SES_URL ou PROD_SES_URL

// Ne pas toucher les paramètres de la version stable
#elif (IS_STABLE_SEEKIOS_VER == 1)
	/* Misc options */
	#define FAKE_POSITIONS_ACTIVATED						0	// 1 : activate fake random positions / 0 : use GPS positions
	#define ADDITIONAL_LED_INSTRUCTIONS						0	// 1 : add led instructions when sms wakeup processing / 0 : regular led instructions
	#define WDT_ACTIVATED									1
	#define BLE_ACTIVATED									0
	#define POWER_TESTS_ACTIVATED							0
	#define SIMULATE_SENDING_FAILURES						0
	#define POWER_SAVING_START								0
	#define DEBUG_MODE										0
	#define ACTIVATE_BATTERY_LOG							0
	#define ACTIVATE_WRONG_NMEA_FRAME_LOGS					0
	#define ACTIVATE_NMEA_FRAME_PRINT						0
	#define ACTIVATE_DEBUG_LOGS								0
	#define ACTIVATE_GPS_LOGS								0 
	#define TESTING_ACTIVATED								0
	#define USB_ACTIVATED									0

	/* Set to 1 to delete this part in flash */
	#define DELETE_FLASH_PAGE_INDEX_SEEKIOS_ID				0
	#define DELETE_FLASH_PAGE_INDEX_SAVE_MESSAGES			0
	#define DELETE_FLASH_PAGE_PAGE_INDEX_SEEKIOS_VERSION	0
	#define DELETE_FLASH_PAGE_PAGE_INDEX_FIRST_RUN			0
	#define DELETE_FLASH_PAGE_PAGE_INDEX_TEST_DATAFLASH		0
	#define DELETE_FLASH_PAGE_PAGE_INDEX_SEEKIOS_PEERED		0
	#define DELETE_FLASH_PAGE_INDEX_IN_POWER_SAVING			0
	
	/* When cloud not available */
	#define TEST_FAKE_DONT_MOVE			0
	#define TEST_FAKE_ZONE				0
	#define TEST_FAKE_TRACKING			0
	#define TEST_FAKE_ON_DEMAND			0
	#define TEST_FAKE_ADMIN_TEST		0
	#define TEST_FULL_FRAME				0 // Activating this will override the received frame with a hard-coded frame

	/* GSM/GPRS Options */
	#define SEEKIOS_EMBEDDED_SERVICE_URL	PROD_SES_URL
#endif

#define FUNCTION_RESULT_UNKNOWN	2
#define FUNCTION_SUCCESS		1
#define FUNCTION_FAILURE		0

#define SEND_LIST_SIZE			20
#define MAX_MESSAGE_LENGTH		256 // la taille maximale d'un message en réception
#define MAX_NUMBER_MESSAGES		2 // le nombre maximal de messages envoyés d'un coup par le cloud
#define TX_BUFFER_SIZE			256
#define MAX_TRIES_AT_COMMAND	10

#define SHORT_WAIT		10
#define MEDIUM_WAIT		1000
#define LONG_WAIT		30000
#define VERY_LONG_WAIT	60000
#define SUPER_LONG_WAIT	120000

#define MAX_CELL_DATA 6
#define CRLF "\r\n"

#define MAX_LEDS_ACTIONS				3
#define MAX_LEDS_ACTION_RECEIVE_TIME	1000
#define LED_BLIKING_PERIOD_MS			500

#define FUSES_START_ADDRESS					(void*)0x3fec0
#define FUSES_SIZE							64
/* The fuses contain the BOOT_BYTE, which has 3 significant values :
0xAA : App OK not requesting for an update
0xBB : App OK but waiting for an update, if Seekios connected
0xFF or any other : Memory corrupted. Waiting for an update */
#define FUSES_INDEX_BOOT_BYTE	0
#define FUSES_INDEX_VERSION		5
#define FUSES_INDEX_SUBVERSION	6

#define SEEKIOS_FLASH_DATA_START_ADDRESS	0x3ff00
#define SEEKIOS_FLASH_DATA_SIZE				64

bool seekiosManagerStarted;

#define DELAY_MS(x) seekiosManagerStarted ? vTaskDelay(x) : delay_ms(x)

#endif /* GLOBAL_VAR_H_ */