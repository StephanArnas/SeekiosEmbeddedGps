/* Ce status manager permet d'encapsuler le statut du seekios, tous les changements/lecture de status passent par
les fonctions de ce fichier
TODO : les fonctions de parsage devraient ausi être découpées dans un autre fichier*/

#ifndef STATUS_MANAGER_H_
#define STATUS_MANAGER_H_

#include <stdlib.h>
#include <string.h>
#include <peripheralManager/GPS_manager.h>
#include <stdbool.h>
#include <tools/string_helper.h>
#include <seekiosManager/power_state_manager.h>
#include <seekiosManager/seekios_info_manager.h>

#define NB_MAX_COORDINATES 10
#define NB_MAX_DECIMALS_IN_COORDINATES 9

#define DONT_MOVE_STATE_RAS				0 // slope detection activated, waiting for a tap
#define DONT_MOVE_STATE_MOTION_DETECTED	1 // tap detected, actively watching at the accel values
#define DONT_MOVE_STATE_SUSPEND			2 // suspended, nothing to do
#define DONT_MOVE_STATE_TRACKING		3 // tracking

#define ZONE_STATE_CHECK_POSITION			0 // With a task, checking if the seekios is out of zone
#define ZONE_STATE_WAIT_SIGNIFICANT_MOTION	1 // Seekios was in zone, waiting for a movement to check position again
#define ZONE_STATE_SUSPEND					2 // suspended, nothing to do
#define ZONE_STATE_TRACKING					3 // tracking

typedef enum { // TODO : enums elems should be unique
	MODE_STATUS_NONE,
	MODE_STATUS_WAITING,
	MODE_STATUS_ON_DEMAND,
	MODE_STATUS_SOS,
	MODE_STATUS_TRACKING,
	MODE_STATUS_DONT_MOVE,
	MODE_STATUS_ZONE,
}E_MODE_STATUS;

typedef struct {
	uint32_t modeID;
	uint16_t refreshRate; // in MINUTES
	Coordinate coordinates[NB_MAX_COORDINATES];
	uint8_t nbCoordinates;
	bool useExtraOptions; // use extra option in some mode, for exemple in the don't move mode, use the BLE to detect user proximity
}ModeParameters;

typedef struct {
	bool isPowerSavingEnabled;
	int8_t powerSavingCultureHoursOffset;
}PowerSavingConfig;

typedef struct{
	E_MODE_STATUS status;
	uint8_t state;
}ModeStatus;

typedef struct{
	ModeStatus modeStatus;
	ModeParameters modeParameters;
	PowerSavingConfig powerSavingConfig;
}ModeConfig;

void statusManager_initStatusManager(void);
void statusManager_updateConfigFromMessage(char* message);
void statusManager_switchBackPreviousConfig(void);
void StatusManager_printParameters(ModeParameters);
void statusManager_clearOnDemandRequestFlag(void);
void statusManager_clearNewConfigFlag(void);
void statusManager_useLastParsedConfigAsRunningConfig(void);
void statusManager_getRunningConfig(ModeConfig* configPtr);
void statusManager_setRunningConfig(ModeConfig* configPtr);
void statusManager_getLastParsedConfig(ModeConfig* destPtr);
void statusManager_updateLastParsedConfigFromMessage(char* message);
void statusManager_initRunningMode(void);
void statusManager_initLastParsedMode(void);
bool statusManager_isNewConfig(void);
bool statusManager_isOnDemandRequest(void);
char* statusManager_getCurrentStatusString(char*);
uint8_t statusManager_getRunningStatusState(void);
uint8_t statusManager_getRunningConfigCoordinates(Coordinate coordinates[NB_MAX_COORDINATES]);
uint32_t statusManager_getRunningConfigModeId(void);
void statusManager_setRunningConfigStatusState(uint8_t state);
void statusManager_getLastParsedStatus(ModeStatus* seekiosStatus);
uint8_t statusManager_getLastParsedStatusState(void);
void statusManager_getRunningStatus(ModeStatus* seekiosStatus);
uint16_t statusManager_getRunningRefreshRate(void);
void statusManager_setTestLastParsedConfig(void);
void statusManager_setRunningConfigAndClearRequestModeConfigBits(void);
bool statusManager_isLastParsedModeNew(void);
bool statusManager_isModeCanceled(void);
bool statusManager_isSOSAuthorized(void);
uint8_t statusManager_getGPRSExpirationTime(void);
void statusManager_processMessage(char* message);
bool statusManager_getRunningIsPowerSavingEnabled(void);
bool statusManager_getLastParsedIsPowerSavingEnabled(void);
void statusManager_getRunningPowerSavingConfig(PowerSavingConfig *configPtr);
void statusManager_getLastParsedPowerSavingConfig(PowerSavingConfig *configPtr);
void statusManager_setRunningIsPowerSavingEnabled(bool isPowerSavingEnabled);
bool statusManager_isFirstLastParsedMode(void);

#endif /* STATUS_MANAGER_H_ */