/* A Bison parser, made from grammer.y, by GNU bison 1.75.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON	1

/* Pure parsers.  */
#define YYPURE	0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     WHERE = 258,
     GROUP = 259,
     BY = 260,
     FROM = 261,
     SELECT = 262,
     STRING = 263,
     ID = 264,
     STOP = 265,
     AS = 266,
     HAVING = 267,
     YYSTOP = 268,
     YYLIKE = 269,
     YYOR = 270,
     YYAND = 271
   };
#endif
#define WHERE 258
#define GROUP 259
#define BY 260
#define FROM 261
#define SELECT 262
#define STRING 263
#define ID 264
#define STOP 265
#define AS 266
#define HAVING 267
#define YYSTOP 268
#define YYLIKE 269
#define YYOR 270
#define YYAND 271




/* Copy the first part of user declarations.  */
#line 20 "grammer.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <smacq.h>
#include "smacq-parser.h"
//#define DEBUG
  
  extern int yylex();
  extern void yy_scan_string(const char *);
  void yyerror(char *);
  extern char * yytext;
  extern char * yystring;

  struct arglist {
    char * arg;
    char * rename;
    struct arglist * next;
    struct arglist * func_args;
    int isfunc;
  };

  static struct graph newmodule(char * module, struct arglist * alist);
  static void graph_join(struct graph * graph, struct graph newg);
  static struct graph newgroup(struct group, struct vphrase);
  static void arglist2argv(struct arglist * al, int * argc, char *** argv);
  static char * arglist2str(struct arglist * al);
  static struct arglist * newarg(char * arg, int isfunc, struct arglist * func_args);
  static struct arglist * arglist_append(struct arglist * tail, struct arglist * addition);
  static struct vphrase newvphrase(char * verb, struct arglist * args);
  static struct graph optimize_bools(dts_comparison *);
  static dts_comparison * comp_join(dts_comparison *, dts_comparison *, int isor);
  static dts_comparison * comp_new(char * field, dts_compare_operation op, char * value);

  struct graph nullgraph = { head: NULL, tail: NULL };
  
  smacq_graph * Graph;



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#ifndef YYSTYPE
#line 84 "grammer.y"
typedef union {
  struct graph graph;
  struct arglist * arglist;
  struct vphrase vphrase;
  char * string;
  struct group group;
  dts_compare_operation op;
  dts_comparison * comp;
} yystype;
/* Line 193 of /usr/share/bison/yacc.c.  */
#line 155 "grammer.c"
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif

#ifndef YYLTYPE
typedef struct yyltype
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} yyltype;
# define YYLTYPE yyltype
# define YYLTYPE_IS_TRIVIAL 1
#endif

/* Copy the second part of user declarations.  */


/* Line 213 of /usr/share/bison/yacc.c.  */
#line 176 "grammer.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];	\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  9
#define YYLAST   115

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  24
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  28
/* YYNRULES -- Number of rules. */
#define YYNRULES  60
/* YYNRULES -- Number of states. */
#define YYNSTATES  101

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   271

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    23,     2,     2,     2,     2,     2,     2,
      18,    19,     2,     2,    17,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      20,    22,    21,     2,     2,     2,     2,     2,     2,     2,
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
      15,    16
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     6,     7,    12,    14,    18,    20,    25,
      27,    30,    32,    36,    38,    41,    43,    48,    50,    53,
      55,    57,    59,    61,    63,    65,    67,    71,    73,    78,
      80,    82,    84,    86,    88,    90,    92,    94,    96,    98,
     103,   106,   111,   114,   116,   119,   121,   124,   126,   130,
     132,   136,   140,   144,   146,   148,   152,   157,   159,   161,
     163
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      25,     0,    -1,    27,    10,    -1,    -1,    43,    28,    32,
      33,    -1,    26,    -1,     6,    31,    29,    -1,    26,    -1,
      17,    44,    30,    29,    -1,    26,    -1,    11,    35,    -1,
      42,    -1,    18,    27,    19,    -1,    26,    -1,     3,    49,
      -1,    26,    -1,     4,     5,    46,    34,    -1,    26,    -1,
      12,    44,    -1,    37,    -1,    36,    -1,     8,    -1,     9,
      -1,    15,    -1,    16,    -1,    39,    -1,    39,    11,    35,
      -1,    35,    -1,    41,    18,    46,    19,    -1,    37,    -1,
      36,    -1,    20,    -1,    21,    -1,    22,    -1,    18,    -1,
      19,    -1,    23,    -1,    37,    -1,    48,    -1,    48,    18,
      46,    19,    -1,    48,    46,    -1,    48,    18,    46,    19,
      -1,    40,    45,    -1,    26,    -1,    40,    45,    -1,    26,
      -1,    38,    47,    -1,    26,    -1,    17,    38,    47,    -1,
      37,    -1,    18,    49,    19,    -1,    49,    15,    49,    -1,
      49,    16,    49,    -1,    50,    -1,    35,    -1,    35,    51,
      35,    -1,    48,    18,    46,    19,    -1,    22,    -1,    21,
      -1,    20,    -1,    14,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    95,    95,   105,   107,   120,   121,   130,   131,   150,
     151,   154,   155,   158,   159,   164,   165,   168,   169,   172,
     173,   176,   178,   179,   180,   183,   184,   187,   188,   191,
     192,   198,   199,   200,   201,   202,   203,   206,   209,   210,
     213,   214,   217,   220,   221,   224,   225,   228,   229,   232,
     239,   240,   241,   242,   245,   246,   247,   252,   253,   254,
     255
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "WHERE", "GROUP", "BY", "FROM", "SELECT", 
  "STRING", "ID", "STOP", "AS", "HAVING", "YYSTOP", "YYLIKE", "YYOR", 
  "YYAND", "','", "'('", "')'", "'<'", "'>'", "'='", "'!'", "$accept", 
  "queryline", "null", "query", "from", "joins", "as", "source", "where", 
  "group", "having", "word", "string", "id", "arg", "argument", "boolarg", 
  "function", "pverbphrase", "verbphrase", "boolargs", "moreboolargs", 
  "args", "moreargs", "verb", "boolean", "test", "op", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,    44,    40,    41,
      60,    62,    61,    33
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    24,    25,    26,    27,    28,    28,    29,    29,    30,
      30,    31,    31,    32,    32,    33,    33,    34,    34,    35,
      35,    36,    37,    37,    37,    38,    38,    39,    39,    40,
      40,    40,    40,    40,    40,    40,    40,    41,    42,    42,
      43,    43,    44,    45,    45,    46,    46,    47,    47,    48,
      49,    49,    49,    49,    50,    50,    50,    51,    51,    51,
      51
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     0,     4,     1,     3,     1,     4,     1,
       2,     1,     3,     1,     2,     1,     4,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     4,
       2,     4,     2,     1,     2,     1,     2,     1,     3,     1,
       3,     3,     3,     1,     1,     3,     4,     1,     1,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,    22,    23,    24,     0,     0,    49,     3,     3,     1,
       2,     0,     5,     3,    21,     3,    45,    27,    20,    19,
       3,    25,     0,    40,     0,     3,    11,    38,     0,    13,
       3,     0,     0,    47,    46,     0,     3,     0,     0,     7,
       6,     3,     0,    54,    19,     0,    14,    53,     0,    15,
       4,    41,     3,    26,    19,     0,    12,    34,    35,    31,
      32,    33,    36,    30,    29,     3,     3,     0,     0,    60,
      59,    58,    57,     0,     3,     0,     0,     3,    48,    28,
      43,     3,    42,     0,     9,     3,    39,    50,    55,     0,
      51,    52,     3,    44,    10,     8,    56,     0,    17,    16,
      18
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     4,    16,     5,    13,    40,    85,    25,    30,    50,
      99,    17,    18,    19,    20,    21,    65,    22,    26,     7,
      66,    82,    23,    34,    45,    46,    47,    73
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -59
static const yysigned_char yypact[] =
{
      99,   -59,   -59,   -59,     6,     8,   -59,     9,    76,   -59,
     -59,    91,   -59,    13,   -59,     5,   -59,   -59,   -59,    12,
       2,    22,    16,   -59,    99,    26,   -59,    40,    87,   -59,
      55,    25,     5,   -59,   -59,     5,     5,    47,    32,   -59,
     -59,     5,    87,    90,    50,    51,    -7,   -59,    65,   -59,
     -59,   -59,     2,   -59,   -59,    52,   -59,   -59,   -59,   -59,
     -59,   -59,   -59,   -59,   -59,    32,    61,    67,    41,   -59,
     -59,   -59,   -59,     5,     5,    87,    87,     5,   -59,   -59,
     -59,    32,   -59,     5,   -59,    26,   -59,   -59,   -59,    69,
     -59,   -59,    62,   -59,   -59,   -59,   -59,    32,   -59,   -59,
     -59
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -59,   -59,    -3,    66,   -59,    14,   -59,   -59,   -59,   -59,
     -59,     4,   -36,     0,    81,   -59,   -58,   -59,   -59,   -59,
      -4,    17,   -10,    49,     1,   -39,   -59,   -59
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -50
static const yysigned_char yytable[] =
{
       6,     8,    63,    68,    12,    31,     9,    81,    75,    76,
      29,     6,    27,    14,     1,    11,    28,    33,    10,    32,
       2,     3,    39,    81,     6,     8,    55,    49,    44,    63,
     -37,    67,    43,    35,    36,    54,    90,    91,    64,    53,
      14,     1,    44,    38,    51,    63,    43,     2,     3,    33,
      57,    58,    59,    60,    61,    62,    75,    76,    41,    48,
      87,    63,    80,    84,    89,    64,    56,    92,   -49,    74,
      77,    79,    83,    54,    97,    44,    44,    88,    80,    43,
      43,    64,    39,    54,    14,     1,    86,    94,    96,    98,
      37,     2,     3,   100,    15,    14,     1,    64,    93,    95,
       1,    78,     2,     3,    69,    42,     2,     3,     1,    24,
      70,    71,    72,    52,     2,     3
};

static const unsigned char yycheck[] =
{
       0,     0,    38,    42,     7,    15,     0,    65,    15,    16,
      13,    11,    11,     8,     9,     6,     3,    20,    10,    17,
      15,    16,    25,    81,    24,    24,    36,    30,    28,    65,
      18,    41,    28,    11,    18,    35,    75,    76,    38,    35,
       8,     9,    42,    17,    19,    81,    42,    15,    16,    52,
      18,    19,    20,    21,    22,    23,    15,    16,    18,     4,
      19,    97,    65,    66,    74,    65,    19,    77,    18,    18,
       5,    19,    11,    73,    12,    75,    76,    73,    81,    75,
      76,    81,    85,    83,     8,     9,    19,    83,    19,    92,
      24,    15,    16,    97,    18,     8,     9,    97,    81,    85,
       9,    52,    15,    16,    14,    18,    15,    16,     9,    18,
      20,    21,    22,    32,    15,    16
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     9,    15,    16,    25,    27,    37,    43,    48,     0,
      10,     6,    26,    28,     8,    18,    26,    35,    36,    37,
      38,    39,    41,    46,    18,    31,    42,    48,     3,    26,
      32,    46,    17,    26,    47,    11,    18,    27,    17,    26,
      29,    18,    18,    35,    37,    48,    49,    50,     4,    26,
      33,    19,    38,    35,    37,    46,    19,    18,    19,    20,
      21,    22,    23,    36,    37,    40,    44,    46,    49,    14,
      20,    21,    22,    51,    18,    15,    16,     5,    47,    19,
      26,    40,    45,    11,    26,    30,    19,    19,    35,    46,
      49,    49,    46,    45,    35,    29,    19,    12,    26,    34,
      44
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)           \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#define YYLEX	yylex ()

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
} while (0)
# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*-----------------------------.
| Print this symbol on YYOUT.  |
`-----------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yysymprint (FILE* yyout, int yytype, YYSTYPE yyvalue)
#else
yysymprint (yyout, yytype, yyvalue)
    FILE* yyout;
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyout, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyout, yytoknum[yytype], yyvalue);
# endif
    }
  else
    YYFPRINTF (yyout, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyout, ")");
}
#endif /* YYDEBUG. */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yydestruct (int yytype, YYSTYPE yyvalue)
#else
yydestruct (yytype, yyvalue)
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  switch (yytype)
    {
      default:
        break;
    }
}



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of parse errors so far.  */
int yynerrs;


int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with.  */

  if (yychar <= 0)		/* This means end of input.  */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more.  */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

      /* We have to keep this `#if YYDEBUG', since we use variables
	 which are defined only if `YYDEBUG' is set.  */
      YYDPRINTF ((stderr, "Next token is "));
      YYDSYMPRINT ((stderr, yychar1, yylval));
      YYDPRINTF ((stderr, "\n"));
    }

  /* If the proper action on seeing token YYCHAR1 is to reduce or to
     detect an error, take that action.  */
  yyn += yychar1;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yychar1)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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



#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn - 1, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] >= 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif
  switch (yyn)
    {
        case 2:
#line 96 "grammer.y"
    { 
#ifdef DEBUG
	   	smacq_graph_print(stderr, yyvsp[-1].graph.head, 0); 
#endif
		Graph = yyvsp[-1].graph.head;
		return 0;
	   }
    break;

  case 4:
#line 108 "grammer.y"
    {
	   	yyval.graph.head = (yyval.graph.tail = NULL);
	   	graph_join(&(yyval.graph), yyvsp[-2].graph);
		graph_join(&(yyval.graph), yyvsp[-1].graph);
		if (yyvsp[0].group.args) {
			graph_join(&(yyval.graph), newgroup(yyvsp[0].group, yyvsp[-3].vphrase));
		} else {
			graph_join(&(yyval.graph), newmodule(yyvsp[-3].vphrase.verb, yyvsp[-3].vphrase.args));
		}
	   }
    break;

  case 5:
#line 120 "grammer.y"
    { yyval.graph.head = NULL; yyval.graph.tail = NULL; }
    break;

  case 6:
#line 122 "grammer.y"
    { 
		yyval.graph = yyvsp[-1].graph; 
	   	if (yyvsp[0].arglist) {
			graph_join(&(yyval.graph), newmodule("join", yyvsp[0].arglist));
		}
	   }
    break;

  case 7:
#line 130 "grammer.y"
    { yyval.arglist = NULL; }
    break;

  case 8:
#line 132 "grammer.y"
    {
		struct arglist * atail;
		yyval.arglist = newarg(arglist2str(yyvsp[-2].arglist), 0, NULL);

	   	fprintf(stderr, "got a join with '%s' as %s.\n", yyval.arglist->arg, yyvsp[-1].string); 

		if (!yyvsp[-1].string) {
			yyerror("Joins must be aliased with \"as <alias>\"");
		}

		atail = arglist_append(yyval.arglist, newarg(yyvsp[-1].string, 0, NULL));

	   	if (yyvsp[0].arglist) {
			atail = arglist_append(atail, yyvsp[0].arglist);
		} 
	   }
    break;

  case 9:
#line 150 "grammer.y"
    { yyval.string = NULL; }
    break;

  case 10:
#line 151 "grammer.y"
    { yyval.string = yyvsp[0].string; }
    break;

  case 12:
#line 155 "grammer.y"
    { yyval.graph = yyvsp[-1].graph; }
    break;

  case 13:
#line 158 "grammer.y"
    { yyval.graph = nullgraph; }
    break;

  case 14:
#line 159 "grammer.y"
    { yyval.graph = optimize_bools(yyvsp[0].comp); }
    break;

  case 15:
#line 164 "grammer.y"
    { yyval.group.args = NULL; yyval.group.having = NULL;}
    break;

  case 16:
#line 165 "grammer.y"
    { yyval.group.args = yyvsp[-1].arglist; yyval.group.having = yyvsp[0].arglist; }
    break;

  case 17:
#line 168 "grammer.y"
    { yyval.arglist = NULL; }
    break;

  case 18:
#line 169 "grammer.y"
    { yyval.arglist = yyvsp[0].arglist; }
    break;

  case 21:
#line 176 "grammer.y"
    { yyval.string = yystring; }
    break;

  case 22:
#line 178 "grammer.y"
    { yyval.string = yystring; }
    break;

  case 23:
#line 179 "grammer.y"
    { yyval.string = "or"; }
    break;

  case 24:
#line 180 "grammer.y"
    { yyval.string = "and"; }
    break;

  case 26:
#line 184 "grammer.y"
    { yyval.arglist->rename = yyvsp[0].string; }
    break;

  case 27:
#line 187 "grammer.y"
    { yyval.arglist = newarg(yyvsp[0].string, 0, NULL); }
    break;

  case 28:
#line 188 "grammer.y"
    { yyval.arglist = newarg(yyvsp[-3].string, 1, yyvsp[-1].arglist); }
    break;

  case 29:
#line 191 "grammer.y"
    { yyval.arglist = newarg(yyvsp[0].string, 0, NULL); }
    break;

  case 30:
#line 193 "grammer.y"
    { 
	    	char * str = malloc(sizeof(char *) * (strlen(yyvsp[0].string)+2)); 
		sprintf(str,"\"%s\"", yyvsp[0].string); 
		yyval.arglist = newarg(str, 0, NULL); 
	    }
    break;

  case 31:
#line 198 "grammer.y"
    { yyval.arglist = newarg("<", 0, NULL); }
    break;

  case 32:
#line 199 "grammer.y"
    { yyval.arglist = newarg(">", 0, NULL); }
    break;

  case 33:
#line 200 "grammer.y"
    { yyval.arglist = newarg("=", 0, NULL); }
    break;

  case 34:
#line 201 "grammer.y"
    { yyval.arglist = newarg("(", 0, NULL); }
    break;

  case 35:
#line 202 "grammer.y"
    { yyval.arglist = newarg(")", 0, NULL); }
    break;

  case 36:
#line 203 "grammer.y"
    { yyval.arglist = newarg("!", 0, NULL); }
    break;

  case 38:
#line 209 "grammer.y"
    { yyval.graph = newmodule(yyvsp[0].string, NULL); }
    break;

  case 39:
#line 210 "grammer.y"
    { yyval.graph = newmodule(yyvsp[-3].string, yyvsp[-1].arglist); }
    break;

  case 40:
#line 213 "grammer.y"
    { yyval.vphrase = newvphrase(yyvsp[-1].string, yyvsp[0].arglist); }
    break;

  case 41:
#line 214 "grammer.y"
    { yyval.vphrase = newvphrase(yyvsp[-3].string, yyvsp[-1].arglist); }
    break;

  case 42:
#line 217 "grammer.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 43:
#line 220 "grammer.y"
    { yyval.arglist = NULL; }
    break;

  case 44:
#line 221 "grammer.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 45:
#line 224 "grammer.y"
    { yyval.arglist = NULL; }
    break;

  case 46:
#line 225 "grammer.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 47:
#line 228 "grammer.y"
    { yyval.arglist = NULL; }
    break;

  case 48:
#line 229 "grammer.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 50:
#line 239 "grammer.y"
    { yyval.comp = yyvsp[-1].comp; }
    break;

  case 51:
#line 240 "grammer.y"
    { yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, 1); }
    break;

  case 52:
#line 241 "grammer.y"
    { yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, 0); }
    break;

  case 54:
#line 245 "grammer.y"
    { yyval.comp = comp_new(yyvsp[0].string, EXIST, NULL); }
    break;

  case 55:
#line 246 "grammer.y"
    { yyval.comp = comp_new(yyvsp[-2].string, yyvsp[-1].op, yyvsp[0].string); }
    break;

  case 56:
#line 247 "grammer.y"
    { yyval.comp = comp_new(yyvsp[-3].string, FUNC, arglist2str(yyvsp[-1].arglist));
					  yyval.comp->arglist = yyvsp[-1].arglist;
	}
    break;

  case 57:
#line 252 "grammer.y"
    { yyval.op = EQUALITY; }
    break;

  case 58:
#line 253 "grammer.y"
    { yyval.op = GT; }
    break;

  case 59:
#line 254 "grammer.y"
    { yyval.op = LT; }
    break;

  case 60:
#line 255 "grammer.y"
    { yyval.op = LIKE; }
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 1377 "grammer.c"

  yyvsp -= yylen;
  yyssp -= yylen;


#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;


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
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyssp > yyss)
	    {
	      YYDPRINTF ((stderr, "Error: popping "));
	      YYDSYMPRINT ((stderr,
			    yystos[*yyssp],
			    *yyvsp));
	      YYDPRINTF ((stderr, "\n"));
	      yydestruct (yystos[*yyssp], *yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yydestruct (yychar1, yylval);
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
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

      YYDPRINTF ((stderr, "Error: popping "));
      YYDSYMPRINT ((stderr,
		    yystos[*yyssp], *yyvsp));
      YYDPRINTF ((stderr, "\n"));

      yydestruct (yystos[yystate], *yyvsp);
      yyvsp--;
      yystate = *--yyssp;


#if YYDEBUG
      if (yydebug)
	{
	  short *yyssp1 = yyss - 1;
	  YYFPRINTF (stderr, "Error: state stack now");
	  while (yyssp1 != yyssp)
	    YYFPRINTF (stderr, " %d", *++yyssp1);
	  YYFPRINTF (stderr, "\n");
	}
#endif
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 258 "grammer.y"



static dts_environment * Tenv;

smacq_graph * smacq_build_query(dts_environment * tenv, int argc, char ** argv) {
  int size = 0;
  int i;
  char * qstr; 
  smacq_graph * graph;
  int res;

  Tenv = tenv;

  for (i=0; i<argc; i++) {
  	size += strlen(argv[i]);
  }
  size += argc;

  qstr = (char*)malloc(size);
  	
  for (i=0; i<argc; i++) {
  	strcatn(qstr, size, argv[i]);
  	strcatn(qstr, size, " ");
  }

  /* LOCK */

  yy_scan_string(qstr);
  /* fprintf(stderr, "parsing buffer: %s\n", qstr); */

  res = yyparse();

  graph = Graph;

  /* UNLOCK */

  if (res) {
    perror("parse error");
    return NULL;
  }

  if (!graph) {
    fprintf(stderr, "parse error\n");
  }

  return graph;
}

void yyerror(char * msg) {
  fprintf(stderr, "Error: %s near %s\n", msg, yytext);
  exit(-1);
}

static struct vphrase newvphrase(char * verb, struct arglist * args) {
  	struct vphrase vphrase;
	vphrase.verb = verb;
	vphrase.args = args;
	if (!strcmp(verb, "select")) {
     		/* SQL's select is really a projection operation */
     		vphrase.verb = "project";
	}
	return vphrase;
}

static void arglist2argv(struct arglist * alist, int * argc, char *** argvp) {
	int i;
	char ** argv;
	struct arglist * al;

	for(i=0, al=alist; al; i++) {
		al=al->next;
	}
	argv = malloc(sizeof(char **) * i);
	*argc = i;
	*argvp = argv;

	for(i=0, al=alist; i<*argc; i++, al=al->next) {
		argv[i] = al->arg;
	}
}

static char * arglist2str(struct arglist * alist) {
	char * argstr;
	struct arglist * al;
	int len = 1;

	for(al=alist; al; al=al->next) 
		len += strlen(al->arg) + 1;

	argstr = malloc(len);

	for(al=alist; al; al=al->next) {
		strcatn(argstr, len, al->arg);
		strcatn(argstr, len, " ");
	}

	return argstr;
}

static void graph_join(struct graph * graph, struct graph newg) {
	if (!newg.head) 
		return; /* Do nothing */

	assert(graph);

	if (!graph->head) {
		graph->head = newg.head;
		graph->tail = newg.tail;
		return;
	}

	/* fprintf(stderr, "Adding %s after %s\n", newg.head->name, graph->tail->name); */

	/* Splice them together */
	assert(graph->tail);

	smacq_add_child(graph->tail, newg.head); 
	graph->tail = newg.tail;
}
	
static void graph_append(struct graph * graph, smacq_graph * newmod) {
	if (graph->tail) 
		smacq_add_child(graph->tail, newmod); 
	graph->tail = newmod;
	if (! graph->head) 
		graph->head = newmod;
}

static struct arglist * arglist_append(struct arglist * tail, struct arglist * addition) {
	for (; tail->next; tail=tail->next) ;
	tail->next = addition;
	for (; tail->next; tail=tail->next) ;

	return tail;
}
	
static struct graph newgroup(struct group group, struct vphrase vphrase) {
  /*
   * This function violates some abstractions by knowing the 
   * calling syntax for "groupby" and constructing arguments for it.
   */
  struct arglist * atail;
  struct graph g = { NULL, NULL};
  struct arglist * ap;
  int argcont = 0;
  
  if (!group.args) { 
    /* Do nothing if this was "group by" NULL */
    return newmodule(vphrase.verb, vphrase.args);
  }
  
  atail = arglist_append(group.args, newarg("--", 0, NULL));
  
  /* Insert function operations */
  for(ap=vphrase.args; ap; ap=ap->next) {
    /* fprintf(stderr, "group arg %s isfunc = %d\n", ap->arg, ap->isfunc); */
    if (ap->isfunc) {
      if (argcont) 
	atail = arglist_append(atail, newarg("|", 0, NULL));
      atail = arglist_append(atail, newarg(ap->arg, 0, NULL));
      atail = arglist_append(atail, ap->func_args);
      ap->isfunc = 0;
      argcont = 1;
    }
  }
  if (argcont) 
    g = newmodule("groupby", group.args);
  
  if (group.having) 
    graph_join(&g, newmodule("filter", group.having));
  
  graph_join(&g, newmodule(vphrase.verb, vphrase.args));
  
  return g;
}


static struct graph newmodule(char * module, struct arglist * alist) {
     struct arglist * anew;
     struct graph graph = { head: NULL, tail: NULL };

     int argc;
     char ** argv;

     int rename_argc = 1;
     char ** rename_argv = NULL;
     struct arglist * ap;

     anew = newarg(module, 0, NULL);
     arglist_append(anew, alist);

     for(ap=anew; ap; ap=ap->next) {
        /* Check for rename options on arguments */
     	if (ap->rename) {
		rename_argc += 2;
		rename_argv = realloc(rename_argv, rename_argc * sizeof(char*));
		rename_argv[rename_argc - 2] = ap->arg;
		rename_argv[rename_argc - 1] = ap->rename;
		ap->arg = ap->rename;
	}

	/* Check for function arguments */
	if (ap->isfunc) 
		graph_join(&graph, newmodule(ap->arg, ap->func_args));
     }

     if (rename_argc > 1) {
        /* We need to splice in a rename module before this module */
     	rename_argv[0] = "rename";
        graph_append(&graph, smacq_new_module(rename_argc, rename_argv));
     }

     arglist2argv(anew, &argc, &argv);
     graph_append(&graph, smacq_new_module(argc, argv));

     return graph;
}

static struct arglist * newarg(char * arg, int isfunc, struct arglist * func_args) {
     struct arglist * al = calloc(1, sizeof(struct arglist));
     al->arg = arg;
     if (isfunc) {
     	al->func_args = func_args;
	al->isfunc = 1;
     }

     return(al);
}

static char * opstr(dts_comparison * comp) {
  switch (comp->op) {
  case LT:
    return "<";

  case GT:
    return ">";

  case EQUALITY:
    return "==";

  case INEQUALITY:
    return "!=";

  case LIKE:
    return "like";

  case EXIST:
    return "";

  case FUNC:
    return "[FUNC]";

 case AND:
    return "[AND]";

 case OR:
    return "[OR]";
  }

  return "[ERR]";
}


static char * print_comparison(dts_comparison * comp) {
  int size = 20;
  char * buf;
  dts_comparison * c;
  char * b;

  if (comp->fieldname) 
    size += strlen(comp->fieldname);

  if (comp->valstr)
    size += strlen(comp->valstr);

  buf = malloc(size);

  if (comp->op == EXIST) {
    snprintf(buf, size, "(%s)", comp->fieldname);
  } else if (comp->op == FUNC) {
    snprintf(buf, size, "%s(%s)", comp->fieldname, comp->valstr);
  } else if (comp->op == OR) {
    buf[0] = '\0';

    for (c = comp->group; c; c=c->next) {
      assert(c);
      b = print_comparison(c);
      size += strlen(b) + 5;
      buf = realloc(buf, size);
      strcatn(buf, size, b);
      free(b);

      if (c->next) {
	strcatn(buf, size, " or ");
      }
    }
  } else {
    snprintf(buf, size, "(%s %s \"%s\")", comp->fieldname, opstr(comp), comp->valstr);
  }

  return(buf);
}

static struct graph optimize_bools(dts_comparison * comp) {
  dts_comparison *c;
  struct arglist * arglist;
  struct graph g;

  g.head = NULL; g.tail = NULL;
 
  for(c=comp; c; c = c->next) {
    if (c->op == AND) {
      assert (! "untested");
      graph_join(&g, optimize_bools(c->group));
    } else if (c->op == FUNC) {
      graph_join(&g, newmodule(c->fieldname, c->arglist));
    } else if (c->op == OR) {
      arglist = newarg(print_comparison(c), 0, NULL);
      graph_join(&g, newmodule("filter", arglist)); 
    } else {
      arglist = newarg(print_comparison(c), 0, NULL);
      graph_join(&g, newmodule("filter", arglist));
    }
  }

  return g;
}

static dts_comparison * comp_join(dts_comparison * lh, dts_comparison * rh, int isor) {
  dts_comparison * ret = NULL;

  if (!lh) return rh;
  if (!rh) return lh;

  if (!isor) {
   	ret = lh;
	for (; lh->next; lh=lh->next) ;

	lh->next = rh;
  } else if (isor) {
        dts_comparison * left, * right;
        if (lh->next) {
		left = calloc(1,sizeof(dts_comparison));
		left->op = AND;
		left->group = lh;
	} else {
		left = lh;
	}
	if (rh->next) {
		right = calloc(1,sizeof(dts_comparison));
		right->op = AND;
		right->group = rh;
	} else {
		right = rh;
	}

        ret = calloc(1,sizeof(dts_comparison));
	ret->op = OR;
	ret->group = left;
	left->next = right;
  } else {
  	assert(!"Shouldn't get here!");
  }
  	
  return ret;
}


static dts_comparison * comp_new(char * field, dts_compare_operation op, char * value) {
     dts_comparison * comp = calloc(1,sizeof(dts_comparison));

     comp->op = op;
     comp->valstr = value;
     comp->fieldname = field;

     if (comp->op != FUNC) {
       comp->field = Tenv->requirefield(Tenv, field);
     }

     return comp;
}


