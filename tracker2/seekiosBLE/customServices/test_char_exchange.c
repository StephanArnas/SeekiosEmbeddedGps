#include "test_char_exchange.h"

/* This custom seekios_service BLE service  is used to transmit to a BLE client informations about the Seekios
Transmitted infos : - Seekios ID
- Current Config infos */

uint8_t testchar_initial_value[TEST_CHAR_LENGTH] = {0};

void init_test_char_exchange_service(gatt_service_handler_t *test_char_exchange_serv){

	test_char_exchange_serv->serv_handle = 0;
	test_char_exchange_serv->serv_uuid.type = AT_BLE_UUID_128;

	test_char_exchange_serv->serv_uuid.uuid[0] = (uint8_t)TEST_CHAR_EXHANGE_UUID[15];
	test_char_exchange_serv->serv_uuid.uuid[1] = (uint8_t)TEST_CHAR_EXHANGE_UUID[14];
	test_char_exchange_serv->serv_uuid.uuid[2] = (uint8_t)TEST_CHAR_EXHANGE_UUID[13];
	test_char_exchange_serv->serv_uuid.uuid[3] = (uint8_t)TEST_CHAR_EXHANGE_UUID[12];
	test_char_exchange_serv->serv_uuid.uuid[4] = (uint8_t)TEST_CHAR_EXHANGE_UUID[11];
	test_char_exchange_serv->serv_uuid.uuid[5] = (uint8_t)TEST_CHAR_EXHANGE_UUID[10];
	test_char_exchange_serv->serv_uuid.uuid[6] = (uint8_t)TEST_CHAR_EXHANGE_UUID[9];
	test_char_exchange_serv->serv_uuid.uuid[7] = (uint8_t)TEST_CHAR_EXHANGE_UUID[8];
	test_char_exchange_serv->serv_uuid.uuid[8] = (uint8_t)TEST_CHAR_EXHANGE_UUID[7];
	test_char_exchange_serv->serv_uuid.uuid[9] = (uint8_t)TEST_CHAR_EXHANGE_UUID[6];
	test_char_exchange_serv->serv_uuid.uuid[10] =(uint8_t)TEST_CHAR_EXHANGE_UUID[5];
	test_char_exchange_serv->serv_uuid.uuid[11] =(uint8_t)TEST_CHAR_EXHANGE_UUID[4];
	test_char_exchange_serv->serv_uuid.uuid[12] =(uint8_t)TEST_CHAR_EXHANGE_UUID[3];
	test_char_exchange_serv->serv_uuid.uuid[13] =(uint8_t)TEST_CHAR_EXHANGE_UUID[2];
	test_char_exchange_serv->serv_uuid.uuid[14] =(uint8_t)TEST_CHAR_EXHANGE_UUID[1];
	test_char_exchange_serv->serv_uuid.uuid[15] =(uint8_t)TEST_CHAR_EXHANGE_UUID[0];

	// Characteristic Info for test char
	test_char_exchange_serv->serv_chars.char_val_handle = 0;

	// UUID Seekios Info
	test_char_exchange_serv->serv_chars.uuid.type = AT_BLE_UUID_128;
	test_char_exchange_serv->serv_chars.uuid.uuid[0] = (uint8_t)TEST_CHAR_UUID[15];
	test_char_exchange_serv->serv_chars.uuid.uuid[1] = (uint8_t)TEST_CHAR_UUID[14];
	test_char_exchange_serv->serv_chars.uuid.uuid[2] = (uint8_t)TEST_CHAR_UUID[13];
	test_char_exchange_serv->serv_chars.uuid.uuid[3] = (uint8_t)TEST_CHAR_UUID[12];
	test_char_exchange_serv->serv_chars.uuid.uuid[4] = (uint8_t)TEST_CHAR_UUID[11];
	test_char_exchange_serv->serv_chars.uuid.uuid[5] = (uint8_t)TEST_CHAR_UUID[10];
	test_char_exchange_serv->serv_chars.uuid.uuid[6] = (uint8_t)TEST_CHAR_UUID[9];
	test_char_exchange_serv->serv_chars.uuid.uuid[7] = (uint8_t)TEST_CHAR_UUID[8];
	test_char_exchange_serv->serv_chars.uuid.uuid[8] = (uint8_t)TEST_CHAR_UUID[7];
	test_char_exchange_serv->serv_chars.uuid.uuid[9] = (uint8_t)TEST_CHAR_UUID[6];
	test_char_exchange_serv->serv_chars.uuid.uuid[10] =(uint8_t)TEST_CHAR_UUID[5];
	test_char_exchange_serv->serv_chars.uuid.uuid[11] =(uint8_t)TEST_CHAR_UUID[4];
	test_char_exchange_serv->serv_chars.uuid.uuid[12] =(uint8_t)TEST_CHAR_UUID[3];
	test_char_exchange_serv->serv_chars.uuid.uuid[13] =(uint8_t)TEST_CHAR_UUID[2];
	test_char_exchange_serv->serv_chars.uuid.uuid[14] =(uint8_t)TEST_CHAR_UUID[1];
	test_char_exchange_serv->serv_chars.uuid.uuid[15] =(uint8_t)TEST_CHAR_UUID[0];

	// Properties
	test_char_exchange_serv->serv_chars.properties = AT_BLE_CHAR_RELIABLE_WRITE;

	// Value
	test_char_exchange_serv->serv_chars.init_value = testchar_initial_value;
	test_char_exchange_serv->serv_chars.value_init_len = sizeof(int8_t)*TEST_CHAR_LENGTH;
	test_char_exchange_serv->serv_chars.value_max_len = sizeof(int8_t)*TEST_CHAR_LENGTH;

	#if BLE_PAIR_ENABLE
	test_char_exchange_serv->serv_chars.value_permissions = (AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR
	| AT_BLE_ATTR_WRITABLE_REQ_AUTHN_NO_AUTHR);

	#else
	test_char_exchange_serv->serv_chars.value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR
	| AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	#endif

	/* User defined name */
	test_char_exchange_serv->serv_chars.user_desc = NULL;
	test_char_exchange_serv->serv_chars.user_desc_len = 0;
	test_char_exchange_serv->serv_chars.user_desc_max_len = 0;

	/*user description permissions*/
	test_char_exchange_serv->serv_chars.user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	
	/*client config permissions*/
	test_char_exchange_serv->serv_chars.client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	
	/*server config permissions*/
	test_char_exchange_serv->serv_chars.server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	
	/*user desc handles*/
	test_char_exchange_serv->serv_chars.user_desc_handle = 0;
	
	/*client config handles*/
	test_char_exchange_serv->serv_chars.client_config_handle = 0;
	
	/*server config handles*/
	test_char_exchange_serv->serv_chars.server_config_handle = 0;
	
	/* presentation format */
	test_char_exchange_serv->serv_chars.presentation_format = NULL;
}

at_ble_status_t tce_primary_service_define(gatt_service_handler_t *test_char_exchange_primary_service)
{
	return (at_ble_primary_service_define(&test_char_exchange_primary_service->serv_uuid,
	&test_char_exchange_primary_service->serv_handle,
	NULL, TEST_CHAR_EXCHANGE_SERVICE_COUNT,
	&test_char_exchange_primary_service->serv_chars, TEST_CHAR_EXCHANGE_CHARASTERISTIC_COUNT));
}

/* Todo : handle more than a uint8_t : a string */
uint8_t tce_set_test_char_value(at_ble_characteristic_changed_t *change_params, gatt_service_handler_t *tce_handler, uint8_t result[TEST_CHAR_LENGTH])
{
	if (change_params->char_handle == tce_handler->serv_chars.char_val_handle)
	{
		memcpy(result, change_params->char_new_value, TEST_CHAR_LENGTH);
		return FUNCTION_SUCCESS;
	}
	return FUNCTION_FAILURE;
}