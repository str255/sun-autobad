/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "startime.h"


#define gettext_noop(String) String

const char 						*dayStringArray[]=
  {gettext_noop("Sunday   "),
   gettext_noop("Monday    "),
   gettext_noop("Tuesday    "),
   gettext_noop("Wednesday    "),
   gettext_noop("Thursday   "),
   gettext_noop("Friday  "),
   gettext_noop("Saturday   "), 
		NULL};
const char 						*dayStringArrayComma[]=
{"Sunday,    ","Monday,     ","Tuesday,    ",
 "Wednesday,     ","Thursday,    ","Friday,   ",
 "Saturday,    ", NULL};
const char 						*dayStringArrayPlural[]={"Sundays    ","Mondays    ","Tuesdays   ",
									"Wednesdays    ","Thursdays  ","Fridays  ",
										 "Saturdays     ", NULL};
//const char 						*dayStringArrayComma[8];
//const char 						*dayStringArrayPlural[8];
const char						*small_dow_string[] = {"S","M","T","W","R","F","S"};
Str15						smallPascal_dow_string[] = {"S","M","T","W","R","F","S"};
const char 						*monthStringArray[]={
  gettext_noop(  "January"),
  gettext_noop(  "February"),
  gettext_noop(  "March"),
  gettext_noop(  "April"),
  gettext_noop(  "May"),
  gettext_noop(  "June"),
  gettext_noop(  "July"), 
  gettext_noop(  "August"), 
  gettext_noop(  "September"), 
  gettext_noop(  "October"), 
  gettext_noop(  "November"), 
  gettext_noop(  "December"), 
  NULL };
//const char 						*monthStringArray[13];
const char 						*hebrew_month_names[13]={"Nisan", "Iyyar", "Sivan", "Tammuz", "Av", "Elul",
									"Tishri", "Heshvan", 
                                     "Kislev", "Tevet", "Shevat",
                                     "AdarI", "AdarII"};
const char 						*islamic_month_names[12]={ "Muharram", "Safar", "Rabi' al-Awwal", 
									 "Rabi' al-Thaani", "Jumada al-Ooola", "Jumada al-Ukhra",
									 "Rajab", "Sha'ban", 
                                     "Ramadan", "Shawwal", "Dhu al-Q'adah",
                                     "Dhu al-Hijjah" };
const char 						*indian_civil_month_names[12]={ "Caitra",  "Vaisakha", "Jyaistha", "Asadha", 
									 "Sravana", "Bhadra", "Asvina",
									 "Kartika", "Agrahayana", 
                                     "Pausa", "Magha", "Phalguna"
                                     };
const char 						*coptic_month_names[13]={ "Tut", "Babah", "Hatur", "Kiyak", "Tubah",
									"Amshir", "Baramhat", "Baramundah", "Bashans", "Ba'unah",
									"Abib", "Misra", "al-Nasi" };
const char 						*ethiopic_month_names[13]={ "Maskaram", "Teqemt", "Khedar", "Takhas",
									"Ter", "Yakatit", "Magabit", "Miyazya", "Genbot", "Sane",
									"Hamle", "Nahase", "Paguemen" };
const char 						*leapArray[2] = {"I", "II"};
const char    				    *zodiac[13];
const char dayTableaux_string1[80]= "Moon in house of %s";
const char dayTableaux_string2[80] = "Lunar Lat. %d:%d:%d  Lon. %d:%d:%d Dist. %6.0f (km)";
const char dayTableaux_string3[80] = "Lunar Lat. %d:%d:%d  Lon. %d:%d:%d Dist. %6.0f (mi)";
const char dayTableaux_string4[80] = "Sun in house of %s";
const char dayTableaux_string5[80] = "Solar Lon. %d:%d:%d Distance %f (A.U.)";
const char dayTableaux_string6[80] = "(+%d) Daylight Savings";
const char dayTableaux_string7[80] = "UT";
unsigned const char dayTableaux_pstring8[80] = "Sunrise null              ";
const char dayTableaux_string9[] = "Sunrise";
unsigned const char dayTableaux_pstring10[80] = "Sunset null              ";
const char dayTableaux_string11[80] = "Sunset";
const char dayTableaux_string12[80] = "(+%d) Daylight Savings               "; 
const char dayTableaux_string13[80] = "UT"; 
unsigned const char dayTableaux_pstring14[80] = "Moonrise null            "; 
const char dayTableaux_string15[] = "Moonrise"; 
unsigned const char dayTableaux_pstring16[80] = "Moonset null             "; 
const char dayTableaux_string17[80] = "Moonset"; 
const char dayTableaux_string18[80] = "civil twilight";
const char dayTableaux_string19[80] = "naut. twilight";
const char dayTableaux_string20[80] = "astro. twilight";
const char dayTableaux_string21[80] = "%d days before spring equinox";
const char dayTableaux_string22[80] = "%d days before summer solstice";
const char dayTableaux_string23[80] = "%d days before fall equinox";
const char dayTableaux_string24[80] = "%d days before winter solstice";
const char dayTableaux_string25[80] = "(local time is %f hours before GMT)";          // timezone
const char dayTableaux_string26[80] = "%4.1f years %9.3f weeks %9.1f days from today";          // deltatoday

const char startime_string1[80] = "Midnight    ";
const char startime_string2[80] = "Noon        ";
const char startime_string3[80] = "Week of %d %s %d        ";
const char menuList_week[80]    = "Week        ";

void SysBreakStr(char const*str)
{
  printf("%s\n",str);
}

// void fullTimeString(char* timeString, extended t)
// {
//   //	extern 	char			*monthStringArray[];
//   //	extern 	char			*dayStringArray[];
// 	extended                fd, r_dow = 0., r_day = 0., r_month = 0., r_year = 0., time;
// 	int                     hours, dow, day, month, year;
// 	extended                minutes;

//     CalendarDays(t, r_day, r_month, r_year, r_dow); 
//     //    fd = t;// - 0.5;          
//     //    time = floor(fd);
//     fd = t-0.5;
//     fd = fd - floor(fd);
//     hours = floor(fd*24); // dont + 0.5);       // round to nearest hour.
//     minutes = (fd*24 - hours)*60.0;
//     day = floor(r_day); 
//     dow = r_dow; 
//     month = floor(r_month);
//     year = floor(r_year); 
//     sprintf(timeString,"%s %d:%lf %s %d %d",
// 	    gettext(dayStringArray[dow-1]), hours, minutes, gettext(monthStringArray[month-1]), day, year);
// }
