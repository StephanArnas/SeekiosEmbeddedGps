/*
 * TRNG_Manager.c
 *
 * Created: 25/08/2016 14:25:30
 *  Author: Luc
 */ 

 #include <peripheralManager/TRNG_Manager.h>

 void TRNGManager_init(){
	TRNG->CTRLA.bit.ENABLE = 0;
	TRNG->CTRLA.bit.RUNSTDBY = 0;
	TRNG->CTRLA.bit.ENABLE = 1;
	DELAY_MS(10);
 }

 void TRNGManager_testTRNG(){
	for(uint8_t i = 0; i <  20; i++)
	{
		char buff[20];
		stringHelper_intToString(TRNGManager_getTRN(), buff);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait("\r\n");
	}
 }

 int TRNGManager_getTRN(){
	int result = TRNG->DATA.reg;
	DELAY_MS(10);
	return result;
 }

