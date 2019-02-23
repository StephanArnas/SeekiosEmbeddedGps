/*
 * bma222_sgs.c
 *
 * Created: 30.05.2016 17:36:57
 *  Author: Marc Adolf
 */ 

// 1 - on
// 0 - off
#include "bma222_sgs.h"


uint8_t bma222_reg_overwrite(char reg, uint8_t val);

uint8_t bma222_cs(uint8_t mode)
{
  gpio_set_pin_level(IMU_CS,!mode);	
  return 1;	
}

// mode 1 Stream (0b10)
uint8_t bma222_set_fifo(uint8_t mode)
{
	
	bma222_reg_overwrite(0x3E, (mode <<  6) );
}

uint8_t bma222_shadow_disable(void)
{
	 bma222_cs(1);
	 delay_ms(1);
	 bma222_data[0]= 0x13;
	 bma222_data[1]= 0b01000000;
	 io_write(spi_io, bma222_data, 2);
	 delay_ms(1);
	 bma222_cs(0);
	 return 1;
}

// uint8_t bma222_set_fifo(void)
// {
// 	
// 	 bma222_reg_overwrite(0x3E, 0b10000000);
// 	
// 	 return 1;
// }


//filling complete 64Byte state into array bma222_data
uint8_t bma222_get_state(void)
{
	//bma222_shadow_disable();
	 bma222_cs(1);
	 delay_ms(1);
	 bma222_data[0]= 0x80;
	 io_write(spi_io, bma222_data, 1);
	 io_read(spi_io, bma222_data, 128);
	 delay_ms(3);
	 bma222_cs(0);
	 return 1;
}




char bma222_get_id(void)
{
	bma222_get_state();
	return bma222_data[0];
}

uint8_t bma222_get_acceleration(char *acc)
{
	bma222_get_state();
 	acc[0] = bma222_data[3];
 	acc[1] = bma222_data[5];
 	acc[2] = bma222_data[7];
	acc[3] = 0;
}

uint8_t bma222_get_temperature(void)
{
	delay_ms(3);
	bma222_get_state();
	uint8_t a = bma222_data[8];
	return ((uint8_t)(bma222_data[8]+46)/2);
}

uint8_t bma222_reg_overwrite(char reg, uint8_t val)
{
	bma222_cs(1);
	delay_ms(1);
	bma222_data[0]= reg;  //register address
	bma222_data[1]= val;
	io_write(spi_io, bma222_data, 2);
	delay_ms(1);
	bma222_cs(0);
	return 1;	
}

uint8_t bma222_reg_write(char reg, char val)
{
	bma222_cs(1);
	delay_ms(1);
	bma222_data[0]= reg;  //register address
	bma222_data[1]= val | bma222_data[(uint8_t)reg];
	io_write(spi_io, bma222_data, 2);
	delay_ms(1);
	bma222_cs(0);
	return 1;	
}

uint8_t bma222_reset_tap_interrupts(void)
{
	bma222_get_state();
	
	bma222_reg_overwrite(0x1E, 0b00000000);
	bma222_reg_overwrite(0x1B, 0b00000000);
	bma222_reg_overwrite(0x18, 0b00000000);
	bma222_reg_overwrite(0x19, 0b00000000);
	bma222_reg_overwrite(0x16, 0b00000000);
	bma222_reg_overwrite(0x27, 0b00000000);
	bma222_reg_overwrite(0x28, 0b00000000);
	bma222_reg_overwrite(0x29, 0b00000000);
}

uint8_t bma222_set_tap_interrupt1(void)
{
	bma222_get_state();
	
	bma222_reg_write(0x1E, 0b00010000);
	bma222_reg_write(0x1B, 0b00000000);
	bma222_reg_write(0x19, 0b00100000);
	bma222_reg_write(0x16, 0b00100000);
	bma222_reg_write(0x27, 0b00100000);
	bma222_reg_write(0x28, 0b00100000);
    bma222_reg_write(0x29, 0b00100000);
	bma222_get_state();
//  bma222_cs(1);
//  delay_ms(1);
//  //enable interrupt source: tap
//  bma222_data[0]= 0x1E;  //register address
//  bma222_data[1]|= 0b00010000;
//  io_write(spi_io, bma222_data, 2);
//  delay_ms(1);
//  bma222_cs(0);
//  //set tap to double tap (0b00010000)
//  //set tap to single tap (0b00100000)
//  delay_ms(1);
//  bma222_data[0]= 0x19;  //register address
//  bma222_data[1]|= 0b00100000;  //double
//  send_debug(" set Tap interrupt\n\r");
//  delay_ms(1);
//  io_write(spi_io, bma222_data, 2);
//  delay_ms(1);
//  bma222_cs(0);
//  delay_ms(1);
//  bma222_cs(1);
//  delay_ms(1);
//  // tap-enable x16
//  bma222_data[0]= 0x16;  //register address
//  bma222_data[1]= 0b00100000; // single tap enable
// // bma222_data[1]|= 0b00010000; // double tap enable
//  io_write(spi_io, bma222_data, 2);
//  bma222_cs(0);
}


                                  //(uint_8 *dur, uint_8 sensitivity)   
uint8_t bma222_set_motion_interrupt1(void)
{
	bma222_get_state();
	bma222_reg_write(0x1E, 0b00001000); //enable source slo_mo
    bma222_reg_write(0x28, 0b00101011); //slope threshold
    bma222_reg_write(0x29, 0b00100000); //slo_no_mot_threshold -> 32 * 15mg = 480mg  sensitivity of detection
	
	bma222_reg_write(0x27, 0b11111100); //slomo duration 4 sec  continuous movement
	bma222_reg_write(0x18, 0b00000111); //activate interrupt slo_mo on channel x,y,z
	bma222_reg_write(0x19, 0b00001000); //int1 slo_mo activate
	bma222_reg_overwrite(0x10, 0b00001000); // set Bandwidth    1000 = 64ms  1001 = 32ms 1010 = 16ms ...  1111 = 0,5ms
	bma222_reg_overwrite(0x21, 0b00000010); // latch Interrupt    1110 = 50ms  0010 = 500 ms 
	
	bma222_get_state();
}


uint8_t bma222_set_motion_interrupt(uint8_t duration, uint8_t sensitivity)
{
	bma222_get_state();
	bma222_reg_write(0x1E, 0b00001000); //enable source slo_mo
	bma222_reg_write(0x28, 0b00001011); //slope threshold
	bma222_reg_write(0x29, 0b00001011); //slo-no-mot-th  sensitivity of detection
	
	bma222_reg_write(0x27, 0b11111111); //slomo duration 4 sec  continuous movement
	bma222_reg_write(0x18, 0b00000111); //activate interrupt slo_mo on channel x,y,z
	bma222_reg_write(0x19, 0b00001000); //int1 slo_mo activate
	bma222_reg_overwrite(0x10, 0b00001000); // set Bandwidth    1000 = 64ms  1001 = 32ms 1010 = 16ms ...  1111 = 0,5ms
	bma222_reg_overwrite(0x21, 0b00000010); // latch Interrupt    1110 = 50ms  0010 = 500 ms 
	bma222_get_state();
}


uint8_t bma222_set_tap_interrupt2(void)
{
		bma222_get_state();
	
	bma222_reg_write(0x1E, 0b00010000); //enable source tap
	bma222_reg_write(0x1B, 0b00010000); //activate interrupt double tap
	bma222_reg_write(0x16, 0b00010000); //double tap enable
	bma222_get_state();
// 	bma222_cs(1);
// 	delay_ms(1);
// 	//enable interrupt source: tap
// 	bma222_data[0]= 0x1E;  //register address
// 	bma222_data[1]|= 0b00010000;
// 	io_write(spi_io, bma222_data, 2);
// 	delay_ms(1);
// 	bma222_cs(0);
// 	//set tap to double tap (0b00010000)
// 	//set tap to single tap (0b00100000)
// 	delay_ms(1);
// 	bma222_cs(1);
// 	delay_ms(1);
// 	bma222_data[0]= 0x1B;  //register address
// 	bma222_data[1]= 0b00010000;  //double
// 	send_debug(" set Tap interrupt\n\r");
// 	delay_ms(1);
// 	io_write(spi_io, bma222_data, 2);
// 	delay_ms(1);
// 	bma222_cs(0);
// 	delay_ms(1);
// 	bma222_cs(1);
// 	delay_ms(1);
// 	// tap-enable x16
// 	bma222_data[0]= 0x16;  //register address
// 	//bma222_data[1]= 0b00100000; // single tap enable
// 	bma222_data[1]|= 0b00010000; // double tap enable
// 	io_write(spi_io, bma222_data, 2);
// 	bma222_cs(0);
}
