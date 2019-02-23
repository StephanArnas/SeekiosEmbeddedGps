/*
 * bma222_sgs.h
 *
 * Created: 30.05.2016 17:37:31
 *  Author: Marc Adolf
 */ 


#ifndef BMA222_H_
#define BMA222_H_

#include "stdint.h"
#include "serial_sgs.h"
#include "port_sgs.h"
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <seekiosCore/seekios.h>
volatile char bma222_data[128];

void bma222_set_fifo(uint8_t mode);


	/**
	 * \brief Chipselect IMU-Sensor
	 *	
	 * \param[in] mode 0 - deselect  1 - selsect	
	 * \return 1 if successful
	 */
uint8_t bma222_cs(uint8_t mode);


	/**
	 * \brief Get status of all registers
	 *
	 * \param[out] fills the bma222_data - array
	 * \return 1 if successful
	 */
uint8_t bma222_get_state(void);


char bma222_set_sleepmode(uint8_t sleepmode);
char bma222_set_sleep_duration(uint8_t sleepduration);
/**
	 * \brief set filter bandwidth
	 *	
	 * \param[in]  bw  bandwith
	 	
	 * \return written register value
	 */
char bma222_set_bandwith(uint8_t bw);

	/**
	 * \brief write value to register
	 *	
	 * \param[in] reg  Register
	 * \param[in] val Value to be written in register
	 	
	 * \return 1 if successful
	 */
uint8_t bma222_reg_write(char reg, char val);


	/**
	 * \brief Get temperature on BMA-chip
	 *
	 * \return temperature.
	 */
uint8_t bma222_get_temperature(void);


	/**
	 * \brief Set the sleep modes of the chip
	 *
	 * \return nothing.
	 */

char bma222_suspend(uint8_t sleepmode);

	/**
	 * \brief Set the sleep modes of the chip
	 *
	 * \return nothing.
	 */

char bma222_get_id(void);

/**
	 * \brief get acceleration-values of the tree channels
	 *
	 * \param[out] a pointer to the array with the 3 Values X-Y-Z
	 *
	 * \return 1 if successful.
	 */
	 
uint8_t bma222_get_acceleration(int8_t *acc);

/**
	 * \brief Clear all tap-interrupts
	 *
	 * This function clears all interrupts for tap-detection
	 *
	 * \return 1 if successful
	 */
void bma222_reset_tap_interrupts(void);

	/**
	 * \brief Set Interrupt line 1
	 *
	 * This function initializes the first tap-interrupt on line 1
	 *
	 * \return Initialization status.
	 */
void bma222_set_single_tap_interrupt(void);

	/**
	 * \brief Set Interrupt line 2
	 *
	 * This function initializes the second interrupt on line 2
	 *
	 * \return Initialization status.
	 */
void bma222_set_double_tap_interrupt2(void);

	/**
	 * \brief Set Interrupt line 1 to motion interrupt
	 *
	 * This function initializes the slow motion interrupt on line 1
	 
	  * \param[in] duration  Register
	  * \param[in] sensitivity to be written in register
	 *
	 * \return Initialization status.
	 */
void bma222_set_motion_interrupt1(void);


/**
	 * \brief write value to register
	 
	 *  Each measured acceleration has to be different from the one before to fire the interrupt
	 *  If all accelerations differ over time, we are moving, and it was not only a tap or shock
	 *	
	 * \param[in] sense_duration   entity:  [duration] * 100ms
	 * \param[in] sensitivity  in  quantitys of 15mg each.
	 * \return 1 if successful
	 */

uint8_t bma222_set_motion_interrupt(uint8_t sensitivity);
#endif /* BMA222_H_ */

uint8_t bma222_reg_overwrite(char reg, uint8_t val);

uint8_t bma222_reg_read(char reg);

void bma222_set_slope_motion_interrupt(uint8_t sensitivity);