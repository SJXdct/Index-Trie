/*=============================================================================
#     FileName: command_scanner.ll
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Taihua He
#        Email: hetaihua@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-17 13:34:21
#      History:
=============================================================================*/

%option prefix="command"

/* autotools ylwrap seem to like this filename */
%option outfile="command_scanner.c"

%option noyywrap
%option nounput

%{

  #include <string.h>

  /*#include "IndexField.hh"*/
  /*#include "Query.hh"*/

  #include "query.h"
  #include "command_parser.hpp"

  void commanderror(const char*);
  extern int cmd_parse_lineno;

  #define YY_NO_UNPUT

%}

ID      [a-zA-Z][a-zA-Z0-9\_\-\.]+
INT     -?[0-9]+
DBL     -?[0-9]*\.[0-9]*
WHITE   [ \t]+
COMMENT \#.*
NEWLINE \n
IP	[0-9\*]+\.[0-9\*]+\.[0-9\*]+\.[0-9\*]+
DAY [0-9]+-[0-9]+-[0-9]+
TIME [0-9]+:[0-9]+

%%
"{"		return LBRACE;
"}"		return RBRACE;
"("		return LPAREN;
")"		return RPAREN;
"["		return LBRACK;
"]"		return RBRACK;
";"		return SEMICOLON;
":"		return COLON;
"-"		return DASH;
"*"		return ASTERISK;
"="     return EQUALITY;
"&"     return AND;
"|"     return OR;

"sip"       return TOK_SIP;
"dip"       return TOK_DIP;
"sport"     return TOK_SPORT;
"dport"     return TOK_DPORT;
"proto"     return TOK_PROTO;
"help"		return TOK_HELP;
"start"     return TOK_TIME_START;
"end"       return TOK_TIME_END;
"save"      return TOK_SAVE;
"count"     return TOK_COUNT;
"show"      return TOK_SHOW;

{INT}		 { commandlval.i64=atoi(commandtext); return TOK_INTEGER; }
{ID}		 { commandlval.s=strdup(commandtext); return TOK_ID; }
{DBL}		 { commandlval.d=strtod(commandtext, NULL); return TOK_DOUBLE; }
{IP}		 { commandlval.ipaddress = strdup(commandtext); return TOK_IP_ADDR; } 
{DAY}        { commandlval.day = strdup(commandtext); return TOK_DAY; }
{TIME}       { commandlval.time = strdup(commandtext); return TOK_TIME; }
\"[^"]*\"		 { commandlval.s=strdup(commandtext+1);
		   commandlval.s[strlen(commandlval.s)-1]=0;
		   return TOK_STRING;
		 }
{WHITE}
{COMMENT}
{NEWLINE}	 { cmd_parse_lineno++; }
.		 commanderror("Illegal character in command string");

%%

