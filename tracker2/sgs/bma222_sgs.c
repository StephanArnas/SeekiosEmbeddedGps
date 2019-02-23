/*
 * bma222_sgs.c
 *
 * Created: 30.05.2016 17:36:57
 *  Author: Marc Adolf
 */ 

// 1 - on
// 0 - off
#include "bma222_sgs.h"


uint8_t bma222_reg_read(char reg)
{
	uint8_t tmp_reg;
	bma222_cs(1);
	DELAY_MS(2);
	tmp_reg = reg|0x80;  //register address plus read-bit
	io_write(spi_io, &tmp_reg, 1);
 	DELAY_MS(1);
 	io_read(spi_io,  &tmp_reg, 1);
 	DELAY_MS(1);
	bma222_cs(0);
	return tmp_reg;
}



uint8_t bma222_reg_overwrite(char reg, uint8_t val);

uint8_t bma222_cs(uint8_t mode)
{
  gpio_set_pin_level(IMU_CS,!mode);	
  return 1;	
}

// mode 1 Stream (0b10)
void bma222_set_fifo(uint8_t mode)
{
	bma222_reg_overwrite(0x3E, (mode <<  6) );
}

uint8_t bma222_shadow_disable(void)
{
	 bma222_cs(1);
	 DELAY_MS(1);
	 bma222_data[0]= 0x13;
	 bma222_data[1]= 0b01000000;
	 io_write(spi_io, bma222_data, 2);
	 DELAY_MS(1);
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
	 DELAY_MS(1);
	 bma222_data[0]= 0x80;
	 io_write(spi_io, bma222_data, 1);
	 io_read(spi_io, bma222_data, 128);
	 DELAY_MS(3);
	 bma222_cs(0);
	 return 1;
}




	
	
char bma222_set_bandwith(uint8_t bw)
{	
	uint8_t bw_regval;
	bw_regval = bw & 0x0F;
	bma222_reg_overwrite(0x10, bw_regval);
	bw_regval= bma222_reg_read(0x10);
	return bw_regval;
	
}
	
	
char bma222_set_sleep_duration(uint8_t sleepduration)
{
	uint8_t reg_val;
	reg_val= bma222_reg_read(0x11);
	reg_val= reg_val & 0b11100000;                 //safe only the upper bits of the register 
	sleepduration = sleepduration & 0b00001111;
	reg_val= reg_val | (sleepduration << 1);	   //4 bit sleepduration shifted 1 bit to the left
	                 //Register   sleepduration
	bma222_reg_overwrite(0x11, reg_val);
	reg_val= 0xFF;
	reg_val= bma222_reg_read(0x11);
	return reg_val;
}	
	
char bma222_set_sleepmode(uint8_t sleepmode)
{
		uint8_t reg_val;
		reg_val= bma222_reg_read(0x11) & 0b00011111;
		if (sleepmode == 0)
		{
			send_debug("Setting BMA to normal mode.\r\n");
			reg_val= reg_val | 0x00;   //0x00 normal mode
		}
		if (sleepmode == 1)
		{
			send_debug("Setting BMA to low power mode.\r\n");
			reg_val= reg_val | 0x40;   //0x40 is the low_power-bit
		}
		else if (sleepmode == 2)
		{
			send_debug("Setting BMA to suspend mode.\r\n");
			reg_val= reg_val | 0x80;   //0x80 is the suspend-bit
		}
		else
		{
			reg_val &= ~0b11100001;     // reset 0x8 and 0x4
		}
		//Register   Suspend-Bit
		bma222_reg_overwrite(0x11, reg_val);
		reg_val= 0xFF;
		reg_val= bma222_reg_read(0x11);
		return reg_val;
}
	

char bma222_get_id(void)
{
	uint8_t id;
	id = bma222_reg_read(0x0);
	return id;
}

uint8_t bma222_get_acceleration(int8_t *acc)
{
 	acc[0] = bma222_reg_read(0x3);
 	acc[1] = bma222_reg_read(0x5);
 	acc[2] = bma222_reg_read(0x7);
	acc[3] = 0;
	return 0;
}

uint8_t bma222_get_temperature(void)
{
	uint32_t temp;
	temp =bma222_reg_read(0x8);
	return ((uint8_t)(temp+46)/2);
}

uint8_t bma222_reg_overwrite(char reg, uint8_t val)
{
	bma222_cs(1);
	DELAY_MS(2);
	bma222_data[0]= reg;  //register address
	bma222_data[1]= val;
	io_write(spi_io, bma222_data, 2);
	DELAY_MS(1);
	bma222_cs(0);
	return bma222_reg_read(reg);	
}

uint8_t bma222_reg_write(char reg, char val)
{	
	val = val | bma222_reg_read(reg);
	bma222_cs(1);
	DELAY_MS(2);
	io_write(spi_io, &reg, 1);
	io_write(spi_io, &val, 1);
	DELAY_MS(1);
	bma222_cs(0);
	DELAY_MS(1);
	return bma222_reg_read(reg);	
}



void bma222_reset_tap_interrupts(void)
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
	bma222_reg_overwrite(0x10, 0b00000000);
	bma222_reg_overwrite(0x21, 0b00000000);
}

void bma222_set_single_tap_interrupt(void)
{
	bma222_reg_write(0x1E, 0b00010000);		//int_src_tap
	bma222_reg_write(0x1B, 0b00000000);		//int2_d_tap
	bma222_reg_write(0x19, 0b00100000);		//int1_s_tap
	bma222_reg_write(0x16, 0b00100000);		//s_tap_en
	bma222_reg_write(0x27, 0b00100000);		//slo_no_mot_dur obsolet
	bma222_reg_write(0x28, 0b00100000);		//slope_th          "
    bma222_reg_write(0x29, 0b00100000);		//slo_no_mot_th		"
}

void bma222_set_double_tap_interrupt2(void)
{
	bma222_reg_write(0x1E, 0b00010000); //enable source tap
	bma222_reg_write(0x1B, 0b00010000); //activate interrupt double tap
	bma222_reg_write(0x16, 0b00010000); //double tap enable
}

 
void bma222_set_motion_interrupt1(void)
{
//	bma222_get_state();
	bma222_reg_write(0x1E, 0b00001000); //enable source slo_mo
    bma222_reg_write(0x28, 0b00101011); //slope threshold
    bma222_reg_write(0x29, 0b00100000); //slo_no_mot_threshold -> 32 * 15mg = 480mg  sensitivity of detection
	
	bma222_reg_write(0x27, 0b11111100); //slomo duration 4 sec  continuous movement
	bma222_reg_write(0x18, 0b00000111); //activate interrupt slo_mo on channel x,y,z
	bma222_reg_write(0x19, 0b00001000); //int1 slo_mo activate
	bma222_reg_overwrite(0x10, 0b00001000); // set Bandwidth    1000 = 64ms  1001 = 32ms 1010 = 16ms ...  1111 = 0,5ms
	bma222_reg_overwrite(0x21, 0b00000010); // latch Interrupt    1110 = 50ms  0010 = 500 ms 
}


//duration is not possible so it is removed

uint8_t bma222_set_motion_interrupt( uint8_t sensitivity)
{
	bma222_reg_write(0x1E, 0b00001000); //enable source slo_mo
	bma222_reg_write(0x28, 0b00001011); //slope threshold
    bma222_reg_overwrite(0x29, sensitivity); //slo-no-mot-th  sensitivity of detection
	bma222_reg_write(0x27, 0b00000001); //slomo duration 4 sec  continuous movement
	bma222_reg_write(0x27, 0b11111111); //slomo duration 4 sec  continuous movement
	bma222_reg_write(0x18, 0b00000111); //activate interrupt slo_mo on channel x,y,z
	bma222_reg_write(0x19, 0b00001000); //int1 slo_mo activate
	bma222_reg_overwrite(0x10, 0b00001000); // set Bandwidth    1000 = 64ms  1001 = 32ms 1010 = 16ms ...  1111 = 0,5ms
	bma222_reg_overwrite(0x21, 0b00000010); // latch Interrupt    1110 = 50ms  0010 = 500 ms 
	return bma222_reg_read(29);
}

void bma222_set_slope_motion_interrupt(uint8_t sensitivity)
{
	if(seekiosManagerStarted)
	{
		USARTManager_printUsbWait("Slope detection SET.\r\n");
	}
	bma222_reg_overwrite(0x1E, 0b00000100); //enable source slope
	bma222_reg_overwrite(0x28, sensitivity); //slope threshold
	bma222_reg_overwrite(0x27, 0b00000011); //slope duration
	bma222_reg_overwrite(0x16, 0b00000111); //activate interrupt slope on channel x,y,z
	bma222_reg_overwrite(0x19, 0b00000100); //int1 slope activate
	bma222_reg_overwrite(0x10, 0b00001000); // set Bandwidth    1000 = 64ms  1001 = 32ms 1010 = 16ms ...  1111 = 0,5ms
	//bma222_reg_overwrite(0x21, 0b00000010); // latch Interrupt    1110 = 50ms  0010 = 500 ms
	bma222_reg_overwrite(0x21, 0b00001110); // latch Interrupt    1110 = 50ms  0010 = 500 ms
}

void bma222_set_slo_mo_interrupt( uint8_t sensitivity)
{
	if(seekiosManagerStarted)
	{
		USARTManager_printUsbWait("Slo mo detection SET.\r\n");
	}
	bma222_reg_overwrite(0x1E, 0b00001000); //enable source slo_mo
	bma222_reg_overwrite(0x29, sensitivity); //slo-no-mot-th  sensitivity of detection
	bma222_reg_overwrite(0x27, 0b00001111); //slomo duration
	bma222_reg_overwrite(0x18, 0b00000111); //activate interrupt slo_mo on channel x,y,z
	bma222_reg_overwrite(0x19, 0b00001000); //int1 slo_mo activate
	bma222_reg_overwrite(0x10, 0b00001000); // set Bandwidth    1000 = 64ms  1001 = 32ms 1010 = 16ms ...  1111 = 0,5ms
	bma222_reg_overwrite(0x21, 0b00000010); // latch Interrupt    1110 = 50ms  0010 = 500 ms
}

