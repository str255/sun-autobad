/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "parse_options.h"
#include <time.h>   // for timezone
#include "startime.h"
#include <locale.h>

extern Options sun_options;
//extern char glocation[255];

int
parse_options (int argc, char **argv)
{
  int c;

  while (1)
    {
            static struct option long_options[] =
	      {
		{"daylight", required_argument, 0, 'd'},
		{"help",    no_argument,       0, 'h'},		
		{"location", required_argument, 0, 'l'},		
		{"query", required_argument, 0, 'q'},
		{"selftest", no_argument,      0, 's'},
		{"timezone", required_argument, 0, 't'},
		{"verbose",   no_argument,     0, 'v'},
		{0, 0, 0, 0}
	      };
	    /* getopt_long stores the option index here. */
	    int option_index = 0;

	    c = getopt_long (argc, argv, "d:hl:q:st:v",
			     long_options, &option_index);

	    /* Detect the end of the options. */
	    if (c == -1)
	      break;

	    switch (c)
	      {

	      case 'a':
		puts ("option -a\n");
		break;

	      case 'v':
		puts ("option -v\n");
		sun_options.self_verbose = true;
		break;

	      case 'c':
		printf ("option -c with value `%s'\n", optarg);
		break;

	      case 'f':
		printf ("option -f with value `%s'\n", optarg);
		break;
		// real options follow
		
		// daylight savings hour
	      case 'd':
		printf ("option -d with value `%s'\n", optarg);
		printf("user specified daylight savings override %s\n", optarg);
		sun_options.daylight = atof(optarg);
		sun_options.self_guess_daylight_savings = false;
		break;

		// timezone hour
	      case 't':
		printf ("option -t with value `%s'\n", optarg);
		printf("user specified timezone override %s\n", optarg);
		sun_options.timezone = atof(optarg);
		break;


	        // location
	      case 'l':
		printf ("user specified location -l with value `%s'\n", optarg);
		strcpy(sun_options.location, optarg);
		break;

		// query
	      case 'q':
		printf ("query -q with value `%s'\n", optarg);
		strcpy(sun_options.query, optarg);
		sun_options.mode_query = true;
		break;

		// selftest
	      case 's':
		printf ("selftest -s\n");
		sun_options.self_test = true;
		break;

	      case '?':
	      case 'h':
		printf("sun --location <geographic-coordinates> --timezone <hours east of greenwich> --daylight <offset-hours>\n");
		printf("\t--query <query-string> returns 1 if query is true, 0 otherwise\n");
                printf("\t--selftest will run exhaustive test cases\n");
		printf("\t--verbose will print diagnostic information\n");
		printf("\tsun can be called with no (or partial) arguments, and it will guess at timezone, location, and daylight savings.\n");
		printf("\texample: sun --location \"36.048202N 114.950131W 0ft\" --timezone -8.0  --daylight 1 \n");
		exit(0);
		break;

	      default:
		exit(0);
	      }
    }


  /* Print any remaining command line arguments (not options). */
  if (optind < argc)
    {
      printf ("non-option ARGV-elements: ");
      while (optind < argc)
	printf ("%s ", argv[optind++]);
      putchar ('\n');
    }

  return 0;
}


/* latlon = vegas 
 * timezone from unix zone
 * daylight savings from unix zone
 */
void
default_options()
{
  char *old_locale, *saved_locale;
  // Geographic format is fairly flexible latitude, longitude and elevation.
  // For timekeeping, elevation is an idealization, works well for a smooth earth,
  // but not terribly effective for mountainous regions. So, for example, here in
  // Vegas the River Mountains rise in the east, and the Spring mountains are in the
  // west. Las Vegas sits in a valley which effectively makes sunrise later and sunset earlier.
  // Hence, putting an elevation value of 0ft is actually not such a bad idea. 
  // char las_vegas_location[]="36.115656N 115.172456W 1700ft"; // right click, what's here in Google maps
  //  char las_vegas_location[]="36.048202N 114.950131W 0ft"; // right click, what's here in Google maps
  char las_vegas_location[]="36.0N %fW 0ft"; // right click, what's here in Google maps    
  // Use /usr/share/zoneinfo database
  // export TZ=":America/Los_Angeles";
  if (sun_options.self_verbose){
    printf("+------------------------------------------------------+\n");
    printf("timezone (zoneinfo) |%s| |%s|\n", tzname[0], tzname[1]);
  }

  // TZSET SYSTEM CALL
  tzset();
  if (sun_options.self_verbose){
    printf("timezone (zoneinfo) |%s| |%s|\n", tzname[0], tzname[1]);
    printf("\t timezone=%ld seconds %lf hours west of Greenwich\n", timezone, ((double)timezone)/3600.); // timezone defined in time.h 
  }

  /* Get the name of the current locale.  */
  old_locale = setlocale (LC_ALL, NULL);

  /* Copy the name so it won’t be clobbered by setlocale. */
  saved_locale = strdup (old_locale);
  if (saved_locale == NULL)
    printf ("Out of memory");
  

  /* Now change the locale and do some stuff with it. */
  setlocale (LC_ALL, "C");
  sprintf(sun_options.location, las_vegas_location, ((double)timezone)/240.0);
  /* Restore the original locale. */
  setlocale (LC_ALL, saved_locale);
  free (saved_locale);

  sun_options.timezone = -(double)timezone/3600.0;        // negative hours west of greenwich
  sun_options.self_guess_daylight_savings = true;
  sun_options.daylight = 0; 
  sun_options.self_test = false;
  sun_options.self_verbose = false;  
  sun_options.mode_query = false;
  sun_options.mode_twilight = CIVIL_TWILIGHT;
}

double
guess_daylight_savings(double jd, double year, double ds)
{
  double  dst_beg = JulianDaysDOW(1,2,3,year),
    dst_end = JulianDaysDOW(1,1,11,year);
  if (dst_beg<jd and jd<dst_end) return ds;
  return 0;
}

void
print_options()
{
  printf("sun_options.location %s\n", sun_options.location);
  printf("sun_options.timezone %lf\n", sun_options.timezone);
  printf("sun_options.daylight %lf\n", sun_options.daylight);
  printf("sun_options.self_guess_daylight_savings %d\n", sun_options.self_guess_daylight_savings);
  printf("sun_options.self_test %d\n", sun_options.self_test);
  printf("sun_options.self_verbose %d\n", sun_options.self_verbose);
  printf("sun_options.mode_query %d\n", sun_options.mode_query);
  printf("sun_options.mode_twilight %d\n", sun_options.mode_twilight);
}

