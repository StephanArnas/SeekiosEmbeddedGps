/*
 * menu_sgs.h
 *
 * Created: 22.04.2016 16:18:39
 *  Author: Marc Adolf
 */ 


#ifndef MENU_SGS_H_
#define MENU_SGS_H_

//#define MENU_HEADER "testheader\0"


 
#include "core_cm0plus.h" 


#define GSM_MENU_HEADER "\n\r\n\r" \
"-- press a key to change the function mode --\n\r" \
"-- s: start modem                          --\n\r" \
"-- e: stop modem                           --\n\r" \
"-- g: get status                           --\n\r" \
"-- c: call a number                        --\n\r" \
"-- i: get IMEI                             --\n\r" \
"-- h: hibernate modem                      --\n\r" \
"-- m: show this menu                       --\n\r" \
"-- b: back to main menu                    --\n\r"


#define GPS_MENU_HEADER "\n\r\n\r" \
"-- press a key to change the function mode --\n\r" \
"-- s: start GPS receiver                   --\n\r" \
"-- e: stop GPS receiver                    --\n\r" \
"-- l: low power mode starting              --\n\r" \
"-- d: debug modem output                   --\n\r" \
"-- c: calculate actual position            --\n\r" \
"-- g: genenerate google-URL                --\n\r" \
"-- m: show this menu                       --\n\r" \
"-- b: back to main menu                    --\n\r"



#define SPI_MENU_HEADER "\n\r\n\r" \
"-- press a key to change the function mode --\n\r" \
"-- 1: toggle IMU CS                        --\n\r" \
"-- 2: toggle Dataflash CS                  --\n\r" \
"-- 3: info Dataflash                       --\n\r" \
"-- 4: status IMU                           --\n\r" \
"-- 5: IMU set single tap int.              --\n\r" \
"-- 6: IMU set double tap2 int.             --\n\r" \
"-- 7: IMU set motion interrupt             --\n\r" \
"-- 8: IMU reset interrupt-settings         --\n\r" \
"-- 9: toggle sleep of spi-modules          --\n\r" \
"-- s: Get Sleep value						--\n\r" \
"-- t: Toggle Sleep value					--\n\r" \
"-- m: show this menu                       --\n\r" \
"-- b: back to main menu                    --\n\r"

#define MENU_HEADER "\n\r\n\r" \
"-- press a key to change the function mode --\n\r" \
"-- r: Reset controller                     --\n\r" \
"-- d: toggle Satelite-Receiver on/off      --\n\r" \
"-- g: GSM Modem Menu                       --\n\r" \
"-- h: hibernate                            --\n\r" \
"-- s: SPI  Menu                            --\n\r" \
"-- m: show this menu                       --\n\r" \
"-- q: Quit Configuration                   --\n\r" 

/*void display_menu(void);*/
void main_menu(void);
void hardware_menu(void);
void display_learn_menu(void);
void learn_menu(void);
void calibrate_hall_sensors(void);
void debug_hall_sensors(void);
void debug_midi_values(void);
int learn_flaps(void);
int learn_flaps2(void);
int learn_threshold(void);






#endif /* MENU_SGS_H_ */