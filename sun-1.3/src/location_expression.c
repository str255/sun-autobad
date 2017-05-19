/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
#include "location_expression.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>

double createLatitudeLongitude(char *cvalue, char posidir)
{
  int    deg;
  char   dir;
  double min, value;
  char *old_locale, *saved_locale;

  printf("createLatitudeLongitude %s %c\n", cvalue, posidir);

  
  /* Get the name of the current locale.  */
  old_locale = setlocale (LC_ALL, NULL);

  /* Copy the name so it won’t be clobbered by setlocale. */
  saved_locale = strdup (old_locale);
  if (saved_locale == NULL)
    printf ("Out of memory");
  

  /* Now change the locale and do some stuff with it. */
  setlocale (LC_ALL, "C");

  sscanf(cvalue, "%d~%lf%c", &deg, &min, &dir);
  /* Restore the original locale. */
  setlocale (LC_ALL, saved_locale);
  free (saved_locale);

  printf("deg %d min %lf dir %c\n", deg, min, dir);
  value = deg + min/60.0;
  if (dir==posidir) value = -value;
  return value;
}

double createLatitudeLongitudeSec(char *cvalue, char posidir)
{
  char   dir;
  int deg, min;
  double sec, value;
  char *old_locale, *saved_locale;

  printf("createLatitudeLongitudeSec %s %c\n", cvalue, posidir);


  /* Get the name of the current locale.  */
  old_locale = setlocale (LC_ALL, NULL);

  /* Copy the name so it won’t be clobbered by setlocale. */
  saved_locale = strdup (old_locale);
  if (saved_locale == NULL)
    printf ("Out of memory");
  

  /* Now change the locale and do some stuff with it. */
  setlocale (LC_ALL, "C");

  sscanf(cvalue, "%d~%d'%lf%c", &deg, &min, &sec, &dir);
  /* Restore the original locale. */
  setlocale (LC_ALL, saved_locale);
  free (saved_locale);

  printf("deg %d min %d sec %lf dir %c\n", deg, min, sec, dir);
  value = deg+min/60.+sec/3600.;
  if (dir==posidir) value = -value;
  return value;
}

double createLatitudeLongitudeDecimal(char *cvalue, char posidir)
{
  char   dir;
  double deg, value;
  char *old_locale, *saved_locale;
  
  printf("createLatitudeLongitudeDecimal %s %c\n", cvalue, posidir);

  /* Get the name of the current locale.  */
  old_locale = setlocale (LC_ALL, NULL);

  /* Copy the name so it won’t be clobbered by setlocale. */
  saved_locale = strdup (old_locale);
  if (saved_locale == NULL)
    printf ("Out of memory");
  

  /* Now change the locale and do some stuff with it. */
  setlocale (LC_ALL, "C");

  sscanf(cvalue, "%lf%c", &deg, &dir);

  /* Restore the original locale. */
  setlocale (LC_ALL, saved_locale);
  free (saved_locale);

  
  printf("deg %lf dir %c\n", deg, dir);
  value = deg;
  if (dir==posidir) value = -value;
  return value;
}

double createElevation(char *elevation)
{
  double  val;
  printf("createElevation %s\n", elevation);
  sscanf(elevation, "%lfm", &val);
  return val;
}

double createElevationFeet(char *elevation)
{
  double  val;
  printf("createElevation %s\n", elevation);
  sscanf(elevation, "%lfft", &val);
  return val*0.3028;  // convert to meters
}

locationExpression *createLocation(double latitude, double longitude, double elevation)
{
  printf("createLocation: %lf %lf %lf\n", latitude, longitude, elevation);
  locationExpression *b = allocate_locationExpression();
  b->lat = latitude;
  b->lon = longitude;
  b->elevation = elevation;
  return b;
}

locationExpression *allocate_locationExpression()
{
  locationExpression *b = (locationExpression *)malloc(sizeof(locationExpression));

  if (b == NULL)
    return NULL;

  b->lat = 0.0;
  b->lon = 0.0;
  b->elevation = 0.0;

  return b;
}

void delete_locationExpression(locationExpression *b)
{
  if (b == NULL)
    return;
  free(b);
}
