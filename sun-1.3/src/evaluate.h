/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
#ifndef __EVALUATEH__
#define __EVALUATEH__

#include <stdint.h>

enum funcall_e {
  i_nighttime,
  i_daytime,
  i_sunrise,
  i_sunset,
  i_azimuth,
  i_altitude,
  i_now,
  i_month,
  i_dow,
  i_year,
  i_jan,
  i_feb,
  i_mar,
  i_apr,
  i_may,
  i_jun,
  i_jul,
  i_aug,
  i_sep,
  i_oct,
  i_nov,
  i_dec,                      
  i_maxFuncalls,
};

typedef struct {
  uint16_t            itype;
  char                str[32];
  double              (*handler)();
} funcall_t;

void initialize_cvalues();
double evaluate_cvalue(char *cvalue, bool *truth);
double evaluate(SExpression *e);

// handlers
double handle_isnighttime();
double handle_isdaytime();
double handle_sunrise();
double handle_sunset();
double handle_azimuth();
double handle_altitude();
double handle_now();
double handle_month();
double handle_dow();
double handle_year();
double handle_jan();
double handle_feb();
double handle_mar();
double handle_apr();
double handle_may();
double handle_jun();
double handle_jul();
double handle_aug();
double handle_sep();
double handle_oct();
double handle_nov();
double handle_dec();

#endif
