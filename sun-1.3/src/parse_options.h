/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
#ifndef __PARSEOPTIONS__
#define __PARSEOPTIONS__
#include "location_expression.h"

int parse_options (int argc, char **argv);
void default_options();
void print_options();
double guess_daylight_savings(double jd, double year, double ds);

enum modes_twilight {
  CIVIL_TWILIGHT,
  NAUTICAL_TWILIGHT,
  ASTRONOMICAL_TWILIGHT,
  GENERAL_TWILIGHT,
};

typedef struct struct_Options
{
  char                 location[255];
  char                 query[255];
  locationExpression  *latlon;
  double               timezone;   // hours west of greenwich (+)
  double               daylight;   // hours west of greenwich (+)
  double               twilight_angle; 
  int                  self_test;  // default is true
  int                  self_verbose; // default is false
  int                  self_guess_daylight_savings;
  int                  mode_query;
  modes_twilight       mode_twilight;   
} Options;

#endif
