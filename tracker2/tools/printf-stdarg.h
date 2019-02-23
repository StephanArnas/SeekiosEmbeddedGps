/*
 * printf_stdarg.h
 *
 * Created: 19/09/2016 11:28:26
 *  Author: Luc
 */ 


#ifndef PRINTF_STDARG_H_
#define PRINTF_STDARG_H_


#include <stdarg.h>
#include <stdio.h>

int printf(const char *format, ...);
int printf_custom(const char *format, va_list args);
int sprintf(char *out, const char *format, ...);
int sprintf_custom(char* out, const char *format, va_list args);
int snprintf( char *buf, unsigned int count, const char *format, ... );

#endif /* PRINTF-STDARG_H_ */