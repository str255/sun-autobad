%{

  /*
   * Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
   * Parser.y file
   * To generate the parser run: "bison Parser.y"
   */

  #include "expression.h"
  #include "location_expression.h"  
  #include "parser.h"
  #include "lexer.h"



   int yyerror(SExpression **expression, locationExpression **lexpression, yyscan_t scanner, const char *msg) {
    // Add error handling routine as needed
    //    if (lexpression)
    //      printf("yyerror exp %x loc=(%lf %lf %lf) %s\n", expression, (*lexpression)->lat, (*lexpression)->lon, (*lexpression)->elevation, msg);
    //    else
    printf("yyerror (%s)\n", msg);
  }

  %}

%code requires {

  #ifndef YY_TYPEDEF_YY_SCANNER_T
  #define YY_TYPEDEF_YY_SCANNER_T
  typedef void* yyscan_t;
  #endif

 }

%output  "parser.c"
%defines "parser.h"

%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { SExpression **expression }
%parse-param { locationExpression **lexpression }
%parse-param { yyscan_t scanner }
%error-verbose

%union {
  double value;
  char *cvalue; 
  SExpression *expression;
  locationExpression *lexpression;  
}


%left '|' '&' TOKEN_OR TOKEN_AND
%left '<' '>' TOKEN_LESS TOKEN_MORE 
%left '+' '-' TOKEN_PLUS TOKEN_MINUS 
%left '(' ')' TOKEN_LPAREN TOKEN_RPAREN

%token <value> TOKEN_NUMBER
%token <cvalue> TOKEN_VARIABLE TOKEN_LATITUDE  TOKEN_LONGITUDE TOKEN_ELEVATION TOKEN_ELEVATION_FT TOKEN_LATITUDE_DECIMAL TOKEN_LONGITUDE_DECIMAL TOKEN_LATITUDE_SEC TOKEN_LONGITUDE_SEC
%token TOKEN_LESS
%token TOKEN_MORE
%token TOKEN_AND
%token TOKEN_OR
%token TOKEN_PLUS
%token TOKEN_MINUS
%token TOKEN_TIMES
%token TOKEN_DIVISION
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%type <value> latitude longitude elevation 
%type <expression> expr
%type <lexpression> location

%%

input
: expr { *expression = $1; } 
| location { *lexpression = $1; }
;

expr
: expr[L] TOKEN_LESS expr[R] { $$ = createOperation( eLESS, $L, $R ); }
| expr[L] TOKEN_MORE expr[R] { $$ = createOperation( eMORE, $L, $R ); }
| expr[L] TOKEN_AND expr[R] { $$ = createOperation( eAND, $L, $R ); }
| expr[L] TOKEN_OR expr[R] { $$ = createOperation( eOR, $L, $R ); }
| expr[L] TOKEN_PLUS expr[R] { $$ = createOperation( ePLUS, $L, $R ); }
| expr[L] TOKEN_MINUS expr[R] { $$ = createOperation( eMINUS, $L, $R ); }
| expr[L] TOKEN_TIMES expr[R] { $$ = createOperation( eTIMES, $L, $R ); }
| expr[L] TOKEN_DIVISION expr[R] { $$ = createOperation( eDIVISION, $L, $R ); }
| TOKEN_MINUS expr[S] { $$ = createUnaryOperation( eMINUS, $S ); }
| TOKEN_PLUS expr[S] { $$ = createUnaryOperation( ePLUS, $S ); }
| TOKEN_LPAREN expr[E] TOKEN_RPAREN { $$ = $E; }
| TOKEN_NUMBER { $$ = createNumber($1); }
| TOKEN_VARIABLE { $$ = createVariable($1); }
;

location
: latitude[L] longitude[R] { $$ = createLocation( $L, $R ); }
| longitude[L] latitude[R]  { $$ = createLocation( $R, $L ); }
| latitude[L] longitude[M] elevation[R]  { $$ = createLocation( $L, $M, $R ); }
| longitude[L] latitude[M] elevation[R]  { $$ = createLocation( $M, $L, $R ); }
;

latitude
: TOKEN_LATITUDE { $$ = createLatitudeLongitude($1,'S'); }
| TOKEN_LATITUDE_DECIMAL  { $$ = createLatitudeLongitudeDecimal($1,'S'); }
| TOKEN_LATITUDE_SEC  { $$ = createLatitudeLongitudeSec($1,'S'); }
;

longitude
: TOKEN_LONGITUDE { $$ = createLatitudeLongitude($1,'W'); }
| TOKEN_LONGITUDE_DECIMAL  { $$ = createLatitudeLongitudeDecimal($1,'W'); }
| TOKEN_LONGITUDE_SEC  { $$ = createLatitudeLongitudeSec($1,'W'); }
;

elevation
: TOKEN_ELEVATION { $$ = createElevation($1); }
| TOKEN_ELEVATION_FT { $$ = createElevationFeet($1); }
;

%%
