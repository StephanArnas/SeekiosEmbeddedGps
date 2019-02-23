/*
 * powersaving_sgs.h
 *
 * Created: 20.06.2016 12:11:38
 *  Author: Marc Adolf
 */ 


#ifndef POWERSAVING_SGS_H_
#define POWERSAVING_SGS_H_

#include "helper_sgs.h"
#include "port_sgs.h"
#include "main.h"
#include "serial_sgs.h"
#include <seekiosCore/seekios.h>
#include <peripheralManager/WDT_manager.h>

void sleep_deep(void);
void sleep_normal(void);

#endif /* POWERSAVING_SGS_H_ */