/* A Bison parser, made by GNU Bison 1.875a.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

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
     AS = 263,
     HAVING = 264,
     UNION = 265,
     YYSTRING = 266,
     YYID = 267,
     YYNUMBER = 268,
     YYNEQ = 269,
     YYLEQ = 270,
     YYGEQ = 271,
     YYSTOP = 272,
     YYLIKE = 273,
     YYOR = 274,
     YYAND = 275,
     YYNOT = 276
   };
#endif
#define WHERE 258
#define GROUP 259
#define BY 260
#define FROM 261
#define SELECT 262
#define AS 263
#define HAVING 264
#define UNION 265
#define YYSTRING 266
#define YYID 267
#define YYNUMBER 268
#define YYNEQ 269
#define YYLEQ 270
#define YYGEQ 271
#define YYSTOP 272
#define YYLIKE 273
#define YYOR 274
#define YYAND 275
#define YYNOT 276




/* Copy the first part of user declarations.  */
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

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 99 "filter-parser.y"
typedef union YYSTYPE {
  struct graph graph;
  struct arglist * arglist;
  struct vphrase vphrase;
  char * string;
  struct group group;
  dts_compare_operation op;
  enum dts_arith_operand_type arithop;
  dts_comparison * comp;
  struct dts_operand * operand;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 179 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 191 "y.tab.c"

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
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

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
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
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
#define YYFINAL  38
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   116

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  34
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  21
/* YYNRULES -- Number of rules. */
#define YYNRULES  50
/* YYNRULES -- Number of states. */
#define YYNSTATES  78

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   276

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    10,    12,    14,    16,
      18,    20,    22,    26,    28,    33,    37,    39,    44,    46,
      48,    51,    53,    57,    59,    63,    67,    71,    74,    76,
      78,    80,    82,    86,    90,    92,    94,    96,   100,   105,
     107,   109,   111,   113,   115,   117,   119,   121,   123,   125,
     127
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      54,     0,    -1,    -1,    39,    -1,    38,    -1,    37,    -1,
      13,    -1,    11,    -1,    12,    -1,    19,    -1,    20,    -1,
      41,    -1,    41,     8,    36,    -1,    36,    -1,    42,    26,
      43,    27,    -1,    28,    49,    29,    -1,    39,    -1,    40,
      30,    40,    45,    -1,    44,    -1,    35,    -1,    40,    44,
      -1,    35,    -1,    30,    40,    45,    -1,    39,    -1,    26,
      47,    27,    -1,    47,    19,    47,    -1,    47,    20,    47,
      -1,    21,    47,    -1,    51,    -1,    39,    -1,    38,    -1,
      37,    -1,    26,    49,    27,    -1,    50,    53,    50,    -1,
      49,    -1,    48,    -1,    48,    -1,    50,    52,    50,    -1,
      46,    26,    43,    27,    -1,    31,    -1,    32,    -1,    33,
      -1,    16,    -1,    15,    -1,    14,    -1,    18,    -1,    22,
      -1,    23,    -1,    25,    -1,    24,    -1,    47,    17,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,   112,   112,   115,   116,   117,   120,   123,   126,   127,
     128,   131,   132,   135,   136,   137,   142,   145,   146,   149,
     150,   153,   154,   157,   164,   165,   166,   167,   168,   171,
     172,   173,   178,   182,   189,   190,   194,   195,   196,   203,
     204,   205,   206,   207,   208,   209,   212,   213,   214,   215,
     219
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "WHERE", "GROUP", "BY", "FROM", "SELECT", 
  "AS", "HAVING", "UNION", "YYSTRING", "YYID", "YYNUMBER", "YYNEQ", 
  "YYLEQ", "YYGEQ", "YYSTOP", "YYLIKE", "YYOR", "YYAND", "YYNOT", "'+'", 
  "'-'", "'*'", "'/'", "'('", "')'", "'['", "']'", "','", "'='", "'>'", 
  "'<'", "$accept", "null", "word", "number", "string", "id", "arg", 
  "argument", "function", "args", "spacedargs", "moreargs", "verb", 
  "boolean", "operand", "expression", "subexpression", "test", "op", 
  "arithop", "booleanline", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,    43,    45,    42,    47,    40,    41,    91,    93,
      44,    61,    62,    60
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    34,    35,    36,    36,    36,    37,    38,    39,    39,
      39,    40,    40,    41,    41,    41,    42,    43,    43,    44,
      44,    45,    45,    46,    47,    47,    47,    47,    47,    48,
      48,    48,    49,    49,    50,    50,    51,    51,    51,    52,
      52,    52,    52,    52,    52,    52,    53,    53,    53,    53,
      54
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     4,     3,     1,     4,     1,     1,
       2,     1,     3,     1,     3,     3,     3,     2,     1,     1,
       1,     1,     3,     3,     1,     1,     1,     3,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     7,     8,     6,     9,    10,     0,     0,    31,    30,
      29,     0,     0,    35,    34,     0,    28,     0,    27,     0,
      34,     2,    50,     0,     0,    44,    43,    42,    45,    46,
      47,    49,    48,    39,    40,    41,     0,     0,     1,    24,
      32,     0,    19,    13,     5,     4,     3,     2,    11,     0,
       0,    18,    25,    26,     0,    29,    35,    37,    33,    34,
       0,     0,     2,    20,     0,     2,    38,    15,     2,    12,
       3,     0,     0,    21,    17,    14,     2,    22
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,    42,    43,     8,     9,    46,    47,    48,    49,    50,
      51,    74,    11,    12,    13,    14,    15,    16,    36,    37,
      17
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -50
static const yysigned_char yypact[] =
{
      76,   -50,   -50,   -50,   -50,   -50,    76,    76,   -50,   -50,
     -17,    -5,    90,    81,   -50,    47,   -50,    18,   -50,    20,
      -2,    55,   -50,    76,    76,   -50,   -50,   -50,   -50,   -50,
     -50,   -50,   -50,   -50,   -50,   -50,    73,    73,   -50,   -50,
     -50,    73,   -50,   -50,   -50,   -50,     9,    -8,    21,    12,
       6,   -50,   -50,   -50,    73,   -50,   -50,    91,    91,    29,
      91,    55,    55,   -50,    92,    55,   -50,   -50,    52,   -50,
     -50,    23,    55,   -50,   -50,   -50,    52,   -50
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -50,   -49,    26,   -19,   -13,     0,   -46,   -50,   -50,    41,
     -30,    15,   -50,     7,    19,     3,    40,   -50,   -50,   -50,
     -50
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -37
static const yysigned_char yytable[] =
{
      10,    62,    44,     1,     2,     3,    10,    10,    45,   -23,
      20,     4,     5,    18,    19,    68,    62,    63,    38,    73,
      41,    21,    61,    10,    10,    40,    76,    73,    44,    64,
      52,    53,    63,    66,    45,   -16,    55,    55,    65,    23,
      24,    55,    44,    44,    59,    44,    44,    39,    45,    45,
      75,    45,    45,    44,    55,    56,    56,    20,    67,    45,
      56,    25,    26,    27,    70,    28,     1,     2,     3,    29,
      30,    31,    32,    56,     4,     5,    57,    58,    33,    34,
      35,    60,    72,    41,     1,     2,     3,     1,     2,     3,
      69,    77,     4,     5,    60,     4,     5,     6,   -36,    54,
     -36,   -36,     7,     1,     2,     3,    71,    22,   -36,    23,
      24,     4,     5,    29,    30,    31,    32
};

static const unsigned char yycheck[] =
{
       0,    47,    21,    11,    12,    13,     6,     7,    21,    26,
       7,    19,    20,     6,     7,    61,    62,    47,     0,    68,
      28,    26,    30,    23,    24,    27,    72,    76,    47,     8,
      23,    24,    62,    27,    47,    26,    36,    37,    26,    19,
      20,    41,    61,    62,    41,    64,    65,    27,    61,    62,
      27,    64,    65,    72,    54,    36,    37,    54,    29,    72,
      41,    14,    15,    16,    64,    18,    11,    12,    13,    22,
      23,    24,    25,    54,    19,    20,    36,    37,    31,    32,
      33,    41,    30,    28,    11,    12,    13,    11,    12,    13,
      64,    76,    19,    20,    54,    19,    20,    21,    17,    26,
      19,    20,    26,    11,    12,    13,    65,    17,    27,    19,
      20,    19,    20,    22,    23,    24,    25
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    11,    12,    13,    19,    20,    21,    26,    37,    38,
      39,    46,    47,    48,    49,    50,    51,    54,    47,    47,
      49,    26,    17,    19,    20,    14,    15,    16,    18,    22,
      23,    24,    25,    31,    32,    33,    52,    53,     0,    27,
      27,    28,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    47,    47,    26,    39,    48,    50,    50,    49,
      50,    30,    40,    44,     8,    26,    27,    29,    40,    36,
      39,    43,    30,    35,    45,    27,    40,    45
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
#define YYEMPTY		(-2)
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
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
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
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
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
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

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

  if (yyss + yystacksize - 1 <= yyssp)
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
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
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

      if (yyss + yystacksize - 1 <= yyssp)
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
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
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
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 6:
#line 120 "filter-parser.y"
    { yyval.string = yystring; }
    break;

  case 7:
#line 123 "filter-parser.y"
    { yyval.string = yystring; }
    break;

  case 8:
#line 126 "filter-parser.y"
    { yyval.string = yystring; }
    break;

  case 9:
#line 127 "filter-parser.y"
    { yyval.string = "or"; }
    break;

  case 10:
#line 128 "filter-parser.y"
    { yyval.string = "and"; }
    break;

  case 12:
#line 132 "filter-parser.y"
    { yyval.arglist->rename = yyvsp[0].string; }
    break;

  case 13:
#line 135 "filter-parser.y"
    { yyval.arglist = newarg(yyvsp[0].string, WORD, NULL); }
    break;

  case 14:
#line 136 "filter-parser.y"
    { yyval.arglist = newarg(yyvsp[-3].string, FUNCTION, yyvsp[-1].arglist); }
    break;

  case 15:
#line 137 "filter-parser.y"
    { yyval.arglist = newarg("expr", FUNCTION, 
					       newarg(print_operand(yyvsp[-1].operand), WORD, NULL)); 
			                  }
    break;

  case 17:
#line 145 "filter-parser.y"
    { yyval.arglist = yyvsp[-3].arglist; yyval.arglist->next = yyvsp[-1].arglist; yyvsp[-1].arglist->next = yyvsp[0].arglist; }
    break;

  case 19:
#line 149 "filter-parser.y"
    { yyval.arglist = NULL; }
    break;

  case 20:
#line 150 "filter-parser.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 21:
#line 153 "filter-parser.y"
    { yyval.arglist = NULL; }
    break;

  case 22:
#line 154 "filter-parser.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 24:
#line 164 "filter-parser.y"
    { yyval.comp = yyvsp[-1].comp; }
    break;

  case 25:
#line 165 "filter-parser.y"
    { yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, OR); }
    break;

  case 26:
#line 166 "filter-parser.y"
    { yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, AND); }
    break;

  case 27:
#line 167 "filter-parser.y"
    { yyval.comp = comp_join(yyvsp[0].comp, NULL, NOT); }
    break;

  case 29:
#line 171 "filter-parser.y"
    { yyval.operand = comp_operand(FIELD, yyvsp[0].string); }
    break;

  case 30:
#line 172 "filter-parser.y"
    { yyval.operand = comp_operand(CONST, yyvsp[0].string); }
    break;

  case 31:
#line 173 "filter-parser.y"
    { yyval.operand = comp_operand(CONST, yyvsp[0].string); }
    break;

  case 32:
#line 179 "filter-parser.y"
    {	  
				  yyval.operand = yyvsp[-1].operand;  
				}
    break;

  case 33:
#line 183 "filter-parser.y"
    {
				  yyval.operand = comp_arith(parse_tenv, yyvsp[-1].arithop, yyvsp[-2].operand, yyvsp[0].operand); 
				}
    break;

  case 36:
#line 194 "filter-parser.y"
    {  yyval.comp = comp_new(EXIST, yyvsp[0].operand, yyvsp[0].operand); }
    break;

  case 37:
#line 195 "filter-parser.y"
    { yyval.comp = comp_new(yyvsp[-1].op, yyvsp[-2].operand, yyvsp[0].operand); }
    break;

  case 38:
#line 196 "filter-parser.y"
    { 
				  int argc; char ** argv;
				  arglist2argv(yyvsp[-1].arglist, &argc, &argv);
				  yyval.comp = comp_new_func(yyvsp[-3].string, argc, argv, yyvsp[-1].arglist);
				}
    break;

  case 39:
#line 203 "filter-parser.y"
    { yyval.op = EQ; }
    break;

  case 40:
#line 204 "filter-parser.y"
    { yyval.op = GT; }
    break;

  case 41:
#line 205 "filter-parser.y"
    { yyval.op = LT; }
    break;

  case 42:
#line 206 "filter-parser.y"
    { yyval.op = GEQ; }
    break;

  case 43:
#line 207 "filter-parser.y"
    { yyval.op = LEQ; }
    break;

  case 44:
#line 208 "filter-parser.y"
    { yyval.op = NEQ; }
    break;

  case 45:
#line 209 "filter-parser.y"
    { yyval.op = LIKE; }
    break;

  case 46:
#line 212 "filter-parser.y"
    { yyval.arithop = ADD; }
    break;

  case 47:
#line 213 "filter-parser.y"
    { yyval.arithop = SUB; }
    break;

  case 48:
#line 214 "filter-parser.y"
    { yyval.arithop = DIVIDE; }
    break;

  case 49:
#line 215 "filter-parser.y"
    { yyval.arithop = MULT; }
    break;

  case 50:
#line 219 "filter-parser.y"
    {
					Comp = yyvsp[-1].comp;
					return 0;
				}
    break;


    }

/* Line 999 of yacc.c.  */
#line 1345 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

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
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
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
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



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
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
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


#line 225 "filter-parser.y"


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





