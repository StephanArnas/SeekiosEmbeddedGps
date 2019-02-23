#include <tools/crypt_tools.h>



uint8_t cryptTools_buildDigest(uint8_t* input, uint8_t* digest){

	SHA1Context context;

	if(SHA1Reset(&context) != 0){
		return FUNCTION_FAILURE;
	}
	if(SHA1Input(&context, input, strlen(input)) != 0)
	{
		return FUNCTION_FAILURE;
	}
	if(SHA1Result(&context, digest) != 0)
	{
		return FUNCTION_FAILURE;
	}

	return FUNCTION_SUCCESS;
}