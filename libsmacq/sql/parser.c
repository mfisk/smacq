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
#line 65 "/home/mfisk/smacq/libsmacq/sql/parser.y"
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
#line 147 "/home/mfisk/smacq/libsmacq/sql/parser.c"
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
#line 168 "/home/mfisk/smacq/libsmacq/sql/parser.c"

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
#define YYFINAL  29
#define YYLAST   195

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  31
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  28
/* YYNRULES -- Number of rules. */
#define YYNRULES  63
/* YYNRULES -- Number of states. */
#define YYNSTATES  112

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
       2,     2,     2,     2,    30,     2,     2,     2,     2,     2,
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
     104,   106,   108,   110,   112,   114,   116,   119,   124,   130,
     133,   135,   138,   141,   145,   147,   150,   152,   157,   159,
     162,   167,   170,   173
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      50,     0,    -1,    -1,    36,    -1,    35,    -1,    34,    -1,
      12,    -1,    10,    -1,    11,    -1,    18,    -1,    19,    -1,
      38,    -1,    38,     8,    33,    -1,    33,    -1,    39,    20,
      40,    21,    -1,    36,    -1,    37,    22,    37,    42,    -1,
      41,    -1,    32,    -1,    37,    41,    -1,    32,    -1,    22,
      37,    42,    -1,    36,    -1,    20,    44,    21,    -1,    44,
      18,    44,    -1,    44,    19,    44,    -1,    47,    -1,    36,
      -1,    35,    -1,    34,    -1,    20,    46,    49,    46,    21,
      -1,    45,    -1,    45,    -1,    46,    48,    46,    -1,    43,
      20,    40,    21,    -1,    23,    -1,    24,    -1,    25,    -1,
      15,    -1,    14,    -1,    13,    -1,    17,    -1,    26,    -1,
      27,    -1,    28,    -1,    29,    -1,    51,    16,    -1,    58,
      52,    54,    55,    -1,    51,    30,    58,    54,    55,    -1,
      52,    54,    -1,    32,    -1,     6,    53,    -1,    57,    52,
      -1,    20,    51,    21,    -1,    32,    -1,     3,    44,    -1,
      32,    -1,     4,     5,    40,    56,    -1,    32,    -1,     9,
      44,    -1,    43,    20,    40,    21,    -1,    43,    41,    -1,
      43,    40,    -1,    43,    20,    40,    21,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    78,    78,    81,    82,    83,    86,    89,    92,    93,
      94,    97,    98,   101,   102,   105,   108,   109,   112,   113,
     116,   117,   120,   127,   128,   129,   130,   133,   134,   135,
     138,   141,   144,   145,   146,   155,   156,   157,   158,   159,
     160,   161,   164,   165,   166,   167,   171,   181,   192,   203,
     211,   212,   215,   221,   224,   225,   228,   229,   232,   233,
     236,   237,   240,   241
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
  "'='", "'>'", "'<'", "'+'", "'-'", "'/'", "'*'", "'|'", "$accept", 
  "null", "word", "number", "string", "id", "arg", "argument", "function", 
  "args", "spacedargs", "moreargs", "verb", "boolean", "operand", 
  "expression", "test", "op", "arithop", "queryline", "query", "from", 
  "source", "where", "group", "having", "pverbphrase", "verbphrase", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      40,    41,    44,    61,    62,    60,    43,    45,    47,    42,
     124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    31,    32,    33,    33,    33,    34,    35,    36,    36,
      36,    37,    37,    38,    38,    39,    40,    40,    41,    41,
      42,    42,    43,    44,    44,    44,    44,    45,    45,    45,
      46,    46,    47,    47,    47,    48,    48,    48,    48,    48,
      48,    48,    49,    49,    49,    49,    50,    51,    51,    51,
      52,    52,    53,    53,    54,    54,    55,    55,    56,    56,
      57,    57,    58,    58
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     4,     1,     4,     1,     1,     2,
       1,     3,     1,     3,     3,     3,     1,     1,     1,     1,
       5,     1,     1,     3,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     4,     5,     2,
       1,     2,     2,     3,     1,     2,     1,     4,     1,     2,
       4,     2,     2,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     8,     9,    10,    50,    22,     2,     0,     0,
       2,     2,     2,     2,    51,     2,     7,     6,     2,    18,
      13,     5,     4,     3,     2,    11,     0,    62,    17,     1,
      46,     0,     0,    54,    49,     2,     0,     2,     2,    61,
      52,     0,     0,    19,     0,     2,     2,     0,    29,    28,
      27,     0,    55,    31,     0,    26,     2,    53,     0,    63,
       2,    12,     3,     0,     2,     0,     0,     2,     0,     0,
      40,    39,    38,    41,    35,    36,    37,     0,     0,    56,
      47,    60,     0,    20,    16,    14,    48,    23,    42,    43,
      44,    45,     0,     0,    24,    25,     0,    27,    31,    33,
       2,     2,     0,    34,     0,     2,    21,    30,     0,    58,
      57,    59
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    84,    51,    52,    53,    54,    55,    77,    92,     8,
       9,    10,    14,    34,    80,   110,    15,    11
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -74
static const short yypact[] =
{
     125,   175,   -74,   -74,   -74,   -74,   -74,   145,     3,     9,
      12,     4,   125,   156,   -74,     4,   -74,   -74,   173,   -74,
     -74,   -74,   -74,    10,   140,    25,    15,   -74,   -74,   -74,
     -74,    83,   159,   -74,   -74,    12,     7,   173,   173,   -74,
     -74,    -1,   173,   -74,   173,   173,    12,   159,   -74,   -74,
      23,    38,    36,    79,   124,   -74,    57,   -74,    44,   -74,
      45,   -74,   -74,    50,    57,    63,   101,   173,   159,   159,
     -74,   -74,   -74,   -74,   -74,   -74,   -74,   170,    68,   -74,
     -74,   -74,   173,   -74,   -74,   -74,   -74,   -74,   -74,   -74,
     -74,   -74,   170,    54,   -74,   -74,   170,   -74,   -74,   -74,
     173,    45,    55,   -74,    93,    77,   -74,   -74,   159,   -74,
     -74,    36
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -74,     6,    49,   -18,    -5,     0,    -2,   -74,   -74,   -11,
      75,     5,    48,   -23,   -73,   -39,   -74,   -74,   -74,   -74,
      92,    -6,   -74,   -33,    41,   -74,   -74,    81
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -33
static const yysigned_char yytable[] =
{
       6,     6,    56,    29,    98,    35,     5,    41,    66,    40,
       1,    38,     6,    64,    48,    32,    33,     5,     5,    98,
      59,     5,    38,    98,    65,    30,    58,    49,    57,    48,
     -15,     6,    50,    44,    63,    45,    38,    31,    99,    31,
      60,    33,    49,   -22,    62,    94,    95,    50,     7,    13,
      48,    48,    33,   102,    68,    69,    93,   104,    67,    48,
       7,    78,    79,    49,    49,    81,    83,    82,    50,    50,
      79,    85,    49,   100,    48,   103,   107,    97,    48,     7,
     101,    68,    69,   -32,    87,   111,   108,    49,    39,   105,
      48,    49,    97,    61,     2,   -32,    97,   -32,   -32,    43,
     -32,     3,     4,    49,    36,    86,   106,    83,    50,   -32,
       0,   109,    46,    43,    70,    71,    72,     0,    73,    88,
      89,    90,    91,     0,    74,    75,    76,    88,    89,    90,
      91,     1,     0,     0,     0,     0,     2,    70,    71,    72,
       0,    73,     0,     3,     4,     0,     0,    74,    75,    76,
      16,     2,    17,     0,     0,    16,     2,    17,     3,     4,
       0,     0,    42,     3,     4,    18,    16,     2,    17,    16,
       2,    17,     0,     0,     3,     4,    37,     3,     4,    47,
      16,     2,    17,    16,     2,    17,     2,     0,     3,     4,
      96,     3,     4,     3,     4,    12
};

static const yysigned_char yycheck[] =
{
       0,     1,    35,     0,    77,    11,     0,    18,    47,    15,
       6,    13,    12,    46,    32,     3,    10,    11,    12,    92,
      21,    15,    24,    96,    47,    16,    37,    32,    21,    47,
      20,    31,    32,     8,    45,    20,    38,    30,    77,    30,
      42,    35,    47,    20,    44,    68,    69,    47,     0,     1,
      68,    69,    46,    92,    18,    19,    67,    96,    20,    77,
      12,     4,    56,    68,    69,    21,    60,    22,    68,    69,
      64,    21,    77,     5,    92,    21,    21,    77,    96,    31,
      82,    18,    19,     4,    21,   108,     9,    92,    13,   100,
     108,    96,    92,    44,    11,    16,    96,    18,    19,    24,
      21,    18,    19,   108,    12,    64,   101,   101,   108,    30,
      -1,   105,    31,    38,    13,    14,    15,    -1,    17,    26,
      27,    28,    29,    -1,    23,    24,    25,    26,    27,    28,
      29,     6,    -1,    -1,    -1,    -1,    11,    13,    14,    15,
      -1,    17,    -1,    18,    19,    -1,    -1,    23,    24,    25,
      10,    11,    12,    -1,    -1,    10,    11,    12,    18,    19,
      -1,    -1,    22,    18,    19,    20,    10,    11,    12,    10,
      11,    12,    -1,    -1,    18,    19,    20,    18,    19,    20,
      10,    11,    12,    10,    11,    12,    11,    -1,    18,    19,
      20,    18,    19,    18,    19,    20
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     6,    11,    18,    19,    32,    36,    43,    50,    51,
      52,    58,    20,    43,    53,    57,    10,    12,    20,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,     0,
      16,    30,     3,    32,    54,    52,    51,    20,    37,    41,
      52,    40,    22,    41,     8,    20,    58,    20,    34,    35,
      36,    43,    44,    45,    46,    47,    54,    21,    40,    21,
      37,    33,    36,    40,    54,    44,    46,    20,    18,    19,
      13,    14,    15,    17,    23,    24,    25,    48,     4,    32,
      55,    21,    22,    32,    42,    21,    55,    21,    26,    27,
      28,    29,    49,    40,    44,    44,    20,    36,    45,    46,
       5,    37,    46,    21,    46,    40,    42,    21,     9,    32,
      56,    44
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
#line 86 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = yystring; }
    break;

  case 7:
#line 89 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = yystring; }
    break;

  case 8:
#line 92 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = yystring; }
    break;

  case 9:
#line 93 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = "or"; }
    break;

  case 10:
#line 94 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.string = "and"; }
    break;

  case 12:
#line 98 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist->rename = yyvsp[0].string; }
    break;

  case 13:
#line 101 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg(yyvsp[0].string, 0, NULL); }
    break;

  case 14:
#line 102 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = newarg(yyvsp[-3].string, 1, yyvsp[-1].arglist); }
    break;

  case 16:
#line 108 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = yyvsp[-3].arglist; yyval.arglist->next = yyvsp[-1].arglist; yyvsp[-1].arglist->next = yyvsp[0].arglist; }
    break;

  case 18:
#line 112 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = NULL; }
    break;

  case 19:
#line 113 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 20:
#line 116 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = NULL; }
    break;

  case 21:
#line 117 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 23:
#line 127 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = yyvsp[-1].comp; }
    break;

  case 24:
#line 128 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, 1); }
    break;

  case 25:
#line 129 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, 0); }
    break;

  case 27:
#line 133 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.operand = comp_operand(FIELD, yyvsp[0].string); }
    break;

  case 28:
#line 134 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.operand = comp_operand(CONST, yyvsp[0].string); }
    break;

  case 29:
#line 135 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.operand = comp_operand(CONST, yyvsp[0].string); }
    break;

  case 30:
#line 138 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    {
				  yyval.operand = comp_arith(parse_tenv, yyvsp[-2].arithop, yyvsp[-3].operand, yyvsp[-1].operand); 
				}
    break;

  case 32:
#line 144 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = comp_new(EXIST, yyvsp[0].operand, yyvsp[0].operand); }
    break;

  case 33:
#line 145 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = comp_new(yyvsp[-1].op, yyvsp[-2].operand, yyvsp[0].operand); }
    break;

  case 34:
#line 146 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { 
				  int argc; char ** argv;
				  struct dts_operand op;

				  arglist2argv(yyvsp[-1].arglist, &argc, &argv);
				  yyval.comp = comp_new_func(yyvsp[-3].string, argc, argv, yyvsp[-1].arglist);
				}
    break;

  case 35:
#line 155 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = EQ; }
    break;

  case 36:
#line 156 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = GT; }
    break;

  case 37:
#line 157 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = LT; }
    break;

  case 38:
#line 158 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = GEQ; }
    break;

  case 39:
#line 159 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = LEQ; }
    break;

  case 40:
#line 160 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = NEQ; }
    break;

  case 41:
#line 161 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.op = LIKE; }
    break;

  case 42:
#line 164 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arithop = ADD; }
    break;

  case 43:
#line 165 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arithop = SUB; }
    break;

  case 44:
#line 166 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arithop = DIVIDE; }
    break;

  case 45:
#line 167 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.arithop = MULT; }
    break;

  case 46:
#line 172 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { 
#ifdef DEBUG
	   	smacq_graph_print(stderr, yyvsp[-1].graph.head, 0); 
#endif
		Graph = yyvsp[-1].graph.head;
		return 0;
	   }
    break;

  case 47:
#line 182 "/home/mfisk/smacq/libsmacq/sql/parser.y"
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

  case 48:
#line 193 "/home/mfisk/smacq/libsmacq/sql/parser.y"
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

  case 49:
#line 204 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    {
	   	yyval.graph.head = (yyval.graph.tail = NULL);
	   	graph_join(&(yyval.graph), yyvsp[-1].graph);
		graph_join(&(yyval.graph), yyvsp[0].graph);
	   }
    break;

  case 50:
#line 211 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph.head = NULL; yyval.graph.tail = NULL; }
    break;

  case 51:
#line 212 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = yyvsp[0].graph; }
    break;

  case 52:
#line 216 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    {
	   			yyval.graph.head = (yyval.graph.tail = NULL);
	   			graph_join(&(yyval.graph), yyvsp[0].graph);
				graph_join(&(yyval.graph), yyvsp[-1].graph);
			}
    break;

  case 53:
#line 221 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = yyvsp[-1].graph; }
    break;

  case 54:
#line 224 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = nullgraph; }
    break;

  case 55:
#line 225 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = optimize_bools(yyvsp[0].comp); }
    break;

  case 56:
#line 228 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.group.args = NULL; yyval.group.having = NULL;}
    break;

  case 57:
#line 229 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.group.args = yyvsp[-1].arglist; yyval.group.having = newarg(print_comparison(yyvsp[0].comp), 0, NULL); }
    break;

  case 58:
#line 232 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = NULL; }
    break;

  case 59:
#line 233 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.comp = yyvsp[0].comp; }
    break;

  case 60:
#line 236 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = newmodule(yyvsp[-3].string, yyvsp[-1].arglist); }
    break;

  case 61:
#line 237 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.graph = newmodule(yyvsp[-1].string, yyvsp[0].arglist); }
    break;

  case 62:
#line 240 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.vphrase = newvphrase(yyvsp[-1].string, yyvsp[0].arglist); }
    break;

  case 63:
#line 241 "/home/mfisk/smacq/libsmacq/sql/parser.y"
    { yyval.vphrase = newvphrase(yyvsp[-3].string, yyvsp[-1].arglist); }
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 1395 "/home/mfisk/smacq/libsmacq/sql/parser.c"

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


#line 244 "/home/mfisk/smacq/libsmacq/sql/parser.y"


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


