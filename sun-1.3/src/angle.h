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
static const double 	PI180 		      = 1.74532925e-2;  
static const double 	INV_PI180 		  = 1/1.74532925e-2;  
static const double 	HOURSPI 		  = 3.81971863;  
static const double 	INV_HOURSPI 		  = 1/3.81971863;  
extended deg2rad(extended a);
extended rad2deg(extended a);
extended rad2hours(extended a);
extended hours2rad(extended a);
extended deg2hours(extended a);
extended hours2deg(extended a);
extended hms2X(extended hours, extended minutes, extended seconds);
double SIN(double angle);
double COS(double angle); 
void compute_degrees(extended reading, 
	int *in_degrees, int *in_minutes, int *in_seconds);
void compute_degrees2(extended reading, 
	int *in_degrees, int *in_minutes, int *sgn);
void compute_degrees3(extended x, int &ideg, float &fmin, int &isgn);
void compute_degrees_fracsec(extended reading, int *in_degrees, int *in_minutes, extended *in_seconds);
extended positive_hours(extended hours);
void compute_hours(extended reading, int *in_hours, 
	int *in_minutes, int *in_seconds);
void compute_day_hours(extended reading,
	 int *in_days, int *in_hours, int *in_minutes, int *in_seconds);
void compute_degrees_keepSign(extended reading, 
	int &in_degrees, int &in_minutes, int &in_seconds);

