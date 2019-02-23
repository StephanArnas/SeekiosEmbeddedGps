/*
 * TRNG_Manager.h
 *
 * Created: 25/08/2016 14:25:54
 *  Author: Luc
 */ 


#ifndef TRNG_MANAGER_H_
#define TRNG_MANAGER_H_

#include <saml21j18b.h>
#include <seekiosCore/seekios.h>
#include <tools/string_helper.h>
#include <peripheralManager/USART_manager.h>

 void TRNGManager_init(void);
 void TRNGManager_testTRNG(void);
 int TRNGManager_getTRN(void);

#endif /* TRNG_MANAGER_H_ */