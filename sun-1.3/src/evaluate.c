/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
#include <math.h>
#include "expression.h"
#include "evaluate.h"
#include "location_expression.h"
#include "parser.h"
#include "lexer.h"
#include <stdio.h>
#include <search.h>
#include "main.h"
#define MAIN extern
#include "globals.h"

struct hsearch_data   *mactab;
funcall_t funcalls[i_maxFuncalls] = {
    {  i_nighttime          , "is_nighttime", handle_isnighttime, },
    {  i_daytime            , "is_daytime", handle_isdaytime, },  
    
    {  i_sunrise            , "sunrise", handle_sunrise, },
    {  i_sunset             , "sunset",  handle_sunset, },
    {  i_azimuth            , "azimuth",  handle_azimuth, },
    {  i_altitude           , "altitude",  handle_altitude, },

    {  i_now                , "now",     handle_now, },

    {  i_month              , "month",   handle_month, },
    {  i_dow                , "dow",     handle_dow, },
    {  i_year               , "year",    handle_year, },                

    {  i_jan                , "jan",     handle_jan, },
    {  i_feb                , "feb",     handle_feb, },
    {  i_mar                , "mar",     handle_mar, },
    {  i_apr                , "apr",     handle_apr, },
    {  i_may                , "may",     handle_may, },    
    {  i_jun                , "jun",     handle_jun, },
    {  i_jul                , "jul",     handle_jul, },
    {  i_aug                , "aug",     handle_aug, },
    {  i_sep                , "sep",     handle_sep, },
    {  i_oct                , "oct",     handle_oct, },
    {  i_nov                , "nov",     handle_nov, },
    {  i_dec                , "dec",     handle_dec, },            
};

void
initialize_cvalues()
{
  ENTRY        e, *ep;
  int    i, success;
  mactab=(struct hsearch_data *)calloc(i_maxFuncalls, sizeof(struct hsearch_data));
  success=hcreate_r(i_maxFuncalls, mactab);
  if (!success)
    printf("hcreate_r: ran out of memory %d\n", errno);

  for (i=0;i<i_maxFuncalls;i++){
    e.key = funcalls[i].str;
    e.data = (void*)&funcalls[i];
    success = hsearch_r(e, ENTER, &ep, mactab);  
    if (!success)
      printf("hcreate_r: ran out of memory %d\n", errno);
  }
}

double
evaluate_cvalue(char *cvalue, bool *truth)
{
  int       success;
  ENTRY     e, *ep;
  *truth = 0;
  e.key = cvalue;
  e.data = (void*)NULL;
  success = hsearch_r(e, FIND, &ep, mactab);  
  if (ep){
    *truth = 1;
    return ((funcall_t*) (ep->data))->handler();
  }
  else return 0;
}

/* 0.0 is false, 
   non-zero is true */
double 
evaluate(SExpression *e)
{
  if (!e) return 0.0;             // handle unary minus/plus

  switch (e->type) {
  case eVALUE:
    return e->value;
  case eVARIABLE:
    {
      bool   truth;
      double value = evaluate_cvalue(e->cvalue, &truth);
      printf("evaluate: cvalue %s value %lf %d\n", e->cvalue, value, truth);
      return value;
    }
  case eLESS:
    {
      double
	l = evaluate(e->left),
	m = evaluate(e->right);
      if (isnan(l) ||  isnan(m)) return nan("false");
      if (l <= m) return m;
      else return nan("false");
    }
  case eMORE:
    {
      double
	l = evaluate(e->left),
	m = evaluate(e->right);
      if (isnan(l) ||  isnan(m)) return nan("false");
      if (l >= m) return m;
      else return nan("false");
    }
  case eAND:
    {
      double
	l = evaluate(e->left),
	m = evaluate(e->right);
      if (isnan(l) ||  isnan(m)) return nan("false");
      return l && m;
    }
  case eOR:
    {    
      double
	l = evaluate(e->left),
	m = evaluate(e->right);
      if (isnan(l) ||  isnan(m)) return nan("false");
      return l || m;
    }
  case ePLUS:
    {
    return evaluate(e->left) + evaluate(e->right);
    }
  case eMINUS:
    {
    return evaluate(e->left) - evaluate(e->right);
    }
  case eTIMES:
    {
    return evaluate(e->left) * evaluate(e->right);
    }
  case eDIVISION:
    {
    return evaluate(e->left) / evaluate(e->right);
    }
  default:
    // shouldn't be here
    return nan("error");
  }
}

double handle_isnighttime()
{
  return nighttime;
}

double handle_isdaytime()
{
  return daytime;
}

double handle_sunrise()
{
  return jdays_sunrise;
}

double handle_sunset()
{
  return jdays_sunset;
}

double handle_azimuth()
{
  return sun_azimuth;
}

double handle_altitude()
{
  return sun_altitude;
}

double handle_now()
{
  return jdays_now;
}

double handle_month()
{
  return now_month;
}

double handle_dow()
{
  return now_dayOfWeek;
}

double handle_year()
{
  return now_year;
}

/* double handle_season() */
/* { */
/*   now_month */
/* } */

double handle_jan(){  return 1; }
double handle_feb(){  return 2; }
double handle_mar(){  return 3; }
double handle_apr(){  return 4; }
double handle_may(){  return 5; }
double handle_jun(){  return 6; }
double handle_jul(){  return 7; }
double handle_aug(){  return 8; }
double handle_sep(){  return 9; }
double handle_oct(){  return 10; }
double handle_nov(){  return 11; }
double handle_dec(){  return 12; }

