/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 25 "parser.y" /* yacc.c:1909  */


  #ifndef YY_TYPEDEF_YY_SCANNER_T
  #define YY_TYPEDEF_YY_SCANNER_T
  typedef void* yyscan_t;
  #endif

 

#line 54 "parser.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOKEN_OR = 258,
    TOKEN_AND = 259,
    TOKEN_LESS = 260,
    TOKEN_MORE = 261,
    TOKEN_PLUS = 262,
    TOKEN_MINUS = 263,
    TOKEN_LPAREN = 264,
    TOKEN_RPAREN = 265,
    TOKEN_NUMBER = 266,
    TOKEN_VARIABLE = 267,
    TOKEN_LATITUDE = 268,
    TOKEN_LONGITUDE = 269,
    TOKEN_ELEVATION = 270,
    TOKEN_ELEVATION_FT = 271,
    TOKEN_LATITUDE_DECIMAL = 272,
    TOKEN_LONGITUDE_DECIMAL = 273,
    TOKEN_LATITUDE_SEC = 274,
    TOKEN_LONGITUDE_SEC = 275,
    TOKEN_TIMES = 276,
    TOKEN_DIVISION = 277
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 44 "parser.y" /* yacc.c:1909  */

  double value;
  char *cvalue; 
  SExpression *expression;
  locationExpression *lexpression;  

#line 96 "parser.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (SExpression **expression, locationExpression **lexpression, yyscan_t scanner);

#endif /* !YY_YY_PARSER_H_INCLUDED  */
