/**
 * \file
 *
 * \brief Generic CALENDAR functionality implementation.
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

#include "hal_calendar.h"
#include <utils.h>
#include <utils_assert.h>
#include <hal_atomic.h>

#define CALENDAR_VERSION         0x00000001u
#define SECS_IN_LEAP_YEAR        31622400
#define SECS_IN_NON_LEAP_YEAR    31536000
#define SECS_IN_31DAYS           2678400
#define SECS_IN_30DAYS           2592000
#define SECS_IN_29DAYS           2505600
#define SECS_IN_28DAYS           2419200
#define SECS_IN_DAY              86400
#define SECS_IN_HOUR             3600
#define SECS_IN_MINUTE           60
#define DEFAULT_BASE_YEAR        1970

#define SET_ALARM_BUSY           1
#define PROCESS_ALARM_BUSY       2


/** \brief leapyear check
 *  \retval false  not leapyear.
 *  \retval true  leapyear.
 */
static bool leap_year(uint16_t year)
{
	if (year & 3) {
		return false;
	} else {
		return true;
	}
}

/** \brief calculate the seconds in specified year/month
 *  \retval 0  month error.
 */
static uint32_t get_secs_in_month(uint32_t year, uint8_t month)
{
	uint32_t sec_in_month = 0;

	if (leap_year(year)) {
		switch (month) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			sec_in_month = SECS_IN_31DAYS;
			break;
		case 2:
			sec_in_month = SECS_IN_29DAYS;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			sec_in_month = SECS_IN_30DAYS;
			break;
		default:
			break;
		}
	} else {
		switch (month) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			sec_in_month = SECS_IN_31DAYS;
			break;
		case 2:
			sec_in_month = SECS_IN_28DAYS;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			sec_in_month = SECS_IN_30DAYS;
			break;
		default:
			break;
		}
	}

	return sec_in_month;
}

/** \brief convert timestamp to date/time
 */
static int32_t convert_timestamp_to_datetime(
	struct calendar_descriptor *const calendar,
	uint32_t ts,
	struct calendar_date_time *dt)
{
	uint32_t tmp, sec_in_year, sec_in_month;
	uint32_t tmp_year   = calendar->base_year;
	uint8_t tmp_month  = 1;
	uint8_t tmp_day    = 1;
	uint8_t tmp_hour    = 0;
	uint8_t tmp_minutes = 0;

	tmp = ts;

	/* Find year */
	while(true) {
		sec_in_year =
				leap_year(tmp_year) ? SECS_IN_LEAP_YEAR : SECS_IN_NON_LEAP_YEAR;

		if (tmp >= sec_in_year) {
			tmp  -= sec_in_year;
			tmp_year++;
		} else {
			break;
		}
	}
	/* Find month of year */
	while(true) {
		sec_in_month = get_secs_in_month(tmp_year, tmp_month);

		if (tmp >= sec_in_month) {
			tmp  -= sec_in_month;
			tmp_month++;
		} else {
			break;
		}
	}
	/* Find day of month */
	while(true) {
		if (tmp >= SECS_IN_DAY) {
			tmp  -= SECS_IN_DAY;
			tmp_day++;
		} else {
			break;
		}
	}
	/* Find hour of day */
	while(true) {
		if (tmp >= SECS_IN_HOUR) {
			tmp  -= SECS_IN_HOUR;
			tmp_hour++;
		} else {
			break;
		}
	}
	/* Find minute in hour */
	while(true) {
		if (tmp >= SECS_IN_MINUTE) {
			tmp  -= SECS_IN_MINUTE;
			tmp_minutes++;
		} else {
			break;
		}
	}

	dt->date.year    = tmp_year;
	dt->date.month   = tmp_month;
	dt->date.day     = tmp_day;
	dt->time.hour    = tmp_hour;
	dt->time.min = tmp_minutes;
	dt->time.sec = tmp;

	return ERR_NONE;
}

/** \brief convert date/time to timestamp
 *  \return timestamp
 */
static uint32_t convert_datetime_to_timestamp(
	struct calendar_descriptor *const calendar,
	struct calendar_date_time *dt)
{
	uint32_t tmp = 0;
	uint32_t i = 0;
	uint8_t year, month, day, hour, minutes, seconds;

	year    = dt->date.year - calendar->base_year;
	month   = dt->date.month;
	day     = dt->date.day;
	hour    = dt->time.hour;
	minutes = dt->time.min;
	seconds = dt->time.sec;

	/* tot up year field */
	for (i = 0; i < year; ++i) {
		if (leap_year(calendar->base_year + i)) {
			tmp += SECS_IN_LEAP_YEAR;
		} else {
			tmp += SECS_IN_NON_LEAP_YEAR;
		}
	}

	/* tot up month field */
	for (i = 1; i < month; ++i) {
		tmp += get_secs_in_month(dt->date.year, i);
	}

	/* tot up day/hour/minute/second fields */
	tmp += ( day - 1 ) * SECS_IN_DAY;
	tmp += hour * SECS_IN_HOUR;
	tmp += minutes * SECS_IN_MINUTE;
	tmp += seconds;

	return tmp;
}

/** \brief calibrate timestamp to make desired timestamp ahead of current timestamp
 */
static void calibrate_timestamp(struct calendar_descriptor *const calendar,
		struct calendar_alarm *alarm,
		uint32_t current_ts)
{
	struct calendar_date_time dt;
	uint32_t alarm_ts;

	alarm_ts = alarm->data.timestamp;
	convert_timestamp_to_datetime(calendar, current_ts, &dt);

	/* calibrate timestamp */
	switch (alarm->option) {
	case CALENDAR_ALARM_MATCH_SEC:

		if (alarm_ts <= current_ts) {
			alarm_ts += SECS_IN_MINUTE;
		}

		break;
	case CALENDAR_ALARM_MATCH_MIN:

		if (alarm_ts <= current_ts) {
			alarm_ts += SECS_IN_HOUR;
		}

		break;
	case CALENDAR_ALARM_MATCH_HOUR:

		if (alarm_ts <= current_ts) {
			alarm_ts += SECS_IN_DAY;
		}

		break;
	case CALENDAR_ALARM_MATCH_DAY:

		if (alarm_ts <= current_ts) {
			alarm_ts += get_secs_in_month(dt.date.year, dt.date.month);
		}

		break;
	case CALENDAR_ALARM_MATCH_MONTH:

		if (alarm_ts <= current_ts) {
			if (leap_year(dt.date.year)) {
				alarm_ts += SECS_IN_LEAP_YEAR;
			}else {
				alarm_ts += SECS_IN_NON_LEAP_YEAR;
			}
		}

		break;
	/* do nothing for year match */
	case CALENDAR_ALARM_MATCH_YEAR:
	default:
		break;
	}

	/* desired timestamp after calibration */
	alarm->data.timestamp = alarm_ts;
}

/** \brief complete alarm to absolute date/time, then fill up the timestamp
 */
static void fill_alarm(struct calendar_descriptor *const calendar,
		struct calendar_alarm *alarm)
{
	struct calendar_date_time dt;
	uint32_t tmp, current_ts;

	/* get current date/time */
	current_ts = _calendar_get_counter(&calendar->device);
	convert_timestamp_to_datetime(calendar, current_ts, &dt);

	/* complete alarm */
	switch (alarm->option) {
	case CALENDAR_ALARM_MATCH_SEC:
		alarm->data.datetime.date.year = dt.date.year;
		alarm->data.datetime.date.month = dt.date.month;
		alarm->data.datetime.date.day   = dt.date.day;
		alarm->data.datetime.time.hour  = dt.time.hour;
		alarm->data.datetime.time.min  = dt.time.min;
		break;
	case CALENDAR_ALARM_MATCH_MIN:
		alarm->data.datetime.date.year = dt.date.year;
		alarm->data.datetime.date.month = dt.date.month;
		alarm->data.datetime.date.day   = dt.date.day;
		alarm->data.datetime.time.hour  = dt.time.hour;
		break;
	case CALENDAR_ALARM_MATCH_HOUR:
		alarm->data.datetime.date.year = dt.date.year;
		alarm->data.datetime.date.month = dt.date.month;
		alarm->data.datetime.date.day   = dt.date.day;
		break;
	case CALENDAR_ALARM_MATCH_DAY:
		alarm->data.datetime.date.year = dt.date.year;
		alarm->data.datetime.date.month = dt.date.month;
		break;
	case CALENDAR_ALARM_MATCH_MONTH:
		alarm->data.datetime.date.year = dt.date.year;
		break;
	case CALENDAR_ALARM_MATCH_YEAR:
		break;
	default:
		break;
	}

	/* fill up the timestamp */
	tmp = convert_datetime_to_timestamp(calendar, &alarm->data.datetime);
	alarm->data.timestamp = tmp;

	/* calibrate the timestamp */
	calibrate_timestamp(calendar, alarm, current_ts);
}


/** \brief add new alarm into the list in ascending order
 */
static int32_t calendar_add_new_alarm(struct list_descriptor *list,
		struct calendar_alarm *alarm)
{
	struct calendar_descriptor *calendar =
			CONTAINER_OF(list, struct calendar_descriptor, alarms);
	struct calendar_alarm *head, *it, *prev = NULL;

	/*get the head of alarms list*/
	head = (struct calendar_alarm *)list_get_head(list);

	/*if head is null, insert new alarm as head*/
	if (!head) {
		list_insert_as_head(list, alarm);
		_calendar_set_comp(&calendar->device, alarm->data.timestamp);
		return ERR_NONE;
	}

	/*insert the new alarm in accending order, the head will be invoked firstly */
	for (it = head; it;
			it = (struct calendar_alarm *)list_get_next_element(it)) {
		if (alarm->data.timestamp <= it->data.timestamp) {
			break;
		}

		prev = it;
	}

	/*insert new alarm into the list */
	if (it == head) {
		list_insert_as_head(list, alarm);
		/*get the head and set it into register*/
		_calendar_set_comp(&calendar->device, alarm->data.timestamp);
	} else {
		list_insert_after(prev, alarm);
	}

	return ERR_NONE;
}

/** \brief callback for alarm
 */
static void calendar_alarm(struct calendar_dev *const dev)
{
	struct calendar_descriptor *calendar =
			CONTAINER_OF(dev, struct calendar_descriptor, device);

	struct calendar_alarm *head, *it;
	uint32_t current_ts;

	if (( calendar->flags & SET_ALARM_BUSY ) ||
			( calendar->flags & PROCESS_ALARM_BUSY )) {
		calendar->flags |= PROCESS_ALARM_BUSY;
		return;
	}

	/* get current timestamp */
	current_ts = _calendar_get_counter(dev);

	/* get the head */
	head = (struct calendar_alarm *)list_get_head(&calendar->alarms);
	ASSERT(head);

	/* remove all alarms and invoke them*/
	for (it = head; it;
			it = (struct calendar_alarm *)list_get_head(&calendar->alarms)) {
		/* check the timestamp with current timestamp*/
		if (it->data.timestamp <= current_ts) {
			list_remove_head(&calendar->alarms);
			it->callback(calendar);

			if (it->mode == REPEAT) {
				calibrate_timestamp(calendar, it, current_ts);
				calendar_add_new_alarm(&calendar->alarms, it);
			}
		} else {
			break;
		}
	}

	/*if no alarm in the list, register null */
	if (!it) {
		_calendar_register_callback(&calendar->device, NULL);
		return;
	}

	/*put the new head into register */
	_calendar_set_comp(&calendar->device, it->data.timestamp);
}

/** \brief Initialize Calendar
 */
int32_t calendar_init(struct calendar_descriptor *const calendar,
		const void *hw)
{
	int32_t ret = 0;

	/* Sanity check arguments */
	ASSERT(calendar);

	calendar->device.hw = (void *)hw;
	calendar->base_year = DEFAULT_BASE_YEAR;

	ret = _calendar_init(&calendar->device);

	return ret;
}

/** \brief Reset the Calendar
 */
int32_t calendar_deinit(struct calendar_descriptor *const calendar)
{
	/* Sanity check arguments */
	ASSERT(calendar);

	_calendar_deinit(&calendar->device);

	return ERR_NONE;
}

/** \brief Enable the Calendar
 */
int32_t calendar_enable(struct calendar_descriptor *const calendar)
{
	/* Sanity check arguments */
	ASSERT(calendar);

	_calendar_enable(&calendar->device);

	return ERR_NONE;
}

/** \brief Disable the Calendar
 */
int32_t calendar_disable(struct calendar_descriptor *const calendar)
{
	/* Sanity check arguments */
	ASSERT(calendar);

	_calendar_disable(&calendar->device);

	return ERR_NONE;
}

/** \brief Set base year for calendar
 */
int32_t calendar_set_baseyear(struct calendar_descriptor *const calendar,
		const uint32_t p_base_year)
{
	/* Sanity check arguments */
	ASSERT(calendar);

	calendar->base_year = p_base_year;

	return ERR_NONE;
}

/** \brief Set time for calendar
 */
int32_t calendar_set_time(struct calendar_descriptor *const calendar,
		struct calendar_time *const p_calendar_time)
{
	struct calendar_date_time dt;
	uint32_t current_ts, new_ts;

	/* Sanity check arguments */
	ASSERT(calendar);

	/* convert time to timestamp */
	current_ts = _calendar_get_counter(&calendar->device);
	convert_timestamp_to_datetime(calendar, current_ts, &dt);
	dt.time.sec = p_calendar_time->sec;
	dt.time.min = p_calendar_time->min;
	dt.time.hour = p_calendar_time->hour;

	new_ts = convert_datetime_to_timestamp(calendar, &dt);

	_calendar_set_counter(&calendar->device, new_ts);

	return ERR_NONE;
}

/** \brief Set date for calendar
 */
int32_t calendar_set_date(struct calendar_descriptor *const calendar,
		struct calendar_date *const p_calendar_date)
{
	struct calendar_date_time dt;
	uint32_t current_ts, new_ts;

	/* Sanity check arguments */
	ASSERT(calendar);

	/* convert date to timestamp */
	current_ts = _calendar_get_counter(&calendar->device);
	convert_timestamp_to_datetime(calendar, current_ts, &dt);
	dt.date.day = p_calendar_date->day;
	dt.date.month = p_calendar_date->month;
	dt.date.year = p_calendar_date->year;

	new_ts = convert_datetime_to_timestamp(calendar, &dt);

	_calendar_set_counter(&calendar->device, new_ts);

	return ERR_NONE;
}

/** \brief Get date/time for calendar
 */
int32_t calendar_get_date_time(struct calendar_descriptor *const calendar,
		struct calendar_date_time *const date_time)
{
	uint32_t current_ts;

	/* Sanity check arguments */
	ASSERT(calendar);

	/* convert current timestamp to date/time */
	current_ts = _calendar_get_counter(&calendar->device);
	convert_timestamp_to_datetime(calendar, current_ts, date_time);

	return ERR_NONE;
}

/** \brief Set alarm for calendar
 */
int32_t calendar_set_alarm(struct calendar_descriptor *const calendar,
		struct calendar_alarm *const alarm,
		calendar_cb_alarm_t callback)
{
	struct calendar_alarm *head;

	/* Sanity check arguments */
	ASSERT(calendar);
	ASSERT(alarm);

	alarm->callback = callback;

	fill_alarm(calendar, alarm);

	calendar->flags |= SET_ALARM_BUSY;

	head = (struct calendar_alarm *)list_get_head(&calendar->alarms);

	if (head != NULL) {
		/* already added */
		if (is_list_element(&calendar->alarms, alarm)) {
			if (callback == NULL) {
				/* remove alarm */
				list_delete_element(&calendar->alarms, alarm);

				if (!list_get_head(&calendar->alarms)) {
					_calendar_register_callback(&calendar->device, NULL);
				}
			} else {
				/* re-add */
				list_delete_element(&calendar->alarms, alarm);
				calendar_add_new_alarm(&calendar->alarms, alarm);
			}
		} else if (callback != NULL) {
			calendar_add_new_alarm(&calendar->alarms, alarm);
		}

		calendar->flags &= ~SET_ALARM_BUSY;

		if (calendar->flags & PROCESS_ALARM_BUSY) {
			CRITICAL_SECTION_ENTER()
			calendar->flags &= ~PROCESS_ALARM_BUSY;
			_calendar_set_irq(&calendar->device);
			CRITICAL_SECTION_LEAVE()
		}
	} else if (callback != NULL) {
		/* if head is NULL, Register callback*/
		_calendar_register_callback(&calendar->device, calendar_alarm);
		calendar_add_new_alarm(&calendar->alarms, alarm);
	}

	calendar->flags &= ~SET_ALARM_BUSY;
	calendar->flags &= ~PROCESS_ALARM_BUSY;

	return ERR_NONE;
}

/** \brief Retrieve driver version
 *  \return Current driver version
 */
uint32_t calendar_get_version(void)
{
	return CALENDAR_VERSION;
}
