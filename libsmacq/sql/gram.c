/* A Bison parser, made from gram.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	ALL	257
# define	AND	258
# define	ANY	259
# define	ASC	260
# define	BY	261
# define	DESC	262
# define	DISTINCT	263
# define	FROM	264
# define	GROUP	265
# define	NOT	266
# define	OR	267
# define	ORDER	268
# define	PRINT	269
# define	SELECT	270
# define	USING	271
# define	WHERE	272
# define	DELTA	273
# define	UNIQ	274
# define	TOP	275
# define	LAST	276
# define	SPLIT	277
# define	COUNTER	278
# define	DERIVATIVE	279
# define	PCAPFILE	280
# define	PCAPLIVE	281
# define	IDENT	282
# define	FCONST	283
# define	SCONST	284
# define	BITCONST	285
# define	Op	286
# define	ICONST	287
# define	PARAM	288
# define	OP	289
# define	POSTFIXOP	290
# define	UMINUS	291
# define	TYPECAST	292

#line 1 "gram.y"


#ifdef SM_DEBUG
#define YYDEBUG 1
#endif
/*-------------------------------------------------------------------------
 *
 * gram.y
 *	  POSTGRES SQL YACC rules/actions
 *
 * Portions Copyright (c) 1996-2001, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  $Header: /cvsroot/smacq/smacq/libsmacq/sql/gram.y,v 1.6 2002/12/16 22:51:49 wbarber Exp $
 *
 * HISTORY
 *	  AUTHOR			DATE			MAJOR EVENT
 *	  Andrew Yu			Sept, 1994		POSTQUEL to SQL conversion
 *	  Andrew Yu			Oct, 1994		lispy code conversion
 *
 * NOTES
 *	  CAPITALS are used to represent terminal symbols.
 *	  non-capitals are used to represent non-terminals.
 *	  SQL92-specific syntax is separated from plain SQL/Postgres syntax
 *	  to help isolate the non-extensible portions of the parser.
 *
 *	  In general, nothing in this file should initiate database accesses
 *	  nor depend on changeable state (such as SET variables).  If you do
 *	  database accesses, your code will fail when we have aborted the
 *	  current transaction and are just parsing commands to find the next
 *	  ROLLBACK or COMMIT.  If you make use of SET variables, then you
 *	  will do the wrong thing in multi-query strings like this:
 *			SET SQL_inheritance TO off; SELECT * FROM foo;
 *	  because the entire string is parsed by gram.y before the SET gets
 *	  executed.  Anything that depends on the database or changeable state
 *	  should be handled inside parse_analyze() so that it happens at the
 *	  right time not the wrong time.  The handling of SQL_inheritance is
 *	  a good example.
 *
 * WARNINGS
 *	  If you use a list, make sure the datum is a node so that the printing
 *	  routines work.
 *
 *	  Sometimes we assign constants to makeStrings. Make sure we don't free
 *	  those.
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "access/htup.h"
//#include "catalog/pg_type.h"
#include "nodes/params.h"
#include "parsenodes.h" // this now has modified SelectStmt struct (wbarber)
#include "parser/gramparse.h"
//#include "storage/lmgr.h"
//#include "utils/acl.h"
//#include "utils/numeric.h"

#include "pg_list.h"

#ifdef MULTIBYTE
#include "mb/pg_wchar.h"
#else
#define GetStandardEncoding()	0		/* PG_SQL_ASCII */
#define GetStandardEncodingName()	"SQL_ASCII"
#endif

extern List *parsetree;			/* final parse result is delivered here */

static bool QueryIsRule = FALSE;
static Oid	*param_type_info;
static int	pfunc_num_args;


/*
 * If you need access to certain yacc-generated variables and find that
 * they're static by default, uncomment the next line.  (this is not a
 * problem, yet.)
 */
/*#define __YYSCLASS*/

static Node *makeA_Expr(int oper, char *opname, Node *lexpr, Node *rexpr);
/* static Node *makeTypeCast(Node *arg, TypeName *typename); */
static Node *makeStringConst(char *str, TypeName *typename);
/* static Node *makeFloatConst(char *str); */
static Node *makeRowExpr(char *opr, List *largs, List *rargs);
static SelectStmt *findLeftmostSelect(SelectStmt *node);
static Node *makeSetOp(SetOperation op, bool all, Node *larg, Node *rarg);
/* static Node *doNegate(Node *n); */
/* static void doNegateFloat(Value *v); */

#define MASK(b) (1 << (b))


#line 106 "gram.y"
#ifndef YYSTYPE
typedef union
{
	int					ival;
	char				chr;
	char				*str;
	bool				boolean;
	List				*list;
	Node				*node;
	Value				*value;

	Attr				*attr;
	Ident				*ident;

	DefElem				*defelt;
	SortGroupBy			*sortgroupby;
	RangeVar			*range;
	ResTarget			*target;
	ParamNo				*paramno;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		211
#define	YYFLAG		-32768
#define	YYNTBASE	56

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 292 ? yytranslate[x] : 102)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    44,     2,     2,
      50,    51,    42,    40,    54,    41,    47,    43,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    55,    53,
      37,    36,    38,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    48,     2,    49,    45,     2,     2,     2,     2,     2,
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
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      39,    46,    52
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     6,     8,    10,    11,    13,    15,    19,
      23,    25,    28,    30,    32,    40,    42,    44,    46,    48,
      50,    53,    55,    56,    58,    60,    61,    63,    65,    66,
      70,    72,    76,    79,    82,    84,    86,    87,    89,    91,
      95,    99,   100,   103,   104,   108,   110,   114,   116,   121,
     124,   127,   129,   131,   133,   136,   139,   140,   147,   153,
     161,   165,   169,   171,   173,   175,   177,   179,   181,   183,
     185,   187,   189,   191,   193,   195,   197,   199,   202,   205,
     208,   211,   214,   217,   221,   225,   229,   233,   237,   241,
     245,   249,   253,   257,   260,   263,   267,   271,   274,   279,
     281,   283,   286,   289,   292,   295,   298,   301,   305,   309,
     313,   317,   321,   325,   329,   333,   337,   341,   344,   347,
     349,   352,   354,   358,   362,   367,   370,   373,   378,   384,
     390,   395,   397,   402,   409,   410,   412,   416,   420,   423,
     425,   427,   431,   433,   437,   442,   447,   449,   453,   457,
     460,   461,   465,   467,   469,   471,   475,   477,   479,   481,
     483,   485,   487,   489,   491,   494,   496,   498,   500,   502,
     504,   506,   508,   510,   512,   514,   516,   518,   520,   522,
     524,   526,   528,   530,   532,   534,   536,   538,   540,   542,
     544,   546,   548,   550,   552,   554,   556,   558
};
static const short yyrhs[] =
{
      57,     0,    57,    53,    58,     0,    58,     0,    59,     0,
       0,    61,     0,    60,     0,    50,    61,    51,     0,    50,
      60,    51,     0,    63,     0,    62,    68,     0,    63,     0,
      60,     0,    64,    65,    67,    91,    73,    78,    72,     0,
      16,     0,    15,     0,    20,     0,    21,     0,    22,     0,
      65,    66,     0,    66,     0,     0,    95,     0,     3,     0,
       0,     9,     0,     3,     0,     0,    14,     7,    69,     0,
      70,     0,    69,    54,    70,     0,    85,    71,     0,    17,
      83,     0,     6,     0,     8,     0,     0,   104,     0,   110,
       0,   104,    54,   110,     0,    11,     7,    88,     0,     0,
      10,    74,     0,     0,    74,    54,    76,     0,    76,     0,
      74,    54,    75,     0,    75,     0,   100,    50,    98,    51,
       0,   100,    98,     0,   100,    99,     0,    77,     0,    60,
       0,    93,     0,    93,    42,     0,    18,    85,     0,     0,
      50,    80,    51,    83,    82,    60,     0,    50,    80,    51,
      83,    60,     0,    50,    80,    51,    83,    50,    80,    51,
       0,    81,    54,    85,     0,    81,    54,    85,     0,    85,
       0,     5,     0,     3,     0,    32,     0,    84,     0,    40,
       0,    41,     0,    42,     0,    43,     0,    44,     0,    45,
       0,    37,     0,    38,     0,    36,     0,    86,     0,    40,
      85,     0,    41,    85,     0,    44,    85,     0,    45,    85,
       0,    85,    44,     0,    85,    45,     0,    85,    40,    85,
       0,    85,    41,    85,     0,    85,    42,    85,     0,    85,
      43,    85,     0,    85,    44,    85,     0,    85,    45,    85,
       0,    85,    37,    85,     0,    85,    38,    85,     0,    85,
      36,    85,     0,    85,    32,    85,     0,    32,    85,     0,
      85,    32,     0,    85,     4,    85,     0,    85,    13,    85,
       0,    12,    85,     0,    85,    83,    82,    60,     0,    79,
       0,    86,     0,    40,   105,     0,    41,   105,     0,    44,
     105,     0,    45,   105,     0,   105,    44,     0,   105,    45,
       0,   105,    40,   105,     0,   105,    41,   105,     0,   105,
      42,   105,     0,   105,    43,   105,     0,   105,    44,   105,
       0,   105,    45,   105,     0,   105,    37,   105,     0,   105,
      38,   105,     0,   105,    36,   105,     0,   105,    32,   105,
       0,    32,   105,     0,   105,    32,     0,    89,     0,    99,
      87,     0,    95,     0,    50,    85,    51,     0,   100,    50,
      51,     0,   100,    50,    98,    51,     0,   100,    98,     0,
     100,    99,     0,   100,    50,    88,    51,     0,   100,    50,
       3,    88,    51,     0,   100,    50,     9,    88,    51,     0,
     100,    50,    42,    51,     0,    60,     0,    87,    48,    85,
      49,     0,    87,    48,    85,    55,    85,    49,     0,     0,
      85,     0,    88,    54,    85,     0,    85,    10,    88,     0,
      10,    88,     0,    88,     0,    60,     0,    50,   108,    51,
       0,    85,     0,   108,    54,    85,     0,    93,    47,    90,
      87,     0,    96,    47,    90,    87,     0,    94,     0,    90,
      47,    94,     0,    90,    47,    42,     0,    50,    51,     0,
       0,    91,    54,    92,     0,    92,     0,    85,     0,    98,
       0,    93,    47,    42,     0,    42,     0,    99,     0,    99,
       0,    99,     0,    97,     0,    29,     0,    98,     0,    96,
       0,    34,    87,     0,    33,     0,    30,     0,    28,     0,
      28,     0,    28,     0,   101,     0,    28,     0,   101,     0,
     113,     0,    24,     0,    25,     0,    26,     0,    27,     0,
      23,     0,    19,     0,     3,     0,     4,     0,     5,     0,
       6,     0,     8,     0,     9,     0,    10,     0,    11,     0,
      22,     0,    12,     0,    13,     0,    14,     0,    15,     0,
      16,     0,    21,     0,    20,     0,    17,     0,    18,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   241,   249,   255,   263,   264,   314,   315,   318,   322,
     328,   332,   339,   340,   366,   413,   414,   415,   416,   417,
     420,   422,   424,   428,   431,   432,   438,   439,   440,   443,
     446,   447,   450,   458,   459,   460,   461,   472,   474,   476,
     480,   485,   496,   497,   500,   501,   503,   504,   507,   523,
     538,   562,   567,   589,   597,   607,   608,   622,   635,   648,
     654,   660,   664,   670,   671,   674,   674,   676,   677,   678,
     679,   680,   681,   682,   683,   684,   703,   714,   716,   718,
     720,   722,   724,   726,   728,   730,   732,   734,   736,   738,
     740,   742,   745,   747,   749,   752,   754,   756,   759,   769,
     782,   784,   786,   788,   790,   792,   794,   796,   798,   800,
     802,   804,   806,   808,   810,   812,   815,   817,   819,   831,
     833,   841,   843,   845,   854,   867,   880,   893,   902,   915,
     924,   949,   965,   972,   979,   983,   985,   989,   991,   993,
     997,  1003,  1007,  1009,  1013,  1021,  1031,  1033,  1035,  1039,
    1040,  1051,  1053,  1058,  1065,  1076,  1088,  1108,  1114,  1115,
    1120,  1127,  1134,  1141,  1145,  1153,  1154,  1169,  1174,  1179,
    1180,  1186,  1187,  1188,  1226,  1228,  1229,  1230,  1231,  1232,
    1249,  1251,  1252,  1253,  1254,  1255,  1256,  1257,  1258,  1259,
    1260,  1261,  1262,  1263,  1264,  1265,  1266,  1267
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "ALL", "AND", "ANY", "ASC", "BY", "DESC", 
  "DISTINCT", "FROM", "GROUP", "NOT", "OR", "ORDER", "PRINT", "SELECT", 
  "USING", "WHERE", "DELTA", "UNIQ", "TOP", "LAST", "SPLIT", "COUNTER", 
  "DERIVATIVE", "PCAPFILE", "PCAPLIVE", "IDENT", "FCONST", "SCONST", 
  "BITCONST", "Op", "ICONST", "PARAM", "OP", "'='", "'<'", "'>'", 
  "POSTFIXOP", "'+'", "'-'", "'*'", "'/'", "'%'", "'^'", "UMINUS", "'.'", 
  "'['", "']'", "'('", "')'", "TYPECAST", "';'", "','", "':'", 
  "stmtblock", "stmtmulti", "stmt", "SelectStmt", "select_with_parens", 
  "select_no_parens", "select_clause", "simple_select", "select_word", 
  "option_list", "option", "opt_distinct", "sort_clause", "sortby_list", 
  "sortby", "OptUseOp", "group_clause", "from_clause", "from_list", 
  "data_func", "table_ref", "relation_expr", "where_clause", "row_expr", 
  "row_descriptor", "row_list", "sub_type", "all_Op", "MathOp", "a_expr", 
  "c_expr", "opt_indirection", "expr_list", "attr", "attrs", 
  "target_list", "target_el", "relation_name", "attr_name", "AexprConst", 
  "ParamNo", "Iconst", "Sconst", "ColId", "func_name", 
  "func_name_keyword", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    56,    57,    57,    58,    58,    59,    59,    60,    60,
      61,    61,    62,    62,    63,    64,    64,    64,    64,    64,
      65,    65,    65,    66,   102,   102,    67,    67,    67,    68,
      69,    69,    70,    71,    71,    71,    71,   103,   104,   104,
      72,    72,    73,    73,    74,    74,    74,    74,    75,    75,
      75,    76,    76,    77,    77,    78,    78,    79,    79,    79,
      80,    81,    81,    82,    82,    83,    83,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   105,    86,
      86,    86,    86,    86,    86,    86,    86,    86,    86,    86,
      86,    86,    87,    87,    87,    88,    88,   106,   106,   106,
     107,   107,   108,   108,    89,    89,    90,    90,    90,   109,
     109,    91,    91,    92,    92,    92,    92,    93,   110,    94,
      95,    95,    95,    95,    96,    97,    98,    99,   111,   100,
     100,   112,   112,   112,   101,   101,   101,   101,   101,   101,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     3,     1,     1,     0,     1,     1,     3,     3,
       1,     2,     1,     1,     7,     1,     1,     1,     1,     1,
       2,     1,     0,     1,     1,     0,     1,     1,     0,     3,
       1,     3,     2,     2,     1,     1,     0,     1,     1,     3,
       3,     0,     2,     0,     3,     1,     3,     1,     4,     2,
       2,     1,     1,     1,     2,     2,     0,     6,     5,     7,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     2,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     3,     3,     2,     4,     1,
       1,     2,     2,     2,     2,     2,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     1,
       2,     1,     3,     3,     4,     2,     2,     4,     5,     5,
       4,     1,     4,     6,     0,     1,     3,     3,     2,     1,
       1,     3,     1,     3,     4,     4,     1,     3,     3,     2,
       0,     3,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       5,    16,    15,    17,    18,    19,     0,     1,     3,     4,
       7,     6,     0,    10,    22,    13,     0,     5,     0,    11,
     161,   166,   165,   134,    28,    21,    23,   163,   160,   162,
       9,     8,     2,     0,   164,    27,    26,    20,     0,     0,
     179,   178,   174,   175,   176,   177,   167,     0,     0,     0,
       0,     0,     0,   131,    29,    30,    99,    36,    76,   119,
       0,   121,   163,   134,     0,   170,     0,   156,   153,    43,
     152,     0,   162,    97,    93,    77,    78,    79,    80,   131,
       0,     0,    62,     0,     0,    34,    35,     0,     0,    94,
      75,    73,    74,    67,    68,    69,    70,    81,    82,    32,
       0,    66,     0,     0,   120,   167,     0,   125,   126,     0,
       0,     0,    56,     0,     0,     0,   122,    31,    95,    96,
      65,    75,    73,    74,    67,    68,    69,    70,    71,    72,
      33,    92,    91,    89,    90,    83,    84,    85,    86,    87,
      88,    64,    63,     0,   134,   146,   159,   134,     0,     0,
       0,   123,   135,     0,   162,   132,     0,    52,    42,    47,
      45,    51,    53,   157,     0,   151,     0,    41,   155,     0,
      60,    98,     0,   144,   145,     0,     0,   130,   127,     0,
     124,     0,     0,    54,     0,    49,    50,    55,     0,    14,
       0,    58,     0,   148,   147,   128,   129,   136,   133,    46,
      44,     0,     0,     0,    62,    57,    48,    40,    59,     0,
       0,     0
};

static const short yydefgoto[] =
{
     209,     7,     8,     9,    53,    16,    12,    13,    14,    24,
      25,    38,    19,    54,    55,    99,   189,   112,   158,   159,
     160,   161,   167,    56,    80,    81,   143,   100,   101,   152,
      58,    34,   153,    59,   144,    69,    70,    60,   145,    61,
      62,    28,    29,    63,    64,    65
};

static const short yypact[] =
{
      73,-32768,-32768,-32768,-32768,-32768,    73,   -20,-32768,-32768,
      28,-32768,    58,    77,   170,    51,    53,    73,    99,-32768,
  -32768,-32768,-32768,-32768,   100,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,   471,    68,-32768,-32768,-32768,   437,   471,
  -32768,-32768,-32768,-32768,-32768,-32768,     1,   471,   471,   471,
     471,   471,   407,-32768,    54,-32768,-32768,   338,-32768,-32768,
      84,-32768,    85,    89,    55,-32768,   471,-32768,   359,     3,
  -32768,   104,    25,   482,    56,-32768,-32768,    79,-32768,    -3,
      91,    98,   289,   471,   471,-32768,-32768,   471,   496,   220,
     471,   471,   471,   471,   471,   471,   471,   254,   286,-32768,
      44,-32768,   125,   125,    68,-32768,   186,-32768,-32768,   122,
      94,   437,   137,   -12,   496,   471,-32768,-32768,   482,   373,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,    56,   482,   510,   510,    96,    96,    79,    79,    79,
  -32768,-32768,-32768,   107,   121,-32768,-32768,   121,   471,   471,
     119,-32768,   359,   -10,   123,-32768,   471,-32768,   118,-32768,
  -32768,-32768,   131,-32768,    62,-32768,   471,   165,-32768,     9,
     143,-32768,    -4,    68,    68,    60,    95,-32768,-32768,   471,
  -32768,   324,    94,-32768,   148,-32768,-32768,   359,   184,-32768,
     407,-32768,   107,-32768,-32768,-32768,-32768,   359,-32768,-32768,
  -32768,   142,   471,   145,   359,-32768,-32768,   140,-32768,   201,
     202,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,   189,-32768,     0,    15,-32768,-32768,-32768,-32768,
     183,-32768,-32768,-32768,   134,-32768,-32768,-32768,-32768,    39,
      40,-32768,-32768,-32768,    18,-32768,    64,   -80,-32768,   -29,
  -32768,   -60,  -122,-32768,   126,-32768,   113,   -37,    63,    26,
      57,-32768,   -36,   -57,  -105,-32768
};


#define	YYLAST		555


static const short yytable[] =
{
      10,    71,    72,   104,    57,   164,    15,   108,   130,    68,
      73,   -13,   141,   110,   142,    11,   105,    10,    74,    75,
      76,    77,    78,    82,   105,  -154,   175,   176,   107,  -169,
     168,  -169,    11,    17,   169,  -154,  -154,   109,   193,  -154,
      26,   178,   -13,  -154,   179,   146,   146,   141,    30,   142,
      26,  -169,    79,   163,    57,   118,   146,   111,   119,   190,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     154,    27,    18,   162,    71,    72,  -154,   164,  -154,  -154,
     207,    27,    68,   105,   173,    21,   170,   174,     1,     2,
     105,   -12,    21,     3,     4,     5,    93,    94,    95,    96,
      97,    98,    30,    35,    31,   106,    33,   186,    83,    36,
     157,   195,   184,    40,   179,   146,    66,    41,    42,    43,
      44,    45,    46,     6,    98,   163,    84,   181,   185,    20,
      21,   102,   103,    22,    23,    87,  -157,   187,    95,    96,
      97,    98,   114,   171,     6,   162,   196,    84,   201,   179,
     197,   113,   115,   105,    89,   166,    87,     6,    90,    91,
      92,   204,    93,    94,    95,    96,    97,    98,   172,   191,
     177,   155,   182,   183,   180,    89,   188,   156,    21,    90,
      91,    92,   157,    93,    94,    95,    96,    97,    98,   148,
      79,   202,   205,   206,   179,   149,   208,   -61,    39,    20,
      21,   210,   211,    22,    23,    40,    32,    37,   203,    41,
      42,    43,    44,    45,    46,    20,    21,   117,    47,    22,
      23,   199,   200,   -65,   165,   -65,    48,    49,   150,   147,
      50,    51,    39,   192,     0,   194,    52,   151,     0,    40,
       0,     0,     0,    41,    42,    43,    44,    45,    46,    20,
      21,     0,    47,    22,    23,     0,     0,   -71,     0,   -71,
      48,    49,     0,     0,    50,    51,    39,     0,     0,     0,
      52,     0,     0,    40,     0,     0,     0,    41,    42,    43,
      44,    45,    46,    20,    21,     0,     0,    22,    23,   -72,
       0,   -72,     0,    84,     0,     0,     0,     0,    39,    51,
       0,     0,    87,     0,    52,    40,     0,     0,     0,    41,
      42,    43,    44,    45,    46,    20,    21,     0,     0,    22,
      23,    89,     0,     0,     0,    90,    91,    92,    84,    93,
      94,    95,    96,    97,    98,     0,    52,    87,     0,     0,
     116,     0,    84,     0,    85,     0,    86,     0,     0,     0,
       0,    87,     0,     0,     0,    88,    89,     0,     0,     0,
      90,    91,    92,    84,    93,    94,    95,    96,    97,    98,
      89,     0,    87,   198,    90,    91,    92,    84,    93,    94,
      95,    96,    97,    98,     0,     0,     0,     0,     0,     0,
       0,    89,     0,     0,     0,    90,    91,    92,     0,    93,
      94,    95,    96,    97,    98,    89,     0,     0,     0,    90,
      91,    92,     0,    93,    94,    95,    96,    97,    98,    39,
       0,     0,     1,     2,     0,     0,    40,     3,     4,     5,
      41,    42,    43,    44,    45,    46,    20,    21,     0,    47,
      22,    23,     0,     0,     0,     0,     0,    48,    49,    39,
       0,    50,    51,     0,     0,     0,    40,    52,     0,     0,
      41,    42,    43,    44,    45,    46,    20,    21,     0,    47,
      22,    23,     0,     0,     0,     0,     0,    48,    49,    67,
       0,    50,    51,    39,     0,     0,     0,    52,     0,     0,
      40,     0,     0,     0,    41,    42,    43,    44,    45,    46,
      20,    21,     0,    47,    22,    23,     0,     0,     0,     0,
       0,    48,    49,     0,    89,    50,    51,     0,    90,    91,
      92,    52,    93,    94,    95,    96,    97,    98,   120,     0,
       0,     0,   121,   122,   123,     0,   124,   125,   126,   127,
     128,   129,    89,     0,     0,     0,     0,-32768,-32768,     0,
      93,    94,    95,    96,    97,    98
};

static const short yycheck[] =
{
       0,    38,    38,    63,    33,   110,     6,    64,    88,    38,
      39,    14,     3,    10,     5,     0,    28,    17,    47,    48,
      49,    50,    51,    52,    28,     0,   148,   149,    64,    28,
      42,    30,    17,    53,   114,    10,    11,    66,    42,    14,
      14,    51,    14,    18,    54,   102,   103,     3,    51,     5,
      24,    50,    52,   110,    83,    84,   113,    54,    87,    50,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
     106,    14,    14,   110,   111,   111,    51,   182,    53,    54,
     202,    24,   111,    28,   144,    30,   115,   147,    15,    16,
      28,    14,    30,    20,    21,    22,    40,    41,    42,    43,
      44,    45,    51,     3,    51,    50,     7,   164,    54,     9,
     110,    51,    50,    19,    54,   172,    48,    23,    24,    25,
      26,    27,    28,    50,    45,   182,     4,   156,   164,    29,
      30,    47,    47,    33,    34,    13,    47,   166,    42,    43,
      44,    45,    51,   143,    50,   182,    51,     4,   184,    54,
     179,    47,    54,    28,    32,    18,    13,    50,    36,    37,
      38,   190,    40,    41,    42,    43,    44,    45,    47,   169,
      51,    49,    54,    42,    51,    32,    11,    55,    30,    36,
      37,    38,   182,    40,    41,    42,    43,    44,    45,     3,
     190,     7,   192,    51,    54,     9,    51,    54,    12,    29,
      30,     0,     0,    33,    34,    19,    17,    24,   190,    23,
      24,    25,    26,    27,    28,    29,    30,    83,    32,    33,
      34,   182,   182,     3,   111,     5,    40,    41,    42,   103,
      44,    45,    12,   169,    -1,   172,    50,    51,    -1,    19,
      -1,    -1,    -1,    23,    24,    25,    26,    27,    28,    29,
      30,    -1,    32,    33,    34,    -1,    -1,     3,    -1,     5,
      40,    41,    -1,    -1,    44,    45,    12,    -1,    -1,    -1,
      50,    -1,    -1,    19,    -1,    -1,    -1,    23,    24,    25,
      26,    27,    28,    29,    30,    -1,    -1,    33,    34,     3,
      -1,     5,    -1,     4,    -1,    -1,    -1,    -1,    12,    45,
      -1,    -1,    13,    -1,    50,    19,    -1,    -1,    -1,    23,
      24,    25,    26,    27,    28,    29,    30,    -1,    -1,    33,
      34,    32,    -1,    -1,    -1,    36,    37,    38,     4,    40,
      41,    42,    43,    44,    45,    -1,    50,    13,    -1,    -1,
      51,    -1,     4,    -1,     6,    -1,     8,    -1,    -1,    -1,
      -1,    13,    -1,    -1,    -1,    17,    32,    -1,    -1,    -1,
      36,    37,    38,     4,    40,    41,    42,    43,    44,    45,
      32,    -1,    13,    49,    36,    37,    38,     4,    40,    41,
      42,    43,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    -1,    -1,    -1,    36,    37,    38,    -1,    40,
      41,    42,    43,    44,    45,    32,    -1,    -1,    -1,    36,
      37,    38,    -1,    40,    41,    42,    43,    44,    45,    12,
      -1,    -1,    15,    16,    -1,    -1,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    -1,    32,
      33,    34,    -1,    -1,    -1,    -1,    -1,    40,    41,    12,
      -1,    44,    45,    -1,    -1,    -1,    19,    50,    -1,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    -1,    32,
      33,    34,    -1,    -1,    -1,    -1,    -1,    40,    41,    42,
      -1,    44,    45,    12,    -1,    -1,    -1,    50,    -1,    -1,
      19,    -1,    -1,    -1,    23,    24,    25,    26,    27,    28,
      29,    30,    -1,    32,    33,    34,    -1,    -1,    -1,    -1,
      -1,    40,    41,    -1,    32,    44,    45,    -1,    36,    37,
      38,    50,    40,    41,    42,    43,    44,    45,    32,    -1,
      -1,    -1,    36,    37,    38,    -1,    40,    41,    42,    43,
      44,    45,    32,    -1,    -1,    -1,    -1,    37,    38,    -1,
      40,    41,    42,    43,    44,    45
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

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

#line 315 "/usr/share/bison/bison.simple"


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

case 1:
#line 242 "gram.y"
{ 
				  parsetree = yyvsp[0].list;
				  //fprintf(stderr, "stmtblock: parsetree = %p\n", parsetree); // wbarber
				}
    break;
case 2:
#line 250 "gram.y"
{ if (yyvsp[0].node != (Node *)NULL)
					yyval.list = lappend(yyvsp[-2].list, yyvsp[0].node);
				  else
					yyval.list = yyvsp[-2].list;
				}
    break;
case 3:
#line 256 "gram.y"
{ if (yyvsp[0].node != (Node *)NULL)
					yyval.list = makeList1(yyvsp[0].node);
				  else
					yyval.list = NIL;
				}
    break;
case 5:
#line 265 "gram.y"
{ yyval.node = (Node *)NULL; }
    break;
case 8:
#line 319 "gram.y"
{
				yyval.node = yyvsp[-1].node;
			}
    break;
case 9:
#line 323 "gram.y"
{
				yyval.node = yyvsp[-1].node;
			}
    break;
case 10:
#line 329 "gram.y"
{
				yyval.node = yyvsp[0].node;
			}
    break;
case 11:
#line 333 "gram.y"
{
				((SelectStmt *)yyvsp[-1].node)->sortClause = (List *)yyvsp[0].list;
				yyval.node = yyvsp[-1].node;
			}
    break;
case 14:
#line 370 "gram.y"
{
					SelectStmt *n = makeNode(SelectStmt);
					if (strcmp(yyvsp[-6].str, "print") == 0) {
					  n->isPrint = TRUE;
					}
					else {
					  n->isPrint = FALSE;
					}
					n->functionname = yyvsp[-6].str;
					n->optionList = yyvsp[-5].list;
					n->distinctClause = yyvsp[-4].list;
					n->targetList = yyvsp[-3].list;
					n->fromClause = yyvsp[-2].list;
					n->whereClause = yyvsp[-1].node;
					n->groupClause = yyvsp[0].list;
					yyval.node = (Node *)n;
				}
    break;
case 15:
#line 413 "gram.y"
{ yyval.str = "select"; }
    break;
case 16:
#line 414 "gram.y"
{ yyval.str = "print"; }
    break;
case 17:
#line 415 "gram.y"
{ yyval.str = "uniq"; }
    break;
case 18:
#line 416 "gram.y"
{ yyval.str = "top"; }
    break;
case 19:
#line 417 "gram.y"
{ yyval.str = "last"; }
    break;
case 20:
#line 421 "gram.y"
{	yyval.list = lappend(yyvsp[-1].list, yyvsp[0].node);  }
    break;
case 21:
#line 423 "gram.y"
{	yyval.list = makeList1(yyvsp[0].node);  }
    break;
case 22:
#line 425 "gram.y"
{ yyval.list = NIL; }
    break;
case 23:
#line 428 "gram.y"
{ yyval.node = (Node *)yyvsp[0].node; }
    break;
case 24:
#line 431 "gram.y"
{ yyval.boolean = TRUE; }
    break;
case 25:
#line 432 "gram.y"
{ yyval.boolean = FALSE; }
    break;
case 26:
#line 438 "gram.y"
{ yyval.list = makeList1(NIL); }
    break;
case 27:
#line 439 "gram.y"
{ yyval.list = NIL; }
    break;
case 28:
#line 440 "gram.y"
{ yyval.list = NIL; }
    break;
case 29:
#line 443 "gram.y"
{ yyval.list = yyvsp[0].list; }
    break;
case 30:
#line 446 "gram.y"
{ yyval.list = makeList1(yyvsp[0].sortgroupby); }
    break;
case 31:
#line 447 "gram.y"
{ yyval.list = lappend(yyvsp[-2].list, yyvsp[0].sortgroupby); }
    break;
case 32:
#line 451 "gram.y"
{
					yyval.sortgroupby = makeNode(SortGroupBy);
					yyval.sortgroupby->node = yyvsp[-1].node;
					yyval.sortgroupby->useOp = yyvsp[0].str;
				}
    break;
case 33:
#line 458 "gram.y"
{ yyval.str = yyvsp[0].str; }
    break;
case 34:
#line 459 "gram.y"
{ yyval.str = "<"; }
    break;
case 35:
#line 460 "gram.y"
{ yyval.str = ">"; }
    break;
case 36:
#line 461 "gram.y"
{ yyval.str = "<"; /*default*/ }
    break;
case 38:
#line 475 "gram.y"
{	yyval.list = makeList1(makeString(yyvsp[0].str)); }
    break;
case 39:
#line 477 "gram.y"
{	yyval.list = lappend(yyvsp[-2].list, makeString(yyvsp[0].str)); }
    break;
case 40:
#line 481 "gram.y"
{ 
				  fprintf(stderr, "group_clause:\n"); 
				  yyval.list = yyvsp[0].list; 
				}
    break;
case 41:
#line 485 "gram.y"
{ yyval.list = NIL; }
    break;
case 42:
#line 496 "gram.y"
{ yyval.list = yyvsp[0].list; }
    break;
case 43:
#line 497 "gram.y"
{ yyval.list = NIL; }
    break;
case 44:
#line 500 "gram.y"
{ yyval.list = lappend(yyvsp[-2].list, yyvsp[0].node); }
    break;
case 45:
#line 501 "gram.y"
{ yyval.list = makeList1(yyvsp[0].node); }
    break;
case 46:
#line 503 "gram.y"
{ yyval.list = lappend(yyvsp[-2].list, yyvsp[0].node); }
    break;
case 47:
#line 504 "gram.y"
{ yyval.list = makeList1(yyvsp[0].node); }
    break;
case 48:
#line 509 "gram.y"
{
					FuncCall *n = makeNode(FuncCall);
					List * al;
					A_Const * c = (A_Const *)makeStringConst(yyvsp[-1].str, NULL);
#ifdef SM_DEBUG
					fprintf(stderr, "table_ref: func_name '(' SCONST ')'\n");
#endif
					al = makeList1(c);
					n->funcname = yyvsp[-3].str;
					n->args = al;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					yyval.node = (Node *)n;
				}
    break;
case 49:
#line 524 "gram.y"
{
					FuncCall *n = makeNode(FuncCall);
					List * al;
					A_Const * c = (A_Const *)makeStringConst(yyvsp[0].str, NULL);
#ifdef SM_DEBUG
					fprintf(stderr, "table_ref: func_name SCONST\n");
#endif
					al = makeList1(c);
					n->funcname = yyvsp[-1].str;
					n->args = al;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					yyval.node = (Node *)n;
				}
    break;
case 50:
#line 539 "gram.y"
{
					FuncCall *n = makeNode(FuncCall);
					List * al;
					A_Const * c = (A_Const *)makeStringConst(yyvsp[0].str, NULL);
#ifdef SM_DEBUG
					fprintf(stderr, "table_ref: func_name ColId\n");
#endif
					al = makeList1(c);
					n->funcname = yyvsp[-1].str;
					n->args = al;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					yyval.node = (Node *)n;
				}
    break;
case 51:
#line 564 "gram.y"
{
					yyval.node = (Node *) yyvsp[0].range;
				}
    break;
case 52:
#line 568 "gram.y"
{
					/*
					 * The SQL spec does not permit a subselect
					 * (<derived_table>) without an alias clause,
					 * so we don't either.  This avoids the problem
					 * of needing to invent a unique refname for it.
					 * That could be surmounted if there's sufficient
					 * popular demand, but for now let's just implement
					 * the spec and see if anyone complains.
					 * However, it does seem like a good idea to emit
					 * an error message that's better than "parse error".
					 */
					//fprintf(stderr, "sub-SELECT in FROM must have an alias"
					//  "\n\tFor example, FROM (SELECT ...) [AS] foo");
					//$$ = NULL; //wbarber
					SubLink *n = makeNode(SubLink); // wbarber
					n->subselect = yyvsp[0].node;
					yyval.node = (Node *)n;
				}
    break;
case 53:
#line 590 "gram.y"
{
					/* default inheritance */
					yyval.range = makeNode(RangeVar);
					yyval.range->relname = yyvsp[0].str;
					yyval.range->inhOpt = INH_DEFAULT;
					yyval.range->name = NULL;
				}
    break;
case 54:
#line 598 "gram.y"
{
					/* inheritance query */
					yyval.range = makeNode(RangeVar);
					yyval.range->relname = yyvsp[-1].str;
					yyval.range->inhOpt = INH_YES;
					yyval.range->name = NULL;
				}
    break;
case 55:
#line 607 "gram.y"
{ yyval.node = yyvsp[0].node; }
    break;
case 56:
#line 608 "gram.y"
{ yyval.node = NULL;  /* no qualifiers */ }
    break;
case 57:
#line 623 "gram.y"
{
					SubLink *n = makeNode(SubLink);
					n->lefthand = yyvsp[-4].list;
					n->oper = (List *) makeA_Expr(OP, yyvsp[-2].str, NULL, NULL);
					if (strcmp(yyvsp[-2].str, "<>") == 0)
						n->useor = TRUE;
					else
						n->useor = FALSE;
					n->subLinkType = yyvsp[-1].ival;
					n->subselect = yyvsp[0].node;
					yyval.node = (Node *)n;
				}
    break;
case 58:
#line 636 "gram.y"
{
					SubLink *n = makeNode(SubLink);
					n->lefthand = yyvsp[-3].list;
					n->oper = (List *) makeA_Expr(OP, yyvsp[-1].str, NULL, NULL);
					if (strcmp(yyvsp[-1].str, "<>") == 0)
						n->useor = TRUE;
					else
						n->useor = FALSE;
					n->subLinkType = MULTIEXPR_SUBLINK;
					n->subselect = yyvsp[0].node;
					yyval.node = (Node *)n;
				}
    break;
case 59:
#line 649 "gram.y"
{
					yyval.node = makeRowExpr(yyvsp[-3].str, yyvsp[-5].list, yyvsp[-1].list);
				}
    break;
case 60:
#line 655 "gram.y"
{
					yyval.list = lappend(yyvsp[-2].list, yyvsp[0].node);
				}
    break;
case 61:
#line 661 "gram.y"
{
					yyval.list = lappend(yyvsp[-2].list, yyvsp[0].node);
				}
    break;
case 62:
#line 665 "gram.y"
{
					yyval.list = makeList1(yyvsp[0].node);
				}
    break;
case 63:
#line 670 "gram.y"
{ yyval.ival = ANY_SUBLINK; }
    break;
case 64:
#line 671 "gram.y"
{ yyval.ival = ALL_SUBLINK; }
    break;
case 67:
#line 676 "gram.y"
{ yyval.str = "+"; }
    break;
case 68:
#line 677 "gram.y"
{ yyval.str = "-"; }
    break;
case 69:
#line 678 "gram.y"
{ yyval.str = "*"; }
    break;
case 70:
#line 679 "gram.y"
{ yyval.str = "/"; }
    break;
case 71:
#line 680 "gram.y"
{ yyval.str = "%"; }
    break;
case 72:
#line 681 "gram.y"
{ yyval.str = "^"; }
    break;
case 73:
#line 682 "gram.y"
{ yyval.str = "<"; }
    break;
case 74:
#line 683 "gram.y"
{ yyval.str = ">"; }
    break;
case 75:
#line 684 "gram.y"
{ yyval.str = "="; }
    break;
case 76:
#line 704 "gram.y"
{	yyval.node = yyvsp[0].node;  }
    break;
case 77:
#line 715 "gram.y"
{	yyval.node = makeA_Expr(OP, "+", NULL, yyvsp[0].node); }
    break;
case 78:
#line 717 "gram.y"
{	/* $$ = doNegate($2); */ }
    break;
case 79:
#line 719 "gram.y"
{	yyval.node = makeA_Expr(OP, "%", NULL, yyvsp[0].node); }
    break;
case 80:
#line 721 "gram.y"
{	yyval.node = makeA_Expr(OP, "^", NULL, yyvsp[0].node); }
    break;
case 81:
#line 723 "gram.y"
{	yyval.node = makeA_Expr(OP, "%", yyvsp[-1].node, NULL); }
    break;
case 82:
#line 725 "gram.y"
{	yyval.node = makeA_Expr(OP, "^", yyvsp[-1].node, NULL); }
    break;
case 83:
#line 727 "gram.y"
{	yyval.node = makeA_Expr(OP, "+", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 84:
#line 729 "gram.y"
{	yyval.node = makeA_Expr(OP, "-", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 85:
#line 731 "gram.y"
{	yyval.node = makeA_Expr(OP, "*", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 86:
#line 733 "gram.y"
{	yyval.node = makeA_Expr(OP, "/", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 87:
#line 735 "gram.y"
{	yyval.node = makeA_Expr(OP, "%", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 88:
#line 737 "gram.y"
{	yyval.node = makeA_Expr(OP, "^", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 89:
#line 739 "gram.y"
{	yyval.node = makeA_Expr(OP, "<", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 90:
#line 741 "gram.y"
{	yyval.node = makeA_Expr(OP, ">", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 91:
#line 743 "gram.y"
{	yyval.node = makeA_Expr(OP, "=", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 92:
#line 746 "gram.y"
{	yyval.node = makeA_Expr(OP, yyvsp[-1].str, yyvsp[-2].node, yyvsp[0].node); }
    break;
case 93:
#line 748 "gram.y"
{	yyval.node = makeA_Expr(OP, yyvsp[-1].str, NULL, yyvsp[0].node); }
    break;
case 94:
#line 750 "gram.y"
{	yyval.node = makeA_Expr(OP, yyvsp[0].str, yyvsp[-1].node, NULL); }
    break;
case 95:
#line 753 "gram.y"
{	yyval.node = makeA_Expr(AND, NULL, yyvsp[-2].node, yyvsp[0].node); }
    break;
case 96:
#line 755 "gram.y"
{	yyval.node = makeA_Expr(OR, NULL, yyvsp[-2].node, yyvsp[0].node); }
    break;
case 97:
#line 757 "gram.y"
{	yyval.node = makeA_Expr(NOT, NULL, NULL, yyvsp[0].node); }
    break;
case 98:
#line 760 "gram.y"
{
					SubLink *n = makeNode(SubLink);
					n->lefthand = makeList1(yyvsp[-3].node);
					n->oper = (List *) makeA_Expr(OP, yyvsp[-2].str, NULL, NULL);
					n->useor = FALSE; /* doesn't matter since only one col */
					n->subLinkType = yyvsp[-1].ival;
					n->subselect = yyvsp[0].node;
					yyval.node = (Node *)n;
				}
    break;
case 99:
#line 770 "gram.y"
{	yyval.node = yyvsp[0].node;  }
    break;
case 100:
#line 783 "gram.y"
{	yyval.node = yyvsp[0].node;  }
    break;
case 101:
#line 785 "gram.y"
{	yyval.node = makeA_Expr(OP, "+", NULL, yyvsp[0].node); }
    break;
case 102:
#line 787 "gram.y"
{	/* $$ = doNegate($2); */ }
    break;
case 103:
#line 789 "gram.y"
{	yyval.node = makeA_Expr(OP, "%", NULL, yyvsp[0].node); }
    break;
case 104:
#line 791 "gram.y"
{	yyval.node = makeA_Expr(OP, "^", NULL, yyvsp[0].node); }
    break;
case 105:
#line 793 "gram.y"
{	yyval.node = makeA_Expr(OP, "%", yyvsp[-1].node, NULL); }
    break;
case 106:
#line 795 "gram.y"
{	yyval.node = makeA_Expr(OP, "^", yyvsp[-1].node, NULL); }
    break;
case 107:
#line 797 "gram.y"
{	yyval.node = makeA_Expr(OP, "+", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 108:
#line 799 "gram.y"
{	yyval.node = makeA_Expr(OP, "-", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 109:
#line 801 "gram.y"
{	yyval.node = makeA_Expr(OP, "*", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 110:
#line 803 "gram.y"
{	yyval.node = makeA_Expr(OP, "/", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 111:
#line 805 "gram.y"
{	yyval.node = makeA_Expr(OP, "%", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 112:
#line 807 "gram.y"
{	yyval.node = makeA_Expr(OP, "^", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 113:
#line 809 "gram.y"
{	yyval.node = makeA_Expr(OP, "<", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 114:
#line 811 "gram.y"
{	yyval.node = makeA_Expr(OP, ">", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 115:
#line 813 "gram.y"
{	yyval.node = makeA_Expr(OP, "=", yyvsp[-2].node, yyvsp[0].node); }
    break;
case 116:
#line 816 "gram.y"
{	yyval.node = makeA_Expr(OP, yyvsp[-1].str, yyvsp[-2].node, yyvsp[0].node); }
    break;
case 117:
#line 818 "gram.y"
{	yyval.node = makeA_Expr(OP, yyvsp[-1].str, NULL, yyvsp[0].node); }
    break;
case 118:
#line 820 "gram.y"
{	yyval.node = makeA_Expr(OP, yyvsp[0].str, yyvsp[-1].node, NULL); }
    break;
case 119:
#line 832 "gram.y"
{	yyval.node = (Node *) yyvsp[0].attr;  }
    break;
case 120:
#line 834 "gram.y"
{
					/* could be a column name or a relation_name */
					Ident *n = makeNode(Ident);
					n->name = yyvsp[-1].str;
					n->indirection = yyvsp[0].list;
					yyval.node = (Node *)n;
				}
    break;
case 121:
#line 842 "gram.y"
{	yyval.node = yyvsp[0].node;  }
    break;
case 122:
#line 844 "gram.y"
{	yyval.node = yyvsp[-1].node; }
    break;
case 123:
#line 846 "gram.y"
{
					FuncCall *n = makeNode(FuncCall);
					n->funcname = yyvsp[-2].str;
					n->args = NIL;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					yyval.node = (Node *)n;
				}
    break;
case 124:
#line 855 "gram.y"
{
					FuncCall *n = makeNode(FuncCall);
					List * al;
					A_Const * c = (A_Const *)makeStringConst(yyvsp[-1].str, NULL);
					al = makeList1(c); // wbarber
					fprintf(stderr, "c_expr: func_name '(' SCONST ')'\n");
					n->funcname = yyvsp[-3].str;
					n->args = al;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					yyval.node = (Node *)n;
				}
    break;
case 125:
#line 868 "gram.y"
{
					FuncCall *n = makeNode(FuncCall);
					List * al;
					A_Const * c = (A_Const *)makeStringConst(yyvsp[0].str, NULL);
					al = makeList1(c); // wbarber
					fprintf(stderr, "c_expr: func_name SCONST \n");
					n->funcname = yyvsp[-1].str;
					n->args = al;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					yyval.node = (Node *)n;
				}
    break;
case 126:
#line 881 "gram.y"
{
					FuncCall *n = makeNode(FuncCall);
					List * al;
					A_Const * c = (A_Const *)makeStringConst(yyvsp[0].str, NULL);
					al = makeList1(c); // wbarber
					fprintf(stderr, "c_expr: func_name ColId \n");
					n->funcname = yyvsp[-1].str;
					n->args = al;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					yyval.node = (Node *)n;
				}
    break;
case 127:
#line 894 "gram.y"
{
					FuncCall *n = makeNode(FuncCall);
					n->funcname = yyvsp[-3].str;
					n->args = yyvsp[-1].list;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					yyval.node = (Node *)n;
				}
    break;
case 128:
#line 903 "gram.y"
{
					FuncCall *n = makeNode(FuncCall);
					n->funcname = yyvsp[-4].str;
					n->args = yyvsp[-1].list;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					/* Ideally we'd mark the FuncCall node to indicate
					 * "must be an aggregate", but there's no provision
					 * for that in FuncCall at the moment.
					 */
					yyval.node = (Node *)n;
				}
    break;
case 129:
#line 916 "gram.y"
{
					FuncCall *n = makeNode(FuncCall);
					n->funcname = yyvsp[-4].str;
					n->args = yyvsp[-1].list;
					n->agg_star = FALSE;
					n->agg_distinct = TRUE;
					yyval.node = (Node *)n;
				}
    break;
case 130:
#line 925 "gram.y"
{
					/*
					 * For now, we transform AGGREGATE(*) into AGGREGATE(1).
					 *
					 * This does the right thing for COUNT(*) (in fact,
					 * any certainly-non-null expression would do for COUNT),
					 * and there are no other aggregates in SQL92 that accept
					 * '*' as parameter.
					 *
					 * The FuncCall node is also marked agg_star = true,
					 * so that later processing can detect what the argument
					 * really was.
					 */
					FuncCall *n = makeNode(FuncCall);
					A_Const *star = makeNode(A_Const);

					star->val.type = T_Integer;
					star->val.val.ival = 1;
					n->funcname = yyvsp[-3].str;
					n->args = makeList1(star);
					n->agg_star = TRUE;
					n->agg_distinct = FALSE;
					yyval.node = (Node *)n;
				}
    break;
case 131:
#line 950 "gram.y"
{
					SubLink *n = makeNode(SubLink);
					n->lefthand = NIL;
					n->oper = NIL;
					n->useor = FALSE;
					n->subLinkType = EXPR_SUBLINK;
					n->subselect = yyvsp[0].node;
					yyval.node = (Node *)n;
				}
    break;
case 132:
#line 966 "gram.y"
{
					A_Indices *ai = makeNode(A_Indices);
					ai->lidx = NULL;
					ai->uidx = yyvsp[-1].node;
					yyval.list = lappend(yyvsp[-3].list, ai);
				}
    break;
case 133:
#line 973 "gram.y"
{
					A_Indices *ai = makeNode(A_Indices);
					ai->lidx = yyvsp[-3].node;
					ai->uidx = yyvsp[-1].node;
					yyval.list = lappend(yyvsp[-5].list, ai);
				}
    break;
case 134:
#line 980 "gram.y"
{	yyval.list = NIL; }
    break;
case 135:
#line 984 "gram.y"
{ yyval.list = makeList1(yyvsp[0].node); }
    break;
case 136:
#line 986 "gram.y"
{ yyval.list = lappend(yyvsp[-2].list, yyvsp[0].node); }
    break;
case 137:
#line 990 "gram.y"
{ yyval.list = lappend(yyvsp[0].list, yyvsp[-2].node); }
    break;
case 138:
#line 992 "gram.y"
{ yyval.list = yyvsp[0].list; }
    break;
case 139:
#line 994 "gram.y"
{ yyval.list = yyvsp[0].list; }
    break;
case 140:
#line 998 "gram.y"
{
					SubLink *n = makeNode(SubLink);
					n->subselect = yyvsp[0].node;
					yyval.node = (Node *)n;
				}
    break;
case 141:
#line 1004 "gram.y"
{	yyval.node = (Node *)yyvsp[-1].list; }
    break;
case 142:
#line 1008 "gram.y"
{	yyval.list = makeList1(yyvsp[0].node); }
    break;
case 143:
#line 1010 "gram.y"
{	yyval.list = lappend(yyvsp[-2].list, yyvsp[0].node); }
    break;
case 144:
#line 1014 "gram.y"
{
					yyval.attr = makeNode(Attr);
					yyval.attr->relname = yyvsp[-3].str;
					yyval.attr->paramNo = NULL;
					yyval.attr->attrs = yyvsp[-1].list;
					yyval.attr->indirection = yyvsp[0].list;
				}
    break;
case 145:
#line 1022 "gram.y"
{
					yyval.attr = makeNode(Attr);
					yyval.attr->relname = NULL;
					yyval.attr->paramNo = yyvsp[-3].paramno;
					yyval.attr->attrs = yyvsp[-1].list;
					yyval.attr->indirection = yyvsp[0].list;
				}
    break;
case 146:
#line 1032 "gram.y"
{ yyval.list = makeList1(makeString(yyvsp[0].str)); }
    break;
case 147:
#line 1034 "gram.y"
{ yyval.list = lappend(yyvsp[-2].list, makeString(yyvsp[0].str)); }
    break;
case 148:
#line 1036 "gram.y"
{ yyval.list = lappend(yyvsp[-2].list, makeString("*")); }
    break;
case 149:
#line 1039 "gram.y"
{ yyval.boolean = TRUE; }
    break;
case 150:
#line 1040 "gram.y"
{ yyval.boolean = TRUE; }
    break;
case 151:
#line 1052 "gram.y"
{	yyval.list = lappend(yyvsp[-2].list, yyvsp[0].target);  }
    break;
case 152:
#line 1054 "gram.y"
{	yyval.list = makeList1(yyvsp[0].target);  }
    break;
case 153:
#line 1059 "gram.y"
{
					yyval.target = makeNode(ResTarget);
					yyval.target->name = NULL;
					yyval.target->indirection = NULL;
					yyval.target->val = (Node *)yyvsp[0].node;
				}
    break;
case 154:
#line 1066 "gram.y"
{
					yyval.target = makeNode(ResTarget);
					yyval.target->name = NULL;
					yyval.target->indirection = NULL;
					yyval.target->val = (Node *)yyvsp[0].str;
					//Ident *n = makeNode(Ident);
					//n->name = $1;
					//n->indirection = $2;
					//$$ = (Node *)n;
				}
    break;
case 155:
#line 1077 "gram.y"
{
					Attr *att = makeNode(Attr);
					att->relname = yyvsp[-2].str;
					att->paramNo = NULL;
					att->attrs = makeList1(makeString("*"));
					att->indirection = NIL;
					yyval.target = makeNode(ResTarget);
					yyval.target->name = NULL;
					yyval.target->indirection = NULL;
					yyval.target->val = (Node *)att;
				}
    break;
case 156:
#line 1089 "gram.y"
{
					Attr *att = makeNode(Attr);
					att->relname = "*";
					att->paramNo = NULL;
					att->attrs = NULL;
					att->indirection = NIL;
					yyval.target = makeNode(ResTarget);
					yyval.target->name = NULL;
					yyval.target->indirection = NULL;
					yyval.target->val = (Node *)att;
				}
    break;
case 157:
#line 1109 "gram.y"
{
					yyval.str = yyvsp[0].str;
				}
    break;
case 158:
#line 1114 "gram.y"
{ yyval.str = yyvsp[0].str; }
    break;
case 159:
#line 1115 "gram.y"
{ yyval.str = yyvsp[0].str; }
    break;
case 160:
#line 1121 "gram.y"
{
					A_Const *n = makeNode(A_Const);
					n->val.type = T_Integer;
					n->val.val.ival = yyvsp[0].ival;
					yyval.node = (Node *)n;
				}
    break;
case 161:
#line 1128 "gram.y"
{
					A_Const *n = makeNode(A_Const);
					n->val.type = T_Float;
					n->val.val.str = yyvsp[0].str;
					yyval.node = (Node *)n;
				}
    break;
case 162:
#line 1135 "gram.y"
{
					A_Const *n = makeNode(A_Const);
					n->val.type = T_String;
					n->val.val.str = yyvsp[0].str;
					yyval.node = (Node *)n;
				}
    break;
case 163:
#line 1142 "gram.y"
{	yyval.node = (Node *)yyvsp[0].paramno;  }
    break;
case 164:
#line 1146 "gram.y"
{
					yyval.paramno = makeNode(ParamNo);
					yyval.paramno->number = yyvsp[-1].ival;
					yyval.paramno->indirection = yyvsp[0].list;
				}
    break;
case 165:
#line 1153 "gram.y"
{ yyval.ival = yyvsp[0].ival; }
    break;
case 166:
#line 1154 "gram.y"
{ yyval.str = yyvsp[0].str; }
    break;
case 167:
#line 1169 "gram.y"
{ yyval.str = yyvsp[0].str; }
    break;
case 168:
#line 1174 "gram.y"
{ yyval.str = yyvsp[0].str; }
    break;
case 169:
#line 1179 "gram.y"
{ yyval.str = yyvsp[0].str; }
    break;
case 170:
#line 1180 "gram.y"
{ yyval.str = yyvsp[0].str; }
    break;
case 171:
#line 1186 "gram.y"
{ yyval.str = yyvsp[0].str; }
    break;
case 172:
#line 1187 "gram.y"
{ yyval.str = yyvsp[0].str; }
    break;
case 173:
#line 1188 "gram.y"
{ yyval.str = yyvsp[0].str; }
    break;
case 174:
#line 1227 "gram.y"
{ yyval.str = "counter"; }
    break;
case 175:
#line 1228 "gram.y"
{ yyval.str = "derivative"; }
    break;
case 176:
#line 1229 "gram.y"
{ yyval.str = "pcapfile"; }
    break;
case 177:
#line 1230 "gram.y"
{ yyval.str = "pcaplive"; }
    break;
case 178:
#line 1231 "gram.y"
{ yyval.str = "split"; }
    break;
case 179:
#line 1232 "gram.y"
{ yyval.str = "delta"; }
    break;
case 180:
#line 1250 "gram.y"
{ yyval.str = "all"; }
    break;
case 181:
#line 1251 "gram.y"
{ yyval.str = "and"; }
    break;
case 182:
#line 1252 "gram.y"
{ yyval.str = "any"; }
    break;
case 183:
#line 1253 "gram.y"
{ yyval.str = "asc"; }
    break;
case 184:
#line 1254 "gram.y"
{ yyval.str = "desc"; }
    break;
case 185:
#line 1255 "gram.y"
{ yyval.str = "distinct"; }
    break;
case 186:
#line 1256 "gram.y"
{ yyval.str = "from"; }
    break;
case 187:
#line 1257 "gram.y"
{ yyval.str = "group"; }
    break;
case 188:
#line 1258 "gram.y"
{ yyval.str = "last"; }
    break;
case 189:
#line 1259 "gram.y"
{ yyval.str = "not"; }
    break;
case 190:
#line 1260 "gram.y"
{ yyval.str = "or"; }
    break;
case 191:
#line 1261 "gram.y"
{ yyval.str = "order"; }
    break;
case 192:
#line 1262 "gram.y"
{ yyval.str = "print"; }
    break;
case 193:
#line 1263 "gram.y"
{ yyval.str = "select"; }
    break;
case 194:
#line 1264 "gram.y"
{ yyval.str = "top"; }
    break;
case 195:
#line 1265 "gram.y"
{ yyval.str = "uniq"; }
    break;
case 196:
#line 1266 "gram.y"
{ yyval.str = "using"; }
    break;
case 197:
#line 1267 "gram.y"
{ yyval.str = "where"; }
    break;
}

#line 705 "/usr/share/bison/bison.simple"


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
#line 1271 "gram.y"


static Node *
makeA_Expr(int oper, char *opname, Node *lexpr, Node *rexpr)
{
	A_Expr *a = makeNode(A_Expr);
	a->oper = oper;
	a->opname = opname;
	a->lexpr = lexpr;
	a->rexpr = rexpr;
	return (Node *)a;
}

static Node *
makeTypeCast(Node *arg, TypeName *typename)
{
	/*
	 * If arg is an A_Const or ParamNo, just stick the typename into the
	 * field reserved for it --- unless there's something there already!
	 * (We don't want to collapse x::type1::type2 into just x::type2.)
	 * Otherwise, generate a TypeCast node.
	 */
	if (IsA(arg, A_Const) &&
		((A_Const *) arg)->typename == NULL)
	{
		((A_Const *) arg)->typename = typename;
		return arg;
	}
	else if (IsA(arg, ParamNo) &&
			 ((ParamNo *) arg)->typename == NULL)
	{
		((ParamNo *) arg)->typename = typename;
		return arg;
	}
	else
	{
		TypeCast *n = makeNode(TypeCast);
		n->arg = arg;
		n->typename = typename;
		return (Node *) n;
	}
}

static Node *
makeStringConst(char *str, TypeName *typename)
{
	A_Const *n = makeNode(A_Const);
	n->val.type = T_String;
	n->val.val.str = str;
	n->typename = typename;

	return (Node *)n;
}

/* makeRowExpr()
 * Generate separate operator nodes for a single row descriptor expression.
 * Perhaps this should go deeper in the parser someday...
 * - thomas 1997-12-22
 */
static Node *
makeRowExpr(char *opr, List *largs, List *rargs)
{
	Node *expr = NULL;
	Node *larg, *rarg;

	if (length(largs) != length(rargs))
		fprintf(stderr,"Unequal number of entries in row expression");

	if (lnext(largs) != NIL)
		expr = makeRowExpr(opr,lnext(largs),lnext(rargs));

	larg = lfirst(largs);
	rarg = lfirst(rargs);

	if ((strcmp(opr, "=") == 0)
	 || (strcmp(opr, "<") == 0)
	 || (strcmp(opr, "<=") == 0)
	 || (strcmp(opr, ">") == 0)
	 || (strcmp(opr, ">=") == 0))
	{
		if (expr == NULL)
			expr = makeA_Expr(OP, opr, larg, rarg);
		else
			expr = makeA_Expr(AND, NULL, expr, makeA_Expr(OP, opr, larg, rarg));
	}
	else if (strcmp(opr, "<>") == 0)
	{
		if (expr == NULL)
			expr = makeA_Expr(OP, opr, larg, rarg);
		else
			expr = makeA_Expr(OR, NULL, expr, makeA_Expr(OP, opr, larg, rarg));
	}
	else
	{
		fprintf(stderr,"Operator '%s' not implemented for row expressions",opr);
	}

	return expr;
}

/* findLeftmostSelect()
 *		Find the leftmost component SelectStmt in a set-operation parsetree.
 */
static SelectStmt *
findLeftmostSelect(SelectStmt *node)
{
	while (node && node->op != SETOP_NONE)
		node = node->larg;
	//Assert(node && IsA(node, SelectStmt) && node->larg == NULL);
	return node;
}

static Node *
makeSetOp(SetOperation op, bool all, Node *larg, Node *rarg)
{
	SelectStmt *n = makeNode(SelectStmt);

	n->op = op;
	n->all = all;
	n->larg = (SelectStmt *) larg;
	n->rarg = (SelectStmt *) rarg;
	return (Node *) n;
}

void parser_init(Oid *typev, int nargs)
{
	QueryIsRule = FALSE;
	/*
	 * Keep enough information around to fill out the type of param nodes
	 * used in postquel functions
	 */
	param_type_info = typev;
	pfunc_num_args = nargs;
}

Oid param_type(int t)
{
	if ((t > pfunc_num_args) || (t <= 0))
		return InvalidOid;
	return param_type_info[t - 1];
}

/*
 * Test whether an a_expr is a plain NULL constant or not.
 */
bool
exprIsNullConstant(Node *arg)
{
	if (arg && IsA(arg, A_Const))
	{
		A_Const *con = (A_Const *) arg;

		if (con->val.type == T_Null &&
			con->typename == NULL)
			return TRUE;
	}
	return FALSE;
}
