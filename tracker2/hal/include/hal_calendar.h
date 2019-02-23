/**
 * \file
 *
 * \brief Generic CALENDAR functionality declaration.
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _HAL_CALENDER_H_INCLUDED
#define _HAL_CALENDER_H_INCLUDED

#include "hpl_calendar.h"
#include <utils_list.h>

#ifdef __cplusplus
	extern "C" {
#endif

struct calendar_descriptor;
struct calendar_date_time;

/** \brief Prototype of callbak on alarm match
 *  \param calendar Pointer to the HAL Calendar instance.
 */
typedef void (*calendar_cb_alarm_t)(struct calendar_descriptor *const calendar);

/** \brief Calendar HAL driver struct
 *
 */
struct calendar_descriptor {
	struct calendar_dev device;
	struct list_descriptor alarms;
	/*base date/time = base_year/1/1/0/0/0(year/month/day/hour/min/sec)*/
	uint32_t base_year;
	uint8_t flags;
};

/** \brief Date&Time struct for calendar
 */
struct calendar_date_time {
	struct calendar_time time;
	struct calendar_date date;
};

/** \brief struct for alarm time
 */
struct calendar_alarm {
	struct list_element elem;
	union {
		struct calendar_date_time datetime;
		uint32_t timestamp;
	} data;
	enum calendar_alarm_option option;
	enum calendar_alarm_mode mode;
	calendar_cb_alarm_t callback;
};

/** \brief Initialize the Calendar HAL instance and hardware
 *  Initialize Calendar HAL.
 *  \param calendar Pointer to the HAL Calendar instance.
 *  \param hw       Pointer to the hardware instance.
 *  \return Operation status of init
 *  \retval 0       Completed sucessfully.
 */
int32_t calendar_init(struct calendar_descriptor *const calendar,
		const void *hw);

/** \brief Reset the Calendar HAL instance and hardware
 *  Reset Calendar instance to hardware defaults.
 *  \param calendar Pointer to the HAL Calendar instance.
 *  \return Operation status of reset.
 *  \retval 0       Completed sucessfully.
 */
int32_t calendar_deinit(struct calendar_descriptor *const calendar);

/** \brief Enable the Calendar HAL instance and hardware
 *  Enable Calendar HAL.
 *  \param calendar Pointer to the HAL Calendar instance.
 *  \return Operation status of init
 *  \retval 0       Completed sucessfully.
 */
int32_t calendar_enable(struct calendar_descriptor *const calendar);

/** \brief Disable the Calendar HAL instance and hardware
 *  Disable Calendar instance to hardware defaults.
 *  \param calendar Pointer to the HAL Calendar instance.
 *  \return Operation status of reset.
 *  \retval 0       Completed sucessfully.
 */
int32_t calendar_disable(struct calendar_descriptor *const calendar);

/** \brief Config the base year for calendar HAL instance and hardware
 *  Set the time to calendar instance.
 *  \param calendar Pointer to the HAL Calendar instance.
 *  \param p_base_year The desired base year.
 *  \retval 0       Completed sucessfully.
 */
int32_t calendar_set_baseyear(struct calendar_descriptor *const calendar,
		const uint32_t p_base_year);

/** \brief Config the time for calendar HAL instance and hardware
 *  Set the time to calendar instance.
 *  \param calendar Pointer to the HAL Calendar instance.
 *  \param p_calendar_time Pointer to the time configuration.
 *  \retval 0       Completed sucessfully.
 */
int32_t calendar_set_time(struct calendar_descriptor *const calendar,
		struct calendar_time *const p_calendar_time);

/** \brief Config the date for calendar HAL instance and hardware
 *  Set the date to calendar instance.
 *  \param calendar Pointer to the HAL Calendar instance.
 *  \param p_calendar_date Pointer to the date configuration.
 *  \return Operation status of time set.
 *  \retval 0       Completed sucessfully.
 */
int32_t calendar_set_date(struct calendar_descriptor *const calendar,
		struct calendar_date *const p_calendar_date);

/** \brief Get the time for calendar HAL instance and hardware
 *  Retrieve the time from calendar instance.
 *  \param calendar Pointer to the HAL Calendar instance.
 *  \param date_time Pointer to value that will be filled with current time.
 *  \return Operation status of time retrieve.
 *  \retval 0       Completed sucessfully.
 */
int32_t calendar_get_date_time(struct calendar_descriptor *const calendar,
		struct calendar_date_time *const date_time);

/** \brief Config the alarm time for calendar HAL instance and hardware
 *  Set the alarm time to calendar instance, if callback is NULL, remove the alarm
 *  when alarm already added, otherwise, ignore the alarm.
 *  \param calendar Pointer to the HAL Calendar instance.
 *  \param alarm Pointer to the configuration.
 *  \param callback Pointer to the callback function.
 *  \return Operation status of alarm time set.
 *  \retval 0       Completed sucessfully.
 */
int32_t calendar_set_alarm(struct calendar_descriptor *const calendar,
		struct calendar_alarm *const alarm,
		calendar_cb_alarm_t callback);

/** \brief Retrieve the current driver version
 *  \return Current driver version
 */
uint32_t calendar_get_version(void);


#ifdef __cplusplus
	}
#endif

#endif /* _HAL_CALENDER_H_INCLUDED */
