#include <seekiosBLE/customProfiles/testBLE.h>

static at_ble_status_t test_ble_adv_report_handler(void *params);
static at_ble_status_t test_ble_connected_state_handler(void *params);
static at_ble_status_t test_ble_disconnect_event_handler(void * params);
static at_ble_status_t test_ble_char_changed_handler(void *params);


static testble_testchar_received_callback_t	_testchar_cb;
static testble_callback_t					_testble_adv_report_cb;
static testble_state_callback_t				_testble_state_cb;

static gatt_service_handler_t	_tce_handle;
static at_ble_handle_t			_tb_connection_handle;
static uint8_t					_testchar_current_value[TEST_CHAR_LENGTH];
static bool	_testBLEConfigured;

void testBLE_init(){
	_testBLEConfigured = false;
}

static const ble_event_callback_t _tb_gap_handle[] = {
	NULL,
	NULL,
	NULL,
	test_ble_adv_report_handler,
	NULL,
	test_ble_connected_state_handler,
	test_ble_disconnect_event_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

static const ble_event_callback_t _tb_gatt_server_handle[] = {
	NULL,
	NULL,
	test_ble_char_changed_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};


static void testble_service_init(void)
{
	init_test_char_exchange_service(&_tce_handle);
}

void tb_register_char_changed_callback(testble_testchar_received_callback_t testchar_fn){
	_testchar_cb = testchar_fn;
}

void tb_register_state_callback(testble_state_callback_t testble_state_fn){
	_testble_state_cb = testble_state_fn;
}

void tb_register_adv_report_callback(testble_callback_t tb_adv_report_cb)
{
	_testble_adv_report_cb = tb_adv_report_cb;
}

static at_ble_status_t test_ble_connected_state_handler(void *params)
{
	at_ble_connected_t *conn_params;
	conn_params = (at_ble_connected_t *)params;
	_tb_connection_handle = conn_params->handle;
	BLEManager_setBleConfigurationBits(BLE_STATE_BIT_TEST_BLE_CONNECTED);
	_testble_state_cb(true);
	ALL_UNUSED(conn_params);
	return AT_BLE_SUCCESS;
}

static at_ble_status_t test_ble_adv_report_handler(void *params){
	at_ble_adv_report_t *adv_report;
	adv_report = (at_ble_adv_report_t *)params;
	_testble_adv_report_cb(adv_report->status);
	ALL_UNUSED(adv_report);
	return AT_BLE_SUCCESS;
}

static at_ble_status_t test_ble_char_changed_handler(void *params)
{
	
	at_ble_characteristic_changed_t *char_handle;
	char_handle = (at_ble_characteristic_changed_t *)params;
	uint8_t temp_val;
	volatile at_ble_characteristic_changed_t change_params;
	memcpy((uint8_t *)&change_params, char_handle,sizeof(at_ble_characteristic_changed_t));
	temp_val = tce_set_test_char_value(&change_params, &_tce_handle, _testchar_current_value);

	if (temp_val != FUNCTION_FAILURE) { // TODO : is it reliable ?
		_testchar_cb(_testchar_current_value);
	}

	return AT_BLE_SUCCESS;
}


static void testble_service_define(){
	if(!(tce_primary_service_define(&_tce_handle) == AT_BLE_SUCCESS)){
		USARTManager_printUsbWait("TCE Service Define Failed");
	}
	else
	{
		USARTManager_printUsbWait("TCE Handle ");
		USARTManager_printInt(_tce_handle.serv_handle);
	}
	USARTManager_printUsbWait("The Supported Services in Seekios Test BLE are:");
	USARTManager_printUsbWait("  -> Seekios Test char exchange Service");
	USARTManager_printUsbWait("\r\n");
}

static at_ble_status_t test_ble_disconnect_event_handler(void * params)
{
	at_ble_disconnected_t *disconnect;
	disconnect = (at_ble_disconnected_t *)params;
	_testble_state_cb(false);
	ALL_UNUSED(disconnect);
	return AT_BLE_SUCCESS;
}

static bool testble_adv(void){
	/*  Set advertisement data from ble_manager*/
	if(!(ble_advertisement_data_set() == AT_BLE_SUCCESS))
	{
		USARTManager_printUsbWait("Fail to set Advertisement data");
	}

	/* Start of advertisement */
	if (at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED,
	AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY,
	APP_TB_FAST_ADV, APP_TB_ADV_TIMEOUT,
	0) == AT_BLE_SUCCESS) {
		USARTManager_printUsbWait("Bluetooth device is in Advertising Mode");
		return true;
	}
	else
	{
		USARTManager_printUsbWait("BLE Adv start Failed");
		return false;
	}
}

/* Returns true if it was successfully configured, false otherwise*/
bool testble_configure(void* param){

	if(!_testBLEConfigured)
	{
		testble_service_init();
		testble_service_define();
		_testBLEConfigured = true;
	}
	
	if(!testble_adv())
	{
		return false;
	}
	
	/* Register BLE-GAP role and BLE-GATT server role */

	if(!ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GAP_EVENT_TYPE, _tb_gap_handle))
	{
		return false;
	}
	if(!ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GATT_SERVER_EVENT_TYPE, _tb_gatt_server_handle))
	{
		return false;
	}
	BLEManager_setBleConfigurationBits(BLE_STATE_BIT_TEST_BLE_CALLBACK_SET);
	return true;
	ALL_UNUSED(param);
}

void testble_deconfigure(){
	ble_mgr_events_callback_handler(UNREGISTER_CALL_BACK, BLE_GAP_EVENT_TYPE, _tb_gap_handle);
	ble_mgr_events_callback_handler(UNREGISTER_CALL_BACK, BLE_GATT_SERVER_EVENT_TYPE, _tb_gatt_server_handle);
	BLEManager_clearBleStateBits(BLE_STATE_BIT_TEST_BLE_CALLBACK_SET);
}

void testble_disconnect(){
	at_ble_status_t status;

	if ((status	= at_ble_disconnect(_tb_connection_handle,	AT_BLE_TERMINATED_BY_USER)) !=	AT_BLE_SUCCESS)
	{
		USARTManager_printUsbWait("Failed to disconnect , reason ");
		USARTManager_printByte(status);
		USARTManager_printUsbWait("\r\n");
	}
	else
	{
		BLEManager_clearBleStateBits(BLE_STATE_BIT_TEST_BLE_CONNECTED);
		USARTManager_printUsbWait("Disconnection request successfull\r\n");
	}
}

void testble_stopAdvertising(){
	at_ble_status_t status;
	
	if ((status = at_ble_adv_stop()) !=	AT_BLE_SUCCESS)
	{
		USARTManager_printUsbWait("Failed to stop advertising , reason ");
		USARTManager_printByte(status);
		USARTManager_printUsbWait("\r\n");
	}
	else
	{
		USARTManager_printUsbWait("Advertising termiation request successfull\r\n");
	}
}