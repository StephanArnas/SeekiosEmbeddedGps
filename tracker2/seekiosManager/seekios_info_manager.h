#ifndef SEEKIOS_INFO_MANAGER_H_
#define SEEKIOS_INFO_MANAGER_H_

#include <peripheralManager/dataflash_manager.h>
#include <peripheralManager/NVM_Manager.h>
#include <messageSender/message_sender.h>
#include <tools/string_helper.h>
#include <stdbool.h>

#define SEEKIOS_UID_SIZE							9
#define SEEKIOS_IDENTITY_VERIFICATION_TIMEOUT_MS	60000
#define SEEKIOS_PEERED_FLASH_SENTENCE				"SEEKIOS_PEERED"

void seekiosInfoManager_initSeekiosInfo(void);
bool seekiosInfoManager_isSeekiosFirstRun(void);
void seekiosInfoManager_getSeekiosUID(uint8_t buff[SEEKIOS_UID_SIZE]);
void seekiosInfoManager_setSeekiosIdentityVerified(void);
void seekiosInfoManager_setSeekiosNotFirstRun(void);
void task_verifySeekiosIdentity(void);
void seekiosInfoManager_catSeekiosVersion(char* resultBuff);
void seekiosInfoManager_updateSeekiosUID(void);
bool seekiosInfoManager_checkIfSeekiosVersionNew(void);
bool seekiosInfoManager_isSeekiosPeered(void);
void seekiosInfoManager_setSeekiosPeered(void);
void seekiosInfoManager_clearSeekiosPeered(void);
void seekiosInfoManager_seekiosVersionToString(char *resultBuff);
void seekiosInfoManager_isFirstInstructionTaken(void);
void seekiosInfoManager_setFirstInstructionTaken(void);

#endif /* SEEKIOS_INFO_MANAGER_H_ */