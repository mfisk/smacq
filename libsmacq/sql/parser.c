/* A Bison parser, made from /home/mfisk/smacq/libsmacq/sql/parser.y, by GNU bison 1.75.  */

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
     AS = 263,
     HAVING = 264,
     YYSTRING = 265,
     YYID = 266,
     YYNEQ = 267,
     YYLEQ = 268,
     YYGEQ = 269,
     YYSTOP = 270,
     YYLIKE = 271,
     YYOR = 272,
     YYAND = 273
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
#define YYNEQ 267
#define YYLEQ 268
#define YYGEQ 269
#define YYSTOP 270
#define YYLIKE 271
#define YYOR 272
#define YYAND 273




/* Copy the first part of user declarations.  */
#line 2 "/home/mfisk/smacq/libsmacq/sql/parser.y"

void yyerror(char*);
#line 25 "/home/mfisk/smacq/libsmacq/sql/parser.y"


#include <smacq-parser.h>
static struct graph nullgraph = { head: NULL, tail: NULL };
static smacq_graph * Graph;



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
#line 55 "/home/mfisk/smacq/libsmacq/sql/parser.y"
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
#line 130 "/home/mfisk/smacq/libsmacq/sql/parser.c"
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
#line 151 "/home/mfisk/smacq/libsmacq/sql/parser.c"

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
#define YYFINAL  16
#define YYLAST   127

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  26
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  28
/* YYNRULES -- Number of rules. */
#define YYNRULES  67
/* YYNRULES -- Number of states. */
#define YYNSTATES  109

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   273

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    25,     2,     2,     2,     2,     2,     2,
      20,    21,     2,     2,    19,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      22,    24,    23,     2,     2,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     6,     7,    12,    15,    17,    21,    23,
      28,    30,    33,    35,    39,    41,    44,    46,    51,    53,
      56,    58,    60,    62,    64,    66,    68,    70,    74,    76,
      81,    83,    85,    87,    89,    91,    93,    95,    97,    99,
     101,   103,   105,   107,   112,   115,   120,   123,   125,   128,
     130,   133,   135,   139,   141,   145,   149,   153,   155,   157,
     161,   166,   168,   170,   172,   174,   176,   178
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      27,     0,    -1,    29,    15,    -1,    -1,    45,    30,    34,
      35,    -1,    30,    34,    -1,    28,    -1,     6,    33,    31,
      -1,    28,    -1,    19,    46,    32,    31,    -1,    28,    -1,
       8,    37,    -1,    44,    -1,    20,    29,    21,    -1,    28,
      -1,     3,    51,    -1,    28,    -1,     4,     5,    48,    36,
      -1,    28,    -1,     9,    46,    -1,    39,    -1,    38,    -1,
      10,    -1,    11,    -1,    17,    -1,    18,    -1,    41,    -1,
      41,     8,    37,    -1,    37,    -1,    43,    20,    48,    21,
      -1,    39,    -1,    38,    -1,    22,    -1,    13,    -1,    23,
      -1,    14,    -1,    24,    -1,    12,    -1,    20,    -1,    21,
      -1,    25,    -1,    39,    -1,    50,    -1,    50,    20,    48,
      21,    -1,    50,    48,    -1,    50,    20,    48,    21,    -1,
      42,    47,    -1,    28,    -1,    42,    47,    -1,    28,    -1,
      40,    49,    -1,    28,    -1,    19,    40,    49,    -1,    39,
      -1,    20,    51,    21,    -1,    51,    17,    51,    -1,    51,
      18,    51,    -1,    52,    -1,    37,    -1,    37,    53,    37,
      -1,    50,    20,    48,    21,    -1,    24,    -1,    23,    -1,
      22,    -1,    14,    -1,    13,    -1,    12,    -1,    16,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    66,    66,    76,    78,    89,    97,    98,   107,   108,
     127,   128,   131,   132,   135,   136,   139,   140,   143,   144,
     147,   148,   151,   154,   155,   156,   159,   160,   163,   164,
     167,   168,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   185,   188,   189,   192,   193,   196,   199,   200,   203,
     204,   207,   208,   211,   218,   219,   220,   221,   224,   225,
     226,   234,   235,   236,   237,   238,   239,   240
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "WHERE", "GROUP", "BY", "FROM", "SELECT", 
  "AS", "HAVING", "YYSTRING", "YYID", "YYNEQ", "YYLEQ", "YYGEQ", "YYSTOP", 
  "YYLIKE", "YYOR", "YYAND", "','", "'('", "')'", "'<'", "'>'", "'='", 
  "'!'", "$accept", "queryline", "null", "query", "from", "joins", "as", 
  "source", "where", "group", "having", "word", "string", "id", "arg", 
  "argument", "boolarg", "function", "pverbphrase", "verbphrase", 
  "boolargs", "moreboolargs", "args", "moreargs", "verb", "boolean", 
  "test", "op", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,    44,
      40,    41,    60,    62,    61,    33
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    26,    27,    28,    29,    29,    30,    30,    31,    31,
      32,    32,    33,    33,    34,    34,    35,    35,    36,    36,
      37,    37,    38,    39,    39,    39,    40,    40,    41,    41,
      42,    42,    42,    42,    42,    42,    42,    42,    42,    42,
      42,    43,    44,    44,    45,    45,    46,    47,    47,    48,
      48,    49,    49,    50,    51,    51,    51,    51,    52,    52,
      52,    53,    53,    53,    53,    53,    53,    53
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     0,     4,     2,     1,     3,     1,     4,
       1,     2,     1,     3,     1,     2,     1,     4,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     3,     1,     4,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     2,     4,     2,     1,     2,     1,
       2,     1,     3,     1,     3,     3,     3,     1,     1,     3,
       4,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       3,     0,    23,    24,    25,     0,     6,     0,     3,    53,
       3,     3,     3,     3,    12,    42,     1,     2,     0,    14,
       5,     3,    22,     3,    49,    28,    21,    20,     3,    26,
       0,    44,     0,     0,     8,     7,     3,     0,    58,    20,
       0,    15,    57,     3,     0,     0,    51,    50,     0,     3,
      13,    37,    33,    35,    38,    39,    32,    34,    36,    40,
      31,    30,     3,     3,     0,     0,    66,    65,    64,    67,
      63,    62,    61,     0,     3,     0,     0,     0,    16,     4,
      45,     3,    27,    20,     0,    47,     3,    46,     0,    10,
       3,    43,    54,    59,     0,    55,    56,     3,    52,    29,
      48,    11,     9,    60,     3,     0,    18,    17,    19
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     5,    24,     7,     8,    35,    90,    13,    20,    79,
     107,    25,    26,    27,    28,    29,    62,    30,    14,    10,
      63,    87,    31,    47,    40,    41,    42,    73
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -49
static const yysigned_char yypact[] =
{
      36,     9,   -49,   -49,   -49,    22,   -49,     8,    25,   -49,
      19,    80,    36,    16,   -49,    32,   -49,   -49,    84,   -49,
     -49,    25,   -49,    53,   -49,   -49,   -49,    41,    17,    48,
      47,   -49,    60,    98,   -49,   -49,    53,    84,    27,    49,
      54,    -8,   -49,    64,    61,    53,   -49,   -49,    53,    53,
     -49,   -49,   -49,   -49,   -49,   -49,   -49,   -49,   -49,   -49,
     -49,   -49,    98,    77,    66,    62,   -49,   -49,   -49,   -49,
     -49,   -49,   -49,    53,    53,    84,    84,    87,   -49,   -49,
     -49,    17,   -49,   -49,    75,   -49,    98,   -49,    53,   -49,
      16,   -49,   -49,   -49,    78,   -49,   -49,    53,   -49,   -49,
     -49,   -49,   -49,   -49,    94,    98,   -49,   -49,   -49
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -49,   -49,     3,   101,    96,    24,   -49,   -49,   103,   -49,
     -49,   -16,   -28,     0,    72,   -49,   -48,   -49,   -49,   -49,
      20,    40,   -19,    46,     7,   -31,   -49,   -49
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -54
static const yysigned_char yytable[] =
{
       9,     9,    38,     6,    44,    60,    65,    11,    15,    75,
      76,    19,     9,     6,    86,     6,    34,    64,    39,    11,
       2,    38,    16,    17,    19,     1,     3,     4,    18,    12,
      84,    46,    82,    61,    60,    33,    45,    39,    86,    66,
      67,    68,     1,    69,    95,    96,    78,     2,    83,    70,
      71,    72,    36,     3,     4,    94,    48,    93,    60,    38,
      38,   -41,    61,    22,     2,    85,    89,    49,    77,   -53,
       3,     4,   101,    83,    74,    39,    39,    60,   104,    75,
      76,    50,    80,    92,    46,    88,    61,    91,    83,    85,
      22,     2,    97,    34,    22,     2,    99,     3,     4,   103,
      23,     3,     4,   105,    37,    61,    21,   106,    22,     2,
      51,    52,    53,    32,   102,     3,     4,    81,    54,    55,
      56,    57,    58,    59,    43,   108,   100,    98
};

static const unsigned char yycheck[] =
{
       0,     1,    18,     0,    23,    33,    37,     0,     1,    17,
      18,     8,    12,    10,    62,    12,    13,    36,    18,    12,
      11,    37,     0,    15,    21,     6,    17,    18,     3,    20,
      49,    28,    48,    33,    62,    19,    19,    37,    86,    12,
      13,    14,     6,    16,    75,    76,    43,    11,    48,    22,
      23,    24,    20,    17,    18,    74,     8,    73,    86,    75,
      76,    20,    62,    10,    11,    62,    63,    20,     4,    20,
      17,    18,    88,    73,    20,    75,    76,   105,    97,    17,
      18,    21,    21,    21,    81,     8,    86,    21,    88,    86,
      10,    11,     5,    90,    10,    11,    21,    17,    18,    21,
      20,    17,    18,     9,    20,   105,    10,   104,    10,    11,
      12,    13,    14,    12,    90,    17,    18,    45,    20,    21,
      22,    23,    24,    25,    21,   105,    86,    81
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     6,    11,    17,    18,    27,    28,    29,    30,    39,
      45,    50,    20,    33,    44,    50,     0,    15,     3,    28,
      34,    30,    10,    20,    28,    37,    38,    39,    40,    41,
      43,    48,    29,    19,    28,    31,    20,    20,    37,    39,
      50,    51,    52,    34,    48,    19,    28,    49,     8,    20,
      21,    12,    13,    14,    20,    21,    22,    23,    24,    25,
      38,    39,    42,    46,    48,    51,    12,    13,    14,    16,
      22,    23,    24,    53,    20,    17,    18,     4,    28,    35,
      21,    40,    37,    39,    48,    28,    42,    47,     8,    28,
      32,    21,    21,    37,    48,    51,    51,     5,    49,    21,
      47,    37,    31,    21,    48,     9,    28,    36,    46
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
#line 67 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { 
#ifdef DEBUG
	   	smacq_graph_print(stderr, yyvsp[-1].graph.head, 0); 
#endif
		Graph = yyvsp[-1].graph.head;
		return 0;
	   }
    break;

  case 4:
#line 79 "/home/mfisk/smacq/libsmacq/sql/parser.y"
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
#line 90 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    {
	   	yyval.graph.head = (yyval.graph.tail = NULL);
	   	graph_join(&(yyval.graph), yyvsp[-1].graph);
		graph_join(&(yyval.graph), yyvsp[0].graph);
	   }
    break;

  case 6:
#line 97 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph.head = NULL; yyval.graph.tail = NULL; }
    break;

  case 7:
#line 99 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { 
		yyval.graph = yyvsp[-1].graph; 
	   	if (yyvsp[0].arglist) {
			graph_join(&(yyval.graph), newmodule("join", yyvsp[0].arglist));
		}
	   }
    break;

  case 8:
#line 107 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = NULL; }
    break;

  case 9:
#line 109 "/home/mfisk/smacq/libsmacq/sql/parser.y"
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

  case 10:
#line 127 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = NULL; }
    break;

  case 11:
#line 128 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = yyvsp[0].string; }
    break;

  case 13:
#line 132 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = yyvsp[-1].graph; }
    break;

  case 14:
#line 135 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = nullgraph; }
    break;

  case 15:
#line 136 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = optimize_bools(yyvsp[0].comp); }
    break;

  case 16:
#line 139 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.group.args = NULL; yyval.group.having = NULL;}
    break;

  case 17:
#line 140 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.group.args = yyvsp[-1].arglist; yyval.group.having = yyvsp[0].arglist; }
    break;

  case 18:
#line 143 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = NULL; }
    break;

  case 19:
#line 144 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = yyvsp[0].arglist; }
    break;

  case 22:
#line 151 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = yystring; }
    break;

  case 23:
#line 154 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = yystring; }
    break;

  case 24:
#line 155 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = "or"; }
    break;

  case 25:
#line 156 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = "and"; }
    break;

  case 27:
#line 160 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist->rename = yyvsp[0].string; }
    break;

  case 28:
#line 163 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg(yyvsp[0].string, 0, NULL); }
    break;

  case 29:
#line 164 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg(yyvsp[-3].string, 1, yyvsp[-1].arglist); }
    break;

  case 30:
#line 167 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg(yyvsp[0].string, 0, NULL); }
    break;

  case 31:
#line 169 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { 
	    	char * str = malloc(sizeof(char *) * (strlen(yyvsp[0].string)+2)); 
		sprintf(str,"\"%s\"", yyvsp[0].string); 
		yyval.arglist = newarg(str, 0, NULL); 
	    }
    break;

  case 32:
#line 174 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg("<", 0, NULL); }
    break;

  case 33:
#line 175 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg("<=", 0, NULL); }
    break;

  case 34:
#line 176 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg(">", 0, NULL); }
    break;

  case 35:
#line 177 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg(">=", 0, NULL); }
    break;

  case 36:
#line 178 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg("=", 0, NULL); }
    break;

  case 37:
#line 179 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg("!=", 0, NULL); }
    break;

  case 38:
#line 180 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg("(", 0, NULL); }
    break;

  case 39:
#line 181 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg(")", 0, NULL); }
    break;

  case 40:
#line 182 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg("!", 0, NULL); }
    break;

  case 42:
#line 188 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = newmodule(yyvsp[0].string, NULL); }
    break;

  case 43:
#line 189 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = newmodule(yyvsp[-3].string, yyvsp[-1].arglist); }
    break;

  case 44:
#line 192 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.vphrase = newvphrase(yyvsp[-1].string, yyvsp[0].arglist); }
    break;

  case 45:
#line 193 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.vphrase = newvphrase(yyvsp[-3].string, yyvsp[-1].arglist); }
    break;

  case 46:
#line 196 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 47:
#line 199 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = NULL; }
    break;

  case 48:
#line 200 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 49:
#line 203 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = NULL; }
    break;

  case 50:
#line 204 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 51:
#line 207 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = NULL; }
    break;

  case 52:
#line 208 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 54:
#line 218 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = yyvsp[-1].comp; }
    break;

  case 55:
#line 219 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, 1); }
    break;

  case 56:
#line 220 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, 0); }
    break;

  case 58:
#line 224 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = comp_new(yyvsp[0].string, EXIST, NULL, 0); }
    break;

  case 59:
#line 225 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = comp_new(yyvsp[-2].string, yyvsp[-1].op, &(yyvsp[0].string), 1); }
    break;

  case 60:
#line 226 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    {
					int argc; char ** argv;
					arglist2argv(yyvsp[-1].arglist, &argc, &argv);
					yyval.comp = comp_new(yyvsp[-3].string, FUNC, argv, argc);
					yyval.comp->arglist = yyvsp[-1].arglist;
				}
    break;

  case 61:
#line 234 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = EQ; }
    break;

  case 62:
#line 235 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = GT; }
    break;

  case 63:
#line 236 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = LT; }
    break;

  case 64:
#line 237 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = GEQ; }
    break;

  case 65:
#line 238 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = LEQ; }
    break;

  case 66:
#line 239 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = NEQ; }
    break;

  case 67:
#line 240 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = LIKE; }
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 1398 "/home/mfisk/smacq/libsmacq/sql/parser.c"

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


#line 245 "/home/mfisk/smacq/libsmacq/sql/parser.y"


extern void yy_scan_string(char*);

smacq_graph * smacq_build_query(dts_environment * tenv, int argc, char ** argv) {
  int size = 0;
  int i;
  char * qstr; 
  smacq_graph * graph;
  int res;

  parse_tenv = tenv;

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

  /* LOCK */
  pthread_mutex_lock(&local_lock);

  yy_scan_string(qstr);
  //fprintf(stderr, "parsing buffer: %s\n", qstr); 

  res = yyparse();

  graph = Graph;

  /* UNLOCK */
  pthread_mutex_unlock(&local_lock);

  if (res) {
    fprintf(stderr, "smacq_build_query: error parsing query: %s\n", qstr);
    return NULL;
  }

  if (!graph) {
    fprintf(stderr, "unknown parse error\n");
  }

  return graph;
}

void yyerror(char * msg) {
  fprintf(stderr, "%s near %s\n", msg, yytext-1);
}

