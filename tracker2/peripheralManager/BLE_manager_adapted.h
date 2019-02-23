/*
 * BLE_manager_adapted.h
 *
 * Created: 24/06/2016 09:29:48
 *  Author: Luc
 */ 


#ifndef BLE_MANAGER_ADAPTED_H_
#define BLE_MANAGER_ADAPTED_H_

#include <ble_manager.h>
#include <ble_config.h>
#include <platform.h>
#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <seekiosBLE/customProfiles/dontMoveBLE.h>

typedef enum{
	BLE_STATE_NONE,
	BLE_STATE_ADVERTISING,
	BLE_STATE_CONNECTED,
	BLE_STATE_DISCONNECTED
}E_BLE_STATE;

void BLEManager_init(void);
void BLEManager_powerOnBLE(void);
void BLEManager_powerOffBLE(void);
void BLEManager_sleep(void);
void BLEManager_wakeUp(void);
void BLEManager_stopAdvertising(void);
void task_BleEventGetter(void);
void BLEManager_setBleConfigurationBits(EventBits_t bitsToSet);
void BLEManager_clearBleStateBits(EventBits_t bitsToClear);
void BLEManager_cancelBleUse(void);
void BLEManager_printEvent(at_ble_events_t events);
bool BLEManager_wakeBLEAndgetMACAddress(char* resultBuff);
void BLEManager_convertMacAddressByteToString(uint8_t addr[AT_BLE_ADDR_LEN], uint8_t *resultBuff);

#define BLE_STATE_BIT_DONT_MOVE_CALLBACK_SET	(1 << 0)
#define BLE_STATE_BIT_DONT_MOVE_CONNECTED		(1 << 1)
#define BLE_STATE_BIT_TEST_BLE_CALLBACK_SET		(1 << 2)
#define BLE_STATE_BIT_TEST_BLE_CONNECTED		(1 << 3)

#endif /* BLE_MANAGER_ADAPTED_H_ */