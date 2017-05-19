/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
#ifndef __MAINH__
#define __MAINH__

double get_now_jdays();
void get_now_sunrise_sunset(double dj, 
			      double tzi, 
			      double ds,
			      double gp, 
			      double gl, 
			      double msl_km);
void get_now_twilight_rise_set(double dj, 
			      double tzi, 
			      double ds,
			      double gp, 
			      double gl, 
			      double msl_km);
bool do_query();

#endif
