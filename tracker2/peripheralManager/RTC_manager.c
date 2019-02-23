#include <peripheralManager/RTC_manager.h>

static int32_t convertTimestampToDateTime(uint32_t ts,struct calendar_date_time *dt);
static bool leapYear(uint16_t year);
static uint32_t getSecsInMonth(uint32_t year, uint8_t month);
static BaseType_t takeCalendarSem(void);
static BaseType_t giveCalendarSem(void);

static bool _isCalendarInitialized;
extern struct calendar_descriptor CALENDAR_X;
static struct calendar_alarm alarm1;
static struct calendar_alarm alarm2;
static SemaphoreHandle_t _calendarSem;
int count = 0;

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

void RTCManager_init(){
	struct calendar_date date;
	struct calendar_time time;

	date.year = 1970;
	date.month = 1;
	date.day = 1;

	time.hour = 0;
	time.min = 0;
	time.sec = 0;
	
	_calendarSem = xSemaphoreCreateRecursiveMutex();

	if(takeCalendarSem()==pdPASS)
	{
		calendar_set_date(&CALENDAR_X, &date);
		DELAY_MS(100);
		calendar_set_time(&CALENDAR_X, &time);
		DELAY_MS(100);
		calendar_enable(&CALENDAR_X);
		giveCalendarSem();
	}

	_isCalendarInitialized = false;

}

static void callback1()
{
	send_debug("Callback 1");
}

static void callback2()
{
	send_debug("Callback 2");
}

/* In this example, we set the calendar at a date, then we set and alarm, then we set an alarm between the calendar date and the alarm. */
void RTCManager_calendarAlarmsExample1()
{
	struct calendar_date date;
	struct calendar_time time;

	date.year = 1970;
	date.month = 1;
	date.day = 1;

	time.hour = 0;
	time.min = 0;
	time.sec = 0;

	if(takeCalendarSem()==pdPASS)
	{
		calendar_set_date(&CALENDAR_X, &date);
		DELAY_MS(100);
		calendar_set_time(&CALENDAR_X, &time);
		DELAY_MS(100);
		calendar_enable(&CALENDAR_X);
		giveCalendarSem();
	}

	alarm1.data.datetime.date.year = 1970;
	alarm1.data.datetime.date.month = 1;
	alarm1.data.datetime.date.day = 1;

	alarm1.data.datetime.time.hour = 0;
	alarm1.data.datetime.time.min  = 0;
	alarm1.data.datetime.time.sec  = 10;

	alarm1.option = CALENDAR_ALARM_MATCH_YEAR;
	alarm1.mode = ONESHOT; 

	calendar_set_alarm(&CALENDAR_X, &alarm1, callback1);

	alarm2.data.datetime.date.year = 1970;
	alarm2.data.datetime.date.month = 1;
	alarm2.data.datetime.date.day = 1;
	
	alarm2.data.datetime.time.hour = 0;
	alarm2.data.datetime.time.min  = 0;
	alarm2.data.datetime.time.sec  = 5;
	
	alarm2.option = CALENDAR_ALARM_MATCH_YEAR;
	alarm2.mode = ONESHOT;

	calendar_set_alarm(&CALENDAR_X, &alarm2, callback2);

}

/* In this example, we set the calendar at a date, then we set and alarm, then we set the calendar to a date past the first alarm, then we set another alarm. */
void RTCManager_calendarAlarmsExample2()
{
	struct calendar_date date;
	struct calendar_time time;

	date.year = 1970;
	date.month = 1;
	date.day = 1;

	time.hour = 0;
	time.min = 0;
	time.sec = 0;

	if(takeCalendarSem()==pdPASS)
	{
		calendar_set_date(&CALENDAR_X, &date);
		DELAY_MS(100);
		calendar_set_time(&CALENDAR_X, &time);
		DELAY_MS(100);
		calendar_enable(&CALENDAR_X);
		giveCalendarSem();
	}

	alarm1.data.datetime.date.year = 1970;
	alarm1.data.datetime.date.month = 1;
	alarm1.data.datetime.date.day = 1;

	alarm1.data.datetime.time.hour = 0;
	alarm1.data.datetime.time.min  = 0;
	alarm1.data.datetime.time.sec  = 5;

	alarm1.option = CALENDAR_ALARM_MATCH_YEAR;
	alarm1.mode = ONESHOT;

	calendar_set_alarm(&CALENDAR_X, &alarm1, callback1);

	date.year = 1970;
	date.month = 1;
	date.day = 1;

	time.hour = 0;
	time.min = 0;
	time.sec = 2;

	if(takeCalendarSem()==pdPASS)
	{
		calendar_set_date(&CALENDAR_X, &date);
		DELAY_MS(100);
		calendar_set_time(&CALENDAR_X, &time);
		DELAY_MS(100);
		//calendar_enable(&CALENDAR_X);
		giveCalendarSem();
	}


	alarm2.data.datetime.date.year = 1970;
	alarm2.data.datetime.date.month = 1;
	alarm2.data.datetime.date.day = 1;
	
	alarm2.data.datetime.time.hour = 0;
	alarm2.data.datetime.time.min  = 0;
	alarm2.data.datetime.time.sec  = 20;
	
	alarm2.option = CALENDAR_ALARM_MATCH_YEAR;
	alarm2.mode = ONESHOT;
	
	calendar_set_alarm(&CALENDAR_X, &alarm2, callback2);

	RTCManager_removeAlarm(&alarm1);

}

bool RTCManager_isCalendarInitialized(){
	return _isCalendarInitialized;
}

void RTCManager_initDateTime(struct tm* datePtr){
	datePtr->tm_year = 0;
	datePtr->tm_mon = 0;
	datePtr->tm_mday = 0;
	datePtr->tm_hour = 0;
	datePtr->tm_min = 0;
	datePtr->tm_sec = 0;
	datePtr->tm_wday = 0;
	datePtr->tm_yday = 0;
	datePtr->tm_isdst = 0;
}

void RTCManager_copyDateTime(struct tm* resultPtr, struct tm toCopy){
	resultPtr->tm_year = toCopy.tm_year;
	resultPtr->tm_mon = toCopy.tm_mon;
	resultPtr->tm_mday = toCopy.tm_mday;
	resultPtr->tm_hour = toCopy.tm_hour;
	resultPtr->tm_min = toCopy.tm_min;
	resultPtr->tm_sec = toCopy.tm_sec;
	resultPtr->tm_wday = toCopy.tm_wday;
	resultPtr->tm_yday = toCopy.tm_yday;
	resultPtr->tm_isdst = toCopy.tm_isdst;
}

static BaseType_t takeCalendarSem()
{
	return xSemaphoreTakeRecursive(_calendarSem, 1000);
}

static BaseType_t giveCalendarSem()
{
	return xSemaphoreGiveRecursive(_calendarSem);
}

bool RTCManager_getCurrentDateTime(struct calendar_date_time *currentDateTime)
{
	bool success = false;
	if(takeCalendarSem()==pdPASS)
	{
		calendar_get_date_time(&CALENDAR_X, &currentDateTime);
		success = true;
		giveCalendarSem();
	}
	return success;
}

void RTCManager_getCurrentTm(struct tm* resultPtr){

	struct calendar_date_time currentDateTime;
	RTCManager_getCurrentDateTime(&currentDateTime);

	struct tm tempTm;

	tempTm.tm_year = currentDateTime.date.year - 1900;
	tempTm.tm_mon = currentDateTime.date.month - 1;
	tempTm.tm_mday = currentDateTime.date.day;
	tempTm.tm_hour = currentDateTime.time.hour;
	tempTm.tm_min = currentDateTime.time.min;
	tempTm.tm_sec = currentDateTime.time.sec;

	time_t t = mktime(&tempTm);
	gmtime_r(&t, resultPtr); // to get wday
}

time_t RTCManager_getCurrentTimestamp(){
	if(RTCManager_isCalendarInitialized())
	{
		struct tm tempTm;
		struct calendar_date_time currentDateTime;
		if(takeCalendarSem()==pdPASS)
		{
			calendar_get_date_time(&CALENDAR_X, &currentDateTime);
			giveCalendarSem();
		}
		tempTm.tm_year = currentDateTime.date.year - 1900;
		tempTm.tm_mon = currentDateTime.date.month - 1;
		tempTm.tm_mday = currentDateTime.date.day;
		tempTm.tm_hour = currentDateTime.time.hour;
		tempTm.tm_min = currentDateTime.time.min;
		tempTm.tm_sec = currentDateTime.time.sec;

		return mktime(&tempTm);
	}
	else
	{
		return 0;
	}

}

/* Update calendar datetime */
void RTCManager_updateSeekiosDatetime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec){

	struct calendar_date date;
	struct calendar_time time;

	date.year = year;
	date.month = month;
	date.day = day;

	time.hour = hour;
	time.min = min;
	time.sec = sec;

	if(takeCalendarSem()==pdPASS)
	{
		calendar_set_date(&CALENDAR_X, &date);
		DELAY_MS(100);
		calendar_set_time(&CALENDAR_X, &time);
		DELAY_MS(100);
		giveCalendarSem();
	}
	
	_isCalendarInitialized = true;
}

bool RTCManager_setCalendarFromMessage(char* message){
	
	message += 2;
	message[strlen(message)-1] = 0;

	time_t timestamp = atoi(message);
	if(timestamp < 0){
		return false;
	}

	volatile BaseType_t wm = uxTaskGetStackHighWaterMark(NULL);
	UNUSED(wm);
	struct tm currentDateTime;
	gmtime_r(&timestamp, &currentDateTime); // conversion en temps UTC
	RTCManager_updateSeekiosDatetime(currentDateTime.tm_year + 1900, currentDateTime.tm_mon + 1, currentDateTime.tm_mday, currentDateTime.tm_hour, currentDateTime.tm_min, currentDateTime.tm_sec);
	
	// TODO : check calendar values

	return true;
}

/* Computes the date time after a delay compared with the current date
delayMs is in SECONDS*/
static bool getDelayDateTime(int delaySec, struct calendar_date_time* dtPtr){
	struct calendar_date_time currentDateTime;
	if(takeCalendarSem()==pdPASS)
	{
		calendar_get_date_time(&CALENDAR_X, &currentDateTime);
		giveCalendarSem();
	}
	
	struct tm currentTm; // used for time conversion methods
	currentTm.tm_year = currentDateTime.date.year - 1900;
	currentTm.tm_mon = currentDateTime.date.month - 1;
	currentTm.tm_mday = currentDateTime.date.day;
	currentTm.tm_hour = currentDateTime.time.hour;
	currentTm.tm_min = currentDateTime.time.min;
	currentTm.tm_sec = currentDateTime.time.sec;

	time_t timestamp = mktime(&currentTm);
	if(timestamp != -1){
		timestamp += delaySec;
		convertTimestampToDateTime(timestamp, dtPtr);
		return FUNCTION_SUCCESS;
		} else {
		return FUNCTION_FAILURE;
	}
}

/* Sets a oneshot alarm that is bound to trigger once the delay delaySec passed as a parameter is reached. */
bool RTCManager_setDelayedCallback(int delaySec, struct calendar_alarm* alarm, calendar_cb_alarm_t callbackFunc){
	if(getDelayDateTime(delaySec, &(alarm->data.datetime)) == FUNCTION_SUCCESS){
		USARTManager_printUsbWait("ALARM SET AT :");
		RTCManager_printTime(&alarm->data.datetime);
		USARTManager_printUsbWait("\r\n");
		alarm->option = CALENDAR_ALARM_MATCH_YEAR;
		alarm->mode = ONESHOT;
		if(takeCalendarSem()==pdPASS)
		{
			calendar_set_alarm(&CALENDAR_X, alarm, callbackFunc);
			giveCalendarSem();
			return true;
		}
		volatile uint32_t wm = uxTaskGetStackHighWaterMark(NULL);
		UNUSED(wm);
		vTaskDelay(500);
	}
	return false;
}

static void tmToCalendarDateTime(struct tm *tmDate, struct calendar_date_time * dateTime){
	dateTime->date.day		= tmDate->tm_mday;
	dateTime->date.year		= tmDate->tm_year+1900;
	dateTime->date.month	= tmDate->tm_mon+1;
	dateTime->time.hour		= tmDate->tm_hour;
	dateTime->time.min		= tmDate->tm_min;
	dateTime->time.sec		= 0;
}

/* Set alarm interrupt to be triggered when configured alarm time has come */
void RTCManager_setAlarm(struct calendar_alarm *const alarm, calendar_cb_alarm_t callback){
	USARTManager_printUsbWait("ALARM SET AT :");
	RTCManager_printTime(&alarm->data.datetime);
	USARTManager_printUsbWait("\r\n");
	if(takeCalendarSem()==pdPASS)
	{
		calendar_set_alarm(&CALENDAR_X, alarm, callback);
		giveCalendarSem();
	}
}

void RTCManager_setAlarmFromTm(struct tm* tmDatePtr,struct calendar_alarm *alarm, calendar_cb_alarm_t callback){
	tmToCalendarDateTime(tmDatePtr, &(alarm->data.datetime));
	alarm->option = CALENDAR_ALARM_MATCH_YEAR;
	alarm->mode = ONESHOT;
	RTCManager_setAlarm(alarm, callback);
	vTaskDelay(500);
}


void RTCManager_removeSecondsFromTm(struct tm* sourcePtr, struct tm* resultPtr, uint32_t secondsToRemove){
	time_t sourceTimestamp = mktime(sourcePtr);
	time_t resultTimestamp = sourceTimestamp - secondsToRemove;
	gmtime_r(&resultTimestamp, resultPtr);
}

/* Remove scheduled alarm */
bool RTCManager_removeAlarm(struct calendar_alarm *const alarm){
	bool functionResult = true;
	if(takeCalendarSem()==pdPASS)
	{
		if (!is_list_element(&CALENDAR_X.alarms, alarm)){
			functionResult = false;
		}
		else
		{
			list_delete_element(&CALENDAR_X.alarms, alarm);
			if (!list_get_head(&CALENDAR_X.alarms)) {
				_calendar_register_callback(&CALENDAR_X.device, NULL);
			}
		}
		
		giveCalendarSem();
	}
	else
	{
		functionResult = false;
	}
	
	return functionResult;
}

/* Prints a string of the date with the format : DD/MM/YYYY - HH:MM SSs */
void RTCManager_printTime(struct calendar_date_time *dt){
	char smallBuff[5];
	stringHelper_intToString(dt->date.day, (uint8_t*)smallBuff);
	USARTManager_printUsbWait(smallBuff);
	USARTManager_printUsbWait("/");
	stringHelper_intToString(dt->date.month, (uint8_t*)smallBuff);
	USARTManager_printUsbWait(smallBuff);
	USARTManager_printUsbWait("/");
	stringHelper_intToString(dt->date.year, (uint8_t*)smallBuff);
	USARTManager_printUsbWait(smallBuff);
	USARTManager_printUsbWait(" - ");
	stringHelper_intToString(dt->time.hour, (uint8_t*)smallBuff);
	USARTManager_printUsbWait(smallBuff);
	USARTManager_printUsbWait(":");
	stringHelper_intToString(dt->time.min, (uint8_t*)smallBuff);
	USARTManager_printUsbWait(smallBuff);
	USARTManager_printUsbWait(" ");
	stringHelper_intToString(dt->time.sec, (uint8_t*)smallBuff);
	USARTManager_printUsbWait(smallBuff);
	USARTManager_printUsbWait("s");
}


void RTCManager_printTimeFromTimestamp(uint32_t timestamp){
	struct tm currentTime;
	gmtime_r((time_t*)&timestamp, &currentTime);
	char smallBuff[5];
	stringHelper_intToString(currentTime.tm_mday, (uint8_t*)smallBuff);
	USARTManager_printUsbWait(smallBuff);
	USARTManager_printUsbWait("/");
	stringHelper_intToString(currentTime.tm_mon+1, (uint8_t*)smallBuff);
	USARTManager_printUsbWait(smallBuff);
	USARTManager_printUsbWait("/");
	stringHelper_intToString(currentTime.tm_year+1900, (uint8_t*)smallBuff);
	USARTManager_printUsbWait(smallBuff);
	USARTManager_printUsbWait(" - ");
	stringHelper_intToString(currentTime.tm_hour, (uint8_t*)smallBuff);
	USARTManager_printUsbWait(smallBuff);
	USARTManager_printUsbWait(":");
	stringHelper_intToString(currentTime.tm_min, (uint8_t*)smallBuff);
	USARTManager_printUsbWait(smallBuff);
	USARTManager_printUsbWait(" ");
	stringHelper_intToString(currentTime.tm_sec, (uint8_t*)smallBuff);
	USARTManager_printUsbWait(smallBuff);
	USARTManager_printUsbWait("s");
}


static bool leapYear(uint16_t year)
{
	if (year & 3) {
		return false;
		} else {
		return true;
	}
}

static uint32_t getSecsInMonth(uint32_t year, uint8_t month)
{
	uint32_t sec_in_month = 0;

	if (leapYear(year)) {
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

static int32_t convertTimestampToDateTime(uint32_t ts,struct calendar_date_time *dt)
{
	uint32_t tmp, sec_in_year, sec_in_month;
	uint32_t tmp_year   = DEFAULT_BASE_YEAR;
	uint8_t tmp_month  = 1;
	uint8_t tmp_day    = 1;
	uint8_t tmp_hour    = 0;
	uint8_t tmp_minutes = 0;

	tmp = ts;

	/* Find year */
	while(true) {
		sec_in_year =
		leapYear(tmp_year) ? SECS_IN_LEAP_YEAR : SECS_IN_NON_LEAP_YEAR;

		if (tmp >= sec_in_year) {
			tmp  -= sec_in_year;
			tmp_year++;
			} else {
			break;
		}
	}
	/* Find month of year */
	while(true) {
		sec_in_month = getSecsInMonth(tmp_year, tmp_month);

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

/* A utiliser dans le futur ? */
bool RTCManager_isAlarmScheduled(struct calendar_alarm* alarmPtr)
{
	bool functionResult = false;
	if(takeCalendarSem() == pdPASS)
	{
		for(struct list_element *elem = list_get_head(&CALENDAR_X.alarms); elem != NULL; elem = elem->next)
		{
			if((struct list_element*)alarmPtr == elem)
			{
				functionResult = true;
				break;
			}
		}
		giveCalendarSem();
	}
	return functionResult;
}