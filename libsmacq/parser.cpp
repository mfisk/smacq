/* A Bison parser, made from /Users/mfisk/cvs/smacq/libsmacq/parser.ypp
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

#line 3 "parser.ypp"

void yysmacql_error(char*);
     #define    yymaxdepth yysmacql_maxdepth
     #define    yyparse yysmacql_parse
     #define    yylex   yysmacql_lex
     #define    yyerror yysmacql_error
     #define    yylval  yysmacql_lval
     #define    yychar  yysmacql_char
     #define    yydebug yysmacql_debug
     #define    yypact  yysmacql_pact
     #define    yyr1    yysmacql_r1
     #define    yyr2    yysmacql_r2
     #define    yydef   yysmacql_def
     #define    yychk   yysmacql_chk
     #define    yypgo   yysmacql_pgo
     #define    yyact   yysmacql_act
     #define    yyexca  yysmacql_exca
     #define yyerrflag yysmacql_errflag
     #define yynerrs    yysmacql_nerrs
     #define    yyps    yysmacql_ps
     #define    yypv    yysmacql_pv
     #define    yys     yysmacql_s
     #define    yy_yys  yysmacql_yys
     #define    yystate yysmacql_state
     #define    yytmp   yysmacql_tmp
     #define    yyv     yysmacql_v
     #define    yy_yyv  yysmacql_yyv
     #define    yyval   yysmacql_val
     #define    yylloc  yysmacql_lloc
     #define yyreds     yysmacql_reds
     #define yytoks     yysmacql_toks
     #define yylhs      yysmacql_yylhs
     #define yylen      yysmacql_yylen
     #define yydefred yysmacql_yydefred
     #define yydgoto    yysmacql_yydgoto
     #define yysindex yysmacql_yysindex
     #define yyrindex yysmacql_yyrindex
     #define yygindex yysmacql_yygindex
     #define yytable     yysmacql_yytable
     #define yycheck     yysmacql_yycheck
     #define yyname   yysmacql_yyname
     #define yyrule   yysmacql_yyrule

#include <smacq-parser.h>
#include <SmacqGraph.h>

static SmacqGraph * nullgraph = NULL;
SmacqGraph * Graph;
extern int yylex();
#line 79 "parser.ypp"

#include <smacq-parser.h>

#line 109 "parser.ypp"
#ifndef YYSTYPE
typedef union {
  SmacqGraph * graph;
  struct arglist * arglist;
  struct vphrase vphrase;
  char * string;
  struct group group;
  dts_compare_operation op;
  enum dts_arith_operand_type arithop;
  dts_comparison * comp;
  struct dts_operand * operand;
  joinlist * join_list;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		137
#define	YYFLAG		-32768
#define	YYNTBASE	35

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 275 ? yytranslate[x] : 65)

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
       2,     2,     2,     2,    34,     2,     2,     2,     2,     2,
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
     106,   108,   110,   112,   114,   116,   118,   120,   122,   125,
     129,   132,   138,   141,   144,   151,   153,   156,   160,   165,
     167,   169,   173,   175,   178,   180,   185,   187,   190,   192,
     195,   199,   204,   206,   212,   214
};
static const short yyrhs[] =
{
      -1,    39,     0,    38,     0,    37,     0,    13,     0,    11,
       0,    12,     0,    19,     0,    20,     0,    41,     0,    41,
       8,    36,     0,    36,     0,    42,    26,    43,    27,     0,
      28,    48,    29,     0,    39,     0,    40,    30,    40,    45,
       0,    44,     0,    35,     0,    40,    44,     0,    35,     0,
      30,    40,    45,     0,    26,    46,    27,     0,    46,    19,
      46,     0,    46,    20,    46,     0,    21,    46,     0,    50,
       0,    39,     0,    38,     0,    37,     0,    26,    48,    27,
       0,    49,    52,    49,     0,    48,     0,    47,     0,    47,
       0,    49,    51,    49,     0,    42,    26,    43,    27,     0,
      31,     0,    32,     0,    33,     0,    16,     0,    15,     0,
      14,     0,    18,     0,    22,     0,    23,     0,    25,     0,
      24,     0,    54,    17,     0,    62,    55,    60,     0,    62,
      59,     0,    54,    34,    62,    59,    60,     0,     3,    46,
       0,    58,    59,     0,     6,    63,    36,    30,    56,    59,
       0,    36,     0,    63,    36,     0,    36,    30,    56,     0,
      63,    36,    30,    56,     0,    58,     0,    35,     0,     6,
      62,    57,     0,    35,     0,     3,    46,     0,    35,     0,
       4,     5,    43,    61,     0,    35,     0,     9,    46,     0,
      63,     0,    42,    43,     0,    26,    54,    27,     0,    42,
      26,    43,    27,     0,    42,     0,    26,    63,    22,    64,
      27,     0,    63,     0,    63,    22,    64,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   123,   126,   127,   128,   131,   134,   137,   138,   139,
     142,   143,   146,   147,   148,   155,   158,   159,   162,   163,
     166,   167,   175,   176,   177,   178,   179,   182,   183,   184,
     188,   193,   199,   201,   204,   206,   207,   214,   215,   216,
     217,   218,   219,   220,   223,   224,   225,   226,   230,   240,
     250,   255,   265,   268,   274,   282,   287,   291,   296,   303,
     305,   308,   316,   318,   321,   323,   326,   328,   331,   332,
     335,   337,   338,   339,   346,   348
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
  "'<'", "'|'", "null", "word", "number", "string", "id", "arg", 
  "argument", "function", "args", "spacedargs", "moreargs", "boolean", 
  "operand", "expression", "subexpression", "test", "op", "arithop", 
  "queryline", "query", "fromwhere", "joins", "from0", "from", "where", 
  "group", "having", "action", "parenquery", "moreparenquery", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    35,    36,    36,    36,    37,    38,    39,    39,    39,
      40,    40,    41,    41,    41,    42,    43,    43,    44,    44,
      45,    45,    46,    46,    46,    46,    46,    47,    47,    47,
      48,    48,    49,    49,    50,    50,    50,    51,    51,    51,
      51,    51,    51,    51,    52,    52,    52,    52,    53,    54,
      54,    54,    54,    55,    55,    56,    56,    56,    56,    57,
      57,    58,    59,    59,    60,    60,    61,    61,    62,    62,
      63,    63,    63,    63,    64,    64
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     4,     3,     1,     4,     1,     1,     2,
       1,     3,     3,     3,     3,     2,     1,     1,     1,     1,
       3,     3,     1,     1,     1,     3,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     3,
       2,     5,     2,     2,     6,     1,     2,     3,     4,     1,
       1,     3,     1,     2,     1,     4,     1,     2,     1,     2,
       3,     4,     1,     5,     1,     3
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,     0,     7,     8,     9,     0,    15,     1,     0,     1,
      68,     6,     5,     0,     0,    29,    28,    27,     0,    52,
      33,    32,     0,    26,     0,    68,     1,     0,    18,    12,
       4,     3,     2,     1,    10,     0,    69,    17,    48,     0,
       0,     0,    62,     1,     1,    50,    25,     0,    32,     1,
       0,     0,    42,    41,    40,    43,    44,    45,    47,    46,
      37,    38,    39,     0,     0,    70,     0,     0,     0,    27,
      33,    32,     0,     0,     1,    19,     0,     1,     1,    63,
       1,    68,     0,    64,    49,    53,    22,    30,     0,    23,
      24,    35,    31,    72,    74,     0,    71,    14,     1,    11,
       2,     0,     1,     0,    60,    61,    59,     0,     1,    36,
       0,    73,     0,    20,    16,    13,    51,     0,     1,    75,
       1,    55,     1,     0,     0,    66,    65,    21,     0,    54,
      56,    67,    57,     0,    58,     0,     0,     0
};

static const short yydefgoto[] =
{
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
     114,    19,    20,    21,    22,    23,    63,    64,   135,     8,
      43,   122,   105,    44,    45,    84,   126,     9,    10,    95
};

static const short yypact[] =
{
     156,   227,-32768,-32768,-32768,   156,-32768,   203,    -5,    31,
  -32768,-32768,-32768,   227,   227,-32768,-32768,    28,    36,    27,
     147,-32768,   176,-32768,   -24,     1,   215,   238,-32768,-32768,
  -32768,-32768,    28,   191,    20,    39,-32768,-32768,-32768,   117,
     227,   117,-32768,    71,    32,-32768,-32768,     5,    16,   215,
     227,   227,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,   238,   238,-32768,   117,    49,   238,-32768,
  -32768,    50,   248,   215,   215,-32768,   167,   215,    32,    27,
      72,   167,    83,-32768,-32768,-32768,-32768,-32768,    63,-32768,
  -32768,   248,   248,    65,    70,    69,-32768,-32768,    67,-32768,
  -32768,    74,    71,   117,-32768,-32768,-32768,    79,   215,-32768,
     117,-32768,   215,-32768,-32768,-32768,-32768,   243,    90,-32768,
      67,    82,    32,   167,   227,-32768,-32768,-32768,   243,-32768,
      85,    27,-32768,   243,-32768,   102,   104,-32768
};

static const short yypgoto[] =
{
      75,   -17,    81,   121,    17,   -25,-32768,     0,   -22,   -13,
     -15,     2,     6,    -8,     9,-32768,-32768,-32768,-32768,   108,
  -32768,  -107,-32768,    43,   -33,    37,-32768,   -32,    -3,    40
};


#define	YYLAST		273


static const short yytable[] =
{
       7,    18,    25,    65,    67,     7,    48,    78,    74,    80,
      39,    85,    38,    18,    18,    46,    47,     6,    17,    71,
      75,   132,     6,    66,    50,    51,   134,    88,    76,    39,
      17,    17,    86,    70,    40,    40,    72,    41,    81,     7,
      18,     7,    79,    87,    69,   102,    50,    51,    98,    74,
      18,    18,    89,    90,   -15,   101,     6,    17,     6,    99,
      48,    75,    49,    94,   107,    77,    93,    17,    17,    70,
      70,    80,    91,    92,    70,    82,    96,    72,   103,    97,
      69,    69,    15,     6,    42,    69,   118,   120,   108,   129,
     109,    26,   110,   100,    15,    15,   111,   112,   100,   124,
     121,   115,   136,     7,   137,   127,   130,    94,    15,   117,
      93,   121,   128,    24,   123,   133,   121,    93,    83,    42,
       6,    15,    16,   106,    18,   123,   131,     6,    93,     2,
     123,    15,    15,    93,    16,    16,     3,     4,     0,   116,
     100,    17,     0,     5,    15,    15,     0,     0,    16,    15,
     119,   -34,     0,    42,     0,   104,     0,     0,     0,     1,
       0,    16,     0,     0,   -34,     0,   -34,   -34,     2,     0,
       0,    16,    16,   113,   -34,     3,     4,    83,    11,     2,
      12,   -34,     5,     0,    16,    16,     3,     4,     0,    16,
      52,    53,    54,   125,    55,   113,     0,    42,    56,    57,
      58,    59,    11,     2,    12,    15,     0,    60,    61,    62,
       3,     4,     0,     0,    11,     2,    12,     0,     0,    27,
       0,    73,     3,     4,     0,   -72,    11,     2,    12,    26,
       0,    27,     0,     0,     3,     4,     0,     0,    11,     2,
      12,     0,     0,    27,     0,    16,     3,     4,    13,    11,
       2,    12,     0,    14,    11,     2,    12,     3,     4,     0,
       0,     0,     3,     4,    68,     0,     0,     0,     0,     5,
      56,    57,    58,    59
};

static const short yycheck[] =
{
       0,     1,     5,    27,    26,     5,    14,    39,    33,    41,
      34,    44,    17,    13,    14,    13,    14,     0,     1,    27,
      33,   128,     5,    22,    19,    20,   133,    49,     8,    34,
      13,    14,    27,    27,     3,     3,    27,     6,    41,    39,
      40,    41,    40,    27,    27,    78,    19,    20,    73,    74,
      50,    51,    50,    51,    26,    77,    39,    40,    41,    76,
      68,    74,    26,    66,    81,    26,    66,    50,    51,    63,
      64,   103,    63,    64,    68,     4,    27,    68,     6,    29,
      63,    64,     1,    66,     9,    68,   108,   112,     5,   122,
      27,    26,    22,    76,    13,    14,    27,    30,    81,     9,
     117,    27,     0,   103,     0,   120,   123,   110,    27,    30,
     110,   128,    30,     5,   117,    30,   133,   117,    43,    44,
     103,    40,     1,    80,   124,   128,   124,   110,   128,    12,
     133,    50,    51,   133,    13,    14,    19,    20,    -1,   102,
     123,   124,    -1,    26,    63,    64,    -1,    -1,    27,    68,
     110,     4,    -1,    78,    -1,    80,    -1,    -1,    -1,     3,
      -1,    40,    -1,    -1,    17,    -1,    19,    20,    12,    -1,
      -1,    50,    51,    98,    27,    19,    20,   102,    11,    12,
      13,    34,    26,    -1,    63,    64,    19,    20,    -1,    68,
      14,    15,    16,   118,    18,   120,    -1,   122,    22,    23,
      24,    25,    11,    12,    13,   124,    -1,    31,    32,    33,
      19,    20,    -1,    -1,    11,    12,    13,    -1,    -1,    28,
      -1,    30,    19,    20,    -1,    22,    11,    12,    13,    26,
      -1,    28,    -1,    -1,    19,    20,    -1,    -1,    11,    12,
      13,    -1,    -1,    28,    -1,   124,    19,    20,    21,    11,
      12,    13,    -1,    26,    11,    12,    13,    19,    20,    -1,
      -1,    -1,    19,    20,    26,    -1,    -1,    -1,    -1,    26,
      22,    23,    24,    25
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
#line 131 "parser.ypp"
{ yyval.string = yystring; }
    break;
case 6:
#line 134 "parser.ypp"
{ yyval.string = yystring; }
    break;
case 7:
#line 137 "parser.ypp"
{ yyval.string = yystring; }
    break;
case 8:
#line 138 "parser.ypp"
{ yyval.string = "or"; }
    break;
case 9:
#line 139 "parser.ypp"
{ yyval.string = "and"; }
    break;
case 11:
#line 143 "parser.ypp"
{ yyval.arglist->rename = yyvsp[0].string; }
    break;
case 12:
#line 146 "parser.ypp"
{ yyval.arglist = newarg(yyvsp[0].string, WORD, NULL); }
    break;
case 13:
#line 147 "parser.ypp"
{ yyval.arglist = newarg(yyvsp[-3].string, FUNCTION, yyvsp[-1].arglist); }
    break;
case 14:
#line 149 "parser.ypp"
{ 
		yyval.arglist = newarg("expr", FUNCTION, 
		newarg(print_operand(yyvsp[-1].operand), WORD, NULL)); 
	   }
    break;
case 16:
#line 158 "parser.ypp"
{ yyval.arglist = yyvsp[-3].arglist; yyval.arglist->next = yyvsp[-1].arglist; yyvsp[-1].arglist->next = yyvsp[0].arglist; }
    break;
case 18:
#line 162 "parser.ypp"
{ yyval.arglist = NULL; }
    break;
case 19:
#line 163 "parser.ypp"
{ yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;
case 20:
#line 166 "parser.ypp"
{ yyval.arglist = NULL; }
    break;
case 21:
#line 167 "parser.ypp"
{ yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;
case 22:
#line 175 "parser.ypp"
{ yyval.comp = yyvsp[-1].comp; }
    break;
case 23:
#line 176 "parser.ypp"
{ yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, OR); }
    break;
case 24:
#line 177 "parser.ypp"
{ yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, AND); }
    break;
case 25:
#line 178 "parser.ypp"
{ yyval.comp = comp_join(yyvsp[0].comp, NULL, NOT); }
    break;
case 27:
#line 182 "parser.ypp"
{ yyval.operand = comp_operand(FIELD, yyvsp[0].string); }
    break;
case 28:
#line 183 "parser.ypp"
{ yyval.operand = comp_operand(CONST, yyvsp[0].string); }
    break;
case 29:
#line 184 "parser.ypp"
{ yyval.operand = comp_operand(CONST, yyvsp[0].string); }
    break;
case 30:
#line 190 "parser.ypp"
{	  
				  yyval.operand = yyvsp[-1].operand;  
				}
    break;
case 31:
#line 194 "parser.ypp"
{
				  yyval.operand = parse_dts->comp_arith(yyvsp[-1].arithop, yyvsp[-2].operand, yyvsp[0].operand); 
				}
    break;
case 34:
#line 205 "parser.ypp"
{  yyval.comp = comp_new(EXIST, yyvsp[0].operand, yyvsp[0].operand); }
    break;
case 35:
#line 206 "parser.ypp"
{ yyval.comp = comp_new(yyvsp[-1].op, yyvsp[-2].operand, yyvsp[0].operand); }
    break;
case 36:
#line 207 "parser.ypp"
{ 
				  int argc; char ** argv;
				  arglist2argv(yyvsp[-1].arglist, &argc, &argv);
				  yyval.comp = comp_new_func(yyvsp[-3].string, argc, argv, yyvsp[-1].arglist);
				}
    break;
case 37:
#line 214 "parser.ypp"
{ yyval.op = EQ; }
    break;
case 38:
#line 215 "parser.ypp"
{ yyval.op = GT; }
    break;
case 39:
#line 216 "parser.ypp"
{ yyval.op = LT; }
    break;
case 40:
#line 217 "parser.ypp"
{ yyval.op = GEQ; }
    break;
case 41:
#line 218 "parser.ypp"
{ yyval.op = LEQ; }
    break;
case 42:
#line 219 "parser.ypp"
{ yyval.op = NEQ; }
    break;
case 43:
#line 220 "parser.ypp"
{ yyval.op = LIKE; }
    break;
case 44:
#line 223 "parser.ypp"
{ yyval.arithop = ADD; }
    break;
case 45:
#line 224 "parser.ypp"
{ yyval.arithop = SUB; }
    break;
case 46:
#line 225 "parser.ypp"
{ yyval.arithop = DIVIDE; }
    break;
case 47:
#line 226 "parser.ypp"
{ yyval.arithop = MULT; }
    break;
case 48:
#line 231 "parser.ypp"
{ 
#ifdef DEBUG
	   	yyvsp[-1].graph->print(stderr, 0); 
#endif
		Graph = yyvsp[-1].graph;
		return 0;
	   }
    break;
case 49:
#line 242 "parser.ypp"
{
	   	yyval.graph = yyvsp[-1].graph;
		if (yyvsp[0].group.args) {
			graph_join(&(yyval.graph), newgroup(yyvsp[0].group, yyvsp[-2].graph));
		} else {
			graph_join(&(yyval.graph), yyvsp[-2].graph);
		}
	   }
    break;
case 50:
#line 251 "parser.ypp"
{
	   	yyval.graph = yyvsp[0].graph;
		graph_join(&(yyval.graph), yyvsp[-1].graph);
	   }
    break;
case 51:
#line 256 "parser.ypp"
{
	   	yyval.graph = yyvsp[-4].graph;
		graph_join(&(yyval.graph), yyvsp[-1].graph);
		if (yyvsp[0].group.args) {
			graph_join(&(yyval.graph), newgroup(yyvsp[0].group, yyvsp[-2].graph));
		} else {
			graph_join(&(yyval.graph), yyvsp[-2].graph);
		}
	   }
    break;
case 52:
#line 265 "parser.ypp"
{ yyval.graph = optimize_bools(yyvsp[0].comp); }
    break;
case 53:
#line 270 "parser.ypp"
{ 
		yyval.graph=yyvsp[-1].graph; 
		graph_join(&(yyval.graph), yyvsp[0].graph); 
	   }
    break;
case 54:
#line 275 "parser.ypp"
{
		struct joinlist * jl = new joinlist(yyvsp[-3].string, yyvsp[-4].graph);
		jl->append(yyvsp[-1].join_list);
		yyval.graph = newjoin(jl, yyvsp[0].graph);
	   }
    break;
case 55:
#line 284 "parser.ypp"
{ 
		yyval.join_list = new joinlist(yyvsp[0].string, nullgraph);
	   }
    break;
case 56:
#line 288 "parser.ypp"
{ 
		yyval.join_list = new joinlist(yyvsp[0].string, yyvsp[-1].graph);
	   }
    break;
case 57:
#line 292 "parser.ypp"
{ 
		yyval.join_list = new joinlist(yyvsp[-2].string, nullgraph);
		yyval.join_list->append(yyvsp[0].join_list); 
	   }
    break;
case 58:
#line 297 "parser.ypp"
{ 
		yyval.join_list = new joinlist(yyvsp[-2].string, yyvsp[-3].graph);
		yyval.join_list->append(yyvsp[0].join_list); 
	   }
    break;
case 60:
#line 305 "parser.ypp"
{ yyval.graph = nullgraph; }
    break;
case 61:
#line 310 "parser.ypp"
{
	   	yyval.graph = yyvsp[0].graph;
		graph_join(&(yyval.graph), yyvsp[-1].graph);
	   }
    break;
case 62:
#line 317 "parser.ypp"
{ yyval.graph = nullgraph; }
    break;
case 63:
#line 318 "parser.ypp"
{ yyval.graph = optimize_bools(yyvsp[0].comp); }
    break;
case 64:
#line 322 "parser.ypp"
{ yyval.group.args = NULL; yyval.group.having = NULL;}
    break;
case 65:
#line 323 "parser.ypp"
{ yyval.group.args = yyvsp[-1].arglist; yyval.group.having = yyvsp[0].comp; }
    break;
case 66:
#line 327 "parser.ypp"
{ yyval.comp = NULL; }
    break;
case 67:
#line 328 "parser.ypp"
{ yyval.comp = yyvsp[0].comp; }
    break;
case 69:
#line 332 "parser.ypp"
{ yyval.graph = newmodule(yyvsp[-1].string, yyvsp[0].arglist); }
    break;
case 70:
#line 336 "parser.ypp"
{ yyval.graph = yyvsp[-1].graph; }
    break;
case 71:
#line 337 "parser.ypp"
{ yyval.graph = newmodule(yyvsp[-3].string, yyvsp[-1].arglist); }
    break;
case 72:
#line 338 "parser.ypp"
{ yyval.graph = newmodule(yyvsp[0].string, NULL); }
    break;
case 73:
#line 340 "parser.ypp"
{
		yyvsp[-3].graph->add_graph(yyvsp[-1].graph);
		yyval.graph = yyvsp[-3].graph;
	   }
    break;
case 75:
#line 349 "parser.ypp"
{
		yyvsp[-2].graph->add_graph(yyvsp[0].graph);
		yyval.graph = yyvsp[-2].graph;
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
#line 355 "parser.ypp"


extern void yysmacql_scan_string(char*);

#ifndef SMACQ_OPT_NOPTHREADS
#ifdef PTHREAD_MUTEX_INITIALIZER
  static pthread_mutex_t local_lock = PTHREAD_MUTEX_INITIALIZER;
#else
  static pthread_mutex_t local_lock;
  #warning "No PTHREAD_MUTEX_INITIALIZER"
#endif
#endif

void yysmacql_error(char * msg) {
  fprintf(stderr, "%s near %s\n", msg, yytext-1);
}

