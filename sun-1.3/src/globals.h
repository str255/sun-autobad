/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
#ifndef __GLOBALSH__
#define __GLOBALSH__
/*******************************************
 GLOBALS 
********************************************/
MAIN double       
  jdays_now,                              // NOW julian days since 1900
  jdays_sunrise, 
  jdays_sunset,     
  jdays_twilight_rise,
  jdays_twilight_set,
  sun_azimuth,
  sun_altitude,
  now_day, 
  now_month, 
  now_year, 
  now_dayOfWeek, 
  now_hour;
MAIN int
  nighttime,
  daytime;

#include "parse_options.h"
MAIN Options sun_options;
#endif
