#include <authentication/authentication.h>

static time_t _lastAuthTimestamp;
static bool computeSha1Seekios(uint8_t sha1HashSeekios[SHA1_HASH_SIZE],time_t timestamp);

void authentication_init(){
	_lastAuthTimestamp = 0;
}

/* To authenticate the user, we get its message composed with a crypt(timestamp+idSeekios+salt) + timestamp */
/* If we are able to reproduce crypt(timestamp+idSeekios+salt) And if timestamp > last timestamp received, the authentication is OK */
bool authentication_authenticateUser(uint8_t* key){
	/* Key :
	- 4 Bytes timestamp
	- 20 Bytes Hash SHA1
	*/
	time_t timestamp;
	memcpy(&timestamp, (void*)key, sizeof(time_t));
	uint8_t sha1HashUser[SHA1_HASH_SIZE];
	memcpy(sha1HashUser, key+4, SHA1_HASH_SIZE);
	if(timestamp <= _lastAuthTimestamp)
	{
		return false;
	}

	uint8_t sha1HashSeekios[SHA1_HASH_SIZE];
	if(!computeSha1Seekios(sha1HashSeekios, timestamp))
	{
		return false;
	}
	bool authenticationSuccessful = memcmp(sha1HashSeekios, sha1HashUser, SHA1_HASH_SIZE) == 0;
	if(authenticationSuccessful)
	{
		_lastAuthTimestamp = timestamp;
	}
	return authenticationSuccessful;
}

static bool computeSha1Seekios(uint8_t sha1HashSeekios[SHA1_HASH_SIZE],time_t timestamp)
{
	/* Timestamp size = 9, uidSeekios = 8, salt = ??? */
	uint8_t toCrypt[17 + SALT_SIZE] = {0};
	uint8_t timestampStr[12];
	stringHelper_intToString(timestamp, timestampStr);
	uint8_t seekiosId[11];

	seekiosInfoManager_getSeekiosUID(seekiosId);
	strcpy((char*)toCrypt, (char*)timestampStr);
	strcat((char*)toCrypt, (char*)seekiosId);
	strcat((char*)toCrypt, SALT);
	if(cryptTools_buildDigest(toCrypt, sha1HashSeekios) == FUNCTION_SUCCESS){
		return true;
	}
	return false;
}