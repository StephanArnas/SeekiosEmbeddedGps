/*
* string_helper.c
*
* Created: 10/03/2016 15:04:14
*  Author: Nico
*/

#include <tools/string_helper.h>

static void inplace_reverse(char * str);

/*
Renvoie le premier morceau, découpé par le separator, de la trame passée en paramètre.
Si trame est NULL, renvoie le prochain morceau découpé par séparator
diff avec strtok de string.h : le séparateur peut être plusieurs caractères consécutifs
*/
char* stringHelper_strtok(char *trame, const char* separator, char** arbitraryNextPart)
{
	static char* nextPartOfString;
	if(trame != NULL)	nextPartOfString = trame;
	if(arbitraryNextPart != NULL)	if(*arbitraryNextPart != NULL)	nextPartOfString = *arbitraryNextPart;
	char* toReturn = nextPartOfString;

	//On récupère le pointeur sur le prochain séparateur
	if(nextPartOfString != NULL)	nextPartOfString = strstr(nextPartOfString, separator);
	//S'il existe
	if(nextPartOfString != NULL){
		//On remplace le premier carac du séparateur par un \0 pour clore la chaine
		nextPartOfString[0] = '\0';
		//On avance le pointeur du prochain morceau de chaine au premier caractère qui suis le séparateur
		nextPartOfString = nextPartOfString + strlen(separator);
	}
	if(arbitraryNextPart != NULL)	*arbitraryNextPart = nextPartOfString;

	//On retourne le morceau de string qu'on vient de passer
	return toReturn;
}

/* Returns the strlen, but stops to check for the \0 if it reaches
char at bound index NOT included */
uint32_t stringHelper_boundedStrlen(char* string, uint32_t bound)
{
	uint32_t i=0;
	for(i=0;i<bound;i++)
	{
		if(string[i]==0)
		{
			return i; // not include the 0 in the length
		}
	}
	return bound-1;
}

/*
Compte les occurences de charToCount dans la chaine de caractère string
*/
int stringHelper_occursCount(const char* string, const char* charsToCount)
{
	int count = 0;
	const char *tmp = string;
	while(tmp = strstr(tmp, charsToCount))
	{
		count++;
		tmp++;
	}
	return count;
}

/*
vérifie si la chaine "string" commence par la chaine "toCompare"
*/
int stringHelper_startWith(const char* string, const char* toCompare){
	size_t lenpre = strlen(toCompare),
	lenstr = strlen(string);
	return lenstr < lenpre ? 0 : strncmp(toCompare, string, lenpre) == 0;
}

/*
Transforme le double passé en paramètre en string avec nbDecimal chiffres après la virgule, et le stocke dans resultBuff
Le pointeur retourné est aussi le pointeur de resultBuff

*/
char* stringHelper_doubleToString(double val, char* resultBuff, int nbDecimal){
	int nbNum = stringHelper_getCountOfNumBeforeDecimalPoint(val);
	
	uint8_t isNegative = 0;
	if(val < 0) isNegative = 1;
	
	//On alloue 1 si négatif pour '-' + nbChiffresAvantLaVirgule + 1 pour ',' + nbDecimales + 1 pour '\0'
	int countOfChar = (nbNum + nbDecimal + 2) + isNegative;
	int numToPrint = 0;
	double tempNumber = val;
	
	if(isNegative) {
		resultBuff[0] = '-';
		tempNumber = tempNumber * -1;
	}
	
	for (int i = isNegative; i < countOfChar - 1; i++)
	{
		//Avant la virgule
		if(i < nbNum + isNegative){
			numToPrint = floor(tempNumber / exp10((nbNum + isNegative) - i - 1));
			tempNumber = tempNumber - numToPrint * exp10((nbNum + isNegative) - i - 1);
			resultBuff[i] = numToPrint + '0';
			continue;
		}
		//Après la virgule
		if(i > nbNum + isNegative){
			numToPrint = floor(tempNumber * 10);
			tempNumber = tempNumber * 10 - numToPrint;
			resultBuff[i] = numToPrint + '0';
			continue;
		}
		//Au niveau de la virgule
		if(i == nbNum + isNegative) { resultBuff[i] = '.'; continue; }
	}
	resultBuff[countOfChar - 1] = '\0';
	
	return resultBuff;
}

/*
Transforme l'entier passé en paramètre en string, et le stocke dans resultBuff
Le pointeur retourné est aussi le pointeur de resultBuff
*/
/*char* stringHelper_intToString(int val, char* resultBuff){
	int nbNum = stringHelper_getCountOfNumBeforeDecimalPoint(val);
	
	int isNegative = 0;
	if(val < 0) isNegative = 1;
	
	//On alloue 1 si négatif pour '-' + nbChiffresAvantLaVirgule + nbDecimales + 1 pour '\0'
	int countOfChar = (nbNum + 1) + isNegative;
	int numToPrint = 0;
	int tempNumber = val;
	
	if(isNegative) {
		resultBuff[0] = '-';
		tempNumber = tempNumber * -1;
	}
	
	for (int i = isNegative; i < countOfChar - 1; i++)
	{
		//Avant la virgule
		if(i < nbNum + isNegative){
			numToPrint = floor(tempNumber / exp10((nbNum + isNegative) - i - 1));
			tempNumber = tempNumber - numToPrint * exp10((nbNum + isNegative) - i - 1);
			resultBuff[i] = numToPrint + '0';
			continue;
		}
	}
	resultBuff[countOfChar - 1] = '\0';
	
	return resultBuff;
}*/


char* stringHelper_intToString(int i, uint8_t b[]){
	char const digit[] = "0123456789";
	uint8_t* p = b;
	if(i<0){
		*p++ = '-';
		i *= -1;
	}
	int shifter = i;
	do{ //Move to where representation ends
		++p;
		shifter = shifter/10;
	}while(shifter);
	*p = '\0';
	do{ //Move back, inserting digits as u go
		*--p = digit[i%10];
		i = i/10;
	}while(i);
	return b;
}

uint8_t* stringHelper_byteArrayToHexString(uint8_t bytes[], uint16_t nbBytes, uint8_t string[])
{
	char const digit[] = "0123456789ABCDEF";
	for(uint16_t i=0; i<nbBytes;i++)
	{
		string[2*i]	= digit[(bytes[i] >> 4) & 0b00001111];
		string[2*i+1] = digit[bytes[i] & 0b00001111];
	}
	string[2*nbBytes+1]=0;

	return string;
}

char* stringHelper_byteToHexString(uint8_t byte, uint8_t b[]){
	char const digit[] = "0123456789ABCDEF";
	b[0] = '0';
	b[1] = 'x';
	b[2] = digit[(byte >> 4) & 0b00001111];
	b[3] = digit[byte & 0b00001111];
	b[4] = 0;
	return b;
}

char* stringHelper_longToString(long i, uint8_t b[]){
	char const digit[] = "0123456789";
	uint8_t* p = b;
	if(i<0){
		*p++ = '-';
		i *= -1;
	}
	long shifter = i;
	do{ //Move to where representation ends
		++p;
		shifter = shifter/10;
	}while(shifter);
	*p = '\0';
	do{ //Move back, inserting digits as u go
		*--p = digit[i%10];
		i = i/10;
	}while(i);
	return b;
}

/*
Donne le nombre de chiffre avant la virgule du chiffre passé en paramètre
Ex : 758.654 => 3 ; 7.586869 => 1
*/
int stringHelper_getCountOfNumBeforeDecimalPoint(const double val){
	int result = 1;
	double temp = val;
	if(val < 0) temp = temp * -1;
	while(temp > 10){
		temp = temp / 10;
		result ++;
	}
	return result;
}

/* Inverse une chaine de caractère se finissant par le caractère de terminaison '\0' */
static void inplace_reverse(char * str)
{
	if (str)
	{
		char * end = str + strlen(str) - 1;

		// swap the values in the two given variables
		// XXX: fails when a and b refer to same memory location
		#   define XOR_SWAP(a,b) do\
		{\
			a ^= b;\
			b ^= a;\
			a ^= b;\
		} while (0)

		// walk inwards from both ends of the string,
		// swapping until we get to the middle
		while (str < end)
		{
			XOR_SWAP(*str, *end);
			str++;
			end--;
		}
		#   undef XOR_SWAP
	}
}

/* Convertit un nombre entier en une string dans une base déterminée par baseString
par exemple pour convertir en une string en base 16, on aura baseString = "0123456789"*/
char* stringHelper_convertToBaseX(long long int toConvert, char* rxBuffer, char* baseString){
	int baseSize = strlen(baseString);

	char convertedString[100];
	int i = 0;
	long long int rest = 0;
	long long int toDivide = toConvert;
	do{
		long long int result = toDivide/baseSize;
		rest = toDivide%baseSize;
		convertedString[i] = baseString[rest];
		toDivide = result;
		i++;
	}while(toDivide != 0);
	convertedString[i] = 0; // null termination. Le dernier char est celui du rest = 0, il faut le remplacer.
	inplace_reverse(convertedString);
	strcpy(rxBuffer, convertedString);
	return rxBuffer;
}

/* An implementation of strtok that returns the next occurence of the character in delims
ext : strtok on a,b,,c will see a b and c
strtok_single on a,b,,c will see a b <empty> and c*/
char *strtok_single (char * str, char const * delims)
{
	static char  * src = NULL;
	char  *  p,  * ret = 0;

	if (str != NULL)
	src = str;

	if (src == NULL)
	return NULL;

	if ((p = strpbrk (src, delims)) != NULL) {
		*p  = 0;
		ret = src;
		src = ++p;

		} else if (*src) {
		ret = src;
		src = NULL;
	}

	return ret;
}