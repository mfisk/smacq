/* A Bison parser, made from /home/mfisk/smacq/libsmacq/filter/filter-parser.y, by GNU bison 1.75.  */

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

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
#define yyparse yyfilterparse
#define yylex   yyfilterlex
#define yyerror yyfiltererror
#define yylval  yyfilterlval
#define yychar  yyfilterchar
#define yydebug yyfilterdebug
#define yynerrs yyfilternerrs


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
     AS = 263,
     HAVING = 264,
     YYSTRING = 265,
     YYID = 266,
     YYNUMBER = 267,
     YYNEQ = 268,
     YYLEQ = 269,
     YYGEQ = 270,
     YYSTOP = 271,
     YYLIKE = 272,
     YYOR = 273,
     YYAND = 274
   };
#endif
#define WHERE 258
#define GROUP 259
#define BY 260
#define FROM 261
#define SELECT 262
#define AS 263
#define HAVING 264
#define YYSTRING 265
#define YYID 266
#define YYNUMBER 267
#define YYNEQ 268
#define YYLEQ 269
#define YYGEQ 270
#define YYSTOP 271
#define YYLIKE 272
#define YYOR 273
#define YYAND 274




/* Copy the first part of user declarations.  */
#line 4 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"

#include <smacq.h>
static dts_comparison * Comp;
extern char * yysmacql_text;
#define yyfilterlex yysmacql_lex
#line 30 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"


#include <smacq-parser.h>
//static struct graph nullgraph = { head: NULL, tail: NULL };
//static smacq_graph * Graph;



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
#line 59 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
typedef union {
  struct graph graph;
  struct arglist * arglist;
  struct vphrase vphrase;
  char * string;
  struct group group;
  dts_compare_operation op;
  enum dts_arith_operand_type arithop;
  dts_comparison * comp;
  struct dts_operand * operand;
} yystype;
/* Line 193 of /usr/share/bison/yacc.c.  */
#line 146 "/home/mfisk/smacq/libsmacq/filter/filter-parser.c"
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
#line 167 "/home/mfisk/smacq/libsmacq/filter/filter-parser.c"

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
#define YYFINAL  30
#define YYLAST   116

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  30
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  20
/* YYNRULES -- Number of rules. */
#define YYNRULES  46
/* YYNRULES -- Number of states. */
#define YYNSTATES  72

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   274

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      20,    21,    29,    26,    22,    27,     2,    28,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      25,    23,    24,     2,     2,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    10,    12,    14,    16,
      18,    20,    22,    26,    28,    33,    35,    40,    42,    44,
      47,    49,    53,    55,    59,    63,    67,    69,    71,    73,
      75,    81,    83,    85,    89,    94,    96,    98,   100,   102,
     104,   106,   108,   110,   112,   114,   116
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      49,     0,    -1,    -1,    35,    -1,    34,    -1,    33,    -1,
      12,    -1,    10,    -1,    11,    -1,    18,    -1,    19,    -1,
      37,    -1,    37,     8,    32,    -1,    32,    -1,    38,    20,
      39,    21,    -1,    35,    -1,    36,    22,    36,    41,    -1,
      40,    -1,    31,    -1,    36,    40,    -1,    31,    -1,    22,
      36,    41,    -1,    35,    -1,    20,    43,    21,    -1,    43,
      18,    43,    -1,    43,    19,    43,    -1,    46,    -1,    35,
      -1,    34,    -1,    33,    -1,    20,    45,    48,    45,    21,
      -1,    44,    -1,    44,    -1,    45,    47,    45,    -1,    42,
      20,    39,    21,    -1,    23,    -1,    24,    -1,    25,    -1,
      15,    -1,    14,    -1,    13,    -1,    17,    -1,    26,    -1,
      27,    -1,    28,    -1,    29,    -1,    43,    16,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    72,    72,    75,    76,    77,    80,    83,    86,    87,
      88,    91,    92,    95,    96,    99,   102,   103,   106,   107,
     110,   111,   114,   121,   122,   123,   124,   127,   128,   129,
     132,   135,   138,   139,   140,   149,   150,   151,   152,   153,
     154,   155,   158,   159,   160,   161,   165
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "WHERE", "GROUP", "BY", "FROM", "SELECT", 
  "AS", "HAVING", "YYSTRING", "YYID", "YYNUMBER", "YYNEQ", "YYLEQ", 
  "YYGEQ", "YYSTOP", "YYLIKE", "YYOR", "YYAND", "'('", "')'", "','", 
  "'='", "'>'", "'<'", "'+'", "'-'", "'/'", "'*'", "$accept", "null", 
  "word", "number", "string", "id", "arg", "argument", "function", "args", 
  "spacedargs", "moreargs", "verb", "boolean", "operand", "expression", 
  "test", "op", "arithop", "booleanline", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      40,    41,    44,    61,    62,    60,    43,    45,    47,    42
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    30,    31,    32,    32,    32,    33,    34,    35,    35,
      35,    36,    36,    37,    37,    38,    39,    39,    40,    40,
      41,    41,    42,    43,    43,    43,    43,    44,    44,    44,
      45,    45,    46,    46,    46,    47,    47,    47,    47,    47,
      47,    47,    48,    48,    48,    48,    49
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     4,     1,     4,     1,     1,     2,
       1,     3,     1,     3,     3,     3,     1,     1,     1,     1,
       5,     1,     1,     3,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     7,     8,     6,     9,    10,     0,    29,    28,    27,
       0,     0,    31,     0,    26,     0,     0,     0,     2,    46,
       0,     0,    40,    39,    38,    41,    35,    36,    37,     0,
       1,    23,    42,    43,    44,    45,     0,    18,    13,     5,
       4,     3,     2,    11,     0,     0,    17,    24,    25,     0,
      27,    31,    33,     0,     0,     2,    19,     0,     2,    34,
       0,    30,     2,    12,     3,     0,     0,    20,    16,    14,
       2,    21
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,    37,    38,     7,     8,    41,    42,    43,    44,    45,
      46,    68,    10,    11,    12,    13,    14,    29,    36,    15
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -55
static const yysigned_char yypact[] =
{
      77,   -55,   -55,   -55,   -55,   -55,    77,   -55,   -55,    -9,
      -8,    14,    95,    56,   -55,    15,    86,    39,    91,   -55,
      77,    77,   -55,   -55,   -55,   -55,   -55,   -55,   -55,    80,
     -55,   -55,   -55,   -55,   -55,   -55,    80,   -55,   -55,   -55,
     -55,    -3,    64,    16,     7,    13,   -55,   -55,   -55,    80,
     -55,   -55,   -55,    19,    91,    91,   -55,    91,    91,   -55,
      32,   -55,    23,   -55,   -55,    30,    91,   -55,   -55,   -55,
      23,   -55
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -55,   -54,   -20,   -16,   -11,     0,   -41,   -55,   -55,    20,
     -33,     2,   -55,    -2,   -26,    -1,   -55,   -55,   -55,   -55
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -33
static const yysigned_char yytable[] =
{
       9,    55,    39,    51,    16,    17,     9,    40,    67,    56,
      51,   -22,    18,    62,    55,    30,    67,   -15,    47,    48,
       9,     9,    56,    51,    57,    70,    39,    58,    52,    50,
      19,    40,    20,    21,    59,    53,    50,    63,    39,    39,
      61,    39,    39,    40,    40,    66,    40,    40,    60,    50,
      39,    69,    22,    23,    24,    40,    25,    64,    32,    33,
      34,    35,    26,    27,    28,    32,    33,    34,    35,    22,
      23,    24,    71,    25,     1,     2,     3,     0,    65,    26,
      27,    28,     4,     5,     0,     0,    54,     1,     2,     3,
       1,     2,     3,     0,     0,     4,     5,     6,     4,     5,
      49,     1,     2,     3,    20,    21,     0,    31,     0,     4,
       5,   -32,     0,   -32,   -32,     0,   -32
};

static const yysigned_char yycheck[] =
{
       0,    42,    18,    29,     6,     6,     6,    18,    62,    42,
      36,    20,    20,    54,    55,     0,    70,    20,    20,    21,
      20,    21,    55,    49,     8,    66,    42,    20,    29,    29,
      16,    42,    18,    19,    21,    36,    36,    57,    54,    55,
      21,    57,    58,    54,    55,    22,    57,    58,    49,    49,
      66,    21,    13,    14,    15,    66,    17,    57,    26,    27,
      28,    29,    23,    24,    25,    26,    27,    28,    29,    13,
      14,    15,    70,    17,    10,    11,    12,    -1,    58,    23,
      24,    25,    18,    19,    -1,    -1,    22,    10,    11,    12,
      10,    11,    12,    -1,    -1,    18,    19,    20,    18,    19,
      20,    10,    11,    12,    18,    19,    -1,    21,    -1,    18,
      19,    16,    -1,    18,    19,    -1,    21
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    10,    11,    12,    18,    19,    20,    33,    34,    35,
      42,    43,    44,    45,    46,    49,    43,    45,    20,    16,
      18,    19,    13,    14,    15,    17,    23,    24,    25,    47,
       0,    21,    26,    27,    28,    29,    48,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    43,    43,    20,
      35,    44,    45,    45,    22,    36,    40,     8,    20,    21,
      45,    21,    36,    32,    35,    39,    22,    31,    41,    21,
      36,    41
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
        case 6:
#line 80 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.string = yystring; }
    break;

  case 7:
#line 83 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.string = yystring; }
    break;

  case 8:
#line 86 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.string = yystring; }
    break;

  case 9:
#line 87 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.string = "or"; }
    break;

  case 10:
#line 88 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.string = "and"; }
    break;

  case 12:
#line 92 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.arglist->rename = yyvsp[0].string; }
    break;

  case 13:
#line 95 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.arglist = newarg(yyvsp[0].string, 0, NULL); }
    break;

  case 14:
#line 96 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.arglist = newarg(yyvsp[-3].string, 1, yyvsp[-1].arglist); }
    break;

  case 16:
#line 102 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.arglist = yyvsp[-3].arglist; yyval.arglist->next = yyvsp[-1].arglist; yyvsp[-1].arglist->next = yyvsp[0].arglist; }
    break;

  case 18:
#line 106 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.arglist = NULL; }
    break;

  case 19:
#line 107 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 20:
#line 110 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.arglist = NULL; }
    break;

  case 21:
#line 111 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 23:
#line 121 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.comp = yyvsp[-1].comp; }
    break;

  case 24:
#line 122 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, 1); }
    break;

  case 25:
#line 123 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, 0); }
    break;

  case 27:
#line 127 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.operand = comp_operand(FIELD, yyvsp[0].string); }
    break;

  case 28:
#line 128 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.operand = comp_operand(CONST, yyvsp[0].string); }
    break;

  case 29:
#line 129 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.operand = comp_operand(CONST, yyvsp[0].string); }
    break;

  case 30:
#line 132 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    {
				  yyval.operand = comp_arith(parse_tenv, yyvsp[-2].arithop, yyvsp[-3].operand, yyvsp[-1].operand); 
				}
    break;

  case 32:
#line 138 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.comp = comp_new(EXIST, yyvsp[0].operand, yyvsp[0].operand); }
    break;

  case 33:
#line 139 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.comp = comp_new(yyvsp[-1].op, yyvsp[-2].operand, yyvsp[0].operand); }
    break;

  case 34:
#line 140 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { 
				  int argc; char ** argv;
				  struct dts_operand op;

				  arglist2argv(yyvsp[-1].arglist, &argc, &argv);
				  yyval.comp = comp_new_func(yyvsp[-3].string, argc, argv, yyvsp[-1].arglist);
				}
    break;

  case 35:
#line 149 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.op = EQ; }
    break;

  case 36:
#line 150 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.op = GT; }
    break;

  case 37:
#line 151 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.op = LT; }
    break;

  case 38:
#line 152 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.op = GEQ; }
    break;

  case 39:
#line 153 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.op = LEQ; }
    break;

  case 40:
#line 154 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.op = NEQ; }
    break;

  case 41:
#line 155 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.op = LIKE; }
    break;

  case 42:
#line 158 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.arithop = ADD; }
    break;

  case 43:
#line 159 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.arithop = SUB; }
    break;

  case 44:
#line 160 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.arithop = DIVIDE; }
    break;

  case 45:
#line 161 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    { yyval.arithop = MULT; }
    break;

  case 46:
#line 165 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"
    {
					Comp = yyvsp[-1].comp;
					return 0;
				}
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 1234 "/home/mfisk/smacq/libsmacq/filter/filter-parser.c"

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


#line 171 "/home/mfisk/smacq/libsmacq/filter/filter-parser.y"


static dts_environment * tenv;
extern void yysmacql_scan_string(char *);

#ifdef PTHREAD_MUTEX_INITIALIZER
  static pthread_mutex_t local_lock = PTHREAD_MUTEX_INITIALIZER;
#else
  static pthread_mutex_t local_lock;
  #warning "No PTHREAD_MUTEX_INITIALIZER"
#endif

static void print_comp(dts_environment * tenv, dts_comparison * c) {
	char * op;
	op = "UNDEFINED";

	if (!c) {
		// fprintf(stderr,". (%p)\n", c);
		return;
	}

	switch(c->op) {
		case AND: op = "AND"; break;
		case OR: op = "OR"; break;
		case GT: op = "<"; break;
		case LT: op = ">"; break;
		case EQ: op = "="; break;
		case NEQ: op = "!="; break;
		case GEQ: op = ">="; break;
		case LEQ: op = "<="; break;
		case EXIST: op = "exist"; break;
		case LIKE: op = "like"; break;
		case FUNC: op = "FN"; break;
	}

#if DEBUG
	switch(c->op) {
		case AND:
		case OR:
			fprintf(stderr, "Comparison %p: op %s, next %p, group %p\n", c, op, c->next, c->group);
			break;

		case FUNC:
			fprintf(stderr, "Comparison %p: FN %s(), next %p, group %p\n", c, c->valstr, c->next, c->group);
			break;

		default:
			fprintf(stderr, "Comparison %p: field %d... %s %s, next %p, group %p\n", c, c->field[0], op, c->valstr, c->next, c->group);
			break;
			
	}
#endif

	print_comp(tenv, c->group);
	print_comp(tenv, c->next);

}

dts_comparison * dts_parse_tests(dts_environment * localtenv, int argc, char ** argv) {
  dts_comparison * retval;
  int size = 1;
  int i;
  char * qstr;

  /* LOCK */
  tenv = localtenv;

  for (i=0; i<argc; i++) {
  	size += strlen(argv[i]);
  }
  size += argc;

  qstr = (char*)malloc(size);
  qstr[0] = '\0';
  	
  for (i=0; i<argc; i++) {
  	strcatn(qstr, size, argv[i]);
  	strcatn(qstr, size, " ");
  }
  yysmacql_scan_string(qstr);
  //fprintf(stderr, "parsing filter buffer: %s\n", qstr); 

  if (yyfilterparse()) {
  	/* Should free the comparisons? */
        fprintf(stderr, "got nonzero return parsing boolean %s\n", qstr); 
  	return NULL;
  }

  retval = Comp;

  /* UNLOCK */
  print_comp(localtenv, retval);

  return retval;
}

void yyfiltererror(char * msg) {
  fprintf(stderr, "Error: %s near %s\n", msg, yysmacql_text);
  //exit(-1);
}

#if 0
static struct list list_join(struct list list, struct list newl, int isor) {
	struct list retval;

	assert(newl.head);
	assert(list.head);

	if ((list.head == list.tail || list.isor == isor) && (newl.head == newl.tail || newl.isor == isor)) {
		/* Splice them together */
		assert(list.tail);
		list.tail->next = newl.head;

		retval.head = list.head;
		retval.tail = newl.head;
		retval.isor = isor;

		return retval;
	} else if (list.head == list.tail || list.isor == isor) {
		/* The right list (newl) is a different type, so make it a sublist */
		dts_comparison * comp = calloc(1,sizeof(dts_comparison));

		comp->op = newl.isor ? OR : AND;
		comp->group = newl.head;
		comp->next = NULL;

		list.tail = (list.tail->next = comp);
		list.isor = isor;

		return list;
	} else if (newl.head == newl.tail || newl.isor == isor) {
		/* The left list (list) is a different type, so make it a sublist */
		dts_comparison * comp = calloc(1,sizeof(dts_comparison));

		comp->op = list.isor ? OR : AND;
		comp->group = list.head;
		comp->next = newl.head;

		list.head = comp;
		list.tail = newl.tail;
		list.isor = isor;

		return list;
	} else {
		/* Both sublists are a different type, so make a new meta-list */
		dts_comparison * comp = calloc(1,sizeof(dts_comparison));
		dts_comparison * comp2 = calloc(1,sizeof(dts_comparison));

		comp->op = list.isor ? OR : AND;
		comp->group = list.head;
		comp->next = comp2;

		comp2->op = newl.isor ? OR : AND;
		comp2->group = newl.head;
		comp2->next = NULL;

		list.head = comp;
		list.tail = comp2;
		list.isor = isor;

		return list;
	}
}

static struct list newlist(char * field, dts_compare_operation op, char * value) {
     dts_comparison * comp = calloc(1,sizeof(dts_comparison));
     struct list list;

     comp->op = op;
     comp->valstr = value;
     comp->field = tenv->requirefield(tenv, field);

     list.head = comp;
     list.tail = comp;

     return list;
}
#endif



