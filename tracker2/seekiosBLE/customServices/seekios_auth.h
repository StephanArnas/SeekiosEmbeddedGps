/*
 * seekios_service.h
 *
 * Created: 17/08/2016 09:24:29
 *  Author: Luc
 */ 


#ifndef SEEKIOS_SERVICE_H_
#define SEEKIOS_SERVICE_H_

#include <ble_manager.h>
#include <seekiosCore/seekios.h>

#define INVALID_SAS_PARAM	0xff

#define AUTH_KEY_LENGTH 24 // 4B Timestamp, 20B SHA-1

/* Seekios BLE Service UUID */
#define SEEKIOS_AUTH_UUID						("\x52\x90\xE8\x30\x64\x49\x11\xE6\x8B\x77\x86\xF3\x0C\xA8\x93\xD3")

/* Seekios Info Characteristic UUID*/
#define AUTH_KEY_UUID						("\x65\x70\x26\x7C\x64\x4F\x11\xE6\x8B\x77\x86\xF3\x0C\xA8\x93\xD3")

#define SEEKIOSAUTH_INCLUDED_SERVICE_COUNT (0)

#define SEEKIOSAUTH_CHARASTERISTIC_COUNT (1)

void init_seekios_auth_service(gatt_service_handler_t *seekios_auth_serv);
at_ble_status_t sas_primary_service_define(gatt_service_handler_t *seekiosauth_primary_service);
uint8_t sas_set_auth_key_value(at_ble_characteristic_changed_t *change_params, gatt_service_handler_t *sas_handler, uint8_t result[AUTH_KEY_LENGTH]);

#endif /* SEEKIOS_SERVICE_H_ */