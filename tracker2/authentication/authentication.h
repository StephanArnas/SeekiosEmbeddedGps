#ifndef AUTHENTICATION_H_
#define AUTHENTICATION_H_

#include <tools/crypt_tools.h>
#include <tools/string_helper.h>
#include <time.h>
#include <stdbool.h>
#include <seekiosManager/seekios_info_manager.h>

#define SALT "USr&0F!kFKt*OXC$3U)Nm0g@vdf4mKgl744rgrZoQf&4Tyw@cskkQ7bL!&7-2!zM" // TODO : put true salt value
#define SALT_SIZE 64

void authentication_init();
bool authentication_authenticateUser(uint8_t* key);

#endif /* AUTHENTICATION_H_ */