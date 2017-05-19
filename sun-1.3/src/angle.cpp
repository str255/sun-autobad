/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
//#include <Types.h>
#include <math.h>
#include "startime.h"
#include "angle.h"

extended deg2rad(extended a){ return  PI180*a; }
extended rad2deg(extended a){ return  INV_PI180*a; }
extended rad2hours(extended a){ return  HOURSPI*a; }
extended hours2rad(extended a){ return  INV_HOURSPI*a; }
extended deg2hours(extended a){ return 24.0/360.0*a; }
extended hours2deg(extended a){ return 360.0/24.0*a; }

/* sin function with degree argument */
double SIN(double angle)
{
  angle *= M_PI/180.0;      // convert to radians
  return sin(angle);       
}


/* cos function with degree argument */
double COS(double angle)
{
  angle *= M_PI/180.0;      // convert to radians
  return cos(angle);       
}

/* note that this also works for degrees */
extended hms2X(extended hours, extended minutes, extended seconds){
  if (hours < 0)
   return (-(seconds/60.+minutes)/60. + hours); // make every negative
  else
   return (seconds/60.+minutes)/60. + hours;
}

void compute_degrees_keepSign(extended reading, int &in_degrees, int &in_minutes, int &in_seconds)
{
  extended            t; 
  int				  sgn;
  
  if (reading < 0)  sgn = -1;
  else 				sgn = +1;
  
  reading = sgn * reading;
  
  in_degrees  = floor(reading);
  t           = 60*(reading - in_degrees);
  in_minutes  = floor(t); 
  t           = 60*(t - in_minutes);
  in_seconds  = floor(t+0.5);
  if (in_seconds==60){ in_seconds = 0; in_minutes += 1; }
  if (in_minutes==60){ in_minutes = 0; in_degrees += 1; }
  in_degrees *= sgn;
}


void compute_degrees3(extended x, int &ideg, float &fmin, int &isgn)
{
   if (x < 0) isgn = -1; else isgn = 1;
   x = isgn * x;
   ideg = floor(x); 
   fmin = 60.0 * (x - ideg);
}


/* only valid for degrees 360 */
void compute_degrees(extended reading, 
	int *in_degrees, int *in_minutes, int *in_seconds)
{
  extended            t; 
  int				  sgn = 1;
  
  if (reading < 0){
  	reading += 360.0;
#if 0
    if (floor( -reading )==0) reading = 360+reading; // can't put a - sign on zero.
	else {
     sgn = -1;
 	 reading = - reading;
	}
#endif
  }
  *in_degrees  = floor(reading);
  t           = 60*(reading - *in_degrees);
  *in_minutes  = floor(t); 
  t           = 60*(t - *in_minutes);
  *in_seconds  = floor(t+0.5);
  if (*in_seconds==60){ *in_seconds = 0; *in_minutes += 1; }
  if (*in_minutes==60){ *in_minutes = 0; *in_degrees += 1; }
  *in_degrees *= sgn;
}

/* only valid for degrees 360 */
void compute_degrees2(extended reading, 
	int *in_degrees, int *in_minutes, int *sgn)
{
  extended            t; 
  int				  id, im, is; 
  
  if (reading < 0){
  	*sgn = -1; 
  	reading = -reading;
  }
  else *sgn = 1;
  
  id  = floor(reading);
  t           = reading - id;
  t          *= 60;
  im  		  = floor(t); 
  t           = t - im;
  t          *= 60;
  is          = floor(t+0.5);
  if (is > 30) im += 1;
  if (im >= 60){ im -= 60; id += 1; }
  *in_degrees = id;
  *in_minutes = im;
}

/* only valid for positive readings */
void compute_degrees_fracsec(extended reading, 
	int *in_degrees, int *in_minutes, extended *in_seconds)
{
  extended            t; 
  int				  sgn = 1;

  *in_degrees  = floor(reading);
  t           = 60*(reading - *in_degrees);
  *in_minutes  = floor(t); 
  t           = 60*(t - *in_minutes);
  *in_seconds  = t;
}

extended positive_hours(extended hours)
{
  hours = fmod(hours, 24.0);
  if (hours < 0) hours += 24;
  return hours;
}

void compute_hours(extended reading, int *in_hours, int *in_minutes, int *in_seconds)
{
  extended             t; 
 // reading = 24.0/360.0 * reading;             // convert from degree scale to hours.
  if (reading < 0.0) reading += 24;
  *in_hours  = floor(reading);
  t           = 60*(reading - *in_hours);
  *in_minutes  = floor(t); 
  t           = 60*(t - *in_minutes);
  *in_seconds  = floor(t);
}

void compute_day_hours(extended reading,
	 int *in_days, int *in_hours, int *in_minutes, int *in_seconds)
{
  extended             t; 
  *in_days 		= floor(reading);
  t             = 24*(reading-*in_days);
 // reading = 24.0/360.0 * reading;             // convert from degree scale to hours.
  *in_hours  = floor(t);
  t           = 60*(t - *in_hours);
  *in_minutes  = floor(t); 
  t           = 60*(t - *in_minutes);
  *in_seconds  = floor(t);
}

/*extended mod(extended x, extended m)
{
 if (x >= m) return ( mod(x-m, m) );
 else 
 if (x < 0 ) return ( mod(x+m, m) );
 else return x;
}*/
