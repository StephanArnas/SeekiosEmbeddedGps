#ifndef CRYPT_TOOLS_H_
#define CRYPT_TOOLS_H_

#include <thirdparty/crypt/sha1.h>
#include <seekiosCore/seekios.h>
#include <string.h>

uint8_t cryptTools_buildDigest(uint8_t* input, uint8_t* digest);

#endif /* CRYPT_TOOLS_H_ */