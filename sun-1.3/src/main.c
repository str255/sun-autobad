/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
/*
 * main.c file
 */

#include "expression.h"
#include "location_expression.h"
#include "evaluate.h"
#include "parser.h"
#include "lexer.h"
#include <time.h>
#include <stdio.h>
#include <libintl.h>
#include <locale.h>
#include "startime.h"
#include "sun.h"
#include <math.h>
#include "angle.h"
#include "main.h"
#define MAIN
#include "globals.h"
#include "equationOfTime.h"
#include <getopt.h>
#include "parse_options.h"
#include "self_test.h"

int main(int argc, char *argv[])
{
  SExpression *e = NULL;
  locationExpression *l = NULL;
  char        *errMsg;
  //  sqlite3                  *db;
  int                       success;

  // GNU LOCALE/GETTEXT SUPPORT
  setlocale (LC_ALL, "");
  //  bindtextdomain (PACKAGE, LOCALEDIR);
  //  textdomain (PACKAGE);
  bindtextdomain ("sun", getenv("PWD")); // "/usr/share/locale/");
  textdomain ("sun");
  
  initialize_cvalues();

  /* OPTIONS */
  default_options();
  /* argc argv */
  if (argc > 1){
    printf("argv %s %s\n", argv[0], argv[1]);
    parse_options(argc, argv);
  }

  //  printf("\t daylight=%d\n",  daylight);

  /* FINALIZE LOCATION AND OTHER OPTIONS*/
  l = getLocation(sun_options.location);
  if (l)
    printf("Result of '%s' is (%lf,%lf,%lf)\n", sun_options.location, l->lat, l->lon, l->elevation);
  else
    printf("%s no location specified\n", sun_options.location);
  sun_options.latlon = l;
  if (sun_options.self_verbose) print_options();
  
  /* SET NOW */
  extended dj, dj00, lt = 0, tzi, ds, gp, gl, msl_km, alt, az;
  tzi = sun_options.timezone;  // hours west negative 
  ds  = sun_options.daylight;   // hours summer positive
  gp = sun_options.latlon->lat;
  gl = sun_options.latlon->lon;
  msl_km = sun_options.latlon->elevation;
  dj = get_now_jdays();
  // set again after guessing DST
  ds  = sun_options.daylight;   // hours summer positive

  /* SET SUNRISE / SUNSET */
  get_now_sunrise_sunset(dj, tzi, ds, gp, gl, msl_km);

  /* SET TWILIGHT RISE / SET */
  get_now_twilight_rise_set(dj, tzi, ds, gp, gl, msl_km);

  /* SET SUN ALT AZ */
  dj00 = floor(jdays_now)+0.5;    // 0.5 is 00UT
  lt = (jdays_now - dj00)*24.0;// +tzi +ds;
  sun_alt_az(dj00, lt, tzi, ds, gp, gl, msl_km, az, alt);
  sun_azimuth = az;                                         // globals
  sun_altitude = alt;                                       // globals
  printf(_("Sun's azimuth=%lf degrees clockwise from north and Altitude=%lf degrees above horizon\n"), az, alt);
  
  /* PROCESS QUERY */
  if (sun_options.mode_query) return do_query();

  /* SELF TEST */
  if (sun_options.self_test) do_self_test();
  return 0;
}


double get_now_jdays()
{
  char   timestring[255];
  struct timeval            tv;
  struct timezone           tz;
  /*  time_t  */
  unsigned long long                   // used this for raspberry pi compat
    tv_sec,                            // seconds since 1970
    seconds;                           // seconds since 1900
  extended                  ds,
    now_day, now_month, now_year, now_dayOfWeek;
  // 1970 UNIX TIME
  //  UNIX systems represent time in seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC)  
  gettimeofday(&tv, &tz);
  seconds = tv.tv_sec;

  printf("***************************\n");
  printf(_("    ____   ___  _ _ _\n"
	   "   |  _ \\ / _ \\| | | |\n"
           "   | | | | |_| | | | |\n"
           "   |_| |_|\\___/ \\___/\n"
           "\n"));
  printf(_("Epoch defined as 1970-01-01 00:00:00 +0000 (UTC)\n"));
  printf(_("Epoch seconds (1970)\t%ld\n"), (unsigned long)seconds);
  // seconds from Jan 1, 1900 until Jan 1, 1970 =  2208988800L
  seconds += 2208988800ULL;
  printf(_("Julian seconds (1900)\t%ld\n"), (unsigned long)seconds);

  // JulianDays1900 starts at noon, December 31, 1899. 
  // Thus midnight at greenwich = 0.5
  // days = 24*60*60 seconds
  jdays_now = (extended)seconds / ((extended)24.0*60.0*60.0) + 0.5; // 0.5 is twelve hours from noon to midnight

  /* guess at daylight savings */
  if (sun_options.self_guess_daylight_savings == true){
    CalendarDays(jdays_now, now_day, now_month, now_year, now_dayOfWeek);
    ds = guess_daylight_savings(jdays_now, now_year, 1);
    printf(_("Guessing Daylight Savings (hours) DST=%lf \n"), ds);
    sun_options.daylight = ds;
  }

  //  printf("Julian seconds (1900) \t%ld\n", (unsigned long)seconds);
  //  The Julian date for CE  1899 December 31 12:00:00.0 UT is
  //  JD 2415020.000000
  fullTimeString(timestring, jdays_now);
  printf(_("NOW Universal %s"), timestring);
  fullTimeString(timestring, jdays_now+(sun_options.timezone+sun_options.daylight)/24.0);          // local time
  printf(_("  Julian days %lf\n"), jdays_now + 2415020.000000);
  printf(_("NOW Local %s\n"), timestring);
  printf(_("NOW Julian days (1900) \t%lf\n"), jdays_now);
  printf("***************************\n");
  return jdays_now;
}


void 
get_now_sunrise_sunset(extended dj, 
			      extended tzi, 
			      extended ds,
			      extended gp, 
			      extended gl, 
			      extended msl_km)
{
  extended    dj00, hour;
  int         ihours, iminutes, iseconds;
  extended    au, ad, utu, dju, utd, djd;
  int         never_rise, never_sets;
  char        timestring[255];

#if 0
  dj = jdays_now; //floor(jdays_now)+0.5;    // 0.5 is 00UT
  tzi = -8;                   // nevada  west negative
  ds = 0;                     // daylight savings
  gp = 36.048152;
  gl = -114.951;
  gp = 36.1666;
  gl = -115.1666;
  msl_km = 0;
#endif

  hour = (jdays_now - floor(dj)-0.5)*24.0;
  dj00 = floor(jdays_now)+0.5;    // 0.5 is 00UT
  sun_calculate_rise_tilnow(utu, dju, utd, djd, dj00, tzi, ds, gl, gp, jdays_now, hour, msl_km, never_rise, never_sets, au, ad );

  jdays_sunset = djd + utd/24.0;
  jdays_sunrise = dju + utu/24.0;

  //  printf("Julian days (1900) %lf\n", jdays_now);
  //  printf("Julian days %lf\n", jdays_now+2415020.000000);
  //  printf("*************************************\n");

  if (jdays_sunset > jdays_sunrise){
    printf(_("SUNSET > SUNRISE. (NIGHT)\n"));
    nighttime = true;
    daytime   = false;

    printf(_("          (_)     | |     _                  \n"
"     ____  _  ____| |__ _| |_                 \n"
"    |  _ \\| |/ _  |  _ (_   _)                \n"
"    | | | | ( (_| | | | || |_                 \n"
"    |_| |_|_|\\___ |_| |_| \\__)       \n"
"            (_____|                           \n"
	     "\n"));

    fullTimeString(timestring, dju+(utu+tzi+ds)/24.);
    printf(_("SUNRISE local day %s"), timestring);
    printf(_("  Julian %lf (hours UT=%lf )\n"),  utu/24.+dju + 2415020.000000, utu);
    fullTimeString(timestring, djd+(utd+tzi+ds)/24.);
    printf(_("SUNSET local day %s"), timestring);
    printf(_("  Julian %lf (hours UT=%lf ) \n"),  utd/24.+djd + 2415020.000000, utd);
  }
  else
  if (jdays_sunset < jdays_sunrise){
    printf(_("SUNRISE > SUNSET. (DAY)\n"));
    nighttime = false;
    daytime   = true;
    printf(_("     _\n"
"    | |\n"
"  __| |_____ _   _\n"
" / _  (____ | | | |\n"
"( (_| / ___ | |_| |\n"
" \\____\\_____|\\__  |\n"
	     "            (____/\n"));
    fullTimeString(timestring, djd+(utd+tzi+ds)/24.);
    printf(_("SUNSET local day %s"), timestring);
    printf(_(" Julian %lf (hours UT=%lf)\n"),  utd/24.+djd + 2415020.000000, utd);
    fullTimeString(timestring, dju+(utu+tzi+ds)/24.);
    printf(_("SUNRISE local day %s"), timestring);
    printf(_(" Julian %lf (hours UT=%lf)\n"), utu/24.+dju + 2415020.000000, utu);
  }
			 
  printf("*************************************\n");
  //  return jdays_sunset;
}

void
get_now_twilight_rise_set(double dj, 
			      double tzi, 
			      double ds,
			      double gp, 
			      double gl, 
			      double msl_km)
{
  extended    dj00, hour;
  int         ihours, iminutes, iseconds;
  extended    au, ad, utu, dju, utd, djd, rad_twilight;
  int         never_rise, never_sets;
  char        timestring[255];


  hour = (jdays_now - floor(dj)-0.5)*24.0;
  dj00 = floor(jdays_now)+0.5;    // 0.5 is 00UT
  switch (sun_options.mode_twilight){
  case CIVIL_TWILIGHT:
    rad_twilight = 1.047198e-1;      // civil
    break;
  case NAUTICAL_TWILIGHT:
    rad_twilight = 2.094395e-1;  //nautical
    break;
  case ASTRONOMICAL_TWILIGHT:
    rad_twilight = 3.141593e-1;  //astronomical
    break;
  case GENERAL_TWILIGHT:
    rad_twilight = sun_options.twilight_angle;
    break;
  default:
    rad_twilight = 1.047198e-1;      // civil
    break;
  }
  sun_calculate_twilight_tilnow(utu, dju, utd, djd, dj00, tzi, ds, gl, gp, jdays_now, hour, msl_km, never_rise, never_sets, au, ad, rad_twilight);
  // REGISTER GLOBALS JD..RISE JD..SET
  jdays_twilight_set = djd + utd/24.0;
  jdays_twilight_rise = dju + utu/24.0;

  // FROM HERE VERBOSE PRINT
  //  if (sun_options.self_verbose == false) return;

  // NOW 
  //  printf("Julian days (1900) %lf\n", jdays_now);
  //  printf("Julian days %lf\n", jdays_now+2415020.000000);
  // TWILIGHT RISE
  fullTimeString(timestring, dju+(utu+tzi+ds)/24.);
  printf(_("Twilight rise local day %s"), timestring);
  printf(_("\tJulian %lf (hours UT=%lf)\n"), utu/24.+dju + 2415020.000000, utu);
  // TWILIGHT SET
  fullTimeString(timestring, djd+(utd+tzi+ds)/24.);
  printf(_("Twilight set local day %s"), timestring);
  printf(_("\tJulian %lf (hours UT=%lf)\n"),  utd/24.+djd + 2415020.000000, utd);
  // twilight_rise > sunrise --> in twilight rise
  // twilight_set > sunset --> in twilight set
  ///day  if (jdays_sunset > jdays_sunrise){ 
  //nigth    if (jdays_sunset > jdays_sunrise){
  // end of day 
  /* if ((daytime && jdays_twilight_set > jdays_sunrise) ||  */
  /*     (nighttime && jdays_twilight_rise > jdays_sunset)){ */
  /*   printf("                 _ _  _       _         \n"); */
  /*   printf("       _        (_) |(_)     | |     _  \n"); */
  /*   printf("     _| |_ _ _ _ _| | _  ____| |__ _| |_  \n"); */
  /*   printf("    (_   _) | | | | || |/ _  |  _ (_   _) \n"); */
  /*   printf("      | |_| | | | | || ( (_| | | | || |_ \n"); */
  /*   printf("      \\__) \\___/|_|\\_)_|\\____|_| |_|\\__)  \n"); */
  /*   printf("			(____|            \n"); */
  /*   printf("\n"); */
  /* } */
}

bool do_query()
{
  return logic_withcheck(sun_options.query);
}

