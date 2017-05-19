/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
#include <math.h>
#include <stdio.h>
#include <strings.h>
#include "startime.h"
#include "angle.h"
//#include "sun_intl.h"

extern const char                     *zodiac[];
extern const char 					startime_string1[];
extern const char 					startime_string2[];
extern const char 					startime_string3[];

/* C based time string writers
 */
const char    *zodiac_sign(extended deg)
{  int  zi;
 	zi = floor(deg/30 + 0.5);
 	zi %= 12;
 	return zodiac[zi];
}

const char    *astrologic_zodiac_sign(extended deg)
{
	int		i = floor(deg + 0.5), zi, j;
	i = i%360;
	
	if (329 <= i && i <= 358)   	zi = 0;
	else
	if (359 <= i || i <=  28)   	zi = 1;
	else
	if ( 29 <= i && i <=  58)   	zi = 2;
	else
	if ( 59 <= i && i <=  88)   	zi = 3;
	else
	if ( 89 <= i && i <= 118)   	zi = 4;
	else
	if (119 <= i && i <= 148)   	zi = 5;
	else
	if (149 <= i && i <= 178)   	zi = 6;
	else
	if (179 <= i && i <= 208)   	zi = 7;
	else
	if (209 <= i && i <= 238)   	zi = 8;
	else
	if (239 <= i && i <= 268)   	zi = 9;
	else
	if (269 <= i && i <= 298)   	zi = 10;
	else
	if (299 <= i && i <= 328)   	zi = 11;
	
	j = (i+1)/30-11;
	if (j<0) j += 12;
	if (j != zi) SysBreakStr("astrologic_zodiac_sign j != zi");
 	return zodiac[zi];
}

void fullTimeString(char* timeString, extended t)
{
	extern 	char			*monthStringArray[];
	extern 	char			*dayStringArray[];
	extended                fd, r_dow = 0., r_day = 0., r_month = 0., r_year = 0., time;
	int                     hours, dow, day, month, year;
	extended                minutes;

    CalendarDays(t, r_day, r_month, r_year, r_dow); 
    //    fd = t;// - 0.5;          
    //    time = floor(fd);
    fd = t-0.5;
    fd = fd - floor(fd);
    hours = floor(fd*24); // dont + 0.5);       // round to nearest hour.
    minutes = (fd*24 - hours)*60.0;
    day = floor(r_day); 
    dow = r_dow; 
    month = floor(r_month);
    year = floor(r_year); 
    sprintf(timeString,"%s %d:%lf %s %d %d",
	    gettext(dayStringArray[dow-1]), hours, minutes, gettext(monthStringArray[month-1]), day, year);
}

void DMYTimeString(char* dateString, extended t)
{
	extern 	char			*monthStringArray[];
	extern 	char			*dayStringArray[];
	extended                r_dow = 0., r_day = 0., r_month = 0., r_year = 0.;
	int                     dow, day, month, year;

   if (t<0) dateString[0]='\0';
    CalendarDays(t, r_day, r_month, r_year, r_dow); 
    day = floor(r_day); dow = r_dow; month = r_month; year = r_year;
	sprintf(dateString,"%s, %s %d %d",
		gettext(dayStringArray[dow-1]), gettext(monthStringArray[month-1]), day, year);
}

// void HTimeString(char* dateString, extended time)
// {
//  extended               fd, day, month, year, dayOfWeek;
 
//  // JulianDays starts at noon, December 31, 1899. 
//  // Thus midnight at greenwich = 0.5
//  //CalendarDays(time, day, month, year, dayOfWeek); 
//  time = UniversalToLocal (time);
//  fd = time -0.5;          
//  time = floor(fd);
//  fd -= time;
//  // fd -= 0.5;                     // midnight = 0GMT is 0.5
//  fd = floor(fd*24 + 0.5);          // round to nearest hour.

//  if (fd == 24 || fd == 0)		   // "Midnight"
//      sprintf(dateString, "%s", startime_string1);
// 	else
//    if (fd ==12)						// "Noon"
//    	 sprintf(dateString, startime_string2 , (float)fd-12);
//    else
//    if (fd > 12)
// 	 sprintf(dateString, "%2.0f  PM", (float)fd-12);
//    else
// 	 sprintf(dateString, "%2.0f  AM", (float)fd);
// }

// void basicHTimeString(char* dateString, short hour)
// {
//    if (hour < 0) hour += 24;
//    if (hour == 24 || hour == 0)		   // "Midnight"
//      sprintf(dateString, "%s", startime_string1);
// 	else
//    if (hour ==12)						// "Noon"
//    	 sprintf(dateString, startime_string2 , (float)hour-12);
//    else
//    if (hour > 12)
// 	 sprintf(dateString, "%2.0f  PM", (float)hour-12);
//    else
// 	 sprintf(dateString, "%2.0f  AM", (float)hour);
// }

/* ld = local day
 * lt = local hour
 * jd = julian day
 * ut = universal time
 * ds = daylight savings
 * tz = time zone (west negative)
 */
void 
universalToLocal(extended &ld, extended &lt, extended jd, extended ut, extended ds, extended tz)
{
  lt = ut + tz + ds;
  if (lt >= 0){
    ld = jd;
  }
  else if (lt >= 24){   // next day
    ld = jd+1;
    lt -= 24;
  }
  else {   // previous day
    ld = jd-1;
    lt += 24; 
  }
}

/* pascal based time writers */
void manualDOWDay(Str32 dateString, extended t)
{
	extern 	char			*monthStringArray[];
	extern 	char			*dayStringArray[];
	char					CdateString[32];
	extended                r_dow = 0., r_day = 0., r_month = 0., r_year = 0.;
	int                     dow, day, month;

    CalendarDays(t, r_day, r_month, r_year, r_dow); 
    day = floor(r_day); dow = r_dow; 
	sprintf(dateString,"%s, %d",
		gettext(dayStringArray[dow-1]), day);
	//	StringCopy(dateString, c2pstr(CdateString));
}

void manualDOWDayMonth(Str32 dateString, extended t)
{
	extern 	char			*monthStringArray[];
	extern 	char			*dayStringArray[];
	char					CdateString[32];
	extended                r_dow = 0., r_day = 0., r_month = 0., r_year = 0.;
	int                     dow, day, month, year;

    CalendarDays(t, r_day, r_month, r_year, r_dow); 
    day = floor(r_day); dow = r_dow; month = r_month; year = r_year;
	sprintf(dateString,"%s, %s %d %d",
		gettext(dayStringArray[dow-1]), gettext(monthStringArray[month-1]), day, year);
	//	StringCopy(dateString, c2pstr(CdateString));
}

// void manualDayMonthYear(Str32 dateString, DateTimeRec *date, short day, short month, short year)
// {
// 	extern 	char			*monthStringArray[];
// 	char					CdateString[32];
//     date->day   = day;
// 	date->month = month;
// 	date->year = year; 
// 	sprintf(dateString,"%d %s %d",
// 				date->day, monthStringArray[date->month-1], date->year);
// 	//	StringCopy(dateString, c2pstr(CdateString));
// }

// void  manualWeekMonthYear(Str32 dateString, DateTimeRec *date, short day, short month, short year)
// {
// 	extern 	char			*monthStringArray[];
// 	char					CdateString[64];
//     date->day   = day;
// 	date->month = month;
// 	date->year = year; 
	
// 	// "Week of %d %s %d"
// 	sprintf(CdateString, startime_string3,
// 				date->day, monthStringArray[date->month-1], date->year);
// 	//if (strlen(CdateString) > 32) CdateString[31] = '\0';
// 	StringCopy(dateString, c2pstr(CdateString));
// }
// void  manualDOWMonthYear(Str32 dateString, DateTimeRec *date, short day, short month, short year)
// {
// 	extern 	char			*monthStringArray[];
// 	char					CdateString[52];
// 	extended                jd1, r_dow = 0., r_day = day, r_month = month, r_year = year;
// 	int                     dow;

// 	jd1 = JulianDays(r_day, r_month, r_year);
//     CalendarDays(jd1, r_day, r_month, r_year, r_dow); 
//     day = floor(r_day); dow = r_dow; month = r_month;
//     date->day   = day;
// 	date->month = month;
// 	date->year = year; 
// 	sprintf(CdateString,"%ss of %s %d",
// 				dayStringArray[dow-1], monthStringArray[date->month-1], date->year);
// 	StringCopy(dateString, c2pstr(CdateString));
// }

extended
timeTohours(extended time)
{
extended                      fd = time -0.5;          
 time = floor(fd);
 fd -= time;
 // fd -= 0.5;                     // midnight = 0GMT is 0.5
 fd = floor(fd*24 + 0.5);       // round to nearest hour.
 return fd;
 }

// void manualMonthYear(Str32 dateString, DateTimeRec *date, short month, short year)
// {
// 	extern 	char			*monthStringArray[];
// 	char					CdateString[32];

// 	date->month = month;
// 	date->year = year; 
// 	sprintf(CdateString,"%s %d",
// 				monthStringArray[date->month-1], date->year);
// 	StringCopy(dateString, c2pstr(CdateString));
// }

// void manualYear(Str32 dateString, DateTimeRec *date, short year)
// {
// 	extern 	char			*monthStringArray[];
// 	char					CdateString[32];

// 	date->month = 1;
// 	date->year = year; 
// 	sprintf(CdateString,"%d", date->year);
// 	StringCopy(dateString, c2pstr(CdateString));
// }

// void machineMonthYear(Str32 dateString, DateTimeRec *date)
// {
// 	extern 	char			*monthStringArray[];
// 	unsigned long 			seconds;
// 	;
// 	char					CdateString[32];

// 	GetDateTime(&seconds);
// 	SecondsToDate(seconds, date);
// 	sprintf(CdateString,"%s %d",
// 				monthStringArray[date->month-1], date->year);
// 	StringCopy(dateString, c2pstr(CdateString));
// }

int whichHour(extended t)
{
 t -= 0.5;
 t =  FRAC(t);
 t *= 24.0;
 t += tinyEpsilon;
 if (t < 0) t += 24;
 if (t >= 24) t -= 24;
 return floor( t );
}

extended whichDay(extended t)
{
return floor(t-0.5+tinyEpsilon);
}

// sunday is day 1 and saturday is day 7.
// [dow-1]
void GetSunday(extended time, extended &sunday_time)
{
 extended this_day, this_month, this_year, this_dow;

 CalendarDays(time, this_day, this_month, this_year, this_dow);
 sunday_time = time - this_dow + 1;
}

// truncated integer function. part of Apple SANE numerics.
// Metrowerks adaptation.
extended rint(extended x)
{ 
  //	extended80 t = x;
  //	Rint(&t); 
  x = trunc(x);
  return x;
}

extended JulianDays00to04(extended jd)
{
 return jd - 4*365 + 0.125;
}

// this fudge factor is used in siderealtime in startime.cp
extended JulianDays04to00(extended jd)
{
 return jd + 4*365 - 0.125;
}

extended JulianDays1904(extended jd)
{
 return  JulianDays00to04(JulianDays1900(jd));
}

extended JulianDays1900(extended jd) 
{
 return jd - 2415020;
}

/* in our system we work with a modified 
   julianDay from 1900
   */
extended trueJulianDays(extended jd)
{
 return jd + 2415020;
}

// used on U.S. Naval observatory ephemeris 
// jd is julian date 1900
// ut is universal time in hours
extended centuries_j2000(extended jd) 
{ 
  extended t;
  jd = trueJulianDays(jd);
  t = (jd - 2451545.0)/ 36525.0;
  return t;
}

// julianDays from 1900
// JulianDays starts at noon, December 31, 1899. 
// this is the leading time hog.
// DAY = MIDDAY (0.5)
void CalendarDays(extended julianDay, 	
	extended &day, extended &month, extended &year, extended &dayOfweek)
{ 
 extended 		a,b,c,d,fd,i,g;
 // char				buffer[80];
 
d = julianDay + 0.5;
i = floor(d);
fd = d - i;
if (fabs(fd-1.0)<tinyEpsilon){ fd = 0.0; i += 1.0; }
if (i > -115860){
  a = floor((i/36524.25) + 9.9835726e-1) + 14;
  i = i + 1 + a - floor(a/4.0);
 }
b = floor((i/365.25) + 8.02601e-1);
c = i - floor((365.25*b)+7.50001e-1) + 416;
g = floor(c/30.6001);

day = floor(c - floor(30.6001 * g) + fd);

if (g>13.5) 
month = g - 13;
else
month = g - 1;
if (month > 2.5)
year = b + 1899;
else
year = b + 1900;
//if (year < 1) year -= 1;
julianDay += 2415020;
b = rint((julianDay+1.5)/7)*7;
dayOfweek = rint(julianDay + 1.5  - b) + 1;
// sprintf(buffer, "b %f  julianDay %f  dow %f\n", (float)b, (float)julianDay, (float)dayOfweek);
// SysBreakStr(c2pstr(buffer));
}

// julianDays from 1900
// this is the leading time hog.
// DAY = MIDDAY (0.5)
void CalendarDays_fraction(extended julianDay, 	extended &day)
{ 
 extended a,b,c,d,fd,i,g;

d = julianDay + 0.5;
i = floor(d);
fd = d - i;
if (fabs(fd-1.0)<tinyEpsilon){ fd = 0.0; i += 1.0; }
if (i > -115860){
  a = floor((i/36524.25) + 9.9835726e-1) + 14;
  i = i + 1 + a - floor(a/4.0);
 }
b = floor((i/365.25) + 8.02601e-1);
c = i - floor((365.25*b)+7.50001e-1) + 416;
g = floor(c/30.6001);

day = c - floor(30.6001 * g) + fd;
}

/* given a calendar date convert to internal julian days
   format absolute
   MONTH > 0 NO MONTH 0.
 */
extended JulianDays(extended day, int month, int year)
{
 extended a,b,c,d;

if (month <= 0)
	SysBreakStr("JulianDays non-positive month");
	
// SetRound(TowardZero);
 if (year == 0){
    SysBreakStr("JulianDays no year zero");
 }
 if (year < 1)  year = year + 1;
 
 if (month < 3){
	year = year -1;
	month = month + 12;
 }

a = floor(year/100);
if ((year < 1582) || 
    (year == 1582 && month < 10) ||
	(year == 1582 && month == 10 && day < 5)) 
	b = 0;
else 
    b = 2 - a + floor(a/4);
if (year < 0.0) 
	c = rint((365.25 * year) - 0.75) - 694025;  // truncated integer.
else 
	c = floor(365.25 * year) - 694025;
d = floor(30.6001 * (month + 1));

return  b + c + d + day - 0.5;
}

/* sixth week is last week. 
  dow   = day of week
  wom  = week of month
  dom   = day of month
 */
extended JulianDaysDOW(extended dow, int wom, int month, int year)
{
 extended			 jd0, jd1; 
  extended 		dow0;
  extended 		fday, fmonth, fyear, fdow;
  extended 		dom;                    
     
  // find dow of first day of month
  jd0 = JulianDays(1, month, year);
 CalendarDays(jd0, fday, fmonth, fyear, dow0);  // 6th week 
 
  // calculate day of month
  // and the julian from this
  dom = (wom - 1) * 7 + dow - dow0 + 1;

  jd1 = JulianDays(dom, month, year);

 if  (wom != 6)   return jd1;                   // regular weeks 
//  if (dom > 31 || dom < 0) 
  
CalendarDays(jd1, fday, fmonth, fyear, fdow);  // 6th week 

// Now keep going back until in the month
while (fmonth != month && dom > 0)  {
 	 dom -= 7;   							/* go back seven days */
	 jd1 = JulianDays(dom, month, year);
 	CalendarDays(jd1, fday, fmonth, fyear, fdow);  // 6th week 
}
 
return jd1;
}

// extended MachineTimeToJulianDays(unsigned long seconds)
// {
//  DateTimeRec 			date;
//  SecondsToDate(seconds, &date);
//  return JulianDays04to00(JulianDays((extended)date.day, 
//  	(extended)date.month, (extended)date.year));
// }

/* input is number of seconds since midnight, January 1, 1904
   Based on Duffett-Smith's book Astronomy with your personal computer.
 */
// extended sidereal_time(unsigned long seconds)
// {
//  DateTimeRec 			date;
//  extended				t, r0, r1, t0; // sg;
//  extended				universal_hours;
//  extended				jd1;
 
//  SecondsToDate(seconds, &date);
 
// 	jd1 = JulianDays(date.day, date.month, date.year);
// 	t = ( jd1 ) / 36525.0  - 1;
//  	r0  =  (5.13366e-2 + (2.586222e-5 - 1.722e-9 * t) * t ) * t;
// 	r1  =   6.697374558 + (2400 * (t - ((date.year - 2000.)/100.)));
// 	t0 = r0 + r1;
// 	if (t0 > 24.0) {
// 		t0 = fmod(t0, 24.0);					
// 		t0 += 6.57098244e-2;
// 	}
// 	else 
// 	if (t0 < 0.0) {
// 		t0 = fmod(t0, 24.0);						
// 		t0 -= 6.57098244e-2;
// 	}
// 	t = ((date.second)/60.0 + date.minute)/60.0 + date.hour - GetGmtDeltaHours();

// 	// sidereal + universal = greenwich gst
// 	t = t * 1.002737908 + t0;     
	  			
//     return fmod(t, 24.0);
// }

int dayOfYear(extended julianDay)
{
  extended day, month, year, dow, doy;
  CalendarDays(julianDay, day, month, year, dow);
  doy = julianDay - JulianDays(1, 1, year);
  return (int)doy;
}

extended sidereal_timeJD(extended jd1, extended ut)
{
 extended					t, r0, r1, t0, sg;
 extended 				day, month, year, dayOfweek;

  CalendarDays(jd1, day, month, year, dayOfweek);

   t = ( jd1 ) / 36525.0  - 1;
  r0  =  (5.13366e-2 + (2.586222e-5 - 1.722e-9 * t) * t ) * t;
  r1  =   6.697374558 + (2400 * (t - ((year - 2000.)/100.)));
  t0 = r0 + r1;
  if (t0 > 24.0) {
		t0 = fmod(t0, 24.0);					
		t0 += 6.57098244e-2;
  }
 else 
  if (t0 < 0.0) {
		t0 = fmod(t0, 24.0);						
		t0 -= 6.57098244e-2;
  }

    // sidereal + universal = greenwich gst
    sg = ut * 1.002737908 + t0;     
    sg = fmod(sg, 24.0);			
    return sg;
}

extended sidereal_time_localJD(extended jd1, extended ut, extended  gl)
{
    extended sg;
    sg =  sidereal_timeJD(jd1,  ut);
    return sg + (gl/15);
}

/*extended hrang(extended x, extended sg, extended gl)
{
  extended     	a;
  a = sg + gl/15.0;
  a = fmod(a, 24.0);
  a -= x;
  return a;
}*/

/*  ALL UNITS ARE RADIANS  and distance in EARTH RADII
  true to apparent
	 topocentric apparent hour angle p 
					declination q
		<--- geocentric hour angle x
	                   		declination y
	geographic latitude gp
	height above sealevel ht
	horizontal parallax hp
 */
void parallax(extended &p, extended &q, extended gp, extended ht, extended x, extended y, extended hp, Boolean internalMode)
{
  extended 		c1, s1, u, c2, s2, rs, rc, rp, cx, sy, cy, a, dx, cp;
  extended                   tp = 6.283185308;

if (internalMode == false){
  gp = deg2rad(gp);						// convert to radians
  ht = ht / 6378140.0; 					// convert to earth radii
  x  = tp/24.00 * x;                                                      // convert to radians
  y  = deg2rad(y);                                                     // convert to radians
  hp = deg2rad(hp);					// convert to radians
  }
  
  c1 = cos(gp);
  s1 = sin(gp);
  u = atan(9.96647e-1 * s1 / c1);
  c2  = cos(u);
  s2 = sin(u);
  rs = (9.96647e-1 * s2) + ht * s1;
  rc = c2 + (ht * c1);
  rp = 1/sin(hp);
  cx = cos(x);
  sy = sin(y);
  cy = cos(y);
  a = (rc * sin(x))/((rp * cy) - (rc * cx));
  dx = atan(a);
  p = x + dx;
  cp = cos(p);
  p = fmod(p, tp);
  if (p < 0.0) p += tp;
  q = atan(cp * (rp * sy - rs)/(rp * cy * cx - rc));
  if (q < 0.0) q += tp;
if (internalMode == false){
  p = 24.00/tp * p;                         // convert to hour angles
  q = 360.00/tp * q;			// convert to raidans
  }
}

#if 0
void print_time_string(Str255 time_string, extended time)
{
 int  hours, minutes, seconds;
 char* p = (char *)&time_string[1];
 
 compute_degrees( time, &hours, &minutes, &seconds);
 sprintf(p, "%2.2d:%2.2d:%2.2d", hours, minutes, seconds);
 time_string[0] = 9;
}
#endif


