/*
 * string_helper.h
 *
 * Created: 10/03/2016 15:05:06
 *  Author: Nico
 */ 


#ifndef STRING_HELPER_H_
#define STRING_HELPER_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/* WARNING : /+* dangerous values */
#define BASE_75_STRING "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ[]^-()@!$_={}"
#define BASE_16_STRING "0123456789ABCDEF"

/*
Renvoie le premier morceau, découpé par le separator, de la trame passée en paramètre.
Si trame est NULL, renvoie le prochain morceau découpé par séparator
diff avec strtok de string.h : le séparateur peut être plusieurs caractères consécutifs
*/
char* stringHelper_strtok(char *trame, const char* separator, char** arbitraryNextPart);

/*
Compte les occurences de charToCount dans la chaine de caractère string
*/
int stringHelper_occursCount(const char* string, const char* charToCount);

/*
vérifie si la chaine "string" commence par la chaine "toCompare"
*/
int stringHelper_startWith(const char* string, const char* toCompare);

/*
Transforme le double passé en paramètre en string avec nbDecimal chiffres après la virgule
*/
char* stringHelper_doubleToString(double val, char*, int nbDecimal);

/*
Transforme l'entier passé en paramètre en string
*/
char* stringHelper_intToString(int val, uint8_t resultBuff[]);


char* stringHelper_longToString(long i, uint8_t b[]);

/*
Donne le nombre de chiffre avant la virgule du chiffre passé en paramètre
Ex : 758.654 => 3 ; 7.586869 => 1
*/
int stringHelper_getCountOfNumBeforeDecimalPoint(const double val);

char* stringHelper_convertToBaseX(long long int toConvert, char* rxBuffer, char* baseString);

uint32_t stringHelper_boundedStrlen(char* string, uint32_t bound);

uint8_t* stringHelper_byteArrayToHexString(uint8_t bytes[], uint16_t nbBytes, uint8_t string[]);

char *strtok_single (char * str, char const * delims);

char* stringHelper_byteToHexString(uint8_t byte, uint8_t b[]);

#endif /* STRING_HELPER_H_ */