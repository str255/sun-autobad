/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
/*
 * location_expression.h
 * Definition of the structure used to build the syntax tree.
 */
#ifndef __LOCATIONEXPRESSION_H__
#define __LOCATIONEXPRESSION_H__

typedef struct taglocationExpression
{
  double    lat, lon, elevation;
} locationExpression;


locationExpression *allocate_locationExpression();
void delete_locationExpression(locationExpression *b);
locationExpression *createLocation(double latitude, double longitude, double elevation=0.0);
double createLatitudeLongitude(char *latitude,char posidir);
double createLatitudeLongitudeDecimal(char *latitude,char posidir);
double createLatitudeLongitudeSec(char *latitude,char posidir);
double createElevation(char *elevation);
double createElevationFeet(char *elevation);

#endif // __LOCATIONEXPRESSION_H__
