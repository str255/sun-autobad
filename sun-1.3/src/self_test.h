/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
#ifndef __SELFTEST__
#define __SELFTEST__

void do_self_test();
int yyparse(SExpression **expression, locationExpression **lexpression, yyscan_t scanner);
SExpression *getAST(const char *expr);
bool logic_withcheck(char const *test);
locationExpression *getLocation(const char *expr);
#endif
