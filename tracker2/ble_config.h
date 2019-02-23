/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#ifndef BLE_CONFIG_H
#define BLE_CONFIG_H

#ifdef __cplusplus
		extern "C" {
#endif // __cplusplus

#define IMMEDIATE_ALERT_SERVICE

#define IMMEDIATE_ALERT_SERVICE
#define TX_POWER_SERVICE
#define NENABLE_PTS
#define TXPS_GATT_SERVER
#define LLS_GATT_SERVER
#define IAS_GATT_SERVER
#define LINK_LOSS_SERVICE
#define BLE_PAIR_ENABLE (false)
#define BLE_DEVICE_ROLE BLE_ROLE_ALL

#define ENABLE_POWER_SAVE
#define NEW_EVT_HANDLER
#define ATT_DB_MEMORY

#define UART_FLOWCONTROL_6WIRE_MODE true
#define UART_FLOWCONTROL_4WIRE_MODE false

void ble_wakeup_pin_set_low(void);
void ble_wakeup_pin_set_high(void);

#ifdef __cplusplus
		}
#endif // __cplusplus

#endif // BLE_CONFIG_H
