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

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
#define yyparse yysmacql_parse
#define yylex   yysmacql_lex
#define yyerror yysmacql_error
#define yylval  yysmacql_lval
#define yychar  yysmacql_char
#define yydebug yysmacql_debug
#define yynerrs yysmacql_nerrs


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
     YYAND = 274,
     YYNOT = 275
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
#define YYNOT 275




/* Copy the first part of user declarations.  */
#line 4 "/home/mfisk/smacq/libsmacq/sql/parser.y"

void yysmacql_error(char*);

#include <smacq-parser.h>
static struct graph nullgraph = { head: NULL, tail: NULL };
static smacq_graph * Graph;
#line 36 "/home/mfisk/smacq/libsmacq/sql/parser.y"


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
#line 70 "/home/mfisk/smacq/libsmacq/sql/parser.y"
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
#line 149 "/home/mfisk/smacq/libsmacq/sql/parser.c"
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
#line 170 "/home/mfisk/smacq/libsmacq/sql/parser.c"

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
#define YYLAST   204

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  34
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  29
/* YYNRULES -- Number of rules. */
#define YYNRULES  67
/* YYNRULES -- Number of states. */
#define YYNSTATES  118

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   275

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      25,    26,    23,    21,    29,    22,     2,    24,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      32,    30,    31,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    27,     2,    28,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    33,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20
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
     127,   130,   135,   141,   144,   146,   149,   152,   156,   158,
     161,   163,   168,   170,   173,   178,   181,   184
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      54,     0,    -1,    -1,    39,    -1,    38,    -1,    37,    -1,
      12,    -1,    10,    -1,    11,    -1,    18,    -1,    19,    -1,
      41,    -1,    41,     8,    36,    -1,    36,    -1,    42,    25,
      43,    26,    -1,    27,    49,    28,    -1,    39,    -1,    40,
      29,    40,    45,    -1,    44,    -1,    35,    -1,    40,    44,
      -1,    35,    -1,    29,    40,    45,    -1,    39,    -1,    25,
      47,    26,    -1,    47,    18,    47,    -1,    47,    19,    47,
      -1,    20,    47,    -1,    51,    -1,    39,    -1,    38,    -1,
      37,    -1,    25,    49,    26,    -1,    50,    53,    50,    -1,
      49,    -1,    48,    -1,    48,    -1,    50,    52,    50,    -1,
      46,    25,    43,    26,    -1,    30,    -1,    31,    -1,    32,
      -1,    15,    -1,    14,    -1,    13,    -1,    17,    -1,    21,
      -1,    22,    -1,    24,    -1,    23,    -1,    55,    16,    -1,
      62,    56,    58,    59,    -1,    55,    33,    62,    58,    59,
      -1,    56,    58,    -1,    35,    -1,     6,    57,    -1,    61,
      56,    -1,    25,    55,    26,    -1,    35,    -1,     3,    47,
      -1,    35,    -1,     4,     5,    43,    60,    -1,    35,    -1,
       9,    47,    -1,    46,    25,    43,    26,    -1,    46,    44,
      -1,    46,    43,    -1,    46,    25,    43,    26,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    83,    83,    86,    87,    88,    91,    94,    97,    98,
      99,   102,   103,   106,   107,   108,   113,   116,   117,   120,
     121,   124,   125,   128,   135,   136,   137,   138,   139,   142,
     143,   144,   148,   149,   154,   155,   158,   159,   160,   169,
     170,   171,   172,   173,   174,   175,   178,   179,   180,   181,
     185,   195,   206,   217,   225,   226,   229,   235,   238,   239,
     242,   243,   246,   247,   250,   251,   254,   255
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "WHERE", "GROUP", "BY", "FROM", "SELECT", 
  "AS", "HAVING", "YYSTRING", "YYID", "YYNUMBER", "YYNEQ", "YYLEQ", 
  "YYGEQ", "YYSTOP", "YYLIKE", "YYOR", "YYAND", "YYNOT", "'+'", "'-'", 
  "'*'", "'/'", "'('", "')'", "'['", "']'", "','", "'='", "'>'", "'<'", 
  "'|'", "$accept", "null", "word", "number", "string", "id", "arg", 
  "argument", "function", "args", "spacedargs", "moreargs", "verb", 
  "boolean", "operand", "expression", "subexpression", "test", "op", 
  "arithop", "queryline", "query", "from", "source", "where", "group", 
  "having", "pverbphrase", "verbphrase", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,    43,    45,    42,    47,    40,    41,    91,    93,    44,
      61,    62,    60,   124
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
      54,    55,    55,    55,    56,    56,    57,    57,    58,    58,
      59,    59,    60,    60,    61,    61,    62,    62
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     4,     3,     1,     4,     1,     1,
       2,     1,     3,     1,     3,     3,     3,     2,     1,     1,
       1,     1,     3,     3,     1,     1,     1,     3,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     4,     5,     2,     1,     2,     2,     3,     1,     2,
       1,     4,     1,     2,     4,     2,     2,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     8,     9,    10,    54,    23,     2,     0,     0,
       2,     2,     2,     2,    55,     2,     7,     6,     2,     0,
      19,    13,     5,     4,     3,     2,    11,     0,    66,    18,
       1,    50,     0,     0,    58,    53,     2,     0,     2,     2,
      65,    56,     0,     0,    31,    30,    29,    35,    34,     0,
       0,    20,     0,     2,     2,     0,     0,    29,     0,    59,
      35,    34,     0,    28,     2,    57,     0,    67,    34,    15,
      46,    47,    49,    48,     0,     2,    12,     3,     0,     2,
      27,     0,     2,     0,     0,    44,    43,    42,    45,    39,
      40,    41,     0,     0,    60,    51,    64,    32,    33,     0,
      21,    17,    14,    52,    24,     0,    25,    26,    37,     2,
       2,    38,     2,    22,     0,    62,    61,    63
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,   101,    58,    59,    60,    61,    62,    63,    92,    74,
       8,     9,    10,    14,    35,    95,   116,    15,    11
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -32
static const short yypact[] =
{
     179,   164,   -32,   -32,   -32,   -32,   -32,   120,    11,    -6,
      26,    24,   179,   138,   -32,    24,   -32,   -32,   141,   162,
     -32,   -32,   -32,   -32,   -19,    93,    28,    21,   -32,   -32,
     -32,   -32,    57,   159,   -32,   -32,    26,    25,   141,   141,
     -32,   -32,    38,   162,   -32,   -32,   -32,   -32,    19,   180,
     141,   -32,   181,   141,    26,   159,   159,    46,    47,    -2,
      91,   -32,   112,   -32,    69,   -32,    60,   -32,    61,   -32,
     -32,   -32,   -32,   -32,   162,    67,   -32,   -32,    75,    69,
      -2,   136,   141,   159,   159,   -32,   -32,   -32,   -32,   -32,
     -32,   -32,   162,    97,   -32,   -32,   -32,   -32,   180,   141,
     -32,   -32,   -32,   -32,   -32,    80,   -32,   -32,   180,   141,
      67,   -32,    99,   -32,   159,   -32,   -32,    -2
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -32,     3,    64,     5,     7,     0,    -5,   -32,   -32,   -16,
      -4,     8,    53,   -14,   -15,   -12,     6,   -32,   -32,   -32,
     -32,   111,   126,   -32,   -31,    49,   -32,   -32,   108
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -37
static const yysigned_char yytable[] =
{
       6,     6,    42,     5,    47,    64,   -16,    48,    39,    40,
      31,    30,     6,    34,     5,     5,    83,    84,     5,    46,
      39,    51,    66,    79,    44,    49,    45,    32,    47,    33,
       1,    68,     6,    57,    39,    51,    52,    78,    44,    34,
      45,    80,    81,    46,    68,    75,    53,    69,    44,    49,
      45,    65,    77,     7,    13,    57,    57,    34,    32,    47,
      44,    44,    45,    45,    67,     7,   105,    94,     2,   106,
     107,   -23,    82,    93,    46,     3,     4,    47,   100,    44,
      98,    45,    94,    57,    57,     7,    96,    97,    44,    44,
      45,    45,    46,   112,   110,   -36,    99,    44,   108,    45,
     117,   102,   109,    16,     2,    17,   111,   -36,   114,   -36,
     -36,     3,     4,   100,    57,   115,    76,   -36,   113,    44,
      19,    45,    50,    37,   -36,    85,    86,    87,   103,    88,
      16,     2,    17,    70,    71,    72,    73,    36,     3,     4,
      54,    41,    89,    90,    91,    18,     0,    19,    16,     2,
      17,    16,     2,    17,    83,    84,     3,     4,     0,     3,
       4,     0,   104,    38,     0,    19,     0,     0,    19,    16,
       2,    17,    16,     2,    17,     2,     0,     3,     4,    55,
       3,     4,     3,     4,    56,     1,     0,    43,     0,    12,
       2,    16,     2,    17,     0,     0,     0,     3,     4,     3,
       4,    70,    71,    72,    73
};

static const yysigned_char yycheck[] =
{
       0,     1,    18,     0,    19,    36,    25,    19,    13,    13,
      16,     0,    12,    10,    11,    12,    18,    19,    15,    19,
      25,    25,    38,    54,    19,    19,    19,    33,    43,     3,
       6,    43,    32,    33,    39,    39,     8,    53,    33,    36,
      33,    55,    56,    43,    56,    50,    25,    28,    43,    43,
      43,    26,    52,     0,     1,    55,    56,    54,    33,    74,
      55,    56,    55,    56,    26,    12,    82,    64,    11,    83,
      84,    25,    25,     4,    74,    18,    19,    92,    75,    74,
      74,    74,    79,    83,    84,    32,    26,    26,    83,    84,
      83,    84,    92,   109,    99,     4,    29,    92,    92,    92,
     114,    26,     5,    10,    11,    12,    26,    16,     9,    18,
      19,    18,    19,   110,   114,   112,    52,    26,   110,   114,
      27,   114,    29,    12,    33,    13,    14,    15,    79,    17,
      10,    11,    12,    21,    22,    23,    24,    11,    18,    19,
      32,    15,    30,    31,    32,    25,    -1,    27,    10,    11,
      12,    10,    11,    12,    18,    19,    18,    19,    -1,    18,
      19,    -1,    26,    25,    -1,    27,    -1,    -1,    27,    10,
      11,    12,    10,    11,    12,    11,    -1,    18,    19,    20,
      18,    19,    18,    19,    25,     6,    -1,    25,    -1,    25,
      11,    10,    11,    12,    -1,    -1,    -1,    18,    19,    18,
      19,    21,    22,    23,    24
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     6,    11,    18,    19,    35,    39,    46,    54,    55,
      56,    62,    25,    46,    57,    61,    10,    12,    25,    27,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
       0,    16,    33,     3,    35,    58,    56,    55,    25,    40,
      44,    56,    43,    25,    37,    38,    39,    48,    49,    50,
      29,    44,     8,    25,    62,    20,    25,    39,    46,    47,
      48,    49,    50,    51,    58,    26,    43,    26,    49,    28,
      21,    22,    23,    24,    53,    40,    36,    39,    43,    58,
      47,    47,    25,    18,    19,    13,    14,    15,    17,    30,
      31,    32,    52,     4,    35,    59,    26,    26,    50,    29,
      35,    45,    26,    59,    26,    43,    47,    47,    50,     5,
      40,    26,    43,    45,     9,    35,    60,    47
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
#line 91 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = yystring; }
    break;

  case 7:
#line 94 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = yystring; }
    break;

  case 8:
#line 97 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = yystring; }
    break;

  case 9:
#line 98 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = "or"; }
    break;

  case 10:
#line 99 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = "and"; }
    break;

  case 12:
#line 103 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist->rename = yyvsp[0].string; }
    break;

  case 13:
#line 106 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg(yyvsp[0].string, WORD, NULL); }
    break;

  case 14:
#line 107 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg(yyvsp[-3].string, FUNCTION, yyvsp[-1].arglist); }
    break;

  case 15:
#line 108 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg("expr", FUNCTION, 
					       newarg(print_operand(yyvsp[-1].operand), WORD, NULL)); 
			                  }
    break;

  case 17:
#line 116 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = yyvsp[-3].arglist; yyval.arglist->next = yyvsp[-1].arglist; yyvsp[-1].arglist->next = yyvsp[0].arglist; }
    break;

  case 19:
#line 120 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = NULL; }
    break;

  case 20:
#line 121 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 21:
#line 124 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = NULL; }
    break;

  case 22:
#line 125 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 24:
#line 135 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = yyvsp[-1].comp; }
    break;

  case 25:
#line 136 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, OR); }
    break;

  case 26:
#line 137 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, AND); }
    break;

  case 27:
#line 138 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = comp_join(yyvsp[0].comp, NULL, NOT); }
    break;

  case 29:
#line 142 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.operand = comp_operand(FIELD, yyvsp[0].string); }
    break;

  case 30:
#line 143 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.operand = comp_operand(CONST, yyvsp[0].string); }
    break;

  case 31:
#line 144 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.operand = comp_operand(CONST, yyvsp[0].string); }
    break;

  case 32:
#line 148 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    {	  yyval.operand = yyvsp[-1].operand;  }
    break;

  case 33:
#line 149 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    {
				  yyval.operand = comp_arith(parse_tenv, yyvsp[-1].arithop, yyvsp[-2].operand, yyvsp[0].operand); 
				}
    break;

  case 36:
#line 158 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = comp_new(EXIST, yyvsp[0].operand, yyvsp[0].operand); }
    break;

  case 37:
#line 159 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = comp_new(yyvsp[-1].op, yyvsp[-2].operand, yyvsp[0].operand); }
    break;

  case 38:
#line 160 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { 
				  int argc; char ** argv;
				  struct dts_operand op;

				  arglist2argv(yyvsp[-1].arglist, &argc, &argv);
				  yyval.comp = comp_new_func(yyvsp[-3].string, argc, argv, yyvsp[-1].arglist);
				}
    break;

  case 39:
#line 169 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = EQ; }
    break;

  case 40:
#line 170 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = GT; }
    break;

  case 41:
#line 171 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = LT; }
    break;

  case 42:
#line 172 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = GEQ; }
    break;

  case 43:
#line 173 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = LEQ; }
    break;

  case 44:
#line 174 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = NEQ; }
    break;

  case 45:
#line 175 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = LIKE; }
    break;

  case 46:
#line 178 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arithop = ADD; }
    break;

  case 47:
#line 179 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arithop = SUB; }
    break;

  case 48:
#line 180 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arithop = DIVIDE; }
    break;

  case 49:
#line 181 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arithop = MULT; }
    break;

  case 50:
#line 186 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { 
#ifdef DEBUG
	   	smacq_graph_print(stderr, yyvsp[-1].graph.head, 0); 
#endif
		Graph = yyvsp[-1].graph.head;
		return 0;
	   }
    break;

  case 51:
#line 196 "/home/mfisk/smacq/libsmacq/sql/parser.y"
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

  case 52:
#line 207 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    {
	   	yyval.graph.head = (yyval.graph.tail = NULL);
	   	graph_join(&(yyval.graph), yyvsp[-4].graph);
		graph_join(&(yyval.graph), yyvsp[-1].graph);
		if (yyvsp[0].group.args) {
			graph_join(&(yyval.graph), newgroup(yyvsp[0].group, yyvsp[-2].vphrase));
		} else {
			graph_join(&(yyval.graph), newmodule(yyvsp[-2].vphrase.verb, yyvsp[-2].vphrase.args));
		}
	   }
    break;

  case 53:
#line 218 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    {
	   	yyval.graph.head = (yyval.graph.tail = NULL);
	   	graph_join(&(yyval.graph), yyvsp[-1].graph);
		graph_join(&(yyval.graph), yyvsp[0].graph);
	   }
    break;

  case 54:
#line 225 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph.head = NULL; yyval.graph.tail = NULL; }
    break;

  case 55:
#line 226 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = yyvsp[0].graph; }
    break;

  case 56:
#line 230 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    {
	   			yyval.graph.head = (yyval.graph.tail = NULL);
	   			graph_join(&(yyval.graph), yyvsp[0].graph);
				graph_join(&(yyval.graph), yyvsp[-1].graph);
			}
    break;

  case 57:
#line 235 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = yyvsp[-1].graph; }
    break;

  case 58:
#line 238 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = nullgraph; }
    break;

  case 59:
#line 239 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = optimize_bools(yyvsp[0].comp); }
    break;

  case 60:
#line 242 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.group.args = NULL; yyval.group.having = NULL;}
    break;

  case 61:
#line 243 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.group.args = yyvsp[-1].arglist; yyval.group.having = newarg(print_comparison(yyvsp[0].comp), 0, NULL); }
    break;

  case 62:
#line 246 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = NULL; }
    break;

  case 63:
#line 247 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = yyvsp[0].comp; }
    break;

  case 64:
#line 250 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = newmodule(yyvsp[-3].string, yyvsp[-1].arglist); }
    break;

  case 65:
#line 251 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = newmodule(yyvsp[-1].string, yyvsp[0].arglist); }
    break;

  case 66:
#line 254 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.vphrase = newvphrase(yyvsp[-1].string, yyvsp[0].arglist); }
    break;

  case 67:
#line 255 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.vphrase = newvphrase(yyvsp[-3].string, yyvsp[-1].arglist); }
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 1418 "/home/mfisk/smacq/libsmacq/sql/parser.c"

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


#line 258 "/home/mfisk/smacq/libsmacq/sql/parser.y"


extern void yysmacql_scan_string(char*);
extern char * yysmacql_text;

#ifdef PTHREAD_MUTEX_INITIALIZER
  static pthread_mutex_t local_lock = PTHREAD_MUTEX_INITIALIZER;
#else
  static pthread_mutex_t local_lock;
  #warning "No PTHREAD_MUTEX_INITIALIZER"
#endif

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

  yysmacql_scan_string(qstr);
  //fprintf(stderr, "parsing buffer: %s\n", qstr); 

  res = yysmacql_parse();

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

void yysmacql_error(char * msg) {
  fprintf(stderr, "%s near %s\n", msg, yysmacql_text-1);
}


