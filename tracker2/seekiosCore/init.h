#ifndef INIT_H_
#define INIT_H_

#include <seekiosCore/seekios.h>
#include <messageListener/message_listener.h>
#include <string.h>
#include <statusManager/status_manager.h>
#include <peripheralManager/USART_manager.h>
#include <messageSender/message_sender.h>
#include <peripheralManager/GSMManager.h>
#include <peripheralManager/button_manager.h>
#include <peripheralManager/LED_manager.h>
#include <peripheralManager/RTC_manager.h>
#include <peripheralManager/WDT_manager.h>
#include <seekiosBLE/customProfiles/dontMoveBLE.h>
#include <peripheralManager/dataflash_manager.h>
#include <ble_manager.h>
#include <tests/powerTests.h>

void init_init(void);

#endif /* INIT_H_ */