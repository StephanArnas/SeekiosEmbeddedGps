#include <seekiosManager/seekios_info_manager.h>

/* In this file are the functions that manipulate the Seekios version and UID */
static bool writeSeekiosUIDToExtFlash(uint8_t UID[SEEKIOS_UID_SIZE]);
static bool writeSeekiosUIDToMCUFlash(uint8_t UID[SEEKIOS_UID_SIZE]);
static bool isSeekiosUIDInExtFlash(void);
static bool getSeekiosUIDFromExtFlash(uint8_t result[SEEKIOS_UID_SIZE]);
static void updateSeekiosVersionInExtFlash(void);
static bool getExtFlashSeekiosVersion(uint16_t result[2]);
static void printSeekiosVersion(void);
static bool isSeekiosUIDInMCUFLash(void);
static void writeUIDToFlashIfNotExisting(void);
static uint8_t* computeSeekiosUID(uint8_t* result);
static char* buildUIDFromIMEI(char* IMEIstr, char* result);
static void addSeekiosVersionUpdatedMessage(void);
static void subversionToString(char buff[10]);
static bool isSeekiosPeeredFromExtFlash(void);

const char* tagString = "TAG_STRING:v1.007.5"; // This is used to be able to find the version of any binary

static const uint16_t _seekiosVersion[2] = {SEEKIOS_VERSION, SEEKIOS_SUBVERSION}; // 1: Version, 2: Subversion
static uint8_t	_seekiosUID[SEEKIOS_UID_SIZE];
static bool _isSeekiosPeered;
static bool _firstInstructionTaken = false;

void seekiosInfoManager_initSeekiosInfo()
{
	seekiosInfoManager_updateSeekiosUID();
	if(seekiosInfoManager_checkIfSeekiosVersionNew()){
		addSeekiosVersionUpdatedMessage();
	}
	_isSeekiosPeered = isSeekiosPeeredFromExtFlash();
	if(_isSeekiosPeered) // at the start of the Seekios, it will get its current config this way.
	{
		maskUtilities_setRequestMaskBits(REQUEST_BIT_LISTENER);
	}
}

bool seekiosInfoManager_isSeekiosPeered()
{
	return _isSeekiosPeered;
}

static bool isSeekiosPeeredFromExtFlash()
{
	char readBuf[20] = "";
	dataflashManager_readPage(PAGE_INDEX_SEEKIOS_PEERED,20,(unsigned char*)readBuf);
	return strstr(readBuf, SEEKIOS_PEERED_FLASH_SENTENCE) != NULL;
}

void seekiosInfoManager_setSeekiosPeered()
{
	USARTManager_printUsbWait("Seekios peered set.\r\n");
	_isSeekiosPeered = true;
	char readBuf[20] = "SEEKIOS_PEERED";
	dataflashManager_writeToPage(PAGE_INDEX_SEEKIOS_PEERED,20,(unsigned char*)readBuf);
}

void seekiosInfoManager_clearSeekiosPeered()
{
	_isSeekiosPeered = false;
	char readBuf[20] = {0};
	dataflashManager_readPage(PAGE_INDEX_SEEKIOS_PEERED,20,(unsigned char*)readBuf);
}

void seekiosInfoManager_isFirstInstructionTaken()
{
	return _firstInstructionTaken;
}

void seekiosInfoManager_setFirstInstructionTaken()
{
	_firstInstructionTaken = true;
}

/* Returns the version of the Seekios with he format 1.001 etc */
void seekiosInfoManager_seekiosVersionToString(char *resultBuff)
{
	char buff[10];
	stringHelper_intToString(_seekiosVersion[0], (unsigned char*)buff);
	strcpy(resultBuff, buff);
	strcat(resultBuff, ".");
	subversionToString(buff);
	strcat(resultBuff, buff);
}

void seekiosInfoManager_catSeekiosVersion(char* resultBuff)
{
	strcat(resultBuff, "/");
	char buff[10];
	seekiosInfoManager_seekiosVersionToString(buff);
	strcat(resultBuff, buff);
}

static void subversionToString(char buff[10])
{
	memset(buff, 0, 10);

	if(_seekiosVersion[1] < 100)
	{
		buff[0] = '0';
	}

	if(_seekiosVersion[1] >= 0 && _seekiosVersion[1] < 10)
	{
		buff[1] = '0';
	}

	char tempBuff[10];
	stringHelper_intToString(_seekiosVersion[1], tempBuff);
	strcat(buff, tempBuff);
}

static void addSeekiosVersionUpdatedMessage()
{
	OutputMessage seekiosVersionUpdatedMessage;
	seekiosVersionUpdatedMessage.messageType = MESSAGE_TYPE_SEEKIOS_VERSION_UPDATED;
	seekiosVersionUpdatedMessage.timestamp = RTCManager_getCurrentTimestamp();
	seekiosVersionUpdatedMessage.onSendSuccess = updateSeekiosVersionInExtFlash;
	messageSender_addToSendList(&seekiosVersionUpdatedMessage);
}

void seekiosInfoManager_updateSeekiosUID()
{
	writeUIDToFlashIfNotExisting();
	getSeekiosUIDFromExtFlash(_seekiosUID);
}

static void writeUIDToFlashIfNotExisting()
{
	bool uidInExtFlash = isSeekiosUIDInExtFlash();
	bool uidInMcuFlash = isSeekiosUIDInMCUFLash();
	
	if(!uidInExtFlash || !uidInMcuFlash){

		if(uidInExtFlash)
		{
			getSeekiosUIDFromExtFlash(_seekiosUID);
			writeSeekiosUIDToMCUFlash(_seekiosUID);
		}
		else
		{
			//Il faut allumer le gsm pour récupérer l'IMEI qui est à la base de la construction de l'uidseekios
			if(!GSMManager_isModuleStarted())
			{
				GSMManager_powerOnGSM();
			}
			computeSeekiosUID(_seekiosUID);
			writeSeekiosUIDToExtFlash(_seekiosUID);
			writeSeekiosUIDToMCUFlash(_seekiosUID);
		}
	}
}

static bool isSeekiosUIDInExtFlash()
{
	uint8_t UID[SEEKIOS_UID_SIZE];
	return getSeekiosUIDFromExtFlash(UID);
}

void seekiosInfoManager_getSeekiosUID(uint8_t buff[SEEKIOS_UID_SIZE]){
	strcpy((char*)buff, (char*)_seekiosUID);
}

static bool getSeekiosUIDFromExtFlash(uint8_t result[SEEKIOS_UID_SIZE]){
	char readBuf[20] = "";
	dataflashManager_readPage(PAGE_INDEX_SEEKOIS_ID,20,(unsigned char*)readBuf);
	char* UIDStart = strstr(readBuf,"UID:");
	if(UIDStart != NULL){
		char* UID = UIDStart+4; // We go past the "UID:" chars
		if(strlen(UID) <= 8){
			strcpy((unsigned char*)result, (unsigned char*)UID);
			return true;
		}
	}
	return false;
}

/* Builds the Seekios UID from the IMEI */
static uint8_t* computeSeekiosUID(uint8_t* result){
	uint8_t IMEI[32];
	
	if(GSMManager_getIMEI((char*)IMEI)){
		uint8_t code[16];
		buildUIDFromIMEI((char*)IMEI, (unsigned char*)code);
		strcpy((char*)result, (char*)code);
	}
	
	return result;
}

static char* buildUIDFromIMEI(char* IMEIstr, char* result){
	long long int IMEI = atoll(IMEIstr);
	stringHelper_convertToBaseX(IMEI ,result, BASE_75_STRING);
	return result;
}

/* Récupère le code IMEI, le transforme en code en base 75, et l'inscrit dans la mémoire flash
sous la forme :  [UID:XXXXXXXX]
Retourne true si succès, false sinon*/
static bool writeSeekiosUIDToExtFlash(uint8_t UID[SEEKIOS_UID_SIZE]){
	if(strlen((unsigned char*)UID) > 0){
		char dataToWrite[20] = "UID:";
		strcat(dataToWrite, (char*)UID);
		strcat(dataToWrite,"");
		dataflashManager_writeToPage(PAGE_INDEX_SEEKOIS_ID, strlen(dataToWrite)+1, dataToWrite);
		return true;
	}
	
	return false;
}

static void updateSeekiosVersionInExtFlash()
{
	char dataToWrite[10] = "VER:";
	char buff[10];
	seekiosInfoManager_seekiosVersionToString(buff);
	strcat(dataToWrite, buff);
	dataflashManager_writeToPage(PAGE_INDEX_SEEKIOS_VERSION, strlen(dataToWrite)+1, dataToWrite);
	USARTManager_printUsbWait("[Seekios Firmware Version Updated]\r\n");
}

static bool writeSeekiosUIDToMCUFlash(uint8_t UID[SEEKIOS_UID_SIZE]){
	nvm_erase_row((uint32_t)SEEKIOS_FLASH_DATA_START_ADDRESS);
	if(strlen((char*)UID) > 0){
		uint8_t dataToWrite[SEEKIOS_FLASH_DATA_SIZE] = "UID:";
		strcat((char*)dataToWrite, (unsigned char*)UID);
		strcat((char*)dataToWrite,"");
		nvm_write_buffer(SEEKIOS_FLASH_DATA_START_ADDRESS, dataToWrite, SEEKIOS_FLASH_DATA_SIZE);
		return true;
	}
	return false;
}

/* Updates the variable "SeekiosVersion" according to the data in MCU flash and external flash
Also updates the value in the external flash if the MCU Version is newer
Returns true if the version in the MCU flash is greater than the version in the external flash, false otherwise*/
bool seekiosInfoManager_checkIfSeekiosVersionNew(){
	volatile uint16_t extFlashVersion[2]={0,0};
	bool functionResult = false;
	if(getExtFlashSeekiosVersion(extFlashVersion))
	{
		if(_seekiosVersion[0] != extFlashVersion[0]
		|| (_seekiosVersion[0] == extFlashVersion[0] && _seekiosVersion[1] != extFlashVersion[1]))
		{
			functionResult = true;
		}
		else
		{
			functionResult = false;
		}
	}
	else
	{
		functionResult = true;
	}
	printSeekiosVersion();

	return functionResult;
}

static bool isSeekiosUIDInMCUFLash(){
	uint8_t page[SEEKIOS_FLASH_DATA_SIZE];
	memcpy(page, (void*)SEEKIOS_FLASH_DATA_START_ADDRESS, SEEKIOS_FLASH_DATA_SIZE);
	return strstr((char*)page, "UID:") != NULL;
}

static bool getExtFlashSeekiosVersion(uint16_t result[2]){
	uint8_t readBuf[20] = {0};
	dataflashManager_readPage(PAGE_INDEX_SEEKIOS_VERSION,20,readBuf);
	char* VERStart = strstr((char*)readBuf,"VER:");
	if(VERStart != NULL){
		VERStart = VERStart+4; // We go past the "VER:" chars
		char* token = strtok(VERStart,".");
		if(token != NULL)
		{
			result[0] = atoi(token);
			token = strtok(NULL, ".");
			if(token != NULL)
			{
				result[1] = atoi(token);
				return true;
			}
		}
	}
	return false;
}

/* Checks in the ext flash if this is the first run of the seekios or not by looking for "NOT_FIRST_RUN" in it.
Returns true if it doesn't find it, false otherwise*/
bool seekiosInfoManager_isSeekiosFirstRun(){
	char readBuf[20] = "";
	dataflashManager_readPage(PAGE_INDEX_FIRST_RUN,20,(unsigned char*)readBuf);
	return strstr(readBuf,"NOT_FIRST_RUN") == NULL;
}

/* Writes in flash at the right page "NOT_FIRST_RUN"  */
void seekiosInfoManager_setSeekiosNotFirstRun()
{
	dataflashManager_writeToPage(PAGE_INDEX_FIRST_RUN,strlen("NOT_FIRST_RUN")+1, "NOT_FIRST_RUN");
}

static void printSeekiosVersion(){
	char buff[10];
	USARTManager_printUsbWait("Seekios firmware version : ");
	seekiosInfoManager_seekiosVersionToString(buff);
	USARTManager_printUsbWait(buff);
	USARTManager_printUsbWait("\r\n");
}
