
/*  A Bison parser, made from gram.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	ALL	257
#define	AND	258
#define	ANY	259
#define	ASC	260
#define	BY	261
#define	DESC	262
#define	DISTINCT	263
#define	FROM	264
#define	GROUP	265
#define	NOT	266
#define	OR	267
#define	ORDER	268
#define	PRINT	269
#define	SELECT	270
#define	USING	271
#define	WHERE	272
#define	UNIQ	273
#define	SPLIT	274
#define	COUNT	275
#define	PCAPFILE	276
#define	PCAPLIVE	277
#define	IDENT	278
#define	FCONST	279
#define	SCONST	280
#define	BITCONST	281
#define	Op	282
#define	ICONST	283
#define	PARAM	284
#define	OP	285
#define	POSTFIXOP	286
#define	UMINUS	287
#define	TYPECAST	288

#line 1 "gram.y"


//#define YYDEBUG 1
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
 *	  $Header: /cvsroot/smacq/smacq/libsmacq/sql/gram.y,v 1.1.1.1 2002/10/31 01:00:59 mfisk Exp $
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


#line 104 "gram.y"
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
} YYSTYPE;
#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		197
#define	YYFLAG		-32768
#define	YYNTBASE	52

#define YYTRANSLATE(x) ((unsigned)(x) <= 288 ? yytranslate[x] : 96)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,    40,     2,     2,    46,
    47,    38,    36,    50,    37,    43,    39,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    51,    49,    33,
    32,    34,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    44,     2,    45,    41,     2,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    35,    42,    48
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,     8,    10,    11,    13,    15,    19,    23,
    25,    28,    30,    32,    39,    41,    43,    45,    46,    48,
    50,    51,    55,    57,    61,    64,    67,    69,    71,    72,
    74,    76,    80,    84,    85,    88,    89,    93,    95,    99,
   101,   106,   108,   110,   112,   115,   118,   119,   126,   132,
   140,   144,   148,   150,   152,   154,   156,   158,   160,   162,
   164,   166,   168,   170,   172,   174,   176,   178,   181,   184,
   187,   190,   193,   196,   200,   204,   208,   212,   216,   220,
   224,   228,   232,   236,   239,   242,   246,   250,   253,   258,
   260,   262,   265,   268,   271,   274,   277,   280,   284,   288,
   292,   296,   300,   304,   308,   312,   316,   320,   323,   326,
   328,   331,   333,   337,   341,   346,   351,   357,   363,   368,
   370,   375,   382,   383,   385,   389,   393,   396,   398,   400,
   404,   406,   410,   415,   420,   422,   426,   430,   433,   434,
   438,   440,   442,   446,   448,   450,   452,   454,   456,   458,
   460,   462,   465,   467,   469,   471,   473,   475,   477,   479,
   481,   483,   485,   487,   489,   491,   493,   495,   497,   499,
   501,   503,   505,   507,   509,   511,   513,   515,   517,   519,
   521
};

static const short yyrhs[] = {    53,
     0,    53,    49,    54,     0,    54,     0,    55,     0,     0,
    57,     0,    56,     0,    46,    57,    47,     0,    46,    56,
    47,     0,    59,     0,    58,    62,     0,    59,     0,    56,
     0,    60,    61,    85,    67,    72,    66,     0,    16,     0,
    15,     0,     3,     0,     0,     9,     0,     3,     0,     0,
    14,     7,    63,     0,    64,     0,    63,    50,    64,     0,
    79,    65,     0,    17,    77,     0,     6,     0,     8,     0,
     0,     0,     0,     0,     0,     0,    50,     0,     0,    11,
     7,    82,     0,     0,    10,    68,     0,     0,    68,    50,
    70,     0,    70,     0,    68,    50,    69,     0,    69,     0,
    94,    46,    92,    47,     0,    71,     0,    56,     0,    87,
     0,    87,    38,     0,    18,    79,     0,     0,    46,    74,
    47,    77,    76,    56,     0,    46,    74,    47,    77,    56,
     0,    46,    74,    47,    77,    46,    74,    47,     0,    75,
    50,    79,     0,    75,    50,    79,     0,    79,     0,     5,
     0,     3,     0,    28,     0,    78,     0,    36,     0,    37,
     0,    38,     0,    39,     0,    40,     0,    41,     0,    33,
     0,    34,     0,    32,     0,    80,     0,    36,    79,     0,
    37,    79,     0,    40,    79,     0,    41,    79,     0,    79,
    40,     0,    79,    41,     0,    79,    36,    79,     0,    79,
    37,    79,     0,    79,    38,    79,     0,    79,    39,    79,
     0,    79,    40,    79,     0,    79,    41,    79,     0,    79,
    33,    79,     0,    79,    34,    79,     0,    79,    32,    79,
     0,    79,    28,    79,     0,    28,    79,     0,    79,    28,
     0,    79,     4,    79,     0,    79,    13,    79,     0,    12,
    79,     0,    79,    77,    76,    56,     0,    73,     0,    80,
     0,    36,     0,     0,    37,     0,     0,    40,     0,     0,
    41,     0,     0,     0,    40,     0,     0,    41,     0,     0,
    36,     0,     0,     0,    37,     0,     0,     0,    38,     0,
     0,     0,    39,     0,     0,     0,    40,     0,     0,     0,
    41,     0,     0,     0,    33,     0,     0,     0,    34,     0,
     0,     0,    32,     0,     0,     0,    28,     0,     0,    28,
     0,     0,     0,    28,     0,    83,     0,    93,    81,     0,
    89,     0,    46,    79,    47,     0,    94,    46,    47,     0,
    94,    46,    92,    47,     0,    94,    46,    82,    47,     0,
    94,    46,     3,    82,    47,     0,    94,    46,     9,    82,
    47,     0,    94,    46,    38,    47,     0,    56,     0,    81,
    44,    79,    45,     0,    81,    44,    79,    51,    79,    45,
     0,     0,    79,     0,    82,    50,    79,     0,    79,    10,
    82,     0,    10,    82,     0,    82,     0,    56,     0,    46,
     0,    47,     0,    79,     0,     0,    50,    79,     0,    87,
    43,    84,    81,     0,    90,    43,    84,    81,     0,    88,
     0,    84,    43,    88,     0,    84,    43,    38,     0,    46,
    47,     0,     0,    85,    50,    86,     0,    86,     0,    79,
     0,    87,    43,    38,     0,    38,     0,    93,     0,    93,
     0,    93,     0,    91,     0,    25,     0,    92,     0,    90,
     0,    30,    81,     0,    29,     0,    26,     0,    24,     0,
    24,     0,    24,     0,    95,     0,    24,     0,    95,     0,
     0,     0,    21,     0,    22,     0,    23,     0,    19,     0,
    20,     0,     3,     0,     4,     0,     5,     0,     6,     0,
     8,     0,     9,     0,    10,     0,    11,     0,    12,     0,
    13,     0,    14,     0,    15,     0,    16,     0,    17,     0,
    18,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   237,   245,   251,   259,   260,   310,   311,   314,   318,   324,
   328,   335,   336,   362,   383,   384,   386,   387,   393,   394,
   395,   398,   401,   402,   405,   413,   414,   415,   416,   427,
   429,   431,   435,   440,   451,   452,   455,   456,   458,   459,
   462,   485,   490,   512,   520,   530,   531,   545,   558,   571,
   577,   583,   587,   593,   594,   597,   597,   599,   600,   601,
   602,   603,   604,   605,   606,   607,   626,   637,   639,   641,
   643,   645,   647,   649,   651,   653,   655,   657,   659,   661,
   663,   665,   668,   670,   672,   675,   677,   679,   682,   692,
   705,   707,   709,   711,   713,   715,   717,   719,   721,   723,
   725,   727,   729,   731,   733,   735,   738,   740,   742,   754,
   756,   764,   766,   768,   777,   790,   799,   812,   821,   846,
   862,   869,   876,   880,   882,   886,   888,   890,   894,   900,
   904,   906,   910,   918,   928,   930,   932,   936,   937,   948,
   950,   955,   962,   974,   994,  1000,  1001,  1006,  1013,  1020,
  1027,  1031,  1039,  1040,  1055,  1060,  1065,  1066,  1072,  1073,
  1074,  1112,  1114,  1115,  1116,  1117,  1131,  1133,  1134,  1135,
  1136,  1137,  1138,  1139,  1140,  1141,  1142,  1143,  1144,  1145,
  1146
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","ALL","AND",
"ANY","ASC","BY","DESC","DISTINCT","FROM","GROUP","NOT","OR","ORDER","PRINT",
"SELECT","USING","WHERE","UNIQ","SPLIT","COUNT","PCAPFILE","PCAPLIVE","IDENT",
"FCONST","SCONST","BITCONST","Op","ICONST","PARAM","OP","'='","'<'","'>'","POSTFIXOP",
"'+'","'-'","'*'","'/'","'%'","'^'","UMINUS","'.'","'['","']'","'('","')'","TYPECAST",
"';'","','","':'","stmtblock","stmtmulti","stmt","SelectStmt","select_with_parens",
"select_no_parens","select_clause","simple_select","select_word","opt_distinct",
"sort_clause","sortby_list","sortby","OptUseOp","group_clause","from_clause",
"from_list","data_func","table_ref","relation_expr","where_clause","row_expr",
"row_descriptor","row_list","sub_type","all_Op","MathOp","a_expr","c_expr","opt_indirection",
"expr_list","attr","attrs","target_list","target_el","relation_name","attr_name",
"AexprConst","ParamNo","Iconst","Sconst","ColId","func_name","func_name_keyword", NULL
};
#endif

static const short yyr1[] = {     0,
    52,    53,    53,    54,    54,    55,    55,    56,    56,    57,
    57,    58,    58,    59,    60,    60,    -1,    -1,    61,    61,
    61,    62,    63,    63,    64,    65,    65,    65,    65,    -1,
    -1,    -1,    66,    66,    67,    67,    68,    68,    68,    68,
    69,    70,    70,    71,    71,    72,    72,    73,    73,    73,
    74,    75,    75,    76,    76,    77,    77,    78,    78,    78,
    78,    78,    78,    78,    78,    78,    79,    79,    79,    79,
    79,    79,    79,    79,    79,    79,    79,    79,    79,    79,
    79,    79,    79,    79,    79,    79,    79,    79,    79,    79,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    80,
    80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
    81,    81,    81,    82,    82,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    83,    83,    84,    84,    84,    -1,    -1,    85,
    85,    86,    86,    86,    87,    -1,    88,    89,    89,    89,
    89,    90,    91,    92,    93,    -1,    94,    94,    -1,    -1,
    -1,    95,    95,    95,    95,    95,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1
};

static const short yyr2[] = {     0,
     1,     3,     1,     1,     0,     1,     1,     3,     3,     1,
     2,     1,     1,     6,     1,     1,     1,     0,     1,     1,
     0,     3,     1,     3,     2,     2,     1,     1,     0,     1,
     1,     3,     3,     0,     2,     0,     3,     1,     3,     1,
     4,     1,     1,     1,     2,     2,     0,     6,     5,     7,
     3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     2,     2,     2,
     2,     2,     2,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     2,     2,     3,     3,     2,     4,     1,
     1,     2,     2,     2,     2,     2,     2,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     2,     2,     1,
     2,     1,     3,     3,     4,     4,     5,     5,     4,     1,
     4,     6,     0,     1,     3,     3,     2,     1,     1,     3,
     1,     3,     4,     4,     1,     3,     3,     2,     0,     3,
     1,     1,     3,     1,     1,     1,     1,     1,     1,     1,
     1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1
};

static const short yydefact[] = {     5,
    16,    15,     0,     1,     3,     4,     7,     6,     0,    10,
    21,    13,     0,     5,     0,    11,    20,    19,     0,     9,
     8,     2,     0,     0,   165,   166,   162,   163,   164,   155,
   149,   154,     0,   153,   123,     0,     0,   144,     0,     0,
     0,   120,    90,   142,    67,   110,    36,   141,     0,   112,
   151,   148,   150,   123,     0,   158,    22,    23,    29,     0,
    88,    84,   152,    68,    69,    70,    71,   120,     0,     0,
    53,     0,     0,    85,    66,    64,    65,    58,    59,    60,
    61,    72,    73,     0,    57,     0,     0,    47,     0,     0,
   111,     0,     0,    27,    28,     0,    25,     0,     0,     0,
     0,   113,    86,    87,    83,    82,    80,    81,    74,    75,
    76,    77,    78,    79,    55,    54,     0,    43,    35,    40,
    38,    42,    44,   145,     0,   140,     0,    34,   155,   143,
   123,   135,   147,   123,     0,     0,     0,   114,   124,     0,
   150,    24,    56,    66,    64,    65,    58,    59,    60,    61,
    62,    63,    26,     0,     0,    51,    89,     0,    45,     0,
    46,     0,    14,     0,   133,   134,     0,     0,   119,   116,
     0,   115,   121,     0,     0,    49,     0,    39,    37,     0,
     0,   137,   136,   117,   118,   125,     0,     0,    53,    48,
    41,    33,   122,    50,     0,     0,     0
};

static const short yydefgoto[] = {   195,
     4,     5,     6,    42,    13,     9,    10,    11,    19,    16,
    57,    58,    97,   163,    88,   119,   120,   121,   122,   128,
    43,    69,    70,   117,    84,    85,   139,    45,    63,   140,
    46,   131,    47,    48,    60,   132,    50,    51,    52,    53,
    54,    55,    56
};

static const short yypact[] = {    21,
-32768,-32768,    21,   -38,-32768,-32768,    17,-32768,    30,    32,
    26,     5,    23,    21,    61,-32768,-32768,-32768,   378,-32768,
-32768,-32768,   408,   408,-32768,-32768,-32768,-32768,-32768,    28,
-32768,-32768,   408,-32768,-32768,   408,   408,-32768,   408,   408,
   348,-32768,-32768,   304,-32768,-32768,    -3,-32768,    33,-32768,
    41,-32768,-32768,    45,    55,-32768,    54,-32768,   287,    62,
   419,    59,    72,-32768,-32768,    79,-32768,    19,    91,    71,
   242,   408,   408,   181,   408,   408,   408,   408,   408,   408,
   408,   211,   239,    27,-32768,   113,   378,   121,    -4,   117,
    72,   142,   408,-32768,-32768,   433,-32768,   117,   408,   433,
   408,-32768,   419,   318,    59,   419,   447,   447,    10,    10,
    79,    79,    79,-32768,-32768,-32768,    96,-32768,    94,-32768,
-32768,-32768,   108,-32768,   101,-32768,   408,   137,-32768,-32768,
   106,-32768,-32768,   106,   408,   408,   103,-32768,   304,    -5,
   105,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,    74,     7,    90,-32768,   113,-32768,   134,
   304,   162,-32768,     2,    72,    72,    42,    43,-32768,-32768,
   408,-32768,-32768,   408,   348,-32768,    96,-32768,-32768,   126,
   408,-32768,-32768,-32768,-32768,   304,   273,   127,   304,-32768,
-32768,   131,-32768,-32768,   176,   185,-32768
};

static const short yypgoto[] = {-32768,
-32768,   173,-32768,     0,    24,-32768,-32768,-32768,-32768,-32768,
-32768,    97,-32768,-32768,-32768,-32768,    34,    36,-32768,-32768,
-32768,    16,-32768,    40,   -57,-32768,   -18,-32768,   -52,  -108,
-32768,   107,-32768,   109,   -15,    35,-32768,-32768,-32768,   -83,
   -73,   -78,-32768
};


#define	YYLAST		488


static const short yytable[] = {     7,
    44,    91,    12,    49,    59,    61,    86,   125,   141,   115,
    14,   116,   124,     7,    62,   133,   133,    64,    65,   129,
    66,    67,    71,     8,   133,   129,   167,   168,    17,   115,
   -13,   116,   -13,   130,    18,     1,     2,     8,   153,   182,
    68,   170,   155,    15,   171,   -12,    87,    80,    81,    82,
    83,    20,   175,   103,   104,   105,   106,   107,   108,   109,
   110,   111,   112,   113,   114,    20,     3,    23,    44,    21,
   123,    49,   192,  -157,    59,    89,   180,    72,   165,   125,
   154,   166,   156,    90,   124,   118,    73,  -145,   184,   185,
   133,   171,   171,    72,    78,    79,    80,    81,    82,    83,
    92,    74,    73,    93,    98,    75,    76,    77,   161,    78,
    79,    80,    81,    82,    83,    99,   157,    74,   173,    83,
   101,    75,    76,    77,   174,    78,    79,    80,    81,    82,
    83,    25,    26,    27,    28,    29,    30,   100,   127,   -52,
   129,     3,   123,   158,   135,   159,   160,   162,   164,   169,
   136,   172,   186,    24,   176,   187,   189,   118,     3,    32,
    25,    26,    27,    28,    29,    30,    31,    32,   181,    33,
    34,    35,   191,   194,    68,   196,   190,    36,    37,   137,
   171,    39,    40,   -56,   197,   -56,    22,    41,   138,   142,
   188,   178,    24,   179,   177,   126,   134,     0,   183,    25,
    26,    27,    28,    29,    30,    31,    32,     0,    33,    34,
    35,     0,     0,   -62,     0,   -62,    36,    37,     0,     0,
    39,    40,    24,     0,     0,     0,    41,     0,     0,    25,
    26,    27,    28,    29,    30,    31,    32,     0,     0,    34,
    35,   -63,     0,   -63,     0,    72,     0,     0,     0,     0,
    24,    40,     0,     0,    73,     0,    41,    25,    26,    27,
    28,    29,    30,    31,    32,     0,     0,    34,    35,    74,
     0,     0,     0,    75,    76,    77,    72,    78,    79,    80,
    81,    82,    83,     0,    41,    73,     0,     0,   102,     0,
    72,     0,    94,     0,    95,     0,     0,     0,     0,    73,
    74,     0,     0,    96,    75,    76,    77,    72,    78,    79,
    80,    81,    82,    83,    74,     0,    73,   193,    75,    76,
    77,    72,    78,    79,    80,    81,    82,    83,     0,     0,
     0,    74,     0,     0,     0,    75,    76,    77,     0,    78,
    79,    80,    81,    82,    83,    74,     0,     0,     0,    75,
    76,    77,     0,    78,    79,    80,    81,    82,    83,    24,
     0,     0,     1,     2,     0,     0,    25,    26,    27,    28,
    29,    30,    31,    32,     0,    33,    34,    35,     0,     0,
     0,     0,     0,    36,    37,     0,     0,    39,    40,    24,
     0,     0,     0,    41,     0,     0,    25,    26,    27,    28,
    29,    30,    31,    32,     0,    33,    34,    35,     0,     0,
     0,     0,     0,    36,    37,    38,     0,    39,    40,    24,
     0,     0,     0,    41,     0,     0,    25,    26,    27,    28,
    29,    30,    31,    32,     0,    33,    34,    35,     0,     0,
     0,     0,     0,    36,    37,     0,    74,    39,    40,     0,
    75,    76,    77,    41,    78,    79,    80,    81,    82,    83,
   143,     0,     0,     0,   144,   145,   146,     0,   147,   148,
   149,   150,   151,   152,    74,     0,     0,     0,     0,-32768,
-32768,     0,    78,    79,    80,    81,    82,    83
};

static const short yycheck[] = {     0,
    19,    54,     3,    19,    23,    24,    10,    86,    92,     3,
    49,     5,    86,    14,    33,    89,    90,    36,    37,    24,
    39,    40,    41,     0,    98,    24,   135,   136,     3,     3,
    14,     5,    14,    38,     9,    15,    16,    14,    96,    38,
    41,    47,   100,    14,    50,    14,    50,    38,    39,    40,
    41,    47,    46,    72,    73,    74,    75,    76,    77,    78,
    79,    80,    81,    82,    83,    47,    46,     7,    87,    47,
    86,    87,   181,    46,    93,    43,   160,     4,   131,   158,
    99,   134,   101,    43,   158,    86,    13,    43,    47,    47,
   164,    50,    50,     4,    36,    37,    38,    39,    40,    41,
    46,    28,    13,    50,    43,    32,    33,    34,   127,    36,
    37,    38,    39,    40,    41,    44,   117,    28,    45,    41,
    50,    32,    33,    34,    51,    36,    37,    38,    39,    40,
    41,    19,    20,    21,    22,    23,    24,    47,    18,    50,
    24,    46,   158,    50,     3,    38,    46,    11,    43,    47,
     9,    47,   171,    12,   155,   174,   175,   158,    46,    26,
    19,    20,    21,    22,    23,    24,    25,    26,     7,    28,
    29,    30,    47,    47,   175,     0,   177,    36,    37,    38,
    50,    40,    41,     3,     0,     5,    14,    46,    47,    93,
   175,   158,    12,   158,   155,    87,    90,    -1,   164,    19,
    20,    21,    22,    23,    24,    25,    26,    -1,    28,    29,
    30,    -1,    -1,     3,    -1,     5,    36,    37,    -1,    -1,
    40,    41,    12,    -1,    -1,    -1,    46,    -1,    -1,    19,
    20,    21,    22,    23,    24,    25,    26,    -1,    -1,    29,
    30,     3,    -1,     5,    -1,     4,    -1,    -1,    -1,    -1,
    12,    41,    -1,    -1,    13,    -1,    46,    19,    20,    21,
    22,    23,    24,    25,    26,    -1,    -1,    29,    30,    28,
    -1,    -1,    -1,    32,    33,    34,     4,    36,    37,    38,
    39,    40,    41,    -1,    46,    13,    -1,    -1,    47,    -1,
     4,    -1,     6,    -1,     8,    -1,    -1,    -1,    -1,    13,
    28,    -1,    -1,    17,    32,    33,    34,     4,    36,    37,
    38,    39,    40,    41,    28,    -1,    13,    45,    32,    33,
    34,     4,    36,    37,    38,    39,    40,    41,    -1,    -1,
    -1,    28,    -1,    -1,    -1,    32,    33,    34,    -1,    36,
    37,    38,    39,    40,    41,    28,    -1,    -1,    -1,    32,
    33,    34,    -1,    36,    37,    38,    39,    40,    41,    12,
    -1,    -1,    15,    16,    -1,    -1,    19,    20,    21,    22,
    23,    24,    25,    26,    -1,    28,    29,    30,    -1,    -1,
    -1,    -1,    -1,    36,    37,    -1,    -1,    40,    41,    12,
    -1,    -1,    -1,    46,    -1,    -1,    19,    20,    21,    22,
    23,    24,    25,    26,    -1,    28,    29,    30,    -1,    -1,
    -1,    -1,    -1,    36,    37,    38,    -1,    40,    41,    12,
    -1,    -1,    -1,    46,    -1,    -1,    19,    20,    21,    22,
    23,    24,    25,    26,    -1,    28,    29,    30,    -1,    -1,
    -1,    -1,    -1,    36,    37,    -1,    28,    40,    41,    -1,
    32,    33,    34,    46,    36,    37,    38,    39,    40,    41,
    28,    -1,    -1,    -1,    32,    33,    34,    -1,    36,    37,
    38,    39,    40,    41,    28,    -1,    -1,    -1,    -1,    33,
    34,    -1,    36,    37,    38,    39,    40,    41
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

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

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
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
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
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

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 238 "gram.y"
{ 
				  parsetree = yyvsp[0].list;
				  fprintf(stderr, "stmtblock: parsetree = %p\n", parsetree);
				;
    break;}
case 2:
#line 246 "gram.y"
{ if (yyvsp[0].node != (Node *)NULL)
					yyval.list = lappend(yyvsp[-2].list, yyvsp[0].node);
				  else
					yyval.list = yyvsp[-2].list;
				;
    break;}
case 3:
#line 252 "gram.y"
{ if (yyvsp[0].node != (Node *)NULL)
					yyval.list = makeList1(yyvsp[0].node);
				  else
					yyval.list = NIL;
				;
    break;}
case 5:
#line 261 "gram.y"
{ yyval.node = (Node *)NULL; ;
    break;}
case 8:
#line 315 "gram.y"
{
				yyval.node = yyvsp[-1].node;
			;
    break;}
case 9:
#line 319 "gram.y"
{
				yyval.node = yyvsp[-1].node;
			;
    break;}
case 10:
#line 325 "gram.y"
{
				yyval.node = yyvsp[0].node;
			;
    break;}
case 11:
#line 329 "gram.y"
{
				((SelectStmt *)yyvsp[-1].node)->sortClause = (List *)yyvsp[0].list;
				yyval.node = yyvsp[-1].node;
			;
    break;}
case 14:
#line 366 "gram.y"
{
					SelectStmt *n = makeNode(SelectStmt);
					if (strcmp(yyvsp[-5].str, "PRINT") == 0) {
					  n->isPrint = TRUE;
					}
					else {
					  n->isPrint = FALSE;
					}
					n->distinctClause = yyvsp[-4].list;
					n->targetList = yyvsp[-3].list;
					n->fromClause = yyvsp[-2].list;
					n->whereClause = yyvsp[-1].node;
					n->groupClause = yyvsp[0].list;
					yyval.node = (Node *)n;
				;
    break;}
case 15:
#line 383 "gram.y"
{ yyval.str = "SELECT"; ;
    break;}
case 16:
#line 384 "gram.y"
{ yyval.str = "PRINT"; ;
    break;}
case 17:
#line 386 "gram.y"
{ yyval.boolean = TRUE; ;
    break;}
case 18:
#line 387 "gram.y"
{ yyval.boolean = FALSE; ;
    break;}
case 19:
#line 393 "gram.y"
{ yyval.list = makeList1(NIL); ;
    break;}
case 20:
#line 394 "gram.y"
{ yyval.list = NIL; ;
    break;}
case 21:
#line 395 "gram.y"
{ yyval.list = NIL; ;
    break;}
case 22:
#line 398 "gram.y"
{ yyval.list = yyvsp[0].list; ;
    break;}
case 23:
#line 401 "gram.y"
{ yyval.list = makeList1(yyvsp[0].sortgroupby); ;
    break;}
case 24:
#line 402 "gram.y"
{ yyval.list = lappend(yyvsp[-2].list, yyvsp[0].sortgroupby); ;
    break;}
case 25:
#line 406 "gram.y"
{
					yyval.sortgroupby = makeNode(SortGroupBy);
					yyval.sortgroupby->node = yyvsp[-1].node;
					yyval.sortgroupby->useOp = yyvsp[0].str;
				;
    break;}
case 26:
#line 413 "gram.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 27:
#line 414 "gram.y"
{ yyval.str = "<"; ;
    break;}
case 28:
#line 415 "gram.y"
{ yyval.str = ">"; ;
    break;}
case 29:
#line 416 "gram.y"
{ yyval.str = "<"; /*default*/ ;
    break;}
case 31:
#line 430 "gram.y"
{	yyval.list = makeList1(makeString(yyvsp[0].str)); ;
    break;}
case 32:
#line 432 "gram.y"
{	yyval.list = lappend(yyvsp[-2].list, makeString(yyvsp[0].str)); ;
    break;}
case 33:
#line 436 "gram.y"
{ 
				  fprintf(stderr, "group_clause:\n"); 
				  yyval.list = yyvsp[0].list; 
				;
    break;}
case 34:
#line 440 "gram.y"
{ yyval.list = NIL; ;
    break;}
case 35:
#line 451 "gram.y"
{ yyval.list = yyvsp[0].list; ;
    break;}
case 36:
#line 452 "gram.y"
{ yyval.list = NIL; ;
    break;}
case 37:
#line 455 "gram.y"
{ yyval.list = lappend(yyvsp[-2].list, yyvsp[0].node); ;
    break;}
case 38:
#line 456 "gram.y"
{ yyval.list = makeList1(yyvsp[0].node); ;
    break;}
case 39:
#line 458 "gram.y"
{ yyval.list = lappend(yyvsp[-2].list, yyvsp[0].node); ;
    break;}
case 40:
#line 459 "gram.y"
{ yyval.list = makeList1(yyvsp[0].node); ;
    break;}
case 41:
#line 464 "gram.y"
{
					FuncCall *n = makeNode(FuncCall);
					List * al;
					A_Const * c = (A_Const *)makeStringConst(yyvsp[-1].str, NULL);
					fprintf(stderr, "table_ref: func_name '(' SCONST ')'\n");
					al = makeList1(c);
					n->funcname = yyvsp[-3].str;
					n->args = al;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					yyval.node = (Node *)n;
				;
    break;}
case 42:
#line 487 "gram.y"
{
					yyval.node = (Node *) yyvsp[0].range;
				;
    break;}
case 43:
#line 491 "gram.y"
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
				;
    break;}
case 44:
#line 513 "gram.y"
{
					/* default inheritance */
					yyval.range = makeNode(RangeVar);
					yyval.range->relname = yyvsp[0].str;
					yyval.range->inhOpt = INH_DEFAULT;
					yyval.range->name = NULL;
				;
    break;}
case 45:
#line 521 "gram.y"
{
					/* inheritance query */
					yyval.range = makeNode(RangeVar);
					yyval.range->relname = yyvsp[-1].str;
					yyval.range->inhOpt = INH_YES;
					yyval.range->name = NULL;
				;
    break;}
case 46:
#line 530 "gram.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 47:
#line 531 "gram.y"
{ yyval.node = NULL;  /* no qualifiers */ ;
    break;}
case 48:
#line 546 "gram.y"
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
				;
    break;}
case 49:
#line 559 "gram.y"
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
				;
    break;}
case 50:
#line 572 "gram.y"
{
					yyval.node = makeRowExpr(yyvsp[-3].str, yyvsp[-5].list, yyvsp[-1].list);
				;
    break;}
case 51:
#line 578 "gram.y"
{
					yyval.list = lappend(yyvsp[-2].list, yyvsp[0].node);
				;
    break;}
case 52:
#line 584 "gram.y"
{
					yyval.list = lappend(yyvsp[-2].list, yyvsp[0].node);
				;
    break;}
case 53:
#line 588 "gram.y"
{
					yyval.list = makeList1(yyvsp[0].node);
				;
    break;}
case 54:
#line 593 "gram.y"
{ yyval.ival = ANY_SUBLINK; ;
    break;}
case 55:
#line 594 "gram.y"
{ yyval.ival = ALL_SUBLINK; ;
    break;}
case 58:
#line 599 "gram.y"
{ yyval.str = "+"; ;
    break;}
case 59:
#line 600 "gram.y"
{ yyval.str = "-"; ;
    break;}
case 60:
#line 601 "gram.y"
{ yyval.str = "*"; ;
    break;}
case 61:
#line 602 "gram.y"
{ yyval.str = "/"; ;
    break;}
case 62:
#line 603 "gram.y"
{ yyval.str = "%"; ;
    break;}
case 63:
#line 604 "gram.y"
{ yyval.str = "^"; ;
    break;}
case 64:
#line 605 "gram.y"
{ yyval.str = "<"; ;
    break;}
case 65:
#line 606 "gram.y"
{ yyval.str = ">"; ;
    break;}
case 66:
#line 607 "gram.y"
{ yyval.str = "="; ;
    break;}
case 67:
#line 627 "gram.y"
{	yyval.node = yyvsp[0].node;  ;
    break;}
case 68:
#line 638 "gram.y"
{	yyval.node = makeA_Expr(OP, "+", NULL, yyvsp[0].node); ;
    break;}
case 69:
#line 640 "gram.y"
{	/* $$ = doNegate($2); */ ;
    break;}
case 70:
#line 642 "gram.y"
{	yyval.node = makeA_Expr(OP, "%", NULL, yyvsp[0].node); ;
    break;}
case 71:
#line 644 "gram.y"
{	yyval.node = makeA_Expr(OP, "^", NULL, yyvsp[0].node); ;
    break;}
case 72:
#line 646 "gram.y"
{	yyval.node = makeA_Expr(OP, "%", yyvsp[-1].node, NULL); ;
    break;}
case 73:
#line 648 "gram.y"
{	yyval.node = makeA_Expr(OP, "^", yyvsp[-1].node, NULL); ;
    break;}
case 74:
#line 650 "gram.y"
{	yyval.node = makeA_Expr(OP, "+", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 75:
#line 652 "gram.y"
{	yyval.node = makeA_Expr(OP, "-", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 76:
#line 654 "gram.y"
{	yyval.node = makeA_Expr(OP, "*", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 77:
#line 656 "gram.y"
{	yyval.node = makeA_Expr(OP, "/", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 78:
#line 658 "gram.y"
{	yyval.node = makeA_Expr(OP, "%", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 79:
#line 660 "gram.y"
{	yyval.node = makeA_Expr(OP, "^", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 80:
#line 662 "gram.y"
{	yyval.node = makeA_Expr(OP, "<", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 81:
#line 664 "gram.y"
{	yyval.node = makeA_Expr(OP, ">", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 82:
#line 666 "gram.y"
{	yyval.node = makeA_Expr(OP, "=", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 83:
#line 669 "gram.y"
{	yyval.node = makeA_Expr(OP, yyvsp[-1].str, yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 84:
#line 671 "gram.y"
{	yyval.node = makeA_Expr(OP, yyvsp[-1].str, NULL, yyvsp[0].node); ;
    break;}
case 85:
#line 673 "gram.y"
{	yyval.node = makeA_Expr(OP, yyvsp[0].str, yyvsp[-1].node, NULL); ;
    break;}
case 86:
#line 676 "gram.y"
{	yyval.node = makeA_Expr(AND, NULL, yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 87:
#line 678 "gram.y"
{	yyval.node = makeA_Expr(OR, NULL, yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 88:
#line 680 "gram.y"
{	yyval.node = makeA_Expr(NOT, NULL, NULL, yyvsp[0].node); ;
    break;}
case 89:
#line 683 "gram.y"
{
					SubLink *n = makeNode(SubLink);
					n->lefthand = makeList1(yyvsp[-3].node);
					n->oper = (List *) makeA_Expr(OP, yyvsp[-2].str, NULL, NULL);
					n->useor = FALSE; /* doesn't matter since only one col */
					n->subLinkType = yyvsp[-1].ival;
					n->subselect = yyvsp[0].node;
					yyval.node = (Node *)n;
				;
    break;}
case 90:
#line 693 "gram.y"
{	yyval.node = yyvsp[0].node;  ;
    break;}
case 91:
#line 706 "gram.y"
{	yyval.node = yyvsp[0].node;  ;
    break;}
case 92:
#line 708 "gram.y"
{	yyval.node = makeA_Expr(OP, "+", NULL, yyvsp[0].node); ;
    break;}
case 93:
#line 710 "gram.y"
{	/* $$ = doNegate($2); */ ;
    break;}
case 94:
#line 712 "gram.y"
{	yyval.node = makeA_Expr(OP, "%", NULL, yyvsp[0].node); ;
    break;}
case 95:
#line 714 "gram.y"
{	yyval.node = makeA_Expr(OP, "^", NULL, yyvsp[0].node); ;
    break;}
case 96:
#line 716 "gram.y"
{	yyval.node = makeA_Expr(OP, "%", yyvsp[-1].node, NULL); ;
    break;}
case 97:
#line 718 "gram.y"
{	yyval.node = makeA_Expr(OP, "^", yyvsp[-1].node, NULL); ;
    break;}
case 98:
#line 720 "gram.y"
{	yyval.node = makeA_Expr(OP, "+", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 99:
#line 722 "gram.y"
{	yyval.node = makeA_Expr(OP, "-", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 100:
#line 724 "gram.y"
{	yyval.node = makeA_Expr(OP, "*", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 101:
#line 726 "gram.y"
{	yyval.node = makeA_Expr(OP, "/", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 102:
#line 728 "gram.y"
{	yyval.node = makeA_Expr(OP, "%", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 103:
#line 730 "gram.y"
{	yyval.node = makeA_Expr(OP, "^", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 104:
#line 732 "gram.y"
{	yyval.node = makeA_Expr(OP, "<", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 105:
#line 734 "gram.y"
{	yyval.node = makeA_Expr(OP, ">", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 106:
#line 736 "gram.y"
{	yyval.node = makeA_Expr(OP, "=", yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 107:
#line 739 "gram.y"
{	yyval.node = makeA_Expr(OP, yyvsp[-1].str, yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 108:
#line 741 "gram.y"
{	yyval.node = makeA_Expr(OP, yyvsp[-1].str, NULL, yyvsp[0].node); ;
    break;}
case 109:
#line 743 "gram.y"
{	yyval.node = makeA_Expr(OP, yyvsp[0].str, yyvsp[-1].node, NULL); ;
    break;}
case 110:
#line 755 "gram.y"
{	yyval.node = (Node *) yyvsp[0].attr;  ;
    break;}
case 111:
#line 757 "gram.y"
{
					/* could be a column name or a relation_name */
					Ident *n = makeNode(Ident);
					n->name = yyvsp[-1].str;
					n->indirection = yyvsp[0].list;
					yyval.node = (Node *)n;
				;
    break;}
case 112:
#line 765 "gram.y"
{	yyval.node = yyvsp[0].node;  ;
    break;}
case 113:
#line 767 "gram.y"
{	yyval.node = yyvsp[-1].node; ;
    break;}
case 114:
#line 769 "gram.y"
{
					FuncCall *n = makeNode(FuncCall);
					n->funcname = yyvsp[-2].str;
					n->args = NIL;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					yyval.node = (Node *)n;
				;
    break;}
case 115:
#line 778 "gram.y"
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
				;
    break;}
case 116:
#line 791 "gram.y"
{
					FuncCall *n = makeNode(FuncCall);
					n->funcname = yyvsp[-3].str;
					n->args = yyvsp[-1].list;
					n->agg_star = FALSE;
					n->agg_distinct = FALSE;
					yyval.node = (Node *)n;
				;
    break;}
case 117:
#line 800 "gram.y"
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
				;
    break;}
case 118:
#line 813 "gram.y"
{
					FuncCall *n = makeNode(FuncCall);
					n->funcname = yyvsp[-4].str;
					n->args = yyvsp[-1].list;
					n->agg_star = FALSE;
					n->agg_distinct = TRUE;
					yyval.node = (Node *)n;
				;
    break;}
case 119:
#line 822 "gram.y"
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
				;
    break;}
case 120:
#line 847 "gram.y"
{
					SubLink *n = makeNode(SubLink);
					n->lefthand = NIL;
					n->oper = NIL;
					n->useor = FALSE;
					n->subLinkType = EXPR_SUBLINK;
					n->subselect = yyvsp[0].node;
					yyval.node = (Node *)n;
				;
    break;}
case 121:
#line 863 "gram.y"
{
					A_Indices *ai = makeNode(A_Indices);
					ai->lidx = NULL;
					ai->uidx = yyvsp[-1].node;
					yyval.list = lappend(yyvsp[-3].list, ai);
				;
    break;}
case 122:
#line 870 "gram.y"
{
					A_Indices *ai = makeNode(A_Indices);
					ai->lidx = yyvsp[-3].node;
					ai->uidx = yyvsp[-1].node;
					yyval.list = lappend(yyvsp[-5].list, ai);
				;
    break;}
case 123:
#line 877 "gram.y"
{	yyval.list = NIL; ;
    break;}
case 124:
#line 881 "gram.y"
{ yyval.list = makeList1(yyvsp[0].node); ;
    break;}
case 125:
#line 883 "gram.y"
{ yyval.list = lappend(yyvsp[-2].list, yyvsp[0].node); ;
    break;}
case 126:
#line 887 "gram.y"
{ yyval.list = lappend(yyvsp[0].list, yyvsp[-2].node); ;
    break;}
case 127:
#line 889 "gram.y"
{ yyval.list = yyvsp[0].list; ;
    break;}
case 128:
#line 891 "gram.y"
{ yyval.list = yyvsp[0].list; ;
    break;}
case 129:
#line 895 "gram.y"
{
					SubLink *n = makeNode(SubLink);
					n->subselect = yyvsp[0].node;
					yyval.node = (Node *)n;
				;
    break;}
case 130:
#line 901 "gram.y"
{	yyval.node = (Node *)yyvsp[-1].list; ;
    break;}
case 131:
#line 905 "gram.y"
{	yyval.list = makeList1(yyvsp[0].node); ;
    break;}
case 132:
#line 907 "gram.y"
{	yyval.list = lappend(yyvsp[-2].list, yyvsp[0].node); ;
    break;}
case 133:
#line 911 "gram.y"
{
					yyval.attr = makeNode(Attr);
					yyval.attr->relname = yyvsp[-3].str;
					yyval.attr->paramNo = NULL;
					yyval.attr->attrs = yyvsp[-1].list;
					yyval.attr->indirection = yyvsp[0].list;
				;
    break;}
case 134:
#line 919 "gram.y"
{
					yyval.attr = makeNode(Attr);
					yyval.attr->relname = NULL;
					yyval.attr->paramNo = yyvsp[-3].paramno;
					yyval.attr->attrs = yyvsp[-1].list;
					yyval.attr->indirection = yyvsp[0].list;
				;
    break;}
case 135:
#line 929 "gram.y"
{ yyval.list = makeList1(makeString(yyvsp[0].str)); ;
    break;}
case 136:
#line 931 "gram.y"
{ yyval.list = lappend(yyvsp[-2].list, makeString(yyvsp[0].str)); ;
    break;}
case 137:
#line 933 "gram.y"
{ yyval.list = lappend(yyvsp[-2].list, makeString("*")); ;
    break;}
case 138:
#line 936 "gram.y"
{ yyval.boolean = TRUE; ;
    break;}
case 139:
#line 937 "gram.y"
{ yyval.boolean = TRUE; ;
    break;}
case 140:
#line 949 "gram.y"
{	yyval.list = lappend(yyvsp[-2].list, yyvsp[0].target);  ;
    break;}
case 141:
#line 951 "gram.y"
{	yyval.list = makeList1(yyvsp[0].target);  ;
    break;}
case 142:
#line 956 "gram.y"
{
					yyval.target = makeNode(ResTarget);
					yyval.target->name = NULL;
					yyval.target->indirection = NULL;
					yyval.target->val = (Node *)yyvsp[0].node;
				;
    break;}
case 143:
#line 963 "gram.y"
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
				;
    break;}
case 144:
#line 975 "gram.y"
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
				;
    break;}
case 145:
#line 995 "gram.y"
{
					yyval.str = yyvsp[0].str;
				;
    break;}
case 146:
#line 1000 "gram.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 147:
#line 1001 "gram.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 148:
#line 1007 "gram.y"
{
					A_Const *n = makeNode(A_Const);
					n->val.type = T_Integer;
					n->val.val.ival = yyvsp[0].ival;
					yyval.node = (Node *)n;
				;
    break;}
case 149:
#line 1014 "gram.y"
{
					A_Const *n = makeNode(A_Const);
					n->val.type = T_Float;
					n->val.val.str = yyvsp[0].str;
					yyval.node = (Node *)n;
				;
    break;}
case 150:
#line 1021 "gram.y"
{
					A_Const *n = makeNode(A_Const);
					n->val.type = T_String;
					n->val.val.str = yyvsp[0].str;
					yyval.node = (Node *)n;
				;
    break;}
case 151:
#line 1028 "gram.y"
{	yyval.node = (Node *)yyvsp[0].paramno;  ;
    break;}
case 152:
#line 1032 "gram.y"
{
					yyval.paramno = makeNode(ParamNo);
					yyval.paramno->number = yyvsp[-1].ival;
					yyval.paramno->indirection = yyvsp[0].list;
				;
    break;}
case 153:
#line 1039 "gram.y"
{ yyval.ival = yyvsp[0].ival; ;
    break;}
case 154:
#line 1040 "gram.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 155:
#line 1055 "gram.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 156:
#line 1060 "gram.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 157:
#line 1065 "gram.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 158:
#line 1066 "gram.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 159:
#line 1072 "gram.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 160:
#line 1073 "gram.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 161:
#line 1074 "gram.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 162:
#line 1113 "gram.y"
{ yyval.str = "count"; ;
    break;}
case 163:
#line 1114 "gram.y"
{ yyval.str = "pcapfile"; ;
    break;}
case 164:
#line 1115 "gram.y"
{ yyval.str = "pcaplive"; ;
    break;}
case 165:
#line 1116 "gram.y"
{ yyval.str = "uniq"; ;
    break;}
case 166:
#line 1117 "gram.y"
{ yyval.str = "split"; ;
    break;}
case 167:
#line 1132 "gram.y"
{ yyval.str = "all"; ;
    break;}
case 168:
#line 1133 "gram.y"
{ yyval.str = "and"; ;
    break;}
case 169:
#line 1134 "gram.y"
{ yyval.str = "any"; ;
    break;}
case 170:
#line 1135 "gram.y"
{ yyval.str = "asc"; ;
    break;}
case 171:
#line 1136 "gram.y"
{ yyval.str = "desc"; ;
    break;}
case 172:
#line 1137 "gram.y"
{ yyval.str = "distinct"; ;
    break;}
case 173:
#line 1138 "gram.y"
{ yyval.str = "from"; ;
    break;}
case 174:
#line 1139 "gram.y"
{ yyval.str = "group"; ;
    break;}
case 175:
#line 1140 "gram.y"
{ yyval.str = "not"; ;
    break;}
case 176:
#line 1141 "gram.y"
{ yyval.str = "or"; ;
    break;}
case 177:
#line 1142 "gram.y"
{ yyval.str = "order"; ;
    break;}
case 178:
#line 1143 "gram.y"
{ yyval.str = "print"; ;
    break;}
case 179:
#line 1144 "gram.y"
{ yyval.str = "select"; ;
    break;}
case 180:
#line 1145 "gram.y"
{ yyval.str = "using"; ;
    break;}
case 181:
#line 1146 "gram.y"
{ yyval.str = "where"; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

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

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 1150 "gram.y"


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
