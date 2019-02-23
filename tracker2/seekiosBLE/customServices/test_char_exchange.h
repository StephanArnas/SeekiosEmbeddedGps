/*
 * test_char.h
 *
 * Created: 20/09/2016 16:56:24
 *  Author: Luc
 */ 


#ifndef TEST_CHAR_H_
#define TEST_CHAR_H_

#include <ble_manager.h>
#include <seekiosCore/seekios.h>

#define INVALID_TCE_PARAM	0xff

#define TEST_CHAR_LENGTH 4 // "TEST"

/* test char Service UUID */
#define TEST_CHAR_EXHANGE_UUID				("\xb8\x01\x07\x78\x13\x8f\x4c\x5c\xa5\x9e\x66\xf5\x91\xb8\xaa\x3b")
/* test char Characteristic UUID*/
#define TEST_CHAR_UUID						("\x16\x49\x34\xce\x11\x10\x4c\x51\xa8\xb8\xb9\x95\x11\x64\x5b\xdf")

#define TEST_CHAR_EXCHANGE_SERVICE_COUNT (0)

#define TEST_CHAR_EXCHANGE_CHARASTERISTIC_COUNT (1)

void init_test_char_exchange_service(gatt_service_handler_t *test_char_exchange_serv);
at_ble_status_t tce_primary_service_define(gatt_service_handler_t *test_char_exchange_primary_service);
uint8_t tce_set_test_char_value(at_ble_characteristic_changed_t *change_params, gatt_service_handler_t *tce_handler, uint8_t result[TEST_CHAR_LENGTH]);



#endif /* TEST_CHAR_H_ */