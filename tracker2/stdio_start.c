/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */

#include <sgs/port_sgs.h>
#include "stdio_start.h"

void STDIO_REDIRECT_0_init(void)
{
	usart_sync_enable(&CONSOLE_UART);
	stdio_io_init(&CONSOLE_UART.io);
}

void STDIO_REDIRECT_0_example(void)
{
	/* Print welcom message */
	printf("\r\nHello ATMEL World!\r\n");
}

