/*
 * menu_sgs.h
 *
 * Created: 22.04.2016 16:18:39
 *  Author: Marc Adolf
 */ 


#ifndef MENU_SGS_H_
#define MENU_SGS_H_

//#define MENU_HEADER "testheader\0"



 


#define GSM_MENU_HEADER "\n\r" \
"-- press a key to change the function mode --\n\r" \
"-- s: start modem                          --\n\r" \
"-- e: stop modem                           --\n\r" \
"-- g: get status                           --\n\r" \
"-- c: call a number                        --\n\r" \
"-- d: do SMS                               --\n\r" \
"-- r: repeat test-cycle                    --\n\r" \
"-- h: hibernate modem                      --\n\r" \
"-- m: show this menu                       --\n\r" \
"-- b: back to main menu                    --\n\r"


#define GPS_MENU_HEADER "\n\r" \
"-- press a key to change the function mode --\n\r" \
"-- s: start GPS receiver                   --\n\r" \
"-- e: stop GPS receiver                    --\n\r" \
"-- l: low power mode starting              --\n\r" \
"-- d: debug modem output                   --\n\r" \
"-- c: calculate actual position            --\n\r" \
"-- g: genenerate google-URL                --\n\r" \
"-- m: show this menu                       --\n\r" \
"-- b: back to main menu                    --\n\r"


#define HARDWARE_MENU_HEADER "\n\r" \
"-- press a key to change the function mode --\n\r" \
"-- s: Show status                          --\n\r" \
"-- e: Power status                         --\n\r" \
"-- m: show this menu                       --\n\r" \
"-- b: back to main menu                    --\n\r"


#define SPI_MENU_HEADER "\n\r" \
"-- press a key to change the function mode --\n\r" \
"-- 1: toggle IMU CS                        --\n\r" \
"-- 2: toggle Dataflash CS                  --\n\r" \
"-- 3: read serial number Dataflash         --\n\r" \
"-- 4: read Status IMU                      --\n\r" \
"-- 5: IMU Tap set                          --\n\r" \
"-- m: show this menu                       --\n\r" \
"-- b: back to main menu                    --\n\r"

#define MENU_HEADER "\n\r" \
"-- press a key to change the function mode --\n\r" \
"-- r: Reset controller                     --\n\r" \
"-- g: GSM Modem  menu                      --\n\r" \
"-- d: toggle Satelite-Receiver on/off      --\n\r" \
"-- s: SPI Menu                             --\n\r" \
"-- h: Hardware menu                        --\n\r" \
"-- m: show this menu                       --\n\r" \
"-- q: Quit Configuration                   --\n\r"

volatile char testData[64];

/*void display_menu(void);*/
void main_menu(void);
void display_learn_menu(void);
void learn_menu(void);
void calibrate_hall_sensors(void);
void debug_hall_sensors(void);
void debug_midi_values(void);
int learn_flaps(void);
int learn_flaps2(void);
int learn_threshold(void);






#endif /* MENU_SGS_H_ */