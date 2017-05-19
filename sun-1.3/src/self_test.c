/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
#include "expression.h"
#include "location_expression.h"
#include "evaluate.h"
#include "parser.h"
#include "lexer.h"
#include <time.h>
#include <stdio.h>
#include "startime.h"
#include "sun.h"
#include <math.h>
#include "angle.h"
#include "main.h"
#define MAIN extern
#include "globals.h"
#include "equationOfTime.h"
#include <getopt.h>
#include "parse_options.h"
#include "self_test.h"

SExpression *getAST(const char *expr)
{
  SExpression            *expression;
  locationExpression     *lexpression;
  yyscan_t                scanner;
  YY_BUFFER_STATE         state;
  int                     success;

  if (yylex_init(&scanner)) {
    // couldn't initialize
    return NULL;
  }

  state = yy_scan_string(expr, scanner);

  success = yyparse(&expression, &lexpression, scanner);
  if (success) {
    // error parsing
    printf("parse error on %s\n", expr);
    return NULL;
  }

  yy_delete_buffer(state, scanner);

  yylex_destroy(scanner);

  return expression;
}

bool
logic_withcheck(char const *test)
{
  SExpression *e = getAST(test);
  double result = evaluate(e);
  deleteExpression(e);
  if (isnan(result)){
    printf("Result of '%s' is FALSE\n", test);// result, result+2415020.000000);
    return false;
  }
  printf("Result of '%s' is TRUE\n", test); // result, result+2415020.000000);  
  return true;
}

locationExpression *getLocation(const char *expr)
{
  SExpression *expression;
  locationExpression *lexpression;
  yyscan_t scanner;
  YY_BUFFER_STATE state;
  int             success;

  if (yylex_init(&scanner)) {
    // couldn't initialize
    return NULL;
  }

  state = yy_scan_string(expr, scanner);

  success = yyparse(&expression, &lexpression, scanner);
  if (success) {
    // error parsing
    printf("getLocation parse error %s error code=%d\n", expr, success);
    return NULL;
  }

  yy_delete_buffer(state, scanner);

  yylex_destroy(scanner);

  return lexpression;
}

void do_self_test()
{
  bool        check = true;
  double result = 0;
  //  char test[]="sunrise<now<sunset";
  char test[]="3.1<3.116<3.12";
  char test2[]="114W33.1245 43N32 2.4MSL";
  char test_location[]="114~33.1245W 43~32N 2400ft";
  char test_location2[]="114.5W 43.6N 2400ft";
  char test_location3[]="114~33'33.1245W 43~32'22.11N 2400ft";
  SExpression *e = NULL;
  locationExpression *l = NULL;
  struct timeval            tv;
  struct timezone           tz;
  time_t                    
    tv_sec,                            // seconds since 1970
    seconds;                           // seconds since 1900
  l = getLocation(test_location);
  printf("LOCATION '%s' is latitude=%lf longitude=%lf elevation=%lf\n", test_location, l->lat, l->lon, l->elevation);

  l = getLocation(test_location2);
  printf("LOCATION2 '%s' is latitude=%lf longitude=%lf elevation=%lf\n", test_location2, l->lat, l->lon, l->elevation);

  l = getLocation(test_location3);
  printf("LOCATION2 '%s' is latitude=%lf longitude=%lf elevation=%lf\n", test_location3, l->lat, l->lon, l->elevation);

  e = getAST(test);

  result = evaluate(e);

  printf("Result of '%s' is %f\n", test, result);

  deleteExpression(e);

  check &= logic_withcheck("3.1<3.115<3.12");
  check &= logic_withcheck("+100.0>99.0>60.0>-100.0");
  check &= logic_withcheck("4<5<6<7<8");
  check &= logic_withcheck("4<5>3");
  check &= logic_withcheck("4<5&3>2");
  check &= logic_withcheck("3.1<3.115<3.12");  
  check &= logic_withcheck("sunrise<sunrise+(1/24)");
  check &= logic_withcheck("sunset<sunset+(1/24)");
  check &= logic_withcheck("sunset<now");
  check &= logic_withcheck("sunrise<now");      
  check &= logic_withcheck("1*24");  
  check &= logic_withcheck("1/24");  
  check &= logic_withcheck("jan<feb<mar<apr<may<jun<jul<aug<sep<oct<nov<dec");
  if (check) printf("LOGIC CHECKS PASSED\n");
  else  printf("LOGIC CHECKS FAILED\n");
  // true astonomical reckoned julian days 1 Jan 4713 BC (1/1/-4712)
  extended check_day, check_month, check_year, check_dayOfWeek;
  CalendarDays(-2415020, check_day, check_month, check_year, check_dayOfWeek);
  printf("check: day %lf month %lf year %lf dow %lf\n",
	 check_day, check_month, check_year, check_dayOfWeek);	 
  
  // 1970 UNIX TIME
  //  UNIX systems represent time in seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC)  
  gettimeofday(&tv, &tz);
  printf("gettimeofday timezone info minutes west of Greenwich %d dst %d\n",
	 tz.tz_minuteswest,
	 tz.tz_dsttime);
  seconds = tv.tv_sec;
  printf("Epoch defined as 1970-01-01 00:00:00 +0000 (UTC)\n");
  printf("\tEpoch seconds (1970)\t%ld\n", seconds);
  // seconds from Jan 1, 1900 until Jan 1, 1970 =  2208988800L
  seconds += 2208988800UL;

  // JulianDays1900 starts at noon, December 31, 1899. 
  // Thus midnight at greenwich = 0.5
  // days = 24*60*60 seconds
  extended jdays_now = (extended)seconds / ((extended)24.0*60.0*60.0) + 0.5;
  printf("\tJulian 1900 seconds (1900) \t%ld\n", seconds);
  printf("\tJulian 1900 days \t%lf\n", jdays_now);


  now_hour = (jdays_now - 0.5 - floor(jdays_now-0.5)) * 24.0;
  printf("check hour %lf\n", now_hour);

  CalendarDays(jdays_now, now_day, now_month, now_year, now_dayOfWeek);
  printf("TODAY %d %d %d %d DOW %d\n",
	 (int)jdays_now, (int)now_day, (int)now_month, (int)now_year, (int)now_dayOfWeek);

  extended    dj, tx, ds, gp, gl, msl_km;
  int         ihours, iminutes, iseconds;
  //  char        buffer[80];
  extended    au, ad, utu, dju, utd, djd, tzi;
  int         never_rise, never_sets;


  /*  dj = JulianDays(21, 9, 1997); */
  dj = jdays_now;
  tzi = -8;                   // nevada  west negative
  ds = 1;                     // daylight savings
  gp = 36.048152;
  gl = -114.951;
  msl_km = 0;
  //  gp = hms2X(39, 37, 0);
  //  gl = hms2X(-75, 36, 0);
  sun_calculate_rise(utu, dju, utd, djd, dj, tzi, ds, gl, gp, msl_km, never_rise, never_sets, au, ad );
  compute_hours(utu+tzi+ds, &ihours, &iminutes, &iseconds);
  printf("TODAY SUNRISE %d (%lf) %d:%d:%d\n", (int)dju, utu+tzi+ds, ihours, iminutes, iseconds);

  compute_hours(utd+tzi+ds, &ihours, &iminutes, &iseconds);
  printf("TODAY SUNSET %d (%lf) %d:%d:%d\n", (int)dj, utd+tzi+ds, ihours, iminutes, iseconds);

  double             local_noon = 12  + tzi + ds - gl/15;
  //  dj = JulianDays(19, 7, 2016);
  //  time_sidereal_to_universal(local_noon, ds, tzi, gl, dj+1, 12.0);
  //  local_noon += tzi + ds - gl/15;
  /* noon calculation based on 
   * College Mathematics Journal Vol 34 No 4, September 2003, 
   * Teets, Predicting Sunrise and Sunset Times
   */
  double minutes;
    //    doy = dayOfYear(dj), // JulianDays(20, 7, 2016) - JulianDays(1, 1, 2016),
  //    minutes = 720 - 10*sin(4*M_PI / 365.25 *(doy - 80)) + 8*sin(2*M_PI/365.25*doy);
  //  minutes += (tzi+ds-gl/15)*60;
  //  ihours = floor(minutes/60);
  //  iminutes = floor((minutes/60-ihours)*60);
  //  compute_hours(local_noon, &ihours, &iminutes, &iseconds);
  sun_local_noon_transit(dj, tzi, ds, gl, minutes, ihours, iminutes);
  printf("TODAY NOON (MERIDIAN TRANSIT) %d:%d\n", ihours, iminutes);
  printf("DURATION AM %lf PM %lf minutes\n", minutes - 60*positive_hours(utu+tzi+ds),  60*positive_hours(utd+tzi+ds) - minutes);
  
  extended T, eot, lambda, dec, UTrise, UTset;
#if 0
  // analemma calculation
  //  (%i14) analemma: read_matrix ("analemma1.txt");
  // plot2d([discrete,transpose(analemma)[2],transpose(analemma)[3]]);
  //  plot2d([discrete,transpose(analemma)[3],transpose(analemma)[2]],[ylabel,"equation of time"],[xlabel,"declination"]);
  dj = JulianDays(1, 1, 2016);
  printf("day T DEC\n");
  for (int i = 0; i<365; i++){
    T = centuries_j2000(dj+i);  
    eot = equation_of_time(T, 12-tzi-ds, lambda, dec); // local noon
    printf("%d,%lf,%lf\n", i, eot, dec);
  }
  // months
  for (int i=1; i<=12; i++){
    char  *smonth[]={"jan", "feb","mar","apr","may","jun","jul","aug","sep","oct","nov","dec"};
    dj = JulianDays(1, i, 2016);
    T = centuries_j2000(dj);  
    eot = equation_of_time(T, 12-tzi-ds, lambda, dec); // local noon
    printf("[\"%s\",%lf,%lf],\n", smonth[i-1], dec, eot);
  }
#endif
  T = centuries_j2000(dj);  
  eot = equation_of_time(T, 12-tzi-ds, lambda, dec); // local noon
  minutes = 720 - eot;
  minutes += (tzi+ds-gl/15)*60;
  ihours = floor(minutes/60);
  iminutes = floor((minutes/60-ihours)*60);
  printf("EOT %lf %lf %lf %d:%d\n", eot, minutes, minutes/60, ihours, iminutes);
  printf("DURATION AM %lf PM %lf minutes\n", minutes - 60*positive_hours(utu+tzi+ds),  60*positive_hours(utd+tzi+ds) - minutes);
  UTrise = calculate_rise_time(T, gp, gl, +1, msl_km*1000);
  UTset = calculate_rise_time(T, gp, gl, -1, msl_km*1000);
  printf("UT rise = %lf set = %lf\n", UTrise, UTset); //  msl_km*1000));
  compute_hours(UTrise+tzi+ds, &ihours, &iminutes, &iseconds);
  printf("USNO rise %d:%d:%d\n", ihours, iminutes, iseconds);
  compute_hours(UTset+tzi+ds, &ihours, &iminutes, &iseconds);
  printf("USNO set %d:%d:%d\n", ihours, iminutes, iseconds);  

  
  //  check = selftest_ephemeris_withcheck("mauna kea 4.2km", 17, 1, 2016, -10, 0, 19+50.0/60.0, -(155+28.0/60.0), 4.2, 2);
  //  selftest_ephemeris(dj, tzi, ds, gp, gl, 0, 15);

  /* spot checking. June 26, 2016. Fairbanks AK, Miami FL, 
   */
  check &= selftest_ephemeris_withcheck("mauna kea 4.2km", 17, 1, 2016, -10, 0, 19+50.0/60.0, -(155+28.0/60.0), 4.2, 6+49/60., 18+16/60., 10.);
  check &= selftest_ephemeris_withcheck("mauna kea 4.2km", 17, 2, 2016, -10, 0, 19+50.0/60.0, -(155+28.0/60.0), 4.2, 6+40/60., 18+32/60.);
  check &= selftest_ephemeris_withcheck("mauna kea 4.2km", 17, 3, 2016, -10, 0, 19+50.0/60.0, -(155+28.0/60.0), 4.2, 6+18/60., 18+42/60.);
  check &= selftest_ephemeris_withcheck("mauna kea 4.2km", 17, 4, 2016, -10, 0, 19+50.0/60.0, -(155+28.0/60.0), 4.2, 5+52/60., 18+51/60., 10.);
  check &= selftest_ephemeris_withcheck("mauna kea 4.2km", 17, 5, 2016, -10, 0, 19+50.0/60.0, -(155+28.0/60.0), 4.2, 5+35/60., 19+02/60.);
  check &= selftest_ephemeris_withcheck("mauna kea 4.2km", 17, 6, 2016, -10, 0, 19+50.0/60.0, -(155+28.0/60.0), 4.2, 5+32/60., 19+14/60., 10.);
  check &= selftest_ephemeris_withcheck("mauna kea 4.2km", 17, 7, 2016, -10, 0, 19+50.0/60.0, -(155+28.0/60.0), 4.2, 5+42/60., 19+14/60.);
  check &= selftest_ephemeris_withcheck("mauna kea 4.2km", 17, 8, 2016, -10, 0, 19+50.0/60.0, -(155+28.0/60.0), 4.2, 5+53/60., 18+58/60.);
  check &= selftest_ephemeris_withcheck("mauna kea 4.2km", 17, 9, 2016, -10, 0, 19+50.0/60.0, -(155+28.0/60.0), 4.2, 6+0/60., 18+32/60., 10.);
  check &= selftest_ephemeris_withcheck("mauna kea 4.2km", 17, 10, 2016, -10, 0, 19+50.0/60.0, -(155+28.0/60.0), 4.2, 6+8/60., 18+6/60.);
  check &= selftest_ephemeris_withcheck("mauna kea 4.2km", 17, 11, 2016, -10, 0, 19+50.0/60.0, -(155+28.0/60.0), 4.2, 6+22/60., 17+52/60.);
  check &= selftest_ephemeris_withcheck("mauna kea 4.2km", 17, 12, 2016, -10, 0, 19+50.0/60.0, -(155+28.0/60.0), 4.2, 6+40/60., 17+57/60.);  
  check &= selftest_ephemeris_withcheck("Fairbanks AK", 26, 6, 2016, -8, 0, (64+49.0/60.0), -(147+45.0/60.0), 0, 3+4/60., 0+43/60.);
  check &= selftest_ephemeris_withcheck("Miami FL", 26, 6, 2016, -5, 1, (25+47.0/60.0), -(80+13.0/60.0), 0, 6+32/60., 20+16/60.);
  check &= selftest_ephemeris_withcheck("Honolulu HI", 26, 6, 2016, -10, 0, (21+19.0/60.0), -(157+50.0/60.0), 0, 5+52/60., 19+17/60.);
  check &= selftest_ephemeris_withcheck("Seattle WA", 26, 6, 2016, -8, 1, (47+38.0/60.0), -(122+20.0/60.0), 0, 5+13/60., 21+11/60.);
  check &= selftest_ephemeris_withcheck("Cambridge MA", 26, 6, 2016, -5, 1, (42+22.0/60.0), -(71+7.0/60.0), 0, 5+9/60., 20+25/60.);
  check &= selftest_ephemeris_withcheck("London UK", 26, 6, 2016, 0, 0,  51.514125, -.093689, 0, 3+45/60., 20+22/60.);
  check &= selftest_ephemeris_withcheck("Tel Aviv IS", 26, 6, 2016, +2, 0, 32.084139, 34.799946, 0, 4+36/60., 18+51/60.);
  check &= selftest_ephemeris_withcheck("Tokyo JP", 26, 6, 2016, +9, 0, 35.685, 139.751389, 0, 4+27/60., 19+1/60.);        
  check &= selftest_anyrise();
  check &= selftest_sunrise();
  check &= selftest_sunposition();

  /* equation of time test dec,E,lambda
  (%i76) selftest:equation_of_time_full(centuries_j2000(julian(23,8,1984)), 19+5);
  (%o76)    [11.11485940000455, - 2.401115405012291, 151.0140464918149]
  */
  printf("EQUATION OF TIME\n");
  dj = JulianDays(23, 8, 1984);
  T = centuries_j2000(dj);
  eot = equation_of_time(T, 19.+5., lambda, dec);
  if (lambda<0.0) lambda+=360;
  if (dec<0.0) dec+=360;
  printf("dj %lf T %lf eot %lf dec %lf lambda %lf\n",
	 dj, T, eot, dec, lambda);
  printf("\tcheck [dec=11.11485940000455 degrees, eot=-2.401115405012291 minutes, lambda=151.0140464918149 degrees]\n");
  if (fabs(eot+2.401115405012291)<0.1 && 
      fabs(lambda-151.0140464918149)<0.1 && 
      fabs(dec-11.11485940000455)<0.1) check &= 1;     // true is 1
  else check &= 0;                                     // false is 0

  // SUN AZ/ALT TEST
  /* selftest2:sun_alt_az(julian(3,30,2017),8,1,36, -120, 0);
     [19, 297.1120815341256, 1.714039382226231],  */
  double az, alt, dj00;
  //  dj00 = +0.5;
  sun_alt_az(JulianDays(30,3,2017), 19.+8-1, -8., 1., 36., -120., 0., az, alt);
  printf("check AZ=%lf ALT=%lf\n", az, alt);
  printf("\t272, 3.52\n");
  if (fabs(alt-3.52)<1 && fabs(az-272)<1) check&=1;
  else check &= 0;
  
  if (check == 1)
    printf("PASSED SELFTEST ( RESULT CODE= %d)\n", check);
  else
    printf("FAILED SELFTEST ( RESULT CODE= %d)\n", check);
  printf("**************************************\n");

  //  sprintf(sun_options.location, "%s", las_vegas_location);
  //  sprintf(sun_options.query, "%s", "sunset<now");
  //  printf("DAY\n");
  //  sprintf(sun_options.query, "%s", "sunrise>sunset");

  
  printf("NIGHT\n");
  sprintf(sun_options.query, "%s", "sunrise<sunset");
  check = logic_withcheck(sun_options.query);
  printf("QUERY %s result %d\n", sun_options.query, check);

  printf("DAY\n");
  sprintf(sun_options.query, "%s", "sunrise>sunset");
  check = logic_withcheck(sun_options.query);
  printf("QUERY %s result %d\n", sun_options.query, check);

}
