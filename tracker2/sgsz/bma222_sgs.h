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
#include <sgs/port_sgs.h>
volatile char bma222_data[128];



uint8_t bma222_set_fifo(uint8_t mode);


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
	 * \brief Get hardware-ID of the chip
	 *
	 * \return Hardware-ID.
	 */	
char bma222_get_id(void);

/**
	 * \brief get acceleration-values of the tree channels
	 *
	 * \param[out] a pointer to the array with the 3 Values X-Y-Z
	 *
	 * \return 1 if sucessfull.
	 */
	 
uint8_t bma222_get_acceleration(char *acc);

/**
	 * \brief Clear all tap-interrupts
	 *
	 * This function clear sall interuupts for tap-detection
	 *
	 * \return 1 if sucessful
	 */
uint8_t bma222_reset_tap_interrupts(void);

	/**
	 * \brief Set Interrupt line 1
	 *
	 * This function initializes the first tap-interrupt on line 1
	 *
	 * \return Initialization status.
	 */
uint8_t bma222_set_tap_interrupt1(void);

	/**
	 * \brief Set Interrupt line 2
	 *
	 * This function initializes the second interrupt on line 2
	 *
	 * \return Initialization status.
	 */
uint8_t bma222_set_tap_interrupt2(void);

	/**
	 * \brief Set Interrupt line 1 to motion interrupt
	 *
	 * This function initializes the slow motion interrupt on line 1
	 
	  * \param[in] duration  Register
	  * \param[in] sensitivity to be written in register
	 *
	 * \return Initialization status.
	 */
uint8_t bma222_set_motion_interrupt1(void);


/**
	 * \brief write value to register
	 
	 *  Each measured acceleration has to be different from the one before to fire the interrupt
	 *  If all accelerations differ over time, we are moving, and it was not only a tap or shock
	 *	
	 * \param[in] sense_duration   entity:  [duration] * 100ms
	 * \param[in] sensitivity  in  quantitys of 15mg each.
	 * \return 1 if successful
	 */

uint8_t bma222_set_motion_interrupt(uint8_t sense_duration, uint8_t sensitivity);
#endif /* BMA222_H_ */

