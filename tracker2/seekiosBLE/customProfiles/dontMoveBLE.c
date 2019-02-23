#include <seekiosBLE/customProfiles/dontMoveBLE.h>

static at_ble_status_t dont_move_ble_char_changed_handler(void *params);
static at_ble_status_t dont_move_ble_connected_state_handler(void *params);
static at_ble_status_t dont_move_ble_disconnect_event_handler(void * params);
static at_ble_status_t dont_move_ble_adv_report_handler(void *params);
static bool dontmoveble_adv(void);
static void dontmoveble_service_define(void);
static void dontmoveble_service_init(void);

static dontmoveble_key_received_callback_t	_seekiosauth_cb;
static dontmoveble_callback_t				_dmb_adv_report_cb;
static dontmoveble_state_callback_t			_dontmoveble_state_cb;

static gatt_service_handler_t	_dmb_sas_handle; // SEEKIOS AUTH SERVICE
static at_ble_handle_t			_connection_handle;
static uint8_t					_seekiosauth_current_key_value[AUTH_KEY_LENGTH];
static bool	_dontMoveBLEConfigured;

void dontMoveBle_init(){
	_dontMoveBLEConfigured = false;
}

static const ble_event_callback_t _dmb_gap_handle[] = {
	NULL,
	NULL,
	NULL,
	dont_move_ble_adv_report_handler,
	NULL,
	dont_move_ble_connected_state_handler,
	dont_move_ble_disconnect_event_handler,
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

static const ble_event_callback_t _dmb_gatt_server_handle[] = {
	NULL,
	NULL,
	dont_move_ble_char_changed_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};


static void dontmoveble_service_init(void)
{
	init_seekios_auth_service(&_dmb_sas_handle);
}

void dmb_register_seekiosauth_handler(dontmoveble_key_received_callback_t seekiosauth_fn){
	_seekiosauth_cb = seekiosauth_fn;
}

void dmb_register_state_handler(dontmoveble_state_callback_t dontmoveble_state_fn){
	_dontmoveble_state_cb = dontmoveble_state_fn;
}

void dmb_register_adv_report_handler(dontmoveble_callback_t dmb_adv_report_cb)
{
	_dmb_adv_report_cb = dmb_adv_report_cb;
}

static at_ble_status_t dont_move_ble_connected_state_handler(void *params)
{
	at_ble_connected_t *conn_params;
	conn_params = (at_ble_connected_t *)params;
	_connection_handle = conn_params->handle;
	BLEManager_setBleConfigurationBits(BLE_STATE_BIT_DONT_MOVE_CONNECTED);
	_dontmoveble_state_cb(true);
	ALL_UNUSED(conn_params);
	return AT_BLE_SUCCESS;
}

static at_ble_status_t dont_move_ble_adv_report_handler(void *params){
	at_ble_adv_report_t *adv_report;
	adv_report = (at_ble_adv_report_t *)params;
	_dmb_adv_report_cb(adv_report->status);
	ALL_UNUSED(adv_report);
	return AT_BLE_SUCCESS;
}

static at_ble_status_t dont_move_ble_char_changed_handler(void *params)
{
	
	at_ble_characteristic_changed_t *char_handle;
	char_handle = (at_ble_characteristic_changed_t *)params;
	uint8_t temp_val;
	volatile at_ble_characteristic_changed_t change_params;
	memcpy((uint8_t *)&change_params, char_handle,sizeof(at_ble_characteristic_changed_t));
	temp_val = sas_set_auth_key_value(&change_params, &_dmb_sas_handle, _seekiosauth_current_key_value);

	if (temp_val != FUNCTION_FAILURE) { // TODO : is it reliable ?
		_seekiosauth_cb(_seekiosauth_current_key_value);
	}

	return AT_BLE_SUCCESS;
}


static void dontmoveble_service_define(){
	if(!(sas_primary_service_define(&_dmb_sas_handle) == AT_BLE_SUCCESS)){
		USARTManager_printUsbWait("SAS Service Define Failed");
	}
	else
	{
		USARTManager_printUsbWait("SAS Handle ");
		USARTManager_printInt(_dmb_sas_handle.serv_handle);
	}
	USARTManager_printUsbWait("The Supported Services in Seekios Auth Waiter are:");
	USARTManager_printUsbWait("  -> Seekios Auth Service");
	USARTManager_printUsbWait("\r\n");
}

static at_ble_status_t dont_move_ble_disconnect_event_handler(void * params)
{
	at_ble_disconnected_t *disconnect;
	disconnect = (at_ble_disconnected_t *)params;
	_dontmoveble_state_cb(false);
	ALL_UNUSED(disconnect);
	return AT_BLE_SUCCESS;
}

static bool dontmoveble_adv(void){
	/*  Set advertisement data from ble_manager*/
	if(!(ble_advertisement_data_set() == AT_BLE_SUCCESS))
	{
		USARTManager_printUsbWait("Fail to set Advertisement data");
	}

	/* Start of advertisement */
	if (at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED,
	AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY,
	APP_DMB_FAST_ADV, APP_DMB_ADV_TIMEOUT,
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
bool dontmoveble_configure(void* param){

	if(!_dontMoveBLEConfigured)
	{
		dontmoveble_service_init();
		dontmoveble_service_define();
		_dontMoveBLEConfigured = true;
	}
	
	if(!dontmoveble_adv())
	{
		return false;
	}
	
	/* Register BLE-GAP role and BLE-GATT server role */

	if(!ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GAP_EVENT_TYPE, _dmb_gap_handle))
	{
		return false;
	}
	if(!ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GATT_SERVER_EVENT_TYPE, _dmb_gatt_server_handle))
	{
		return false;
	}
	BLEManager_setBleConfigurationBits(BLE_STATE_BIT_DONT_MOVE_CALLBACK_SET);
	return true;
	ALL_UNUSED(param);
}

void dontmoveble_deconfigure(){
	ble_mgr_events_callback_handler(UNREGISTER_CALL_BACK, BLE_GAP_EVENT_TYPE, _dmb_gap_handle);
	ble_mgr_events_callback_handler(UNREGISTER_CALL_BACK, BLE_GATT_SERVER_EVENT_TYPE, _dmb_gatt_server_handle);
	BLEManager_clearBleStateBits(BLE_STATE_BIT_DONT_MOVE_CALLBACK_SET);
}

void dontmoveble_disconnect(){
	at_ble_status_t status;

	if ((status	= at_ble_disconnect(_connection_handle,	AT_BLE_TERMINATED_BY_USER)) !=	AT_BLE_SUCCESS)
	{
		USARTManager_printUsbWait("Failed to disconnect , reason ");
		USARTManager_printByte(status);
		USARTManager_printUsbWait("\r\n");
	}
	else
	{
		BLEManager_clearBleStateBits(BLE_STATE_BIT_DONT_MOVE_CONNECTED);
		USARTManager_printUsbWait("Disconnection request successfull\r\n");
	}
}

void dontmoveble_stopAdvertising(){
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