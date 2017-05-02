/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SEMICOLON = 258,
     COLON = 259,
     DASH = 260,
     ASTERISK = 261,
     LBRACE = 262,
     RBRACE = 263,
     LBRACK = 264,
     RBRACK = 265,
     EQUALITY = 266,
     LPAREN = 267,
     RPAREN = 268,
     OR = 269,
     AND = 270,
     TOK_INTEGER = 271,
     TOK_STRING = 272,
     TOK_TIME = 273,
     TOK_DAY = 274,
     TOK_DOUBLE = 275,
     TOK_ID = 276,
     TOK_IP_ADDR = 277,
     TOK_SIP = 278,
     TOK_DIP = 279,
     TOK_SPORT = 280,
     TOK_DPORT = 281,
     TOK_PROTO = 282,
     TOK_TIME_START = 283,
     TOK_TIME_END = 284,
     TOK_SAVE = 285,
     TOK_COUNT = 286,
     TOK_SHOW = 287,
     TOK_HELP = 288
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 88 "command_parser.yy"

	uint32_t i;
	int64_t i64;
	char* s;
	double d;
    char *time;
    char *day;
    char *ipaddress;
    OffsetsResult* offsets_result;
	/*IndexField* indexfield_p;*/
	/*IPAddress* ipaddress_p;*/
	/*ConnectionID4* connectionid4_p;*/
	/*QueryResult* queryresult_p;*/
	/*QueryRequest* queryrequest_p;*/



/* Line 2068 of yacc.c  */
#line 101 "command_parser.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE commandlval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE commandlloc;

