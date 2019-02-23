/*
* BLE_manager.c
*
* Created: 24/06/2016 09:27:58
*  Author: Luc
*/

#include <peripheralManager/BLE_manager_adapted.h>

#define BTLC1000_RESET_MS                       ( 50 )

static EventGroupHandle_t _bleConfigurationMaskHandle;

/* PINS :
- BTLC1000_CHIP_ENABLE_PIN :	-> Low = off
-> High = on
*/

/* Initialization of the BLE at the beginning. Can't be re-initted */
void BLEManager_init(){
	ble_device_init(NULL);
	BLEManager_sleep();
	_bleConfigurationMaskHandle = xEventGroupCreate();
}

/* DEPRECATED USE SLEEP/WAKE FUNCTIONS */
/* Powers on the BLE and starts advertising */
void BLEManager_powerOnBLE(){
	platform_init(AT_BLE_UART, true);
}

void BLEManager_convertMacAddressByteToString(uint8_t addr[AT_BLE_ADDR_LEN], uint8_t *resultBuff)
{
	char const digit[] = "0123456789ABCDEF";
	resultBuff[0]	= digit[(addr[5] >> 4) & 0b00001111];
	resultBuff[1]	= digit[addr[5] & 0b00001111];
	resultBuff[2]	= digit[(addr[4] >> 4) & 0b00001111];
	resultBuff[3]	= digit[addr[4] & 0b00001111];
	resultBuff[4]	= digit[(addr[3] >> 4) & 0b00001111];
	resultBuff[5]	= digit[addr[3] & 0b00001111];
	resultBuff[6]	= digit[(addr[2] >> 4) & 0b00001111];
	resultBuff[7]	= digit[addr[2] & 0b00001111];
	resultBuff[8]	= digit[(addr[1] >> 4) & 0b00001111];
	resultBuff[9]	= digit[addr[1] & 0b00001111];
	resultBuff[10]	= digit[(addr[0] >> 4) & 0b00001111];
	resultBuff[11]	= digit[addr[0] & 0b00001111];
	resultBuff[12]=0;
}

bool BLEManager_wakeBLEAndgetMACAddress(char* resultBuff)
{
	bool success = false;
	at_ble_addr_t address = {AT_BLE_ADDRESS_PUBLIC, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
	#if (BLE_ACTIVATED == 1)
	BLEManager_wakeUp();
	if(at_ble_addr_get(&address) == AT_BLE_SUCCESS)
	{
	#endif
		success = true;
		char const digit[] = "0123456789ABCDEF";
		resultBuff[0]	= digit[(address.addr[5] >> 4) & 0b00001111];
		resultBuff[1]	= digit[address.addr[5] & 0b00001111];
		resultBuff[2]	= digit[(address.addr[4] >> 4) & 0b00001111];
		resultBuff[3]	= digit[address.addr[4] & 0b00001111];
		resultBuff[4]	= digit[(address.addr[3] >> 4) & 0b00001111];
		resultBuff[5]	= digit[address.addr[3] & 0b00001111];
		resultBuff[6]	= digit[(address.addr[2] >> 4) & 0b00001111];
		resultBuff[7]	= digit[address.addr[2] & 0b00001111];
		resultBuff[8]	= digit[(address.addr[1] >> 4) & 0b00001111];
		resultBuff[9]	= digit[address.addr[1] & 0b00001111];
		resultBuff[10]	= digit[(address.addr[0] >> 4) & 0b00001111];
		resultBuff[11]	= digit[address.addr[0] & 0b00001111];
		resultBuff[12]=0;
	#if (BLE_ACTIVATED == 1)
	}
	BLEManager_sleep();
	#endif
	
	return success;
}



void BLEManager_printEvent(at_ble_events_t events)
{
	switch(events)
	{
    case AT_BLE_ADV_REPORT							:
		USARTManager_printUsbWait("AT_BLE_ADV_REPORT");
		break;
    case AT_BLE_RAND_ADDR_CHANGED					:
		USARTManager_printUsbWait("AT_BLE_RAND_ADDR_CHANGED");
		break;
    case AT_BLE_CONNECTED							:
		USARTManager_printUsbWait("AT_BLE_CONNECTED");
		break;
    case AT_BLE_DISCONNECTED						:
		USARTManager_printUsbWait("AT_BLE_DISCONNECTED");
		break;
    case AT_BLE_CONN_PARAM_UPDATE_DONE				:
		USARTManager_printUsbWait("AT_BLE_CONN_PARAM_UPDATE_DONE");
		break;
    case AT_BLE_CONN_PARAM_UPDATE_REQUEST			:
		USARTManager_printUsbWait("AT_BLE_CONN_PARAM_UPDATE_REQUEST");
		break;
    case AT_BLE_PAIR_DONE							:
		USARTManager_printUsbWait("AT_BLE_PAIR_DONE");
		break;
    case AT_BLE_PAIR_REQUEST						:
		USARTManager_printUsbWait("AT_BLE_PAIR_REQUEST");
		break;
    case AT_BLE_CHARACTERISTIC_CHANGED				:
		USARTManager_printUsbWait("AT_BLE_CHARACTERISTIC_CHANGED");
		break;
    case AT_BLE_CHARACTERISTIC_CONFIGURATION_CHANGED:
		USARTManager_printUsbWait("AT_BLE_CHARACTERISTIC_CONFIGURATION_CHANGED");
		break;
    case AT_BLE_CUSTOM_EVENT						:
		USARTManager_printUsbWait("AT_BLE_CUSTOM_EVENT");
		break;
    case AT_BLE_EVENT_MAX							:
		USARTManager_printUsbWait("AT_BLE_EVENT_MAX");
		break;
	default:
		USARTManager_printUsbWait("UNKNOWN");
		break;
	}
}

void BLEManager_stopAdvertising(){
	at_ble_status_t status;
	
	if ((status = at_ble_adv_stop()) !=	AT_BLE_SUCCESS)
	{
		USARTManager_printUsbWait("Failed to stop advertising , reason ");
		USARTManager_printByte(status);
	}
	else
	{
		USARTManager_printUsbWait("Advertising termiation request successfull");
	}
}

/* DEPRECATED USE SLEEP/WAKE FUNCTIONS */
/* Stops the advertising and powers off the GPS */
void BLEManager_powerOffBLE(){
	platform_gpio_set(AT_BLE_CHIP_ENABLE, AT_BLE_LOW);
	vTaskDelay(BTLC1000_RESET_MS);
}

void BLEManager_sleep(){
	ble_wakeup_pin_set_low();
}

void BLEManager_wakeUp(){
	ble_wakeup_pin_set_high();	
	vTaskDelay(1000);
}

void BLEManager_setBleConfigurationBits(EventBits_t bitsToSet)
{
	xEventGroupSetBits(_bleConfigurationMaskHandle, bitsToSet);
}

void BLEManager_clearBleStateBits(EventBits_t bitsToClear)
{
	xEventGroupSetBits(_bleConfigurationMaskHandle, bitsToClear);
}

/* If called, cancels any BLE use : removes GAP and GATT events callbacks stops advertising, stops connection */
void BLEManager_cancelBleUse()
{
	BLEManager_wakeUp();

	EventBits_t bleConfigurationMask = xEventGroupGetBits(_bleConfigurationMaskHandle);
	if(bleConfigurationMask & BLE_STATE_BIT_DONT_MOVE_CALLBACK_SET)
	{
		dontmoveble_deconfigure();
	}
	if(bleConfigurationMask & BLE_STATE_BIT_DONT_MOVE_CONNECTED)
	{
		dontmoveble_disconnect();
	}

	BLEManager_stopAdvertising();

	BLEManager_sleep();
}