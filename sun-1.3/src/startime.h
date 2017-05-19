/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
// Copyright (c) 2016 Nicholas C. Strauss
// This file is part of STARTIME.
//
// STARTIME is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// STARTIME is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with STARTIME.  If not, see <http://www.gnu.org/licenses/>.
//					   
#ifndef __STARTIME_STARTIME_
#define __STARTIME_STARTIME_
#include <sys/time.h>
#include "machineLocation.h"
#include "sun_intl.h"

typedef unsigned char Boolean;
typedef double extended;
typedef char Str15[16];
typedef char Str32[32];
typedef char Str255[255];
typedef struct timeval DateTimeRec;
typedef unsigned char *Handle;
char *c2pstr(char *p);
void SysBreakStr(const char *error);

const char *zodiac_sign(extended deg);
const char    *astrologic_zodiac_sign(extended deg);
void fullTimeString(char* timeString, extended t);
void DMYTimeString(char* dateString, extended t);
void HTimeString(char* dateString, extended time);
void basicHTimeString(char* dateString, short hour);
void manualDOWDay(Str32 dateString, extended t);
void manualDOWDayMonth(Str32 dateString, extended t);
void  manualDOWMonthYear(Str32 dateString, DateTimeRec *date, 
	short day, short month, short year);
void manualDayMonthYear(Str32 dateString, DateTimeRec *date, 
	short day, short month, short year);
void manualWeekMonthYear(Str32 dateString, DateTimeRec *date, 
	short day, short month, short year);
void manualYear(Str32 dateString, DateTimeRec *date, short year);
void manualMonthYear(Str32 dateString, DateTimeRec *date, short month, short year);
void machineMonthYear(Str32 dateString, DateTimeRec *date);
void machineYear(Str32 dateString, DateTimeRec *date);
void GetMachineLocalDate(DateTimeRec *date);
long GetGmtDelta(machineLocation* myLocation);
extended GetGmtDeltaHours();
//extended LocalToUniversal(extended t); -ncs deprecated Thu Jun 23 14:11:50 PDT 2016
//extended UniversalToLocal(extended t); -ncs deprecated Thu Jun 23 14:11:50 PDT 2016
void universalToLocal(extended &ld, extended &lt, extended jd, extended ut, extended ds, extended tz); 
extended MachineTimeToJulianDays(unsigned long seconds);
extended JulianDays(extended day, int month, int year);
extended JulianDaysDOW(extended dayOfWeek, int weekOfMonth, int month, int year);
extended JulianDays1900(extended jd);
extended JulianDays1904(extended jd);
extended JulianDays00to04(extended jd);
extended JulianDays04to00(extended jd);
extended trueJulianDays(extended jd);
extended centuries_j2000(extended jd); 
void GetSunday(extended time, extended &sunday_time);
void CalendarDays(extended julianDay, 	
	extended &day, extended &month, extended &year, extended &dayOfweek);
void CalendarDays_fraction(extended julianDay, 	extended &day);
int dayOfYear(extended julianDay);
extended sidereal_time(unsigned long seconds);
extended sidereal_timeJD(extended jd1, extended ut);
extended sidereal_time_localJD(extended jd1, extended ut, extended  gl);
extended hrang(extended x, extended sg, extended gl);
void parallax(extended &p, extended &q, extended gp, extended ht, extended x, extended y, extended hp, Boolean internalMode);
extended whichDay(extended t);
int whichHour(extended t);
extended timeTohours(extended time);  // use whichHour instead 
void extendedTime(Str32 dateString, extended time);
void print_time_string(Str255 time_string, extended time);
void DialogSetHMS(extended coordinate, Handle _hours, Handle _minutes, Handle _seconds);
void DialogGetHMS(extended& coordinate, Handle _hours, Handle _minutes, Handle _seconds);
#define FRAC(a) ((a)-floor((a)))
#define tinyEpsilon      0.000001
// extended rint(extended x);


//void X2reading(DF_ReadingP reading, extended value); // X stands for extended.
//extended reading2X(DF_ReadingP reading);
#endif
