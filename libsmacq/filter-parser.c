/* A Bison parser, made from /Users/mfisk/cvs/SMACQ_1/smacq/libsmacq/filter-parser.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	WHERE	257
# define	GROUP	258
# define	BY	259
# define	FROM	260
# define	SELECT	261
# define	AS	262
# define	HAVING	263
# define	UNION	264
# define	YYSTRING	265
# define	YYID	266
# define	YYNUMBER	267
# define	YYNEQ	268
# define	YYLEQ	269
# define	YYGEQ	270
# define	YYSTOP	271
# define	YYLIKE	272
# define	YYOR	273
# define	YYAND	274
# define	YYNOT	275

#line 4 "filter-parser.y"

     #define    yymaxdepth yyfiltermaxdepth
     #define    yyparse yyfilterparse
     #define    yylex   yyfilterlex
     #define    yyerror yyfiltererror
     #define    yylval  yyfilterlval
     #define    yychar  yyfilterchar
     #define    yydebug yyfilterdebug
     #define    yypact  yyfilterpact
     #define    yyr1    yyfilterr1
     #define    yyr2    yyfilterr2
     #define    yydef   yyfilterdef
     #define    yychk   yyfilterchk
     #define    yypgo   yyfilterpgo
     #define    yyact   yyfilteract
     #define    yyexca  yyfilterexca
     #define yyerrflag yyfiltererrflag
     #define yynerrs    yyfilternerrs
     #define    yyps    yyfilterps
     #define    yypv    yyfilterpv
     #define    yys     yyfilters
     #define    yy_yys  yyfilteryys
     #define    yystate yyfilterstate
     #define    yytmp   yyfiltertmp
     #define    yyv     yyfilterv
     #define    yy_yyv  yyfilteryyv
     #define    yyval   yyfilterval
     #define    yylloc  yyfilterlloc
     #define yyreds     yyfilterreds
     #define yytoks     yyfiltertoks
     #define yylhs      yyfilteryylhs
     #define yylen      yyfilteryylen
     #define yydefred yyfilteryydefred
     #define yydgoto    yyfilteryydgoto
     #define yysindex yyfilteryysindex
     #define yyrindex yyfilteryyrindex
     #define yygindex yyfilteryygindex
     #define yytable     yyfilteryytable
     #define yycheck     yyfilteryycheck
     #define yyname   yyfilteryyname
     #define yyrule   yyfilteryyrule
#include <smacq.h>
static dts_comparison * Comp;
#define yyfilterlex yysmacql_lex
#line 69 "filter-parser.y"

#include <smacq-parser.h>

#line 99 "filter-parser.y"
#ifndef YYSTYPE
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
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		78
#define	YYFLAG		-32768
#define	YYNTBASE	34

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 275 ? yytranslate[x] : 53)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      26,    27,    24,    22,    30,    23,     2,    25,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      33,    31,    32,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    28,     2,    29,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     3,     5,     7,     9,    11,    13,    15,
      17,    19,    23,    25,    30,    34,    36,    41,    43,    45,
      48,    50,    54,    58,    62,    66,    69,    71,    73,    75,
      77,    81,    85,    87,    89,    91,    95,   100,   102,   104,
     106,   108,   110,   112,   114,   116,   118,   120,   122
};
static const short yyrhs[] =
{
      -1,    38,     0,    37,     0,    36,     0,    13,     0,    11,
       0,    12,     0,    19,     0,    20,     0,    40,     0,    40,
       8,    35,     0,    35,     0,    41,    26,    42,    27,     0,
      28,    47,    29,     0,    38,     0,    39,    30,    39,    44,
       0,    43,     0,    34,     0,    39,    43,     0,    34,     0,
      30,    39,    44,     0,    26,    45,    27,     0,    45,    19,
      45,     0,    45,    20,    45,     0,    21,    45,     0,    49,
       0,    38,     0,    37,     0,    36,     0,    26,    47,    27,
       0,    48,    51,    48,     0,    47,     0,    46,     0,    46,
       0,    48,    50,    48,     0,    41,    26,    42,    27,     0,
      31,     0,    32,     0,    33,     0,    16,     0,    15,     0,
      14,     0,    18,     0,    22,     0,    23,     0,    25,     0,
      24,     0,    45,    17,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   112,   115,   116,   117,   120,   123,   126,   127,   128,
     131,   132,   135,   136,   137,   142,   145,   146,   149,   150,
     153,   154,   162,   163,   164,   165,   166,   169,   170,   171,
     175,   180,   186,   188,   191,   193,   194,   201,   202,   203,
     204,   205,   206,   207,   210,   211,   212,   213,   217
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "WHERE", "GROUP", "BY", "FROM", "SELECT", 
  "AS", "HAVING", "UNION", "YYSTRING", "YYID", "YYNUMBER", "YYNEQ", 
  "YYLEQ", "YYGEQ", "YYSTOP", "YYLIKE", "YYOR", "YYAND", "YYNOT", "'+'", 
  "'-'", "'*'", "'/'", "'('", "')'", "'['", "']'", "','", "'='", "'>'", 
  "'<'", "null", "word", "number", "string", "id", "arg", "argument", 
  "function", "args", "spacedargs", "moreargs", "boolean", "operand", 
  "expression", "subexpression", "test", "op", "arithop", "booleanline", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    34,    35,    35,    35,    36,    37,    38,    38,    38,
      39,    39,    40,    40,    40,    41,    42,    42,    43,    43,
      44,    44,    45,    45,    45,    45,    45,    46,    46,    46,
      47,    47,    48,    48,    49,    49,    49,    50,    50,    50,
      50,    50,    50,    50,    51,    51,    51,    51,    52
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     4,     3,     1,     4,     1,     1,     2,
       1,     3,     3,     3,     3,     2,     1,     1,     1,     1,
       3,     3,     1,     1,     1,     3,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,     6,     7,     5,     8,     9,     0,     0,    29,    28,
      27,     0,     0,    33,    32,     0,    26,    25,     0,    32,
       1,    48,     0,     0,    42,    41,    40,    43,    44,    45,
      47,    46,    37,    38,    39,     0,     0,    22,    30,     0,
      18,    12,     4,     3,     2,     1,    10,     0,     0,    17,
      23,    24,     0,    27,    33,    35,    31,    32,     0,     0,
       1,    19,     0,     1,    36,    14,     1,    11,     2,     0,
       0,    20,    16,    13,     1,    21,     0,     0,     0
};

static const short yydefgoto[] =
{
      40,    41,     8,     9,    44,    45,    46,    47,    48,    49,
      72,    12,    13,    14,    15,    16,    35,    36,    76
};

static const short yypact[] =
{
      99,-32768,-32768,-32768,-32768,-32768,    99,    99,-32768,-32768,
     -23,   -16,    29,   107,-32768,    50,-32768,-32768,    -6,   -11,
      78,-32768,    99,    99,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,   102,   102,-32768,-32768,   102,
  -32768,-32768,-32768,-32768,   -23,    58,    12,     6,     4,-32768,
  -32768,-32768,   102,-32768,-32768,   117,   117,     9,   117,    78,
      78,-32768,   118,    78,-32768,-32768,     3,-32768,-32768,    23,
      78,-32768,-32768,-32768,     3,-32768,    53,    54,-32768
};

static const short yypgoto[] =
{
     -29,    -7,   -19,    -3,     0,   -41,-32768,     5,    -2,   -30,
      10,     2,    57,    -5,    64,-32768,-32768,-32768,-32768
};


#define	YYLAST		142


static const short yytable[] =
{
      10,    42,    19,   -15,    60,    11,    10,    10,    17,    18,
      20,    11,    11,    22,    23,    61,    38,    43,    66,    60,
      62,    37,    10,    10,    50,    51,    42,    11,    11,    74,
      61,    64,    63,    70,    57,    53,    53,    71,    65,    53,
      42,    42,    43,    42,    42,    71,    21,    19,    22,    23,
      73,    42,    53,    77,    78,    67,    43,    43,     0,    43,
      43,    69,    68,     0,    24,    25,    26,    43,    27,     1,
       2,     3,    28,    29,    30,    31,     0,     4,     5,     0,
       0,    32,    33,    34,    75,     0,    39,     0,    59,     1,
       2,     3,    54,    54,     0,     0,    54,     4,     5,    55,
      56,     0,     0,    58,     0,     0,    39,     0,     0,    54,
       1,     2,     3,     1,     2,     3,    58,     0,     4,     5,
       6,     4,     5,     0,   -34,     7,   -34,   -34,    52,     1,
       2,     3,     0,     0,   -34,     0,     0,     4,     5,    28,
      29,    30,    31
};

static const short yycheck[] =
{
       0,    20,     7,    26,    45,     0,     6,     7,     6,     7,
      26,     6,     7,    19,    20,    45,    27,    20,    59,    60,
       8,    27,    22,    23,    22,    23,    45,    22,    23,    70,
      60,    27,    26,    30,    39,    35,    36,    66,    29,    39,
      59,    60,    45,    62,    63,    74,    17,    52,    19,    20,
      27,    70,    52,     0,     0,    62,    59,    60,    -1,    62,
      63,    63,    62,    -1,    14,    15,    16,    70,    18,    11,
      12,    13,    22,    23,    24,    25,    -1,    19,    20,    -1,
      -1,    31,    32,    33,    74,    -1,    28,    -1,    30,    11,
      12,    13,    35,    36,    -1,    -1,    39,    19,    20,    35,
      36,    -1,    -1,    39,    -1,    -1,    28,    -1,    -1,    52,
      11,    12,    13,    11,    12,    13,    52,    -1,    19,    20,
      21,    19,    20,    -1,    17,    26,    19,    20,    26,    11,
      12,    13,    -1,    -1,    27,    -1,    -1,    19,    20,    22,
      23,    24,    25
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/sw/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

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

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

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
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

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
	    (To)[yyi] = (From)[yyi];		\
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
#define YYABORT 	goto yyabortlab
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
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


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
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
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

#ifdef YYERROR_VERBOSE

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
#endif

#line 315 "/sw/share/bison/bison.simple"


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

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

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

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
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
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
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
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
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
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

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
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 5:
#line 120 "filter-parser.y"
{ yyval.string = yystring; }
    break;
case 6:
#line 123 "filter-parser.y"
{ yyval.string = yystring; }
    break;
case 7:
#line 126 "filter-parser.y"
{ yyval.string = yystring; }
    break;
case 8:
#line 127 "filter-parser.y"
{ yyval.string = "or"; }
    break;
case 9:
#line 128 "filter-parser.y"
{ yyval.string = "and"; }
    break;
case 11:
#line 132 "filter-parser.y"
{ yyval.arglist->rename = yyvsp[0].string; }
    break;
case 12:
#line 135 "filter-parser.y"
{ yyval.arglist = newarg(yyvsp[0].string, WORD, NULL); }
    break;
case 13:
#line 136 "filter-parser.y"
{ yyval.arglist = newarg(yyvsp[-3].string, FUNCTION, yyvsp[-1].arglist); }
    break;
case 14:
#line 137 "filter-parser.y"
{ yyval.arglist = newarg("expr", FUNCTION, 
					       newarg(print_operand(yyvsp[-1].operand), WORD, NULL)); 
			                  }
    break;
case 16:
#line 145 "filter-parser.y"
{ yyval.arglist = yyvsp[-3].arglist; yyval.arglist->next = yyvsp[-1].arglist; yyvsp[-1].arglist->next = yyvsp[0].arglist; }
    break;
case 18:
#line 149 "filter-parser.y"
{ yyval.arglist = NULL; }
    break;
case 19:
#line 150 "filter-parser.y"
{ yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;
case 20:
#line 153 "filter-parser.y"
{ yyval.arglist = NULL; }
    break;
case 21:
#line 154 "filter-parser.y"
{ yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;
case 22:
#line 162 "filter-parser.y"
{ yyval.comp = yyvsp[-1].comp; }
    break;
case 23:
#line 163 "filter-parser.y"
{ yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, OR); }
    break;
case 24:
#line 164 "filter-parser.y"
{ yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, AND); }
    break;
case 25:
#line 165 "filter-parser.y"
{ yyval.comp = comp_join(yyvsp[0].comp, NULL, NOT); }
    break;
case 27:
#line 169 "filter-parser.y"
{ yyval.operand = comp_operand(FIELD, yyvsp[0].string); }
    break;
case 28:
#line 170 "filter-parser.y"
{ yyval.operand = comp_operand(CONST, yyvsp[0].string); }
    break;
case 29:
#line 171 "filter-parser.y"
{ yyval.operand = comp_operand(CONST, yyvsp[0].string); }
    break;
case 30:
#line 177 "filter-parser.y"
{	  
				  yyval.operand = yyvsp[-1].operand;  
				}
    break;
case 31:
#line 181 "filter-parser.y"
{
				  yyval.operand = comp_arith(parse_tenv, yyvsp[-1].arithop, yyvsp[-2].operand, yyvsp[0].operand); 
				}
    break;
case 34:
#line 192 "filter-parser.y"
{  yyval.comp = comp_new(EXIST, yyvsp[0].operand, yyvsp[0].operand); }
    break;
case 35:
#line 193 "filter-parser.y"
{ yyval.comp = comp_new(yyvsp[-1].op, yyvsp[-2].operand, yyvsp[0].operand); }
    break;
case 36:
#line 194 "filter-parser.y"
{ 
				  int argc; char ** argv;
				  arglist2argv(yyvsp[-1].arglist, &argc, &argv);
				  yyval.comp = comp_new_func(yyvsp[-3].string, argc, argv, yyvsp[-1].arglist);
				}
    break;
case 37:
#line 201 "filter-parser.y"
{ yyval.op = EQ; }
    break;
case 38:
#line 202 "filter-parser.y"
{ yyval.op = GT; }
    break;
case 39:
#line 203 "filter-parser.y"
{ yyval.op = LT; }
    break;
case 40:
#line 204 "filter-parser.y"
{ yyval.op = GEQ; }
    break;
case 41:
#line 205 "filter-parser.y"
{ yyval.op = LEQ; }
    break;
case 42:
#line 206 "filter-parser.y"
{ yyval.op = NEQ; }
    break;
case 43:
#line 207 "filter-parser.y"
{ yyval.op = LIKE; }
    break;
case 44:
#line 210 "filter-parser.y"
{ yyval.arithop = ADD; }
    break;
case 45:
#line 211 "filter-parser.y"
{ yyval.arithop = SUB; }
    break;
case 46:
#line 212 "filter-parser.y"
{ yyval.arithop = DIVIDE; }
    break;
case 47:
#line 213 "filter-parser.y"
{ yyval.arithop = MULT; }
    break;
case 48:
#line 217 "filter-parser.y"
{
					Comp = yyvsp[-1].comp;
					return 0;
				}
    break;
}

#line 705 "/sw/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

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
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
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
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

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

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 223 "filter-parser.y"


static dts_environment * tenv;
extern void yysmacql_scan_string(char *);

#ifndef SMACQ_OPT_NOPTHREADS
#ifdef PTHREAD_MUTEX_INITIALIZER
  static pthread_mutex_t local_lock = PTHREAD_MUTEX_INITIALIZER;
#else
  static pthread_mutex_t local_lock;
  #warning "No PTHREAD_MUTEX_INITIALIZER"
#endif
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
		case NOT: op = "!"; break;
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
  fprintf(stderr, "Error: %s near %s\n", msg, yytext);
  //exit(-1);
}



