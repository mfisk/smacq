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
     HAVING = 267
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




/* Copy the first part of user declarations.  */
#line 22 "grammer.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <smacq-internal.h>
#include "smacq-parser.h"
#define DEBUG
  
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
  void print_graph(struct filter * f);

  struct graph nullgraph = { head: NULL, tail: NULL };
  
  struct filter * Graph;



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
#line 79 "grammer.y"
typedef union {
  struct graph graph;
  struct arglist * arglist;
  struct vphrase vphrase;
  char * string;
  struct group group;
} yystype;
/* Line 193 of /usr/share/bison/yacc.c.  */
#line 143 "grammer.c"
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
#line 164 "grammer.c"

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
#define YYFINAL  7
#define YYLAST   78

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  20
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  24
/* YYNRULES -- Number of rules. */
#define YYNRULES  45
/* YYNRULES -- Number of states. */
#define YYNSTATES  75

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   267

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    19,     2,     2,     2,     2,     2,     2,
      14,    15,     2,     2,    13,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      16,    18,    17,     2,     2,     2,     2,     2,     2,     2,
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
       5,     6,     7,     8,     9,    10,    11,    12
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     6,     7,    12,    14,    18,    20,    24,
      26,    30,    32,    35,    37,    42,    44,    47,    49,    51,
      53,    55,    57,    61,    63,    68,    70,    72,    74,    76,
      78,    80,    82,    84,    86,    88,    93,    96,   101,   104,
     106,   109,   111,   114,   116,   120
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      21,     0,    -1,    23,    10,    -1,    -1,    38,    24,    27,
      28,    -1,    22,    -1,     6,    26,    25,    -1,    22,    -1,
      13,    39,    25,    -1,    37,    -1,    14,    23,    15,    -1,
      22,    -1,     3,    39,    -1,    22,    -1,     4,     5,    41,
      29,    -1,    22,    -1,    12,    39,    -1,    32,    -1,    31,
      -1,     8,    -1,     9,    -1,    34,    -1,    34,    11,    30,
      -1,    30,    -1,    36,    14,    41,    15,    -1,    32,    -1,
      31,    -1,    16,    -1,    17,    -1,    18,    -1,    14,    -1,
      15,    -1,    19,    -1,    32,    -1,    43,    -1,    43,    14,
      41,    15,    -1,    43,    41,    -1,    43,    14,    41,    15,
      -1,    35,    40,    -1,    22,    -1,    35,    40,    -1,    22,
      -1,    33,    42,    -1,    22,    -1,    13,    33,    42,    -1,
      32,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    88,    88,    98,   100,   113,   114,   123,   124,   135,
     136,   139,   140,   143,   144,   147,   148,   151,   152,   155,
     157,   159,   160,   163,   164,   167,   168,   174,   175,   176,
     177,   178,   179,   182,   185,   186,   189,   190,   193,   196,
     197,   200,   201,   204,   205,   208
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "WHERE", "GROUP", "BY", "FROM", "SELECT", 
  "STRING", "ID", "STOP", "AS", "HAVING", "','", "'('", "')'", "'<'", 
  "'>'", "'='", "'!'", "$accept", "queryline", "null", "query", "from", 
  "joins", "source", "where", "group", "having", "word", "string", "id", 
  "arg", "argument", "boolarg", "function", "pverbphrase", "verbphrase", 
  "boolargs", "moreboolargs", "args", "moreargs", "verb", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,    44,    40,    41,    60,    62,    61,    33
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    20,    21,    22,    23,    24,    24,    25,    25,    26,
      26,    27,    27,    28,    28,    29,    29,    30,    30,    31,
      32,    33,    33,    34,    34,    35,    35,    35,    35,    35,
      35,    35,    35,    36,    37,    37,    38,    38,    39,    40,
      40,    41,    41,    42,    42,    43
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     0,     4,     1,     3,     1,     3,     1,
       3,     1,     2,     1,     4,     1,     2,     1,     1,     1,
       1,     1,     3,     1,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     4,     2,     4,     2,     1,
       2,     1,     2,     1,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,    20,     0,     0,    45,     3,     3,     1,     2,     0,
       5,     3,    19,     3,    41,    23,    18,    17,     3,    21,
       0,    36,     0,     3,     9,    34,     0,    11,     3,     0,
       0,    43,    42,     0,     3,     0,     0,     7,     6,     3,
      30,    31,    27,    28,    29,    32,    26,    25,     3,    12,
       0,    13,     4,    37,     3,    22,    17,     0,    10,     3,
       0,    39,     3,    38,     3,    44,    24,     8,    35,    40,
       3,     0,    15,    14,    16
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     2,    14,     3,    11,    38,    23,    28,    52,    73,
      15,    16,    17,    18,    19,    48,    20,    24,     5,    49,
      63,    21,    32,     6
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -45
static const yysigned_char yypact[] =
{
       1,   -45,    12,    11,   -45,    19,    -3,   -45,   -45,    -1,
     -45,    14,   -45,     7,   -45,   -45,   -45,    13,    16,    20,
      18,   -45,     1,    21,   -45,    23,    59,   -45,    31,    24,
       7,   -45,   -45,     7,     7,    25,    59,   -45,   -45,     7,
     -45,   -45,   -45,   -45,   -45,   -45,   -45,   -45,    59,   -45,
      36,   -45,   -45,   -45,    16,   -45,   -45,    28,   -45,    21,
      30,   -45,    59,   -45,     7,   -45,   -45,   -45,   -45,   -45,
      34,    59,   -45,   -45,   -45
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -45,   -45,    -4,    27,   -45,   -12,   -45,   -45,   -45,   -45,
      26,    -6,     0,    22,   -45,   -44,   -45,   -45,   -45,   -33,
      -8,   -11,     3,    42
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -34
static const yysigned_char yytable[] =
{
       4,    10,    29,    59,    62,    12,     1,    27,     1,     4,
       1,    13,     7,    22,    31,    12,     1,    26,    62,    37,
      46,     8,     4,    57,    51,     9,    47,   -33,    60,    30,
      46,    33,    34,    56,    36,    50,    47,    39,    74,    53,
      58,    64,    46,    66,    61,    68,    71,    67,    47,    35,
      31,    25,    54,    70,    69,    37,    46,    65,    61,    55,
       0,     0,    47,     0,     0,    46,    72,    12,     1,     0,
       0,    47,     0,    40,    41,    42,    43,    44,    45
};

static const yysigned_char yycheck[] =
{
       0,     5,    13,    36,    48,     8,     9,    11,     9,     9,
       9,    14,     0,    14,    18,     8,     9,     3,    62,    23,
      26,    10,    22,    34,    28,     6,    26,    14,    39,    13,
      36,    11,    14,    33,    13,     4,    36,    14,    71,    15,
      15,     5,    48,    15,    48,    15,    12,    59,    48,    22,
      54,     9,    30,    64,    62,    59,    62,    54,    62,    33,
      -1,    -1,    62,    -1,    -1,    71,    70,     8,     9,    -1,
      -1,    71,    -1,    14,    15,    16,    17,    18,    19
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     9,    21,    23,    32,    38,    43,     0,    10,     6,
      22,    24,     8,    14,    22,    30,    31,    32,    33,    34,
      36,    41,    14,    26,    37,    43,     3,    22,    27,    41,
      13,    22,    42,    11,    14,    23,    13,    22,    25,    14,
      14,    15,    16,    17,    18,    19,    31,    32,    35,    39,
       4,    22,    28,    15,    33,    30,    32,    41,    15,    39,
      41,    22,    35,    40,     5,    42,    15,    25,    15,    40,
      41,    12,    22,    29,    39
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
#line 89 "grammer.y"
    { 
#ifdef DEBUG
	   	print_graph(yyvsp[-1].graph.head); 
#endif
		Graph = yyvsp[-1].graph.head;
		return 0;
	   }
    break;

  case 4:
#line 101 "grammer.y"
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
#line 113 "grammer.y"
    { yyval.graph.head = NULL; yyval.graph.tail = NULL; }
    break;

  case 6:
#line 115 "grammer.y"
    { 
		yyval.graph = yyvsp[-1].graph; 
	   	if (yyvsp[0].arglist) {
			graph_join(&(yyval.graph), newmodule("join", yyvsp[0].arglist));
		}
	   }
    break;

  case 7:
#line 123 "grammer.y"
    { yyval.arglist = NULL; }
    break;

  case 8:
#line 125 "grammer.y"
    {
		yyval.arglist = newarg(arglist2str(yyvsp[-1].arglist), 0, NULL);
	   	/* fprintf(stderr, "got a join (,) with %s\n", $$->arg); */

	   	if (yyvsp[0].arglist) {
			yyval.arglist = arglist_append(yyval.arglist, yyvsp[0].arglist);
		} 
	   }
    break;

  case 10:
#line 136 "grammer.y"
    { yyval.graph = yyvsp[-1].graph; }
    break;

  case 11:
#line 139 "grammer.y"
    { yyval.graph = nullgraph; }
    break;

  case 12:
#line 140 "grammer.y"
    { yyval.graph = newmodule("filter", yyvsp[0].arglist); }
    break;

  case 13:
#line 143 "grammer.y"
    { yyval.group.args = NULL; yyval.group.having = NULL;}
    break;

  case 14:
#line 144 "grammer.y"
    { yyval.group.args = yyvsp[-1].arglist; yyval.group.having = yyvsp[0].arglist; }
    break;

  case 15:
#line 147 "grammer.y"
    { yyval.arglist = NULL; }
    break;

  case 16:
#line 148 "grammer.y"
    { yyval.arglist = yyvsp[0].arglist; }
    break;

  case 19:
#line 155 "grammer.y"
    { yyval.string = yystring; }
    break;

  case 20:
#line 157 "grammer.y"
    { yyval.string = yystring; }
    break;

  case 22:
#line 160 "grammer.y"
    { yyval.arglist->rename = yyvsp[0].string; }
    break;

  case 23:
#line 163 "grammer.y"
    { yyval.arglist = newarg(yyvsp[0].string, 0, NULL); }
    break;

  case 24:
#line 164 "grammer.y"
    { yyval.arglist = newarg(yyvsp[-3].string, 1, yyvsp[-1].arglist); }
    break;

  case 25:
#line 167 "grammer.y"
    { yyval.arglist = newarg(yyvsp[0].string, 0, NULL); }
    break;

  case 26:
#line 169 "grammer.y"
    { 
	    	char * str = malloc(sizeof(char *) * (strlen(yyvsp[0].string)+2)); 
		sprintf(str,"\"%s\"", yyvsp[0].string); 
		yyval.arglist = newarg(str, 0, NULL); 
	    }
    break;

  case 27:
#line 174 "grammer.y"
    { yyval.arglist = newarg("<", 0, NULL); }
    break;

  case 28:
#line 175 "grammer.y"
    { yyval.arglist = newarg(">", 0, NULL); }
    break;

  case 29:
#line 176 "grammer.y"
    { yyval.arglist = newarg("=", 0, NULL); }
    break;

  case 30:
#line 177 "grammer.y"
    { yyval.arglist = newarg("(", 0, NULL); }
    break;

  case 31:
#line 178 "grammer.y"
    { yyval.arglist = newarg(")", 0, NULL); }
    break;

  case 32:
#line 179 "grammer.y"
    { yyval.arglist = newarg("!", 0, NULL); }
    break;

  case 34:
#line 185 "grammer.y"
    { yyval.graph = newmodule(yyvsp[0].string, NULL); }
    break;

  case 35:
#line 186 "grammer.y"
    { yyval.graph = newmodule(yyvsp[-3].string, yyvsp[-1].arglist); }
    break;

  case 36:
#line 189 "grammer.y"
    { yyval.vphrase = newvphrase(yyvsp[-1].string, yyvsp[0].arglist); }
    break;

  case 37:
#line 190 "grammer.y"
    { yyval.vphrase = newvphrase(yyvsp[-3].string, yyvsp[-1].arglist); }
    break;

  case 38:
#line 193 "grammer.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 39:
#line 196 "grammer.y"
    { yyval.arglist = NULL; }
    break;

  case 40:
#line 197 "grammer.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 41:
#line 200 "grammer.y"
    { yyval.arglist = NULL; }
    break;

  case 42:
#line 201 "grammer.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 43:
#line 204 "grammer.y"
    { yyval.arglist = NULL; }
    break;

  case 44:
#line 205 "grammer.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 1253 "grammer.c"

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


#line 211 "grammer.y"



struct filter * smacq_build_query(int argc, char ** argv) {
  int size = 0;
  int i;
  char * qstr; 
  struct filter * graph;
  int res;

  for (i=0; i<argc; i++) {
  	size += strlen(argv[i]);
  }
  size += argc;

  qstr = (char*)malloc(size);
  	
  for (i=0; i<argc; i++) {
  	strcat(qstr, argv[i]);
  	strcat(qstr, " ");
  }

  /* LOCK */

  yy_scan_string(qstr);
  /* fprintf(stderr, "parsing buffer: %s\n", qstr); */

  res = yyparse();

  graph = Graph;

  /* UNLOCK */

  if (res) {
  	return NULL;
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
	int len = 0;

	for(al=alist; al; al=al->next) 
		len += strlen(al->arg) + 1;

	argstr = calloc(1, sizeof(char *) * len + 1);

	for(al=alist; al; al=al->next) {
		strcat(argstr, al->arg);
		strcat(argstr, " ");
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
	
static void graph_append(struct graph * graph, struct filter * newmod) {
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
#if 0
	atail = arglist_append(atail, newarg(vphrase.verb, 0, NULL));
	atail = arglist_append(atail, vphrase.args);

	g = newmodule("groupby", group.args);

	if (group.having) 
		graph_join(&g, newmodule("filter", group.having));
#else
	if (argcont) 
		g = newmodule("groupby", group.args);

	if (group.having) 
		graph_join(&g, newmodule("filter", group.having));

	graph_join(&g, newmodule(vphrase.verb, vphrase.args));
#endif

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

void print_graph(struct filter * f) {
	int i;
	if (!f) return;

	printf("Graph node %s (%p):\n", f->name, f);
	for (i=0; i<f->argc; i++) {
		printf("\tArgument %s\n", f->argv[i]);
	}
	for (i=0; i<f->numchildren; i++) {
		printf("\tChild %d is %s (%p)\n", i, f->next[i]->name, f->next[i]);
		print_graph(f->next[i]);
	}
}



