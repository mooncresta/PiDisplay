#include "PiDisplay.h"
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

void initialiseEpoch()
{
  struct timeval tv ;

  gettimeofday (&tv, NULL) ;
  epochMilli = (uint64_t)tv.tv_sec * (uint64_t)1000    + (uint64_t)(tv.tv_usec / 1000) ;
  epochMicro = (uint64_t)tv.tv_sec * (uint64_t)1000000 + (uint64_t)(tv.tv_usec) ;
}

/*
 * millis:
 *	Return a number of milliseconds as an unsigned int.
 *********************************************************************************
 */

unsigned int millis (void)
{
  struct timeval tv ;
  uint64_t now ;

  gettimeofday (&tv, NULL) ;
  now  = (uint64_t)tv.tv_sec * (uint64_t)1000 + (uint64_t)(tv.tv_usec / 1000) ;

  return (uint32_t)(now - epochMilli) ;
}

/* the current weekday */
int TimeClass::weekday() {
	return (1);
}

/* the weekday for the given time */
int TimeClass::weekday(time_t t) {
	return (1);
}

time_t TimeClass::now() {
	time_t t;
	return (t);
}

int TimeClass::hour() {
        return (1);
}
int TimeClass::hour(time_t t) {
        return (1);
}

int TimeClass::second() {
        return (1);
}
int TimeClass::second(time_t t) {
        return (1);
}

int TimeClass::year() {
        return (1);
}
int TimeClass::year(time_t t) {
        return (1);
}

int TimeClass::minute() {
        return (1);
}
int TimeClass::minute(time_t t) {
        return (1);
}

int TimeClass::month() {
        return (1);
}
int TimeClass::month(time_t t) {
        return (1);
}

int TimeClass::day() {
        return (1);
}

int TimeClass::day(time_t t ) {
        return (1);
}

//// Time Functions

int timerEvaluate(const struct TimerObject on_time, const struct TimerObject off_time, const unsigned int currentTime)
{
  unsigned int start_time = tmConvert_t(Time.year(currentTime), Time.month(currentTime), Time.day(currentTime), on_time.hour, on_time.minute, 0);
  unsigned int end_time = tmConvert_t(Time.year(currentTime), Time.month(currentTime), Time.day(currentTime), off_time.hour, off_time.minute, 0);

  if (start_time < end_time)
  {
    if (start_time < currentTime && currentTime < end_time) return 1;
    else return 0;
  }
  else if (start_time > end_time)
  {
    if (currentTime > start_time || currentTime < end_time) return 1;
    else return 0;
  }
  else return 0;
}

inline time_t tmConvert_t(int YYYY, unsigned short MM, unsigned short DD, unsigned short hh, unsigned short mm, unsigned short ss)  // inlined for speed
{
  struct tm t;
  t.tm_year = YYYY-1900;
  t.tm_mon = MM - 1;
  t.tm_mday = DD;
  t.tm_hour = hh;
  t.tm_min = mm;
  t.tm_sec = ss;
  t.tm_isdst = 0;  // not used
  time_t t_of_day = mktime(&t);
  return t_of_day;
}

// North American Daylight Savings

#ifdef DST_NORTH_AMERICA
bool IsDST(int dayOfMonth, int month, int dayOfWeek)
{
  if (month < 3 || month > 11)
  {
    return false;
  }
  if (month > 3 && month < 11)
  {
    return true;
  }
  int previousSunday = dayOfMonth - (dayOfWeek - 1);
  if (month == 3)
  {
    return previousSunday >= 8;
  }
  return previousSunday <= 0;
}
#endif

#ifdef DST_CENTRAL_EUROPE
bool IsDst(int day, int month, int dayOfWeek)
{
  if (month < 3 || month > 10)
  {
    return false;
  }
  if (month > 3 && month < 10)
  {
    return true;
  }
  int previousSunday = day - dayOfWeek;
  if (month == 3)
  {
    return previousSunday >= 25;
  }
  if (month == 10) return previousSunday < 25;
  {
    return false;
  }
}

#endif
