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
#line 4 "parser.y"

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
static struct graph nullgraph = { head: NULL, tail: NULL };
static smacq_graph * Graph;
#line 76 "parser.y"

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
#line 106 "parser.y"
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
#line 181 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 193 "y.tab.c"

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
#define YYFINAL  45
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   223

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  35
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  29
/* YYNRULES -- Number of rules. */
#define YYNRULES  68
/* YYNRULES -- Number of states. */
#define YYNSTATES  116

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
     127,   130,   132,   136,   140,   145,   150,   153,   155,   157,
     160,   163,   165,   168,   170,   175,   177,   180,   183
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      55,     0,    -1,    -1,    40,    -1,    39,    -1,    38,    -1,
      13,    -1,    11,    -1,    12,    -1,    19,    -1,    20,    -1,
      42,    -1,    42,     8,    37,    -1,    37,    -1,    43,    26,
      44,    27,    -1,    28,    50,    29,    -1,    40,    -1,    41,
      30,    41,    46,    -1,    45,    -1,    36,    -1,    41,    45,
      -1,    36,    -1,    30,    41,    46,    -1,    40,    -1,    26,
      48,    27,    -1,    48,    19,    48,    -1,    48,    20,    48,
      -1,    21,    48,    -1,    52,    -1,    40,    -1,    39,    -1,
      38,    -1,    26,    50,    27,    -1,    51,    54,    51,    -1,
      50,    -1,    49,    -1,    49,    -1,    51,    53,    51,    -1,
      47,    26,    44,    27,    -1,    31,    -1,    32,    -1,    33,
      -1,    16,    -1,    15,    -1,    14,    -1,    18,    -1,    22,
      -1,    23,    -1,    25,    -1,    24,    -1,    56,    17,    -1,
      57,    -1,    26,    57,    27,    -1,    56,    10,    56,    -1,
      63,    59,    60,    61,    -1,    58,    63,    60,    61,    -1,
      59,    60,    -1,    63,    -1,    60,    -1,    56,    34,    -1,
       6,    56,    -1,    36,    -1,     3,    48,    -1,    36,    -1,
       4,     5,    44,    62,    -1,    36,    -1,     9,    48,    -1,
      47,    44,    -1,    47,    26,    44,    27,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   119,   119,   122,   123,   124,   127,   130,   133,   134,
     135,   138,   139,   142,   143,   144,   149,   152,   153,   156,
     157,   160,   161,   164,   171,   172,   173,   174,   175,   178,
     179,   180,   185,   189,   196,   197,   201,   202,   203,   210,
     211,   212,   213,   214,   215,   216,   219,   220,   221,   222,
     226,   236,   237,   238,   241,   251,   261,   266,   267,   270,
     273,   276,   277,   280,   281,   284,   285,   288,   289
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
  "'<'", "'|'", "$accept", "null", "word", "number", "string", "id", 
  "arg", "argument", "function", "args", "spacedargs", "moreargs", "verb", 
  "boolean", "operand", "expression", "subexpression", "test", "op", 
  "arithop", "queryline", "query", "barequery", "pipedquery", "from", 
  "where", "group", "having", "verbphrase", 0
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
      44,    61,    62,    60,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    35,    36,    37,    37,    37,    38,    39,    40,    40,
      40,    41,    41,    42,    42,    42,    43,    44,    44,    45,
      45,    46,    46,    47,    48,    48,    48,    48,    48,    49,
      49,    49,    50,    50,    51,    51,    52,    52,    52,    53,
      53,    53,    53,    53,    53,    53,    54,    54,    54,    54,
      55,    56,    56,    56,    57,    57,    57,    57,    57,    58,
      59,    60,    60,    61,    61,    62,    62,    63,    63
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     4,     3,     1,     4,     1,     1,
       2,     1,     3,     1,     3,     3,     3,     2,     1,     1,
       1,     1,     3,     3,     1,     1,     1,     3,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     3,     3,     4,     4,     2,     1,     1,     2,
       2,     1,     2,     1,     4,     1,     2,     2,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     2,     8,     9,    10,     2,    61,    23,     2,
       0,     0,    51,     0,     2,    58,    57,     7,     6,     0,
       0,    31,    30,    29,     0,    62,    35,    34,     0,    28,
      60,     0,    51,     2,     0,    19,    13,     5,     4,     3,
       2,    11,     0,    67,    18,     1,     2,    50,    59,     2,
      56,     2,    27,     0,    34,     2,     0,     0,    44,    43,
      42,    45,    46,    47,    49,    48,    39,    40,    41,     0,
       0,    52,     0,     0,    29,    35,    34,     0,     0,     2,
      20,     0,     2,    53,     2,     2,    24,    32,     0,    25,
      26,    37,    33,    68,    15,     2,    12,     3,     0,     0,
      63,    55,    54,    38,     0,    21,    17,    14,     2,     2,
       2,    22,     0,    65,    64,    66
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     7,    36,    21,    22,    39,    40,    41,    42,    43,
      44,   106,    24,    25,    26,    27,    28,    29,    69,    70,
      10,    11,    12,    13,    14,    15,   101,   114,    16
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -38
static const short yypact[] =
{
     121,   184,   121,   -38,   -38,   -38,   121,   -38,   -38,   163,
       7,    14,   -38,    77,    20,   -38,    11,   -38,   -38,   184,
     184,   -38,   -38,     1,     4,    -8,   118,   -38,   135,   -38,
      -1,    -1,    -9,   166,   187,   -38,   -38,   -38,   -38,    10,
     143,    18,    13,   -38,   -38,   -38,   121,   -38,   -38,    20,
     -38,    20,   -38,   112,    16,   166,   184,   184,   -38,   -38,
     -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   187,
     187,   -38,    27,   187,   -38,   -38,     8,   195,   166,   166,
     -38,   203,   166,    -1,    36,    36,   -38,   -38,    31,   -38,
     -38,   195,   195,   -38,   -38,    42,   -38,   -38,    55,    63,
     -38,   -38,   -38,   -38,   166,   -38,   -38,   -38,   166,    42,
      67,   -38,   184,   -38,   -38,    -8
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -38,    -5,    25,     5,    38,     0,   -37,   -38,   -38,   -23,
     -35,   -21,    49,    -4,    29,   -12,    41,   -38,   -38,   -38,
     -38,    19,    85,   -38,    76,    15,     9,   -38,    87
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -37
static const yysigned_char yytable[] =
{
       8,    23,     8,    79,    35,    80,     8,    45,    54,    46,
      72,    56,    57,     8,    37,    52,    53,     2,    71,    23,
      23,    30,    76,     1,    46,    31,    81,   -23,    35,    50,
      55,    47,    88,    48,    74,    35,   -16,    94,    37,    82,
      99,    95,    79,    87,    80,    37,     8,    38,    48,     9,
      35,     9,    89,    90,    93,     9,    23,    23,   103,    98,
      37,    54,     9,    75,    84,    83,    85,   109,   108,    74,
      74,    38,   104,    74,    35,    77,   112,    35,    38,   100,
     100,    97,   107,    37,    37,   110,    37,    37,   111,     3,
     105,    32,    51,    38,   102,     9,     4,     5,    75,    75,
      49,     0,    75,    35,   105,   113,    96,     0,   115,    37,
      91,    92,    23,    37,    77,     0,    38,    38,     0,    38,
      38,   -36,   -36,     0,     1,     0,     0,     2,   -36,     0,
       0,    56,    57,     3,     0,   -36,     0,   -36,   -36,    86,
       4,     5,    38,     0,     0,   -36,    38,     6,     0,    58,
      59,    60,   -36,    61,    17,     3,    18,    62,    63,    64,
      65,     0,     4,     5,     0,     0,    66,    67,    68,     0,
       0,    34,     0,    78,    17,     3,    18,    17,     3,    18,
       0,     0,     4,     5,     0,     4,     5,     0,     0,    33,
       0,    34,     0,     0,    34,    17,     3,    18,    17,     3,
      18,     0,     0,     4,     5,    19,     4,     5,     0,     0,
      20,     0,     0,    73,    17,     3,    18,    62,    63,    64,
      65,     0,     4,     5
};

static const yysigned_char yycheck[] =
{
       0,     1,     2,    40,     9,    40,     6,     0,    20,    10,
      33,    19,    20,    13,     9,    19,    20,     6,    27,    19,
      20,     2,    34,     3,    10,     6,     8,    26,    33,    14,
      26,    17,    55,    34,    34,    40,    26,    29,    33,    26,
       4,    78,    79,    27,    79,    40,    46,     9,    34,     0,
      55,     2,    56,    57,    27,     6,    56,    57,    27,    82,
      55,    73,    13,    34,    49,    46,    51,   104,     5,    69,
      70,    33,    30,    73,    79,    34,     9,    82,    40,    84,
      85,    81,    27,    78,    79,   108,    81,    82,   109,    12,
      95,     6,    16,    55,    85,    46,    19,    20,    69,    70,
      13,    -1,    73,   108,   109,   110,    81,    -1,   112,   104,
      69,    70,   112,   108,    73,    -1,    78,    79,    -1,    81,
      82,     3,     4,    -1,     3,    -1,    -1,     6,    10,    -1,
      -1,    19,    20,    12,    -1,    17,    -1,    19,    20,    27,
      19,    20,   104,    -1,    -1,    27,   108,    26,    -1,    14,
      15,    16,    34,    18,    11,    12,    13,    22,    23,    24,
      25,    -1,    19,    20,    -1,    -1,    31,    32,    33,    -1,
      -1,    28,    -1,    30,    11,    12,    13,    11,    12,    13,
      -1,    -1,    19,    20,    -1,    19,    20,    -1,    -1,    26,
      -1,    28,    -1,    -1,    28,    11,    12,    13,    11,    12,
      13,    -1,    -1,    19,    20,    21,    19,    20,    -1,    -1,
      26,    -1,    -1,    26,    11,    12,    13,    22,    23,    24,
      25,    -1,    19,    20
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,     6,    12,    19,    20,    26,    36,    40,    47,
      55,    56,    57,    58,    59,    60,    63,    11,    13,    21,
      26,    38,    39,    40,    47,    48,    49,    50,    51,    52,
      56,    56,    57,    26,    28,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,     0,    10,    17,    34,    63,
      60,    59,    48,    48,    50,    26,    19,    20,    14,    15,
      16,    18,    22,    23,    24,    25,    31,    32,    33,    53,
      54,    27,    44,    26,    40,    49,    50,    51,    30,    41,
      45,     8,    26,    56,    60,    60,    27,    27,    44,    48,
      48,    51,    51,    27,    29,    41,    37,    40,    44,     4,
      36,    61,    61,    27,    30,    36,    46,    27,     5,    41,
      44,    46,     9,    36,    62,    48
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
#line 127 "parser.y"
    { yyval.string = yystring; }
    break;

  case 7:
#line 130 "parser.y"
    { yyval.string = yystring; }
    break;

  case 8:
#line 133 "parser.y"
    { yyval.string = yystring; }
    break;

  case 9:
#line 134 "parser.y"
    { yyval.string = "or"; }
    break;

  case 10:
#line 135 "parser.y"
    { yyval.string = "and"; }
    break;

  case 12:
#line 139 "parser.y"
    { yyval.arglist->rename = yyvsp[0].string; }
    break;

  case 13:
#line 142 "parser.y"
    { yyval.arglist = newarg(yyvsp[0].string, WORD, NULL); }
    break;

  case 14:
#line 143 "parser.y"
    { yyval.arglist = newarg(yyvsp[-3].string, FUNCTION, yyvsp[-1].arglist); }
    break;

  case 15:
#line 144 "parser.y"
    { yyval.arglist = newarg("expr", FUNCTION, 
					       newarg(print_operand(yyvsp[-1].operand), WORD, NULL)); 
			                  }
    break;

  case 17:
#line 152 "parser.y"
    { yyval.arglist = yyvsp[-3].arglist; yyval.arglist->next = yyvsp[-1].arglist; yyvsp[-1].arglist->next = yyvsp[0].arglist; }
    break;

  case 19:
#line 156 "parser.y"
    { yyval.arglist = NULL; }
    break;

  case 20:
#line 157 "parser.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 21:
#line 160 "parser.y"
    { yyval.arglist = NULL; }
    break;

  case 22:
#line 161 "parser.y"
    { yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; }
    break;

  case 24:
#line 171 "parser.y"
    { yyval.comp = yyvsp[-1].comp; }
    break;

  case 25:
#line 172 "parser.y"
    { yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, OR); }
    break;

  case 26:
#line 173 "parser.y"
    { yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, AND); }
    break;

  case 27:
#line 174 "parser.y"
    { yyval.comp = comp_join(yyvsp[0].comp, NULL, NOT); }
    break;

  case 29:
#line 178 "parser.y"
    { yyval.operand = comp_operand(FIELD, yyvsp[0].string); }
    break;

  case 30:
#line 179 "parser.y"
    { yyval.operand = comp_operand(CONST, yyvsp[0].string); }
    break;

  case 31:
#line 180 "parser.y"
    { yyval.operand = comp_operand(CONST, yyvsp[0].string); }
    break;

  case 32:
#line 186 "parser.y"
    {	  
				  yyval.operand = yyvsp[-1].operand;  
				}
    break;

  case 33:
#line 190 "parser.y"
    {
				  yyval.operand = comp_arith(parse_tenv, yyvsp[-1].arithop, yyvsp[-2].operand, yyvsp[0].operand); 
				}
    break;

  case 36:
#line 201 "parser.y"
    {  yyval.comp = comp_new(EXIST, yyvsp[0].operand, yyvsp[0].operand); }
    break;

  case 37:
#line 202 "parser.y"
    { yyval.comp = comp_new(yyvsp[-1].op, yyvsp[-2].operand, yyvsp[0].operand); }
    break;

  case 38:
#line 203 "parser.y"
    { 
				  int argc; char ** argv;
				  arglist2argv(yyvsp[-1].arglist, &argc, &argv);
				  yyval.comp = comp_new_func(yyvsp[-3].string, argc, argv, yyvsp[-1].arglist);
				}
    break;

  case 39:
#line 210 "parser.y"
    { yyval.op = EQ; }
    break;

  case 40:
#line 211 "parser.y"
    { yyval.op = GT; }
    break;

  case 41:
#line 212 "parser.y"
    { yyval.op = LT; }
    break;

  case 42:
#line 213 "parser.y"
    { yyval.op = GEQ; }
    break;

  case 43:
#line 214 "parser.y"
    { yyval.op = LEQ; }
    break;

  case 44:
#line 215 "parser.y"
    { yyval.op = NEQ; }
    break;

  case 45:
#line 216 "parser.y"
    { yyval.op = LIKE; }
    break;

  case 46:
#line 219 "parser.y"
    { yyval.arithop = ADD; }
    break;

  case 47:
#line 220 "parser.y"
    { yyval.arithop = SUB; }
    break;

  case 48:
#line 221 "parser.y"
    { yyval.arithop = DIVIDE; }
    break;

  case 49:
#line 222 "parser.y"
    { yyval.arithop = MULT; }
    break;

  case 50:
#line 227 "parser.y"
    { 
#ifdef DEBUG
	   	smacq_graph_print(stderr, yyvsp[-1].graph.head, 0); 
#endif
		Graph = yyvsp[-1].graph.head;
		return 0;
	   }
    break;

  case 52:
#line 237 "parser.y"
    { yyval.graph = yyvsp[-1].graph; }
    break;

  case 53:
#line 238 "parser.y"
    { smacq_graph * g = yyvsp[-2].graph.head; while(g->next_graph) g=g->next_graph; g->next_graph = yyvsp[0].graph.head; yyval.graph = yyvsp[-2].graph; }
    break;

  case 54:
#line 242 "parser.y"
    {
	   	yyval.graph = yyvsp[-2].graph;
		graph_join(&(yyval.graph), yyvsp[-1].graph);
		if (yyvsp[0].group.args) {
			graph_join(&(yyval.graph), newgroup(yyvsp[0].group, yyvsp[-3].vphrase));
		} else {
			graph_join(&(yyval.graph), newmodule(yyvsp[-3].vphrase.verb, yyvsp[-3].vphrase.args));
		}
	   }
    break;

  case 55:
#line 252 "parser.y"
    {
	   	yyval.graph = yyvsp[-3].graph;
		graph_join(&(yyval.graph), yyvsp[-1].graph);
		if (yyvsp[0].group.args) {
			graph_join(&(yyval.graph), newgroup(yyvsp[0].group, yyvsp[-2].vphrase));
		} else {
			graph_join(&(yyval.graph), newmodule(yyvsp[-2].vphrase.verb, yyvsp[-2].vphrase.args));
		}
	   }
    break;

  case 56:
#line 262 "parser.y"
    {
	   	yyval.graph = yyvsp[-1].graph;
		graph_join(&(yyval.graph), yyvsp[0].graph);
	   }
    break;

  case 57:
#line 266 "parser.y"
    { yyval.graph = newmodule(yyvsp[0].vphrase.verb, yyvsp[0].vphrase.args); }
    break;

  case 59:
#line 270 "parser.y"
    { yyval.graph = yyvsp[-1].graph; }
    break;

  case 60:
#line 273 "parser.y"
    { yyval.graph = yyvsp[0].graph; }
    break;

  case 61:
#line 276 "parser.y"
    { yyval.graph = nullgraph; }
    break;

  case 62:
#line 277 "parser.y"
    { yyval.graph = optimize_bools(yyvsp[0].comp); }
    break;

  case 63:
#line 280 "parser.y"
    { yyval.group.args = NULL; yyval.group.having = NULL;}
    break;

  case 64:
#line 281 "parser.y"
    { yyval.group.args = yyvsp[-1].arglist; yyval.group.having = yyvsp[0].comp; }
    break;

  case 65:
#line 284 "parser.y"
    { yyval.comp = NULL; }
    break;

  case 66:
#line 285 "parser.y"
    { yyval.comp = yyvsp[0].comp; }
    break;

  case 67:
#line 288 "parser.y"
    { yyval.vphrase = newvphrase(yyvsp[-1].string, yyvsp[0].arglist); }
    break;

  case 68:
#line 289 "parser.y"
    { yyval.vphrase = newvphrase(yyvsp[-3].string, yyvsp[-1].arglist); }
    break;


    }

/* Line 999 of yacc.c.  */
#line 1494 "y.tab.c"

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


#line 292 "parser.y"


extern void yysmacql_scan_string(char*);

#ifndef SMACQ_OPT_NOPTHREADS
#ifdef PTHREAD_MUTEX_INITIALIZER
  static pthread_mutex_t local_lock = PTHREAD_MUTEX_INITIALIZER;
#else
  static pthread_mutex_t local_lock;
  #warning "No PTHREAD_MUTEX_INITIALIZER"
#endif
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
  fprintf(stderr, "%s near %s\n", msg, yytext-1);
}


