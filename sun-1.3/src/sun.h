/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
#ifndef __SUNH__
#define __SUNH__
void geocentric_ecliptic_latlon_to_radec(extended direction, 
	extended &p, extended &q, extended &x, extended &y, extended dj );
void obliquity(extended dj , extended &ob );
void nutation(extended dj, extended &dp, extended &d0);
void time_of_rising(extended x, extended y, extended di, extended gp,
	extended &lu, extended &ld, extended &ad, extended &au,
	int &never_sets, int &never_rises);
void anomaly(extended am, extended ec, extended &ae, extended &at);
void meeus_sun_position(extended day, extended month, extended year, extended& theta_app);
void sun_local_noon_transit(extended julianDay, extended tzi, extended ds, extended gl,
			    extended &minutes, int &ihours, int &iminutes);
void sun_position(extended dj, extended ut, 
	extended &sr, extended &rr, extended &d2, extended &d3, extended &am);
void time_sidereal_to_universal(extended &ut, extended ds, extended tz, extended gl,
 	extended dj, extended tm);
void sun_calculate_rise(extended &utu, extended &dju, extended &utd, extended &djd,
	extended dj, extended tz, extended ds,
	extended gl, extended gp, extended msl_km, int &never_rises, int &never_sets,
	extended &au, extended &ad);
void sun_calculate_rise_tilnow(extended &utu, extended &dju, extended &utd, extended &djd,
	extended dj, extended tz, extended ds,
       extended gl, extended gp, extended now, extended now_hour, extended msl_km, int &never_rises, int &never_sets,
	extended &au, extended &ad);
void sun_calculate_civil_twilight(extended &utu, extended &dju, extended &utd, extended &djd, 
	extended dj, extended tz, extended ds,
	extended gl, extended gp, int &never_rises, int &never_sets,
	extended &au, extended &ad);
void sun_calculate_twilight_tilnow(extended &utu, extended &dju, extended &utd, extended &djd,
	extended dj, extended tz, extended ds,
        extended gl, extended gp, extended now, extended now_hour, extended msl_km, int &never_rises, int &never_sets,
        extended &au, extended &ad, extended rad_twilight);
void sun_calculate_rise_gen(extended &utu, extended &dju, extended &utd, extended &djd,
	extended dj, extended tz, extended ds,
	extended gl, extended gp, int &never_rises, int &never_sets,
	extended &au, extended &ad, extended di);
void sun_calculate_rise_gen_tilnow(extended &utu, extended &dju, extended &utd, extended &djd,
	extended dj, extended tz, extended ds,
        extended gl, extended gp, extended now, extended now_hour, int &never_rises, int &never_sets,
	extended &au, extended &ad, extended di);

void geocentric_ecliptic_latlon_to_radec(extended direction, 
	extended &p, extended &q, extended &x, extended &y, extended dj );
void obliquity(extended dj , extended &ob );
void nutation(extended dj, extended &dp, extended &d0);
void time_of_rising(extended x, extended y, extended di, extended gp,
	extended &lu, extended &ld, extended &ad, extended &au,
	int &never_sets, int &never_rises);
void anomaly(extended am, extended ec, extended &ae, extended &at);
void meeus_sun_position(extended day, extended month, extended year, extended& theta_app);
void sun_local_noon_transit(extended julianDay, extended tzi, extended ds, extended gl,
			    extended &minutes, int &ihours, int &iminutes);
void sun_position(extended dj, extended ut, 
	extended &sr, extended &rr, extended &d2, extended &d3, extended &am);
void time_sidereal_to_universal(extended &ut, extended ds, extended tz, extended gl,
 	extended dj, extended tm);
void sun_calculate_rise(extended &utu, extended &dju, extended &utd, extended &djd,
	extended dj, extended tz, extended ds,
	extended gl, extended gp, extended msl_km, int &never_rises, int &never_sets,
	extended &au, extended &ad);
void sun_calculate_civil_twilight(extended &utu, extended &dju, extended &utd, extended &djd, 
	extended dj, extended tz, extended ds,
	extended gl, extended gp, int &never_rises, int &never_sets,
	extended &au, extended &ad);
void sun_calculate_rise(extended &utu, extended &dju, extended &utd, extended &djd,
	extended dj, extended tz, extended ds,
	extended gl, extended gp, extended now, extended msl_km, int &never_rises, int &never_sets,
	extended &au, extended &ad);

void solar_arguments(extended t, 
	extended &l, extended &g, extended &lambda, extended &epsilon);
void time_equation(extended &e, extended &gha, extended &dec, extended &sd,
 	extended g, extended lambda, extended epsilon, extended ut);
void rising_sun(extended &ut, extended jd, extended direction);
bool selftest_sunposition();
bool selftest_sunrise();
bool selftest_anyrise();
void selftest_ephemeris(extended dj, extended tz, extended ds, extended gp, extended gl, extended msl_km, int nodays);
int selftest_ephemeris_withcheck(const char *comment, extended day, extended month, extended year, extended tz, extended ds, extended gp, extended gl, extended msl_km, extended check_ltu, extended check_ltd, extended minutes=5.0);


#endif
