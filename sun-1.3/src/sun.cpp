/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
//#include "macmac.h"
//#include "menu.h"
//#include <Picker.h>	
//#include <Scrap.h>
//#include <Resources.h>
//#include <SegLoad.h>
#include <math.h>
#include <stdio.h>
#include <strings.h>
#include "startime.h"

//#include "application.h"

//#include <OSUtils.h>
//#include <TextUtils.h>
//#include "util.h"
#include "angle.h"
//#include <SANE.h>
//#include "alerts.h"
//#include "preferences.h"
#include "sun.h"
#include "equationOfTime.h"

#define DEG2RAD(x) 1.745329252e-2L * (x)
#define RAD2DEG(x) 5.729577951e1L * (x)
#define SQR(x) (x)*(x)
const extended TP = 2 * M_PI;
extended fmod_plus(extended a, extended b, extended& day);
void meeus_sun_position(extended day, extended month, extended year, extended &theta_app);

//extern  Preferences 			prefs;

short angleTwilight = 0, typeTwilight = 0;

extended fmod_plus(extended a, extended b, extended& d)
{
	extended 	val;
	start:
	if (a < 0){ a += b; d = -1; goto start; }
	if (a > b){ a -= b; d = +1; goto start; }
	return a;
}

void nutation(extended dj, extended &dp, extended &d0)
{
extended t, a, b, l1, l2, t2, d1, d2, m1, m2, n1, n2;
// dj is julian days. t in centuries.
t = dj/36525.0;
t2 = t*t;
a = 1.000021358e2 * t;
b = 360.0 * ( a -  rint(a));
l1 = 2.796967e2 + 3.03e-4 * t2 + b;
l2 = 2.0 * DEG2RAD( l1 );
a = 1.336855231e3 *t;
b = 360.0 * ( a - rint(a));
d1 = 2.704342e2 - 1.133e-3 * t2 + b;
d2 = 2.0 * DEG2RAD (d1);
a = 9.999736056e1 * t;
b = 360.0 * ( a - rint(a));
m1 = 3.584758e2 - 1.5e-4 * t2 + b;
m1 = DEG2RAD(m1);
a = 1.325552359e3 * t;
b = 360.0*(a - rint(a));
m2 =  2.961046e2 + 9.192e-3 * t2 + b;
m2 = DEG2RAD(m2);
a = 5.372616667*t;
b = 360.0 * (a - rint(a));
n1 = 2.591833e2 + 2.078e-3 * t2 - b;
n1 = DEG2RAD(n1);
n2 = 2*DEG2RAD(n1);

dp = (-17.2327 - 1.737e-2 * t) * sin(n1);
dp = dp + (-1.2729 - 1.3e-4*t)*sin(l2) + 2.088e-1 * sin(n2);
dp = dp - 2.037e-1* sin(d2) + (1.261e-1 - 3.1e-4*t) * sin(m1);
dp = dp + 6.75e-2 * sin(m2)    - (4.97e-2 -1.2e-4*t)*sin(l2+m1);
dp = dp - 3.42e-2 * sin(d2-n1) - 2.61e-2 * sin(d2+ m2);
dp = dp + 2.14e-2 * sin(l2-m1) - 1.49e-2 * sin(l2-d2+m2);
dp = dp + 1.24e-2 * sin(l2-n1) + 1.14e-2 * sin(d2-m2);

d0 = (9.21 + 9.1e-4 * t) * cos(n1);
d0 = d0 + (5.522e-1 - 2.9e-4 * t) * cos(l2) - 9.04e-2 * cos(n2);
d0 = d0 + 8.84e-2 * cos(d2   ) + 2.16e-2 * cos(l2+m1);
d0 = d0 + 1.83e-2 * cos(d2-n1) + 1.13e-2 * cos(d2+m2); 
d0 = d0 - 9.3e-3 * cos(l2-m1) - 6.6e-3 * cos(l2-n1); 

dp = dp/3600.0; d0 = d0/3600.0;
}

/* dj = number julian days since 1900 January 0.5
   do = nutation in obliquity. 
   ob = obliquity (degrees)
 */
void obliquity( extended dj, extended &ob )
{  
	extended 	d0, dp, t, a;

	nutation(dj, dp, d0);
	
	t  = (dj/36525.0) - 1;
	a  = (4.6185e1 + (6.0e-4 - 1.81e-3 * t) * t) * t;
	ob = 23.43929167 - (a/3600.0) + d0;
}

/* p,q <- x,y
   ob = oblquity
	direction = +1
   p = ecliptic longitude
   q = ecliptic latitude
   x = ra
   y = dec
	direction = -1
   p = ra
   q = dec
   x = ecliptic longitude
   y = ecliptic latitude
 */
void geocentric_ecliptic_latlon_to_radec(extended direction, 
	extended &p, extended &q, extended &x, extended &y,  extended dj)
{
extended 	e;
extended	ob, se, ce, sy, cy, ty, cx, sx, s, a;
extended    dd;

x = DEG2RAD(x);
y = DEG2RAD(y);
// calculate obliquity
obliquity( dj, ob );
e = DEG2RAD (ob); 
se = sin(e);
ce = cos(e);
sy = sin(y);
cy = cos(y);
if ( fabs(cy)< 1e-20 ) cy = 1e-20;
ty = sy/cy;
cx = cos(x);
sx = sin(x);
s = (sy*ce) - (cy*se*sx*direction);
q = asin(s);
a = (sx * ce) + (ty * se * direction);
p = atan(a/cx);
if (cx < 0) p = p + M_PI;

p = fmod_plus(p, TP, dd);

p = RAD2DEG(p);
q = RAD2DEG(q);
}

/* di = vertical displacement (correction term) = 9.89e-3 radians
   gl = geographic lon,  gp = geographic lat
   lu = time of rising, au = azimuth of rising
   ld = time of setting ad = azimuth of setting
   x = geocentric coords (radec)
   y = geocentric coords
   not used extended &gl, 
   */
void time_of_rising(extended x, extended y, extended di,extended gp,
	extended &lu, extended &ld, extended &ad, extended &au,
	int &never_sets, int &never_rises)
{
 extended sd, cd, cf, sf, ch, ca, h, a, b, cy, sy;
 extended    dd;
 
 au = 0.0;
 ad = 0.0;
 gp = DEG2RAD(gp);
 x = DEG2RAD(x);
 y = DEG2RAD(y);
 never_sets = false; never_rises = false;
 cf = cos(gp);
 sf = sin(gp);
 cy = cos(y);
 sy = sin(y);
 sd = sin(di);
 cd = cos(di);
 ch = -(sd + (sf * sy))/(cf * cy);
 if (ch < -1.0) never_sets = true;
 if (ch > 1.0) never_rises = true;
 if (never_sets || never_rises) return;
 ca = (sy + (sd * sf))/(cd * cf);
 h = acos(ch);
 au = acos(ca);
 b = RAD2DEG(h)/15.0;   //  convert to hours
 a = RAD2DEG(x)/15.0;
 lu = 24.0 + a - b;
 ld = a + b;
 ad = TP - au;

 lu = fmod_plus(lu, 24.0, dd);
 ld = fmod_plus(ld, 24.0, dd);
 au = fmod_plus(au, TP, dd); 
 ad = fmod_plus(ad, TP, dd); 
 au = RAD2DEG(au);
 ad = RAD2DEG(ad);
}

/* ec = eccentricity
   am = mean anomaly (degrees)
   ae = eccentric anomaly (degrees)
   at = true anomaly (degrees)
   */
void anomaly(extended am, extended ec, extended &ae, extended &at)
{	extended a, m, d = 1.0;

am = DEG2RAD(am);
m = am - TP * floor(am/TP);
ae = m;
while (1){
	d = ae - (ec * sin(ae)) - m;
	if (fabs(d) < 1e-9) break;
	d = d/(1.0 - (ec * cos(ae)));
	ae = ae - d;
}
a = (1.0 + ec)/(1.0 - ec); 
a = sqrt(a);
a *= tan(ae / 2.0);
at = 2.0 * atanl(a);                
at = RAD2DEG(at);
ae = RAD2DEG(ae);
}


/*  longitude refererred to the mean equinox of the date
   dj = julian day 
   ut = universal time (hours)
   sr = ecliptic longitude
   rr = distance of sun to earth
   d3 = correction to rr
   d2 = correction to sr
 */
void sun_position(extended dj, extended ut, 
	extended &sr, extended &rr, extended &d2, extended &d3, extended &am)
{ extended t, t2, a, b, l, m1, ec, ae, at;
  extended a1, b1, c1, d1, e1, h1;
  extended dd;
	t = (dj/36525.0L) + (ut/8.766e5L);
	t2 = SQR(t);
	a = 1.000021359e2L * t;
	b = 360.0 * (a - rint(a));
	l = 2.7969668e2L + 3.025e-4L * t2 + b;
	a = 9.999736042e1L * t;
	b = 360.0 * (a - rint(a));
	m1 = 3.5847583e2L - (1.5e-4L + 3.3e-6L * t) * t2 + b;
	
	// ec is essentially the constant.
	ec = 1.675104e-2L - 4.18e-5L * t - 1.26e-7L * t2;
	am = m1;
	anomaly(am, ec, ae, at);  // in degrees
	at = DEG2RAD(at);
    ae = DEG2RAD(ae);

	// a1
	a = 6.255209472e1L * t;
	b = 360.0 * (a - rint(a));
	a1 = DEG2RAD(153.23 + b);
	// b1
	a = 1.251041894e2L * t;
	b = 360.0 * (a - rint(a));
	b1 = DEG2RAD(216.57 + b);
	// c1
	a = 9.156766028e1L * t;
	b = 360.0 * (a - rint(a));
	c1 = DEG2RAD(312.69 + b);
	// d1
	a = 1.236853095e3L * t;
	b = 360.0 * (a - rint(a));
	d1 = DEG2RAD(350.74 - 1.44e-3*t2 + b);
	e1 = DEG2RAD(231.19 + 20.2 * t);
	a = 1.831353208e2L * t;
	b = 360.0 * (a - rint(a)); 
	
	h1 = DEG2RAD(353.4 + b);
	d2 = 1.34e-3L * cos(a1) + 1.54e-3L * cos(b1) + 2e-3L * cos(c1);
	d2 += 1.79e-3L * sin(d1) + 1.78e-3L * sin(e1);
	d3 = 5.43e-6L * sin(a1) + 1.575e-5L * sin(b1);
	d3 += 1.627e-5L * sin(c1)  + 3.076e-5L * cos(d1);
	d3 += 9.27e-6L * sin(h1);
	
    // d2, d3 are small correction terms.
	sr = at + DEG2RAD(l - m1 + d2);
	rr = 1.0000002L * (1.0 - ec * cos(ae)) + d3;
	sr = fmod_plus(sr, TP, dd);
	sr = RAD2DEG(sr);
}

void sun_calculate_rise(extended &utu, extended &dju, extended &utd, extended &djd,
			extended dj, extended tz, extended ds,
			extended gl, extended gp, extended msl_km, int &never_rises, int &never_sets,
			extended &au, extended &ad)
{
	extended       di;
	const extended earth_radius_km = 6371.0;
	const extended refraction_sundiam_rad = 1.454441e-2; // 50 arc minutes = 16 + 34 = angular radius + refraction
	//    di = DEG2RAD((**prefs.dayTableaux).angleTwilight) + 1.454441e-2;
	di = 1.454441e-2; 
	di = sqrt(2*msl_km/earth_radius_km) + refraction_sundiam_rad;
	sun_calculate_rise_gen(utu, dju, utd, djd, dj, tz, ds, gl, gp, never_rises, never_sets,
			au, ad, di);
}

// time sunrise and sunset less time now
// then we can define night := sunset > sunrise
// and define day := sunrise > sunset
void sun_calculate_rise_tilnow(extended &utu, extended &dju, extended &utd, extended &djd,
			extended dj, extended tz, extended ds,
 		        extended gl, extended gp, extended now, extended now_hour, extended msl_km, int &never_rises, int &never_sets,
			extended &au, extended &ad)
{
	extended       di;
	const extended earth_radius_km = 6371.0;
	const extended refraction_sundiam_rad = 1.454441e-2; // 50 arc minutes = 16 + 34 = angular radius + refraction
	//    di = DEG2RAD((**prefs.dayTableaux).angleTwilight) + 1.454441e-2;
	di = 1.454441e-2; 
	di = sqrt(2*msl_km/earth_radius_km) + refraction_sundiam_rad;
	sun_calculate_rise_gen_tilnow(utu, dju, utd, djd, dj, tz, ds, gl, gp, now, now_hour, never_rises, never_sets,
			au, ad, di);
}

/*
	sw = 0 civil
	   = 1 nautical
	   = 2 astronomical
 */
void sun_calculate_civil_twilight(extended &utu, extended &dju, extended &utd, extended &djd, 
	extended dj, extended tz, extended ds,
	extended gl, extended gp, int &never_rises, int &never_sets,
	extended &au, extended &ad)
{
	extended 		di = 1.047198e-1;
	//	short 			sw = (**prefs.dayTableaux).typeTwilight;
	short 			sw = typeTwilight;
	if (sw == 1) di = 2.094395e-1;
	else
	if (sw == 2) di = 3.141593e-1;
	sun_calculate_rise_gen(utu, dju, utd, djd, dj, tz, ds, gl, gp, never_rises, never_sets,
			au, ad, di);
}

void sun_calculate_twilight_tilnow(extended &utu, extended &dju, extended &utd, extended &djd,
	extended dj, extended tz, extended ds,
        extended gl, extended gp, extended now, extended now_hour, extended msl_km, int &never_rises, int &never_sets,
				   extended &au, extended &ad, extended rad_twilight)
{
  extended       di;
  const extended earth_radius_km = 6371.0;
  const extended refraction_sundiam_rad = 1.454441e-2; // 50 arc minutes = 16 + 34 = angular radius + refraction

  di = sqrt(2*msl_km/earth_radius_km) + rad_twilight;
  sun_calculate_rise_gen_tilnow(utu, dju, utd, djd, dj, tz, ds, gl, gp, now, now_hour, never_rises, never_sets,
				au, ad, di);
}

/*
 * gl = geographic lon (degrees),  
 * gp = geographic lat (degrees)
 */
void sun_calculate_rise_gen(extended &utu, extended &dju, extended &utd, extended &djd,
			    extended dj, extended tz, extended ds,
			    extended gl, extended gp, int &never_rises, int &never_sets,
			    extended &au, extended &ad, extended di)
{
  extended	sr, rr, d2, d3, am;
  extended	dp, x, y, p, q, d0, lu, utv;
  extended      ut, ld, a, djp;
  int           iter, icase;  // sunrise icase=0, sunset icase=1
  extended      day, month, year, dow;
#ifdef DEBUG
  extended       xx; 
#endif

  // icase=0 sunrise utu  lu
  // icase=1 sunset  utd  ld
  for (icase=0;icase<2;icase++){
    iter = 0;
    ut  = 0.0;
    utv = +0.5 - tz - ds; // 1st guess local midday UT (shouldnt this be 12?)
    while (fabs(utv - ut) > 0.01 && iter++ < 10){
      //      printf("icase=%d iter %d ut %lf utv  %lf\n", icase, iter, ut, utv);
      ut = utv;          // save last guess for event
      a = ut + tz + ds;  // event local time
      // the fine point here is that sun's position is calc'd 
      // first from it's Julian date. In considering local time,
      // the date might be pushed onto another day. Effect
      // is non-trivial.
      if (a > 24.0)	 sun_position((djp = dj-1), utv, sr, rr, d2, d3, am);  // next local day
      else if (a < 0.0)	 sun_position((djp = dj+1), utv, sr, rr, d2, d3, am);  // previous local day
      else               sun_position((djp = dj), utv, sr, rr, d2, d3, am);    // same day
	 
      nutation(djp, dp, d0);
      x =  sr + dp - 5.69e-3;   // x,y units are degrees.
      y = 0;
#ifdef DEBUG
      CalendarDays(djp, day, month, year, dow); 
      meeus_sun_position(day+utd/24.0, month, year, xx);
      if (fabs(x-xx)>0.01){
	SysBreakStr("meeus discrepancy");
	printf("x %lf xx %lf\n", x, xx);
      }
#endif
      geocentric_ecliptic_latlon_to_radec(-1, p, q, x, y, djp ); // p,q units are degrees
      // compute sunup/sundown sidereal time lu/ld 
      time_of_rising(p, q, di, gp, lu, ld, ad, au, never_sets, never_rises);
      if (never_sets == true || never_rises == true) return;                // no change
      if (icase == 0) time_sidereal_to_universal(utv, ds, tz, gl, djp, lu);  // sunup
      else time_sidereal_to_universal(utv, ds, tz, gl, djp, ld);             // sundown
    }
    if (icase == 0){ utu = utv; dju = djp; }
    else { utd = utv; djd = djp; }
  }
}

/*
 * gl = geographic lon (degrees),  
 * gp = geographic lat (degrees)
 * dj = julian day 1900. e.g DJ=42597.500000 (17.000000 8.000000 2016.000000) OO UT 
 *      so 42597.0 would be 12UT on the 16th and +2415020.000000 would be full Julian days.
 *         42595.5 would be 00UT on the 17th
 *         42598.0 would be 12UT on the 17th
 * now = universal time (hours)
 */
void 
sun_calculate_rise_gen_tilnow(extended &utu, extended &dju, extended &utd, extended &djd,
			      extended dj, extended tz, extended ds,
			      extended gl, extended gp, extended now, extended now_hour,
			      int &never_rises, int &never_sets,
			      extended &au, extended &ad, extended di)
{
  extended	sr, rr, d2, d3, am;
  extended	dp, x, y, p, q, d0, lu, utv;
  extended      ut, ld, a, djp;
  int           iter, icase;  // sunrise icase=0, sunset icase=1
  extended      day, month, year, dow;
#ifdef DEBUG
  extended       xx; 
#endif

  // icase=0 sunrise utu  lu
  // icase=1 sunset  utd  ld
  for (icase=0;icase<2;icase++){
    iter = 0;
    ut  = 0.0;
    utv = now_hour; // 1st guess twelve hours earlier from now UT
    djp = dj;
    while (fabs(utv - ut) > 0.01 && iter++ < 10){
      //      printf("icase=%d iter %d ut %lf utv  %lf\n", icase, iter, ut, utv);
      ut = utv;          // save last guess for event

      //      a = ut + tz + ds;  // event local time
      // event must be before now
      // (djp,a) < (dj,now)
      //
      // the fine point here is that sun's position is calc'd 
      // first from it's Julian date. In considering local time,
      // the date might be pushed onto another day. Effect
      // is non-trivial.

      //      printf("til now(%d) %lf %lf %lf %lf \n", icase, dj, ut, dj+ut/24.0, now);
      if (djp+ut/24.0 > now)	 djp = djp-1;  // next local day
      if (djp+ut/24.0 > now)	 djp = djp-1;  // next local day      
      //      else if (a < 0.0)	 sun_position((djp = dj-1), utv, sr, rr, d2, d3, am);  // previous local day
      sun_position(djp, utv, sr, rr, d2, d3, am);    // same day
	 
      nutation(djp, dp, d0);
      x =  sr + dp - 5.69e-3;   // x,y units are degrees.
      y = 0;
#ifdef DEBUG
      CalendarDays(djp, day, month, year, dow); 
      meeus_sun_position(day+utd/24.0, month, year, xx);
      if (fabs(x-xx)>0.01){
	SysBreakStr("meeus discrepancy");
	printf("x %lf xx %lf\n", x, xx);
      }
#endif
      geocentric_ecliptic_latlon_to_radec(-1, p, q, x, y, djp ); // p,q units are degrees
      // compute sunup/sundown sidereal time lu/ld 
      time_of_rising(p, q, di, gp, lu, ld, ad, au, never_sets, never_rises);
      if (never_sets == true || never_rises == true) return;                // no change
      if (icase == 0) time_sidereal_to_universal(utv, ds, tz, gl, djp, lu);  // sunup
      else time_sidereal_to_universal(utv, ds, tz, gl, djp, ld);             // sundown
    }
    if (icase == 0){ utu = utv; dju = djp; }
    else { utd = utv; djd = djp; }
  }
}

/* ds = daylight savings (hours) -- summer positive
   tz = timezone (hours) -- west negative
   gl = longitude (degrees) -- west negative
   tm = local sidereal time (hours)
   dj = julian day. (days)
 */
void time_sidereal_to_universal(extended &ut, extended ds, extended tz, extended gl,
 	extended dj, extended tm)
{
	extended 	t0, t, sg, tl, d, r0, r1, day, month, year, dow;
	extended    dd;
    CalendarDays(dj, day, month, year, dow); 
	d = floor(dj - 0.5) + 0.5;
	t = (d/36525.0)-1;
	r0 = t*(5.13366e-2 + t*(2.586222e-5 - t*1.722e-9));
	r1 = 6.697374558 + 2400.0 * (t - ((year - 2000.0)/100.0));
	t0 = fmod_plus(r0 + r1, 24.0, dd);
	t = fmod_plus(t0 - (ds + tz)*1.002737908, 24.0, dd);
	sg = fmod_plus(tm - (gl/15.0), 24.0, dd);
	if (sg < t) sg += 24.0;
	tl = fmod_plus( (sg-t)*9.972695677e-1, 24.0, dd);
	ut = fmod_plus(tl - ds - tz, 24.0, dd);
}

// only valid for dates after 1582
void meeus_sun_position(extended day, extended month, extended year, extended& theta_app)
{
	int      m, y, a, b;
	extended jd;
    extended t, t2, t3;
    extended l, M, e, c, theta, nu, r, omega;
	//day 	= 12.0;
	//year	= 1978;
	//month   = 11;
	if (month>2){ y = year; m = month; }
	else { y = year -1; m = month +12; }
	a =  rint(y/100.0);
	b =  2 - a + rint(a/4.0);
	jd = rint(365.25*y) + rint(30.6001*(m+1)) + day + 1720994.5 + b;
	// centuries from 1900
	t = (jd - 2415020.0)/36525.0;
	t2 = t*t;
	t3 = t*t2;
	// geometric mean longitude (aka mean equinox) in degrees
	l = 279.69668 + 36000.76892 * t + 0.0003025 * t2;
	l = fmod(l, 360.);
    // mean anomaly
    M = 358.47583 + 35999.04975 * t - 0.000150 * t2-0.0000033 * t3;	
	M = fmod(M, 360.);
    // eccentricity
    e = 0.01675104 - 0.0000418 * t - 0.000000126 * t2;
	// equation of center
	c = (1.919460 - 0.004789*t - 0.000014*t2) * sin(DEG2RAD(M))
	    + (0.020094 - 0.000100*t) * sin(DEG2RAD(2*M))
	    + 0.000293 * sin(DEG2RAD(3*M)); 
    // true long
    theta = l + c;
    theta = fmod(theta, 360.);
    // true anomaly
    nu = M + c;
    // earth radius in a.u.
    r  = (1.0000002 * (1 - e*e)) / (1 + e * cos(DEG2RAD(nu)));
    // apparent long
    omega = 259.18 - 1934.142 * t;
    theta_app = theta - 0.00569 - 0.00479 * sin(DEG2RAD(omega));
}

/* based on U.S. Naval observatory ephemeris */
void solar_arguments(extended t, 
	extended &l, extended &g, extended &lambda, extended &epsilon)
{
	l = 280.460 + 36000.770 * t;
	l = fmod(l, 360.0);
	g = 357.528 + 35999.050 * t;
	lambda = l + 1.915 * SIN(g) + 0.020 * SIN(2 * g);
	epsilon = 23.4393 - 0.01300 * t;
}

/* based on U.S. Naval observatory ephemeris */
void time_equation(extended &e, extended &gha, extended &dec, extended &sd,
 	extended g, extended lambda, extended epsilon, extended ut)
{
 e = -1.915 * SIN(g) - 0.020*SIN(2 * g) + 2.466*SIN(2*lambda) - 0.053*SIN(4*lambda);
 gha = 15*ut - 180 + e;
 dec = asin(SIN(epsilon)*SIN(lambda));
 sd = 0.267/(1-0.017*COS(g));
}

// t is hour angle of body at ut0
// phi = lat, dec = lon
// direction = +1 for rising.
void rising_sun(extended &ut, extended jd, extended direction)
{   extended 	ut_save = 0.0;
	extended 	gha, e, dec, sd, g, epsilon, t, l, lambda, lat, lon, h;
	int         hours, minutes, seconds;
	h = DEG2RAD(-6);                        // height above horizon
	t = centuries_j2000( jd);               // date of calculation.
	t = t + ut/24./36525.0;                  // add in time
	solar_arguments(t, l, g, lambda, epsilon); // lambda = ecliptic longitude
	lambda = fmod(lambda, 360.);
	if (lambda < 0.0) lambda += 360.0;
	compute_degrees(lambda, &hours, &minutes, &seconds);

	time_equation(e, gha, dec, sd, g, lambda, epsilon, ut); // solar ephemeris returns gha and dec.

	while  (fabs(ut - ut_save) < 0.001){
		t = (sin(h) - sin(lat)*sin(lon))/(cos(lat)*cos(lon));
		if (t > 1.0) t = 0.0;
		if (t < -1.0) t = DEG2RAD(180);
		t = acos(t);
		ut_save  = ut;
		ut = ut - (gha + lambda + direction * t)/15;
	}
}

			/*
			dj = JulianDays(23, 8, 1984);	
			rising_sun(0, dj + 2415020, +1); // year 2000 2451545
			dj = JulianDays(24, 8, 1984);	
			rising_sun(0, dj + 2415020, +1); // year 2000 2451545
			dj = JulianDays(25, 8, 1984);	
			rising_sun(0, dj + 2415020, +1); // year 2000 2451545
			dj = JulianDays(26, 8, 1984);	
			rising_sun(0, dj + 2415020, +1); // year 2000 2451545

            */
			//dj = JulianDays(1.5, 1, 2000);
			//obliquity( dj, ob );
			//compute_degrees(ob, &hours, &minutes, &seconds);

			//dj = JulianDays(23, 4, 1995);
			//nutation(dj, dp, d0);
			//dp  *= 3600; d0 *= 3600;
			
/*
			dj = JulianDays(19, 5, 1950);
			x = hms2X(14, 26, 57);
			x *= 360/24;
			y = hms2X(32, 21, 05);
            geocentric_ecliptic_latlon_to_radec(+1, p, q, x, y, dj); // x,y -> p,q
			compute_degrees(p, &hours, &minutes, &seconds);
			compute_degrees(q, &hours, &minutes, &seconds);

			dj = JulianDays(19, 5, 1950);
			x = hms2X(200, 19, 6.66);
			y = hms2X(43, 47, 13.83);
            geocentric_ecliptic_latlon_to_radec(-1, p, q, x, y, dj); // x,y -> p,q
			compute_degrees(p, &hours, &minutes, &seconds);
			compute_degrees(q, &hours, &minutes, &seconds);

			gl = hms2X(62, 10, 12);
			gp = hms2X(-20, 0, 3.4);
			dj = JulianDays(23, 9, 1992);
			x =  hms2X(12, 16, 0);
			x *= 15.0;
			y =  hms2X(14, 34, 0);
			time_of_rising(x, y,  9.89e-3, gp, lu, ld, ad, au, never_sets, never_rises);
			compute_degrees(lu, &hours, &minutes, &seconds);
			compute_degrees(ld, &hours, &minutes, &seconds);
          
			x =  hms2X(6, 36, 7);
			x *= 15.0;
			y =  hms2X(87, 21, 10);
			time_of_rising(x, y,  0.0, gp, lu, ld, ad, au, never_sets, never_rises);
			compute_degrees(lu, &hours, &minutes, &seconds);
			compute_degrees(ld, &hours, &minutes, &seconds);

			x =  hms2X(6, 36, 7);
			x *= 15.0;
			y =  hms2X(-87, 0, 0);
			time_of_rising(x, y,  0.0, gp, lu, ld, ad, au, never_sets, never_rises);
			compute_degrees(lu, &hours, &minutes, &seconds);
			compute_degrees(ld, &hours, &minutes, &seconds);
			
			am = hms2X(220, 23, 10);
			anomaly(am, 0.0167183, ae, at);
			compute_degrees(ae, &hours, &minutes, &seconds);
			compute_degrees(at, &hours, &minutes, &seconds);

			dj = JulianDays(23, 8, 1984);	
			tm = 19; // time;
			tz = -5; // west is negative.
			ds = 0;  // nodaylight savings
			ut = tm - ds - tz;
            sun_position(dj, ut, sr, rr, d2, d3);
			compute_degrees(sr, &hours, &minutes, &seconds);

			dj = JulianDays(26, 8, 1984);			
			tm = 0; // time;
			tz = 0; // west is negative.
			ds = 0;  // nodaylight savings
			ut = tm - ds - tz;
			sun_position(dj, ut, sr, rr, d2, d3);
			compute_degrees(sr, &hours, &minutes, &seconds);

*/

/* noon calculation based on 
 * College Mathematics Journal Vol 34 No 4, September 2003, 
 * Teets, Predicting Sunrise and Sunset Times
 * 720 minutes = 12 hours (noon)
 * two correction terms
 * (a.) earths angle wrt orbital plane varies four times per year. 
 * (b.) day speed varies as elliptical orbit two times per year. 
 * with corrections for time zone (tzi), daylight savings (ds), and longitude (gl)
 */
void
sun_local_noon_transit(extended julianDay, 
		       extended tzi, extended ds, extended gl,
		       extended &minutes, int &ihours, int &iminutes)
{
  double 
    doy = dayOfYear(julianDay); // JulianDays(20, 7, 2016) - JulianDays(1, 1, 2016),
  minutes = 720 - 10*sin(4*M_PI / 365.25 *(doy - 80)) + 8*sin(2*M_PI/365.25*doy);
  minutes += (tzi+ds-gl/15)*60;
  ihours = floor(minutes/60);
  iminutes = floor((minutes/60-ihours)*60);
  //  printf("TODAY NOON (MERIDIAN TRANSIT) %d:%d\n", ihours, iminutes);
}


/* After so many hardware platforms, and operating systems, I have found it
   necessary to doubt results of all numerical floating point calculations.
   Hence the necessity to benchmark all results with prior values from 
   Peter Duffett-Smith's book page 122: 3400 SUN
*/
bool
selftest_sunposition()
{
  extended      dj,                             // julian day number
    ut = 0.0,                                   // universal time hours
    sr,                                         // true geocentric ecliptic longitude (degrees)
    rr,                                         // distance from earth astronomical units (AU)
    d2,                                         // longitude correction term
    d3,                                         // distance correction term
    am,                                         // am = m1 mean anomaly
    d0, dp,                                         // nutation
    x, y, p, q;                                 // (lon,lat), (ra, dec)
  int   ideg, imin;
  extended    fsec;
  ut = 19 + 5;   // local new york time 19hrs = 7pm,  universal (British) time = 19+5 = 24
  dj = JulianDays(23, 8, 1984);
  sun_position(dj, ut, sr, rr, d2, d3, am);

  // TRUE
  compute_degrees_fracsec(sr, &ideg, &imin, &fsec);
  printf("sr=%d %d %lf correct is +151 0 47.14\n", ideg, imin, fsec);
  if (ideg!=151 || imin!= 0 || fabs(fsec-47.14)>.05){ printf("check NO\n"); return false; }

  printf("rr=%lf correct is 1.01099\n", rr);
  x =  sr;          // dp - 5.69e-3;   // longitude degrees.
  y = 0;                    // latitude degrees (accurate to within 1.2 arcseconds)
  geocentric_ecliptic_latlon_to_radec(-1, p, q, x, y, dj); // x,y -> p,q (to radec from latlon)
  // convert from degree to hour angle
  p *= 24.0/360.0;
  compute_degrees_fracsec(p, &ideg, &imin, &fsec);
  printf("right ascension %d %d %lf correct 10 12 13.58\n", ideg, imin, fsec);
  if (ideg!=10 || imin!= 12 || fabs(fsec-13.58)>.05){ printf("check NO\n"); return false; }  

  compute_degrees_fracsec(q, &ideg, &imin, &fsec);
  printf("declination %d %d %lf correct 11 6 56.69\n", ideg, imin, fsec);
  if (ideg!=11 || imin!= 6 || fabs(fsec-56.69)>.05){ printf("check NO\n"); return false; }  

  // APPARENT
  nutation(dj, dp, d0);  
  x = sr + dp - 5.69e-3;
  y = 0;
  compute_degrees_fracsec(x, &ideg, &imin, &fsec);
  printf("apparent sr=%d %d %lf correct is +151 0 12.39\n", ideg, imin, fsec);
  if (ideg!=151 || imin!= 0 || fabs(fsec-12.39)>.05){ printf("check NO\n"); return false; }  
  
  geocentric_ecliptic_latlon_to_radec(-1, p, q, x, y, dj); // x,y -> p,q (to radec from latlon)
  // convert from degree to hour angle
  p *= 24.0/360.0;
  compute_degrees_fracsec(p, &ideg, &imin, &fsec);
  printf("apparent right ascension %d %d %lf correct 10 12 11.37\n", ideg, imin, fsec);
  if (ideg!=10 || imin!= 12 || fabs(fsec-11.37)>.05){ printf("check NO\n"); return false; }  
  
  compute_degrees_fracsec(q, &ideg, &imin, &fsec);
  printf("apparent declination %d %d %lf correct 11 7 9.01\n", ideg, imin, fsec);
  if (ideg!=11 || imin!= 7 || fabs(fsec-9.01)>.05){ printf("check NO\n"); return false; }    

  printf("check OK\n");
  return true;
}

bool
selftest_sunrise()
{
  extended       dj,
    tz,
    ds,
    gp,
    gl,
    msl_km,
    utu, dju,
    utd, djd;
  int ideg, imin;
  extended fsec;
  int  never_rises, never_sets;
  extended au, ad;
  printf("********************************************************************************\n");
  printf("SELFTEST APRIL 23, 1997, WEST TZ=-5 , DS=1, LAT=40N 43MIN 0SEC LON=74W 1MIN 0SEC\n");
  dj = JulianDays(23, 4, 1997);
  tz = -5;
  ds = 1;
  gp = hms2X(40, 43, 0);
  gl = hms2X(-74, 1, 0);
  msl_km = 0;
  sun_calculate_rise(utu, dju, utd, djd, dj, tz, ds, gl, gp, msl_km, never_rises, never_sets, au, ad);
  compute_degrees_fracsec(utu+tz+ds, &ideg, &imin, &fsec); 
  printf("SUNRISE %d %d %lf local time 6:05am\n", ideg, imin, fsec);
  if (fabs(utu+tz+ds-(6+5/60.))>1/60.){ printf("self check NO\n"); return false; }
  compute_degrees_fracsec(utd+tz+ds, &ideg, &imin, &fsec);
  printf("SUNSET %d %d %lf local time 7:44pm\n", ideg, imin, fsec);  
  if (fabs(utd+tz+ds-(19+44/60.))>1/60.) { printf("self check NO\n"); return false; }
  printf("self check OK\n");
  return true;
}

bool
selftest_anyrise()
{
  extended 
    lu,
    ld, 
    ad,
    au,
    x,
    y,
    gl,  // longitude
    gp,  // latitude
    dj,
    di;
  int 
    never_sets,
    never_rises,
    ideg, imin;
  extended fsec;

  printf("********************************************************************************\n");
  printf("SELFTEST SEPTEMBER 23, 1992, LAT=62N 10MIN 12SEC LON=20W 0MIN 3.4SEC\n");
  printf("RIGHT ASCENSION 12 HRS 16 MIN 00 SEC DECLINATION 14 DEG 34 MIN 00 SEC\n");  
  printf("TIMEZONE EAST TZ=4 DS=0\n");
  gl = hms2X(62, 10, 12);
  gp = hms2X(-20, 0, 3.4);
  dj = JulianDays(23, 9, 1992);
  x =  hms2X(12, 16, 0);
  x *= 15.0;
  y =  hms2X(14, 34, 0);
  time_of_rising(x, y,  9.89e-3, gp, lu, ld, ad, au, never_sets, never_rises);
  compute_degrees_fracsec(lu, &ideg, &imin, &fsec);
  printf("anyUP %d %d %lf local sidereal time 6 35 12.47\n", ideg, imin, fsec);
  ideg = 6;
  imin = 35;
  fsec = 12.47;
  if (fabs(ideg+(imin+fsec/60.0)/60.0-lu)>1/60.){ printf("check NO\n"); return false; }

  compute_degrees_fracsec(ld, &ideg, &imin, &fsec);
  printf("anyDOWN %d %d %lf local sidereal time 17 56 47.53\n", ideg, imin, fsec);
  ideg = 17;
  imin = 56;
  fsec = 47.53;
  if (fabs(ideg+(imin+fsec/60.0)/60.0-ld)>1/60.){ printf("check NO\n"); return false; }

  extended 
    ut,                   // universal time
    ds = 0,                   // daylight savings 
    tz = 4;                   // timezone
  time_sidereal_to_universal(ut, ds, tz, gl, dj, lu);
  compute_degrees_fracsec(ut, &ideg, &imin, &fsec);
  printf("anyUP universal %d %d %lf 2 17 47.81\n", ideg, imin, fsec);
  ideg = 2;
  imin = 17;
  fsec = 47.81;
  if (fabs(ideg+(imin+fsec/60.0)/60.0-ut)>1/60.){ printf("check NO\n"); return false; }
  
  dj = JulianDays(23, 9, 1992);
  x = hms2X(12, 16, 0);   // RA hours
  x *= 15.0;
  y = hms2X(14, 34, 0);   // DEC degrees
  gp = hms2X(-20, 0, 3.4);
  di = hms2X(0, 34, 0);
  di *= M_PI/180.0;       // radians
  printf("di=%lf\n", di);
  time_of_rising(x, y, di, gp, lu, ld,  ad, au, never_sets, never_rises);

  compute_degrees_fracsec(lu, &ideg, &imin, &fsec); 
  printf("anyrise %d %d %lf\n", ideg, imin, fsec);
  compute_degrees_fracsec(ld, &ideg, &imin, &fsec);
  printf("anyset %d %d %lf\n", ideg, imin, fsec);  

  printf("check OK\n");
  return true;
}

void
selftest_ephemeris(extended dj, extended tz, extended ds, extended gp, extended gl, extended msl_km, int nodays)
{
  extended
    utu, dju,
    utd, djd;
  extended
    ltu, dlu,   
    ltd, dld;
  int ideg, imin, ideg2, imin2;
  extended fsec, fsec2;
  int  never_rises, never_sets;
  extended au, ad, day, month, year, dow;

  CalendarDays(dj, day, month, year, dow);
  printf("********************************************************************************\n");
  printf("EPHEMERIS DJ=%lf (%lf %lf %lf) DOW=%lf TZ=%lf , DS=%lf, LAT=%lf LON=%lf\n",
	 dj,
	 day, month, year, dow,
	 tz, ds, gp, gl);
  printf("FIFTEEN DAYS\n");

  for (int i=0;i<nodays;i++){
    sun_calculate_rise(utu, dju, utd, djd, dj+i, tz, ds, gl, gp, msl_km, never_rises, never_sets, au, ad);
    universalToLocal(dlu, ltu,  dju, utu, ds, tz);  // sunrise
    universalToLocal(dld, ltd,  djd, utd, ds, tz);  // sunset
    compute_degrees_fracsec(ltu, &ideg, &imin, &fsec); 
    compute_degrees_fracsec(ltd, &ideg2, &imin2, &fsec2);
    printf("SUNRISE %d %d %d %lf SUNSET %d %d %d %lf\n", (int)dlu, ideg, imin, fsec, (int)dld, ideg2, imin2, fsec2);
  }
  printf("********************************************************************************\n");
}


int
selftest_ephemeris_withcheck(const char *comment, extended day, extended month, extended year, extended tz,
			     extended ds, extended gp, extended gl, extended msl_km,
			     extended check_ltu, extended check_ltd, extended minutes)
{
  extended
    dj,
    utu, dju,
    utd, djd;
  extended
    ltu, dlu,   
    ltd, dld;
  int ideg, imin, ideg2, imin2;
  extended fsec, fsec2;
  int  never_rises, never_sets;
  extended au, ad, dow;

  dj = JulianDays(day, month, year);   
  CalendarDays(dj, day, month, year, dow);

  printf("%s DJ=%lf (%lf %lf %lf) DOW=%lf TZ=%lf , DS=%lf, LAT=%lf LON=%lf ",
	 comment,
	 dj,
	 day, month, year, dow,
	 tz, ds, gp, gl);

  sun_calculate_rise(utu, dju, utd, djd, dj, tz, ds, gl, gp, msl_km, never_rises, never_sets, au, ad);
  universalToLocal(dlu, ltu,  dju, utu, ds, tz);  // sunrise
  universalToLocal(dld, ltd,  djd, utd, ds, tz);  // sunset
  // check day and time
  ltu = fmod(ltu, 24.0);
  ltd = fmod(ltd, 24.0);
  printf("ltu: %lf == %lf ltd: %lf == %lf  ", check_ltu, ltu, check_ltd, ltd);
  if (fabs(check_ltu - ltu) > minutes/60.0){ printf("check NO\n"); return 0; }
  if (fabs(check_ltd - ltd) > minutes/60.0){ printf("check NO\n"); return 0; }
  printf("check OK\n");

  double T = centuries_j2000(dj);  
  utu = calculate_rise_time(T, gp, gl, +1, msl_km*1000);
  utd = calculate_rise_time(T, gp, gl, -1, msl_km*1000);
  universalToLocal(dlu, ltu,  dju, utu, ds, tz);  // sunrise
  universalToLocal(dld, ltd,  djd, utd, ds, tz);  // sunset
  ltu = fmod(ltu, 24.0);
  ltd = fmod(ltd, 24.0);
  printf("USNO ltu: %lf == %lf ltd: %lf == %lf \n", check_ltu, ltu, check_ltd, ltd);  
  if (fabs(check_ltu - ltu) > minutes/60.0){ printf("USNO check NO\n"); return 0; }
  if (fabs(check_ltd - ltd) > minutes/60.0){ printf("USNO check NO\n"); return 0; }
  printf("USNO check OK\n");

  return 1;
}
