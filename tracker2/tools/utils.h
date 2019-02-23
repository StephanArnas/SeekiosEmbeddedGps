/*
 * utils.h
 *
 * Created: 15/04/2016 17:29:16
 *  Author: Luc
 */ 


#ifndef UTILS_H_
#define UTILS_H_


#include <seekiosCore/seekios.h>

int utils_repeatFunctionUntilSuccess(int (*callback)(void), int maxTries);
void utils_sortUint16ArrayAsc(uint16_t array[], uint8_t arraySize);


#endif /* UTILS_H_ */