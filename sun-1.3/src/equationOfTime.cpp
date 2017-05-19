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
// low precision ephemeris for the Sun.
// Source: Explanatory Supplement to the Astronomical Almanac
// P. Kenneth Seidelmann, United States Naval Observatory. 
// Nautical Almanac Office, Great Britain. Nautical Almanac Office

#include <math.h>
#include <stdio.h>
#include <strings.h>
#include "startime.h"
#include "angle.h"
#include "equationOfTime.h"


#define sind(x) sin(M_PI/180*x)
#define cosd(x) cos(M_PI/180*x)

/* T is julian day in unit of centuries
   UT is universal time
   output
   dec is declination degrees
   E is equation of time minutes
   lambda is ecliptic longitude (position of earth along orbit) degrees
 */
// equation of time is difference between 12 noon and local transit
// time of local transit is close to time of culmination
// T is time in centuries J2000 (see centuries_j2000)
// L is mean longitude corrected for aberration;
// G is mean anomaly;
// lambda is ecliptic longitude;
// epsilon is obliquity of the ecliptic;
// GHA = Greenwich Hour Angle (GHA)
// dec (or delta) = declination (Dec) of the Sun
// source: Explanatory Supplement to the Astronomical Almanac
// P. Kenneth Seidelmann, United States Naval Observatory. 
// Nautical Almanac Office, Great Britain. Nautical Almanac Office
double
equation_of_time(double T, double UT, double &lambda, double &dec)
{
  double L, G, epsilon, E;
  T = T + UT/24./36525.0;               // add in hours
  L = 280.460 + 36000.770 * T;
  G = 357.528 + 35999.050 * T;
  lambda = L + 1.915 * sind(G) + 0.020 * sind(2*G);
  lambda = fmod(lambda, 360);
  epsilon = 23.4393 - 0.01300*T;
  E = -1.915*sind(G) - 0.020*sind(2*G) + 2.466*sind(2*lambda) - 0.053*sind(4*lambda); // degrees
  E = 60.0/15.0*E;            // minutes
  dec = 180.0/M_PI*asin(sind(epsilon)*sind(lambda));
  dec = fmod(dec, 360);
  return E;
}

// value is in degrees
// E is equation of time (see equation_of_time(double T)
// UT is universal time
// source: Explanatory Supplement to the Astronomical Almanac
// P. Kenneth Seidelmann, United States Naval Observatory. 
// Nautical Almanac Office, Great Britain. Nautical Almanac Office
double
greenwich_hour_angle(double UT, double E)
{
  double GHA;
  GHA = 15*UT - 180 + 15*E/60.;   // degrees
  return GHA;
}

// H is observer elevation AGL meters
// phi is observer latitude degrees north positive (also gp, lat)
// dec is sun's declination degrees
// t is sun hour angle
// source: Explanatory Supplement to the Astronomical Almanac
// P. Kenneth Seidelmann, United States Naval Observatory. 
// Nautical Almanac Office, Great Britain. Nautical Almanac Office
double
sun_hour_angle(double phi, double H, double dec)
{
  double   
    h, 
    cost,
    t;
  h = -50./60. - 0.0353*sqrt(H);  
  cost = ( sind(h) - sind(phi)*sind(dec) )/( cosd(phi)*cosd(dec) );
  if (cost > 1) t = 0;
  else if (cost < -1) t = 180.;
  else t = 180./M_PI*acos(cost); // degrees
  return t;
}


// T is time centuries (see j2000)
// lat is observer latitude degrees (+N)
// lon is observer longitude degress (+E)
// sgn is sign rise +1, sign set -1
// H is observer altitude meters
// source: Explanatory Supplement to the Astronomical Almanac
// P. Kenneth Seidelmann, United States Naval Observatory. 
// Nautical Almanac Office, Great Britain. Nautical Almanac Office
double
calculate_rise_time(double T, double lat, double lon, int sgn, double H)
{
  double  
    UT = 12,
    E,
    lambda,
    dec,
    GHA,
    SHA;
  for (int i = 0; i<100;i++){
    E = equation_of_time(T, UT, lambda, dec);
    GHA = greenwich_hour_angle(UT, E);
    SHA = sun_hour_angle(lat, H, dec);
    UT = UT - (GHA + lon + sgn*SHA)/15;
    if (UT<0) UT = UT + 24;
    else if (UT > 24) UT = UT - 24;
    else UT = UT;
  }
  return UT;
}

/* convert observation local hour angle and declination
   to azimuth and altitude.
   hour_angle is hour angle (gha) in degrees
   dec is declination in degrees
   lat is observer latitude in degrees
   output 
   az is azimuth degrees clockwise from north (pilot's coords)
   alt is altitude degrees
 */
void
observer_coord(double hour_angle, double dec, double lat, double &az, double &alt)
{
  double sh = sind(hour_angle),
    ch = cosd(hour_angle),
    sd = sind(dec),
    cd = cosd(dec),
    sl = sind(lat),
    cl = cosd(lat),
    x,y,z,r;

  x = - ch * cd * sl + sd * cl;
  y = - sh * cd;
  z = ch * cd * cl + sd * sl;
  r = sqrt(x*x + y*y);
  /* now get Alt, Az */
  az = 180/M_PI * atan2(y,x);
  alt = 180/M_PI * atan2(z,r);
  /* correct for negative AZ */
  if (az < 0) az = az + 360;
}
/* convert observation greenwich hour angle and declination
   to azimuth and altitude.
   gha is greenwich hour angle (gha) in degrees
   dec is declination in degrees
   lat is observer latitude in degrees
   lon is observer lonitude in degrees (-West)
   note: formula lha=gha-lon is corrected for west positive convention yielding lha=gha+lon
 */
void
observer_coord_gha(double gha, double dec, double lat, double lon, double &az, double &alt)
{
  double   lha = gha + lon;
  observer_coord(lha, dec, lat, az, alt);
}

double
UT_to_local_time(double UT, double jd, double tz, double ds)
{
  return fmod(UT-tz+ds,24);
}

double 
local_to_UT_time(double LT, double jd, double tz, double ds)
{
  return fmod(LT+tz-ds,24);
}

/* plot alt, az during the day for the sun
   inputs: jd is julian day
   LT is local time
   tz is time zone (+west)
   ds is day light savings (applied automatically be procedure).
   lat is observer latitude (degrees +north)
   lon is observer longitude (degrees -west)
   h is observer height (meters)
   output: list of observations (altaz) and labels for plot2
   az is azimuth degrees clockwise from north (pilot's coords)
   alt is altitude degrees
*/
void
sun_alt_az(double jd, double UT, double tz, double ds, double lat, double lon, double h, double &az, double &alt)
{
  double T, /*UT, UTZ=0, eot,*/ E, lambda, dec, GHA, SHA, t;

  T = centuries_j2000(jd);
  //  UT = local_to_UT_time(LT/60,jd,tz,ds);
  E = equation_of_time(T, UT, lambda, dec);
  printf(_("Equation of Time=%lf minutes declination (DEC)=%lf degrees and ecliptic longitude (lambda)=%lf degrees\n"), E, dec, lambda);
  GHA = greenwich_hour_angle(UT, E);
  observer_coord_gha(GHA, dec, lat, lon, az, alt);
}
