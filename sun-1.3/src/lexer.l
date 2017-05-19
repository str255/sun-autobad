%{

/*
 * Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*
* lexer.l file
* To generate the lexical analyzer run: "flex lexer.l"
*/

#include "expression.h"
#include "location_expression.h"
#include "parser.h"

#include <stdio.h>

%}

%option outfile="lexer.c" header-file="lexer.h"
%option warn nodefault

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

LPAREN      "("
RPAREN      ")"
LESS        "<"
MORE        ">"
AND         "&"
OR          "|"
PLUS        "+"
MINUS       "-"
TIMES       "*"
DIVISION    "/"

NUMBER      [0-9]+
DOUBLE      [0-9]+[.]{0,1}[0-9]+
WS          [ \r\n\t]*
VARIABLE    [a-zA-Z]+
 /* LATITUDE    [0-9]{1,3}[NS][0-9]{0,2}[.]{0,1}[0-9]{0,4}  */
 /* LONGITUDE   [0-9]{1,3}[EW][0-9]{0,2}[.]{0,1}[0-9]{0,4} */
 /* ELEVATION   [0-9]+.[0-9]+MSL */
LATITUDE    [-]{0,1}[0-9]{1,3}~[0-9]{0,2}[.]{0,1}[0-9]{0,4}[NS]
LONGITUDE   [-]{0,1}[0-9]{1,3}~[0-9]{0,2}[.]{0,1}[0-9]{0,4}[EW]
LATITUDE_DECIMAL  [-]{0,1}[0-9]{1,3}[.]{0,1}[0-9]{0,6}[NS]
LONGITUDE_DECIMAL  [-]{0,1}[0-9]{1,3}[.]{0,1}[0-9]{0,6}[EW]
LATITUDE_SEC    [-]{0,1}[0-9]{1,3}~[0-9]{1,2}'[0-9]{0,2}[.]{0,1}[0-9]{0,4}[NS]
LONGITUDE_SEC   [-]{0,1}[0-9]{1,3}~[0-9]{1,2}'[0-9]{0,2}[.]{0,1}[0-9]{0,4}[EW]

ELEVATION   [-]{0,1}[0-9]+(m|M)
ELEVATION_FT   [-]{0,1}[0-9]+(ft|FT)

%%

{WS}            { /* Skip blanks. */ }
{NUMBER}        { sscanf(yytext, "%lf", &yylval->value); return TOKEN_NUMBER; }
{DOUBLE}        { sscanf(yytext, "%lf", &yylval->value); return TOKEN_NUMBER; }
{VARIABLE}      { yylval->cvalue = strdup(yytext); return TOKEN_VARIABLE; }
{PLUS}          { return TOKEN_PLUS; }
{MINUS}         { return TOKEN_MINUS; }
{TIMES}         { return TOKEN_TIMES; }
{DIVISION}      { return TOKEN_DIVISION; }
{LESS}          { return TOKEN_LESS; }
{MORE}          { return TOKEN_MORE; }
{AND}           { return TOKEN_AND; }
{OR}            { return TOKEN_OR; }
{LPAREN}        { return TOKEN_LPAREN; }
{RPAREN}        { return TOKEN_RPAREN; }
{LATITUDE}    { yylval->cvalue = strdup(yytext); return TOKEN_LATITUDE; }
{LONGITUDE}   { yylval->cvalue = strdup(yytext); return TOKEN_LONGITUDE; }
{LATITUDE_DECIMAL}    { yylval->cvalue = strdup(yytext); return TOKEN_LATITUDE_DECIMAL; }
{LONGITUDE_DECIMAL}   { yylval->cvalue = strdup(yytext); return TOKEN_LONGITUDE_DECIMAL; }
{LATITUDE_SEC}    { yylval->cvalue = strdup(yytext); return TOKEN_LATITUDE_SEC; }
{LONGITUDE_SEC}   { yylval->cvalue = strdup(yytext); return TOKEN_LONGITUDE_SEC; }
{ELEVATION}   { yylval->cvalue = strdup(yytext); return TOKEN_ELEVATION; }
{ELEVATION_FT}   { yylval->cvalue = strdup(yytext); return TOKEN_ELEVATION_FT; }
.               {  }

%%

int yyerror(const char *msg) {
fprintf(stderr,"Error:%s\n",msg); return 0;
}

