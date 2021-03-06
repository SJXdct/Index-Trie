/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse         commandparse
#define yylex           commandlex
#define yyerror         commanderror
#define yylval          commandlval
#define yychar          commandchar
#define yydebug         commanddebug
#define yynerrs         commandnerrs
#define yylloc          commandlloc

/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 21 "command_parser.yy"
 
	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>
    #include <stdint.h>
	#include <netinet/in.h>
	#include <pthread.h>
	#include <string>
    #include <time.h>

    #include "../storage/storage.h"
    /*#include "../storage/index.h"*/
    /*using storage::Storage;*/
    using namespace storage;

    #include "query.h"

	// Work around a bug in the relation between bison and GCC 3.x:
	#if defined (__GNUC__) && 3 <= __GNUC__
	#define __attribute__(arglist)
	#endif

	/* Internal datastructures whilse parsing */ 
	/*static QueryRequest * q_req;*/
	/*static QueryResult *q_res; */
	/*static IndexField *q_idxfield;*/
	/*static bool q_mem_only;*/
	/*static bool q_subscribe;*/
    static OffsetsResult *result = NULL;
	static uint64_t start_time;
	static uint64_t end_time;
    /*static char*  sip_index_file;*/
    /*static char*  dip_index_file;*/
    /*static char*  sport_index_file;*/
    /*static char*  dport_index_file;*/
    /*static char*  proto_index_file;*/
    static char*  save_file_name;
	/*static broccoli_worker_thread_data* q_bc_thread;*/

	/*static int nextQueryID = 0;*/

	//extern int yylex(YYSTYPE *lvalp, YYLTYPE *llocp);
	extern int yylex();
	struct yy_buffer_state;
	static pthread_mutex_t cmd_parser_lock;

	typedef struct yy_buffer_state *YY_BUFFER_STATE;
	extern void command_delete_buffer (YY_BUFFER_STATE b);
	extern YY_BUFFER_STATE command_scan_string (const char *yy_str);

	int commanderror(const char*);
	int cmd_parse_lineno=0;
	int cmd_parse_errors=0;
	//  const char* parse_filename=NULL;

	/*
	 * symbols shared with main.cc
	 */
	/*Storage* cmd_parser_storage=NULL;*/
    /*static Storage*       storage = NULL;*/
    static QueryRequest*  request = NULL;
    static Query*         query = NULL;
    FILE *cmd_parser_outfp;
	extern void print_stats(FILE *fp);
	/*extern pcap_t* ph;*/


/* Line 268 of yacc.c  */
#line 147 "command_parser.cpp"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 293 of yacc.c  */
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



/* Line 293 of yacc.c  */
#line 234 "command_parser.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

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


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 259 "command_parser.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  18
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   45

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  34
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  8
/* YYNRULES -- Number of rules.  */
#define YYNRULES  23
/* YYNRULES -- Number of states.  */
#define YYNSTATES  51

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   288

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,    11,    16,    21,    22,    24,    28,
      32,    36,    40,    44,    48,    52,    56,    57,    61,    62,
      66,    68,    70,    73
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      35,     0,    -1,    33,    -1,    30,    40,    36,    38,    39,
      -1,    31,    36,    38,    39,    -1,    32,    36,    38,    39,
      -1,    -1,    37,    -1,    12,    37,    13,    -1,    37,    15,
      37,    -1,    37,    14,    37,    -1,    23,    11,    22,    -1,
      24,    11,    22,    -1,    25,    11,    16,    -1,    26,    11,
      16,    -1,    27,    11,    16,    -1,    -1,    28,    11,    41,
      -1,    -1,    29,    11,    41,    -1,    21,    -1,    17,    -1,
      19,    18,    -1,    18,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   167,   167,   176,   187,   195,   206,   226,   237,   244,
     259,   274,   306,   335,   350,   365,   383,   389,   402,   408,
     422,   431,   445,   462
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SEMICOLON", "COLON", "DASH", "ASTERISK",
  "LBRACE", "RBRACE", "LBRACK", "RBRACK", "EQUALITY", "LPAREN", "RPAREN",
  "OR", "AND", "TOK_INTEGER", "TOK_STRING", "TOK_TIME", "TOK_DAY",
  "TOK_DOUBLE", "TOK_ID", "TOK_IP_ADDR", "TOK_SIP", "TOK_DIP", "TOK_SPORT",
  "TOK_DPORT", "TOK_PROTO", "TOK_TIME_START", "TOK_TIME_END", "TOK_SAVE",
  "TOK_COUNT", "TOK_SHOW", "TOK_HELP", "$accept", "command", "filter",
  "nonempty_filter", "start_time", "end_time", "save_file_name",
  "timestamp", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    34,    35,    35,    35,    35,    36,    36,    37,    37,
      37,    37,    37,    37,    37,    37,    38,    38,    39,    39,
      40,    40,    41,    41
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     5,     4,     4,     0,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     0,     3,     0,     3,
       1,     1,     2,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     6,     6,     2,     0,    21,    20,     6,     0,
       0,     0,     0,     0,     0,    16,     7,    16,     1,    16,
       0,     0,     0,     0,     0,     0,     0,    18,     0,     0,
      18,    18,     8,    11,    12,    13,    14,    15,     0,     0,
       4,    10,     9,     5,     3,    23,     0,    17,     0,    22,
      19
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     5,    15,    16,    27,    40,     8,    47
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -15
static const yytype_int8 yypact[] =
{
      -8,   -13,   -11,   -11,   -15,     6,   -15,   -15,   -11,   -11,
      10,    19,    20,    21,    22,     7,     3,     7,   -15,     7,
      -4,    12,    14,    23,    24,    25,    26,     9,   -11,   -11,
       9,     9,   -15,   -15,   -15,   -15,   -15,   -15,     8,    31,
     -15,   -15,   -15,   -15,   -15,   -15,    27,   -15,     8,   -15,
     -15
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -15,   -15,    -1,    -9,   -14,    -2,   -15,    -5
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      20,     9,    17,    30,     6,    31,    18,    19,     7,    32,
      28,    29,    10,    11,    12,    13,    14,    28,    29,    41,
      42,    21,     1,     2,     3,     4,    45,    46,    43,    44,
      22,    23,    24,    25,    33,    26,    34,    38,    39,    35,
      36,    37,    48,    50,     0,    49
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-15))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int8 yycheck[] =
{
       9,    12,     3,    17,    17,    19,     0,     8,    21,    13,
      14,    15,    23,    24,    25,    26,    27,    14,    15,    28,
      29,    11,    30,    31,    32,    33,    18,    19,    30,    31,
      11,    11,    11,    11,    22,    28,    22,    11,    29,    16,
      16,    16,    11,    48,    -1,    18
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    30,    31,    32,    33,    35,    17,    21,    40,    12,
      23,    24,    25,    26,    27,    36,    37,    36,     0,    36,
      37,    11,    11,    11,    11,    11,    28,    38,    14,    15,
      38,    38,    13,    22,    22,    16,    16,    16,    11,    29,
      39,    37,    37,    39,    39,    18,    19,    41,    11,    18,
      41
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1806 of yacc.c  */
#line 167 "command_parser.yy"
    {
		if (cmd_parser_outfp) {
			fprintf(cmd_parser_outfp, "Online help not yet implemented. See Documentation/TM_HOWTO\n");
		}
        if (request != NULL) {
            request->query_type = HELP;
        }
	}
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 176 "command_parser.yy"
    {
		if (cmd_parser_outfp) {
            fprintf(cmd_parser_outfp, "save filter parsing, file_name = %s!\n", (yyvsp[(2) - (5)].s));
		}
        if (request != NULL) {
            request->query_type = SAVE;
            /*fprintf(cmd_parser_outfp, "save type, request query_type = %d\n", request->query_type);*/
        } else {
            /*fprintf(cmd_parser_outfp,  "request pointer NULL!!!!!\n");*/
        }
    }
    break;

  case 4:

/* Line 1806 of yacc.c  */
#line 187 "command_parser.yy"
    {
		if (cmd_parser_outfp) {
			fprintf(cmd_parser_outfp, "count filter\n");
		}
        if (request != NULL) {
            request->query_type = COUNT;
        }
    }
    break;

  case 5:

/* Line 1806 of yacc.c  */
#line 195 "command_parser.yy"
    {
		if (cmd_parser_outfp) {
			fprintf(cmd_parser_outfp, "commmand_parser.yy:line 197:show filter\n");
		}
        if (request != NULL) {
            request->query_type = SHOW;
        }
    }
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 206 "command_parser.yy"
    {
        if (query == NULL) {
            if (cmd_parser_outfp) {
                fprintf(cmd_parser_outfp, "NULL pointer query!!!\n");
            }
            (yyval.offsets_result) = NULL;
        } else {
            uint32_t ip_begin = 0, ip_end = 0xffffffff;
            if (request == NULL) {
                (yyval.offsets_result) = query->getMemSipOffsets(ip_begin, ip_end);
            } else {
                (yyval.offsets_result) = query->getOffsets( request->sip_index_file, SRCIP, ip_begin, ip_end);
            }
            if (cmd_parser_outfp && (yyval.offsets_result) != NULL) {
                fprintf(cmd_parser_outfp, "Sip Filter parsing, size = %d!\n",
                ((OffsetsResult*)(yyval.offsets_result))->size());
            }
        }
    }
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 226 "command_parser.yy"
    {
        if (cmd_parser_outfp && (yyvsp[(1) - (1)].offsets_result) != NULL) {
            fprintf(cmd_parser_outfp, "() nonempty_filter parsing, size = %d!\n",
            ((OffsetsResult*)(yyvsp[(1) - (1)].offsets_result))->size());
		}
        result = (yyvsp[(1) - (1)].offsets_result);
        (yyval.offsets_result) = (yyvsp[(1) - (1)].offsets_result);
    }
    break;

  case 8:

/* Line 1806 of yacc.c  */
#line 237 "command_parser.yy"
    {
                    (yyval.offsets_result) = (yyvsp[(2) - (3)].offsets_result);
                    if (cmd_parser_outfp && (yyval.offsets_result) != NULL) {
                        fprintf(cmd_parser_outfp, "() nonempty_filter parsing, size = %d!\n",
                        ((OffsetsResult*)(yyval.offsets_result))->size());
                    }
                }
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 244 "command_parser.yy"
    {
                    if ((yyvsp[(1) - (3)].offsets_result) == NULL || (yyvsp[(3) - (3)].offsets_result) == NULL) {
                        (yyval.offsets_result) = NULL;
                        delete((yyvsp[(3) - (3)].offsets_result));
                        delete((yyvsp[(1) - (3)].offsets_result));
                    } else {
                        ((OffsetsResult*)(yyvsp[(1) - (3)].offsets_result))->intersection( (OffsetsResult*)(yyvsp[(3) - (3)].offsets_result) );
                        (yyval.offsets_result) = (yyvsp[(1) - (3)].offsets_result);
                        if (cmd_parser_outfp && (yyval.offsets_result) != NULL) {
                            fprintf(cmd_parser_outfp, "And nonempty_filter parsing, size = %d!\n",
                            ((OffsetsResult*)(yyval.offsets_result))->size());
                        }
                        delete((yyvsp[(3) - (3)].offsets_result));
                    }
                }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 259 "command_parser.yy"
    {
                    if ((yyvsp[(1) - (3)].offsets_result) == NULL || (yyvsp[(3) - (3)].offsets_result) == NULL) {
                        (yyval.offsets_result) = NULL;
                        delete((yyvsp[(1) - (3)].offsets_result));
                        delete((yyvsp[(3) - (3)].offsets_result));
                    } else {
                        ((OffsetsResult*)(yyvsp[(1) - (3)].offsets_result))->merge( (OffsetsResult*)(yyvsp[(3) - (3)].offsets_result) );
                        (yyval.offsets_result) = (yyvsp[(1) - (3)].offsets_result);
                        if (cmd_parser_outfp && (yyval.offsets_result) != NULL) {
                            fprintf(cmd_parser_outfp, "Or nonempty_filter parsing, size = %d!\n",
                            ((OffsetsResult*)(yyval.offsets_result))->size());
                        }
                        delete((yyvsp[(3) - (3)].offsets_result));
                    }
                }
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 274 "command_parser.yy"
    {
                    uint32_t ip_begin = 0, ip_end = 0;
                    char *ptr = strtok( (yyvsp[(3) - (3)].ipaddress), ".");
                    for (int i = 0; i < 4 && ptr; ++i) {
                        if (strcmp(ptr, "*") == 0) {
                            ip_begin <<= 8;
                            ip_end = (ip_end << 8)|0xff;
                        } else {
                            ip_begin = (ip_begin << 8)|atoi(ptr);
                            ip_end = (ip_end << 8)|atoi(ptr);
                        }
                        ptr = strtok(NULL, ".");
                    }
                    free((yyvsp[(3) - (3)].ipaddress));

                    if (query == NULL) {
                        if (cmd_parser_outfp) {
                            fprintf(cmd_parser_outfp, "NULL pointer query!!!\n");
                        }
                        (yyval.offsets_result) = NULL;
                    } else {
                        if (request == NULL) {
                            (yyval.offsets_result) = query->getMemSipOffsets(ip_begin, ip_end);
                        } else {
                            (yyval.offsets_result) = query->getOffsets( request->sip_index_file, SRCIP, ip_begin, ip_end);
                        }
                        if (cmd_parser_outfp && (yyval.offsets_result) != NULL) {
                            fprintf(cmd_parser_outfp, "Sip Filter parsing, size = %d!\n",
                            ((OffsetsResult*)(yyval.offsets_result))->size());
                        }
                    }
                }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 306 "command_parser.yy"
    {
                    uint32_t ip_begin = 0, ip_end = 0;
                    char *ptr = strtok( (yyvsp[(3) - (3)].ipaddress), ".");
                    for (int i = 0; i < 4 && ptr; ++i) {
                        if (strcmp(ptr, "*") == 0) {
                            ip_begin <<= 8;
                            ip_end = (ip_end << 8)|0xff;
                        } else {
                            ip_begin = (ip_begin << 8)|atoi(ptr);
                            ip_end = (ip_end << 8)|atoi(ptr);
                        }
                        ptr = strtok(NULL, ".");
                    }
                    free((yyvsp[(3) - (3)].ipaddress));

                    if (query == NULL) {
                        (yyval.offsets_result) = NULL;
                    } else {
                        if (request == NULL) {
                            (yyval.offsets_result) = query->getMemDipOffsets(ip_begin, ip_end); 
                        } else {
                            (yyval.offsets_result) = query->getOffsets( request->dip_index_file, DSTIP, ip_begin, ip_end);
                        }
                        if (cmd_parser_outfp && (yyval.offsets_result) != NULL) {
                            fprintf(cmd_parser_outfp, "Sip Filter parsing, size = %d!\n",
                            ((OffsetsResult*)(yyval.offsets_result))->size());
                        }
                    }
                }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 335 "command_parser.yy"
    {
                    if (query == NULL) {
                        (yyval.offsets_result) = NULL;
                    } else {
                        if (request == NULL) {
                            (yyval.offsets_result) = query->getMemSportOffsets((yyvsp[(3) - (3)].i), (yyvsp[(3) - (3)].i)); 
                        } else { 
                            (yyval.offsets_result) = query->getOffsets( request->sport_index_file, SPORT, (yyvsp[(3) - (3)].i), (yyvsp[(3) - (3)].i));
                        }
                        if (cmd_parser_outfp && (yyval.offsets_result) != NULL ) {
                            fprintf(cmd_parser_outfp, "Sport Filter parsing, size = %d!\n",
                            ((OffsetsResult*)(yyval.offsets_result))->size());
                        }
                    }
                }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 350 "command_parser.yy"
    {
                    if (query == NULL) {
                        (yyval.offsets_result) = NULL;
                    } else {
                        if (request == NULL) {
                            (yyval.offsets_result) = query->getMemDportOffsets((yyvsp[(3) - (3)].i), (yyvsp[(3) - (3)].i)); 
                        } else {  
                            (yyval.offsets_result) = query->getOffsets( request->dport_index_file, DPORT, (yyvsp[(3) - (3)].i), (yyvsp[(3) - (3)].i));
                        }
                        if (cmd_parser_outfp && (yyval.offsets_result) != NULL) {
                            fprintf(cmd_parser_outfp, "Dport Filter parsing, size = %d!\n",
                            ((OffsetsResult*)(yyval.offsets_result))->size());
                        }
                    }
                }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 365 "command_parser.yy"
    {
                    if (query == NULL) {
                        (yyval.offsets_result) = NULL;
                    } else {
                        if (request == NULL) {
                            (yyval.offsets_result) = query->getMemProtoOffsets((yyvsp[(3) - (3)].i), (yyvsp[(3) - (3)].i)); 
                        } else {  
                            (yyval.offsets_result) = query->getOffsets( request->proto_index_file, PROTO, (yyvsp[(3) - (3)].i), (yyvsp[(3) - (3)].i));
                        }
                        if (cmd_parser_outfp && (yyval.offsets_result) != NULL) {
                            fprintf(cmd_parser_outfp, "proto Filter parsing, size = %d!\n",
                            ((OffsetsResult*)(yyval.offsets_result))->size());
                        }
                    }
                }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 383 "command_parser.yy"
    {
            start_time = 0;
            if (request != NULL) {
                request->start_time = 0;
            }
          }
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 389 "command_parser.yy"
    {
            if (cmd_parser_outfp) {
                fprintf(cmd_parser_outfp, "start timestamp = %lld\n", (yyvsp[(3) - (3)].i64));
            }
            if (request != NULL) {
                request->start_time = (yyvsp[(3) - (3)].i64);
            }
            start_time = (yyvsp[(3) - (3)].i64);
            (yyval.i64) = (yyvsp[(3) - (3)].i64);
          }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 402 "command_parser.yy"
    {
            end_time = 2e10;
            if (request != NULL) {
                request->end_time = 2e10;
            }
        }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 408 "command_parser.yy"
    {
            if (cmd_parser_outfp) {
                fprintf(cmd_parser_outfp, "end timestamp = %lld\n", (yyvsp[(3) - (3)].i64));
            }
            end_time = (yyvsp[(3) - (3)].i64);
            (yyval.i64) = (yyvsp[(3) - (3)].i64);
            
            if (request != NULL) {
                request->end_time = (yyval.i64);
            }
        }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 422 "command_parser.yy"
    {
                (yyval.s) = (yyvsp[(1) - (1)].s);
                /*save_file_name = $1;*/

                if (request != NULL) {
                    request->save_file_name = std::string((yyvsp[(1) - (1)].s));
                }
                free((yyvsp[(1) - (1)].s));
              }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 431 "command_parser.yy"
    {
                (yyval.s) = (yyvsp[(1) - (1)].s);
                /*save_file_name = $1;*/
                if (cmd_parser_outfp) {
                    fprintf(cmd_parser_outfp, "file name in parsing = %s\n", save_file_name);
                }
                if (request != NULL) {
                    request->save_file_name = std::string((yyvsp[(1) - (1)].s));
                }
                free((yyvsp[(1) - (1)].s));
              }
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 445 "command_parser.yy"
    {
            struct tm t;
            char *ptr = strtok((yyvsp[(1) - (2)].s), "-");
            t.tm_year = atoi(ptr) - 1900;
            ptr = strtok(NULL, "-");
            t.tm_mon = atoi(ptr) - 1;
            ptr = strtok(NULL, "-");
            t.tm_mday = atoi(ptr);
            ptr = strtok((yyvsp[(2) - (2)].s),":");
            t.tm_hour = atoi(ptr);
            ptr = strtok(NULL, ":");
            t.tm_min = atoi(ptr);
            t.tm_sec = 0;
            (yyval.i64) = mktime( &t );
            free((yyvsp[(1) - (2)].s));
            free((yyvsp[(2) - (2)].s));
         }
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 462 "command_parser.yy"
    {
            struct tm t;
            time_t rawtime;
            time( &rawtime );
            t = *(localtime(&rawtime));
            char *ptr = strtok((yyvsp[(1) - (1)].s),":");
            t.tm_hour = atoi(ptr);
            ptr = strtok(NULL, ":");
            t.tm_min = atoi(ptr);
            t.tm_sec = 0;
            (yyval.i64) = mktime( &t );
            free((yyvsp[(1) - (1)].s));
         }
    break;



/* Line 1806 of yacc.c  */
#line 1984 "command_parser.cpp"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 476 "command_parser.yy"

     
void command_parser_init(void) {
	pthread_mutex_init(&cmd_parser_lock, NULL);	
}

void command_parser_finish(void) {
	pthread_mutex_destroy(&cmd_parser_lock);
}

int commanderror(const char *msg) {
	if(cmd_parser_outfp)
		fprintf(cmd_parser_outfp, "PARSER ERROR - %s\n", msg);
	/*tmlog(TM_LOG_NOTE, "query", "PARSER ERROR - %s", msg); */
	cmd_parse_errors++;
	return 0;
}

int parse_req_parameters(const char* cmd, QueryRequest* req) {
    pthread_mutex_lock(&cmd_parser_lock);
    
    cmd_parser_outfp = NULL;
    /*cmd_parser_outfp = stdout;*/
    request = req;

    /*fprintf( stdout , "request pointer = %p\n", request);*/
	YY_BUFFER_STATE bufstate = command_scan_string(cmd);

	cmd_parse_errors=0;
	commandparse();
	command_delete_buffer(bufstate);

    /* set request parameters, e.g. save_file_name, timestamps*/
    /*request->start_time = start_time;*/
    /*request->end_time = end_time;*/
    /*if (save_file_name != NULL) {*/
        /*request->save_file_name = std::string(save_file_name);*/
    /*}*/
    /*free( save_file_name );*/

	pthread_mutex_unlock(&cmd_parser_lock);
	return cmd_parse_errors;
}

/* Don't forget to call cmd_parser_init(), before you call the parser */
OffsetsResult* parse_command(const char* cmd, FILE *outfp, Query* q, QueryRequest* req) {
	pthread_mutex_lock(&cmd_parser_lock);
	// set variable parser_storage global to conf_parser.cc
	cmd_parser_outfp = outfp;
    /*storage = s;*/
    query = q;
    request = req;

	YY_BUFFER_STATE bufstate = command_scan_string(cmd);
    /*fprintf(outfp, "save_file_name = %s\n", save_file_name);*/
    /*fprintf(outfp, "start time = %u, end time = %u\n", start_time, end_time );*/

	cmd_parse_errors=0;
	commandparse();
	fflush(outfp);
	command_delete_buffer(bufstate);

    OffsetsResult* offsets_result = result;
    result = NULL;
    /*storage = NULL;*/
    query = NULL;
    request = NULL;
	pthread_mutex_unlock(&cmd_parser_lock);
	/*return cmd_parse_errors;*/
    return offsets_result;
}

