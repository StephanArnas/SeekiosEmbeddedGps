
#ifndef POWER_SAVING_MANAGER_H_
#define POWER_SAVING_MANAGER_H_

#include <peripheralManager/RTC_manager.h>
#include <stdbool.h>
#include <seekiosManager/mask_utilities.h>
#include <tools/led_utilities.h>
#include <tests/test_monitor.h>
#include <messageSender/message_sender.h>

bool powerStateManager_isPowerSavingEnabled(void);
void powerStateManager_setWakeGSMCountdownIfNotSet(void);
void powerStateManager_enablePowerSaving(int8_t cultureHoursOffset);
void powerStateManager_disablePowerSavingIfEnabled(void);
void powerStateManager_setSeekiosOff(void);
bool powerStateManager_isSeekiosOn(void);
void powerStateManager_setSeekiosOn(void);
void powerStateManager_init(bool firstRunSeekios);
void powerStateManager_setPendingActionBits(EventBits_t bitsToSet);
void powerStateManager_clearPendingActionBits(EventBits_t bitsToSet);
EventBits_t powerStateManager_getPendingActionMask(void);
void powerStateManager_clearPendingActionsMask(void);

/* Pending bits are the bits that were received at a moment when we couldn't handle them. (Seekios off or in power saving)
We store them in a mask to put handle them once conditions are met (seekios ON and not in power saving)
Those bits doesn't prevent the deep sleep (which only concerns the interrupt, requests and running masks) */
#define PENDING_ACTION_BIT_SENDER						(1 << 0)
#define PENDING_ACTION_BIT_LISTENER						(1 << 1)
#define PENDING_ACTION_BIT_CHECK_LPC					(1 << 2)
#define PENDING_ACTION_BIT_RING_INDICATOR_INTERRUPT		(1 << 3)

#endif /* POWER_SAVING_MANAGER_H_ */