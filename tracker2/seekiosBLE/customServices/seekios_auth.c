#include "seekios_auth.h"

/* This custom seekios_service BLE service  is used to transmit to a BLE client informations about the Seekios
Transmitted infos : - Seekios ID
- Current Config infos */

uint8_t authkey_initial_value[AUTH_KEY_LENGTH] = {0};

void init_seekios_auth_service(gatt_service_handler_t *seekios_auth_serv){

	seekios_auth_serv->serv_handle = 0;
	seekios_auth_serv->serv_uuid.type = AT_BLE_UUID_128;

	seekios_auth_serv->serv_uuid.uuid[0] = (uint8_t)SEEKIOS_AUTH_UUID[15];
	seekios_auth_serv->serv_uuid.uuid[1] = (uint8_t)SEEKIOS_AUTH_UUID[14];
	seekios_auth_serv->serv_uuid.uuid[2] = (uint8_t)SEEKIOS_AUTH_UUID[13];
	seekios_auth_serv->serv_uuid.uuid[3] = (uint8_t)SEEKIOS_AUTH_UUID[12];
	seekios_auth_serv->serv_uuid.uuid[4] = (uint8_t)SEEKIOS_AUTH_UUID[11];
	seekios_auth_serv->serv_uuid.uuid[5] = (uint8_t)SEEKIOS_AUTH_UUID[10];
	seekios_auth_serv->serv_uuid.uuid[6] = (uint8_t)SEEKIOS_AUTH_UUID[9];
	seekios_auth_serv->serv_uuid.uuid[7] = (uint8_t)SEEKIOS_AUTH_UUID[8];
	seekios_auth_serv->serv_uuid.uuid[8] = (uint8_t)SEEKIOS_AUTH_UUID[7];
	seekios_auth_serv->serv_uuid.uuid[9] = (uint8_t)SEEKIOS_AUTH_UUID[6];
	seekios_auth_serv->serv_uuid.uuid[10] =(uint8_t)SEEKIOS_AUTH_UUID[5];
	seekios_auth_serv->serv_uuid.uuid[11] =(uint8_t)SEEKIOS_AUTH_UUID[4];
	seekios_auth_serv->serv_uuid.uuid[12] =(uint8_t)SEEKIOS_AUTH_UUID[3];
	seekios_auth_serv->serv_uuid.uuid[13] =(uint8_t)SEEKIOS_AUTH_UUID[2];
	seekios_auth_serv->serv_uuid.uuid[14] =(uint8_t)SEEKIOS_AUTH_UUID[1];
	seekios_auth_serv->serv_uuid.uuid[15] =(uint8_t)SEEKIOS_AUTH_UUID[0];

	// Characteristic Info for Auth Key
 	seekios_auth_serv->serv_chars.char_val_handle = 0;

	// UUID Seekios Info
	seekios_auth_serv->serv_chars.uuid.type = AT_BLE_UUID_128;
	seekios_auth_serv->serv_chars.uuid.uuid[0] = (uint8_t)AUTH_KEY_UUID[15];
	seekios_auth_serv->serv_chars.uuid.uuid[1] = (uint8_t)AUTH_KEY_UUID[14];
	seekios_auth_serv->serv_chars.uuid.uuid[2] = (uint8_t)AUTH_KEY_UUID[13];
	seekios_auth_serv->serv_chars.uuid.uuid[3] = (uint8_t)AUTH_KEY_UUID[12];
	seekios_auth_serv->serv_chars.uuid.uuid[4] = (uint8_t)AUTH_KEY_UUID[11];
	seekios_auth_serv->serv_chars.uuid.uuid[5] = (uint8_t)AUTH_KEY_UUID[10];
	seekios_auth_serv->serv_chars.uuid.uuid[6] = (uint8_t)AUTH_KEY_UUID[9];
	seekios_auth_serv->serv_chars.uuid.uuid[7] = (uint8_t)AUTH_KEY_UUID[8];
	seekios_auth_serv->serv_chars.uuid.uuid[8] = (uint8_t)AUTH_KEY_UUID[7];
	seekios_auth_serv->serv_chars.uuid.uuid[9] = (uint8_t)AUTH_KEY_UUID[6];
	seekios_auth_serv->serv_chars.uuid.uuid[10] =(uint8_t)AUTH_KEY_UUID[5];
	seekios_auth_serv->serv_chars.uuid.uuid[11] =(uint8_t)AUTH_KEY_UUID[4];
	seekios_auth_serv->serv_chars.uuid.uuid[12] =(uint8_t)AUTH_KEY_UUID[3];
	seekios_auth_serv->serv_chars.uuid.uuid[13] =(uint8_t)AUTH_KEY_UUID[2];
	seekios_auth_serv->serv_chars.uuid.uuid[14] =(uint8_t)AUTH_KEY_UUID[1];
	seekios_auth_serv->serv_chars.uuid.uuid[15] =(uint8_t)AUTH_KEY_UUID[0];

	// Properties
	seekios_auth_serv->serv_chars.properties = AT_BLE_CHAR_RELIABLE_WRITE;

	// Value
	seekios_auth_serv->serv_chars.init_value = authkey_initial_value;
	seekios_auth_serv->serv_chars.value_init_len = sizeof(int8_t)*AUTH_KEY_LENGTH;
	seekios_auth_serv->serv_chars.value_max_len = sizeof(int8_t)*AUTH_KEY_LENGTH;

	#if BLE_PAIR_ENABLE
	seekios_auth_serv->serv_chars.value_permissions = (AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR
	| AT_BLE_ATTR_WRITABLE_REQ_AUTHN_NO_AUTHR);

	#else
	seekios_auth_serv->serv_chars.value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR
	| AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	#endif

	/* User defined name */
	seekios_auth_serv->serv_chars.user_desc = NULL;
	seekios_auth_serv->serv_chars.user_desc_len = 0;
	seekios_auth_serv->serv_chars.user_desc_max_len = 0;

	/*user description permissions*/
	seekios_auth_serv->serv_chars.user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	
	/*client config permissions*/
	seekios_auth_serv->serv_chars.client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	
	/*server config permissions*/
	seekios_auth_serv->serv_chars.server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	
	/*user desc handles*/
	seekios_auth_serv->serv_chars.user_desc_handle = 0;
	
	/*client config handles*/
	seekios_auth_serv->serv_chars.client_config_handle = 0;
	
	/*server config handles*/
	seekios_auth_serv->serv_chars.server_config_handle = 0;
	
	/* presentation format */
	seekios_auth_serv->serv_chars.presentation_format = NULL;
}

at_ble_status_t sas_primary_service_define(gatt_service_handler_t *seekiosauth_primary_service)
{
	return (at_ble_primary_service_define(&seekiosauth_primary_service->serv_uuid,
			&seekiosauth_primary_service->serv_handle,
			NULL, SEEKIOSAUTH_INCLUDED_SERVICE_COUNT,
			&seekiosauth_primary_service->serv_chars, SEEKIOSAUTH_CHARASTERISTIC_COUNT));
}

uint8_t sas_set_auth_key_value(at_ble_characteristic_changed_t *change_params, gatt_service_handler_t *sas_handler, uint8_t result[AUTH_KEY_LENGTH])
{
	if (change_params->char_handle == sas_handler->serv_chars.char_val_handle)
	{
		memcpy(result, change_params->char_new_value, AUTH_KEY_LENGTH);
		return FUNCTION_SUCCESS;
	}
	return FUNCTION_FAILURE;
}