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
#ifndef __EQUATIONOFTIME__
#define __EQUATIONOFTIME__
double equation_of_time(double T, double UT, double &lambda, double& dec);
double greenwich_hour_angle(double UT, double E);
double sun_hour_angle(double phi, double H, double dec);
double calculate_rise_time(double T, double lat, double lon, int sgn, double H);
void sun_alt_az(double jd, double LT, double tz, double ds, double lat, double lon, double h, double &alt, double &az);

#endif
