
/*  A Bison parser, made from /Users/mfisk/smacq/libsmacq/sql/parser.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	WHERE	257
#define	GROUP	258
#define	BY	259
#define	FROM	260
#define	SELECT	261
#define	AS	262
#define	HAVING	263
#define	YYSTRING	264
#define	YYID	265
#define	YYNEQ	266
#define	YYLEQ	267
#define	YYGEQ	268
#define	YYSTOP	269
#define	YYLIKE	270
#define	YYOR	271
#define	YYAND	272

#line 2 "/Users/mfisk/smacq/libsmacq/sql/parser.y"

void yyerror(char*);

#include <smacq-parser.h>
static struct graph nullgraph = { head: NULL, tail: NULL };
static smacq_graph * Graph;
#line 34 "/Users/mfisk/smacq/libsmacq/sql/parser.y"


#include <smacq-parser.h>
//static struct graph nullgraph = { head: NULL, tail: NULL };
//static smacq_graph * Graph;


#line 61 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
typedef union {
  struct graph graph;
  struct arglist * arglist;
  struct vphrase vphrase;
  char * string;
  struct group group;
  dts_compare_operation op;
  dts_comparison * comp;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		95
#define	YYFLAG		-32768
#define	YYNTBASE	26

#define YYTRANSLATE(x) ((unsigned)(x) <= 272 ? yytranslate[x] : 49)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    19,
    20,     2,     2,    21,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    24,
    22,    23,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    25,     2,     2,     2,     2,     2,     2,
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
    17,    18
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     3,     5,     7,     9,    11,    13,    15,    19,
    21,    26,    28,    33,    35,    37,    40,    42,    46,    48,
    52,    56,    60,    62,    64,    68,    73,    75,    77,    79,
    81,    83,    85,    87,    90,    95,   101,   104,   106,   109,
   112,   116,   118,   121,   123,   128,   130,   133,   138,   141,
   144
};

static const short yyrhs[] = {    -1,
    29,     0,    28,     0,    10,     0,    11,     0,    17,     0,
    18,     0,    31,     0,    31,     8,    27,     0,    27,     0,
    32,    19,    33,    20,     0,    29,     0,    30,    21,    30,
    35,     0,    34,     0,    26,     0,    30,    34,     0,    26,
     0,    21,    30,    35,     0,    29,     0,    19,    37,    20,
     0,    37,    17,    37,     0,    37,    18,    37,     0,    38,
     0,    27,     0,    27,    39,    27,     0,    36,    19,    33,
    20,     0,    22,     0,    23,     0,    24,     0,    14,     0,
    13,     0,    12,     0,    16,     0,    41,    15,     0,    48,
    42,    44,    45,     0,    41,    25,    48,    44,    45,     0,
    42,    44,     0,    26,     0,     6,    43,     0,    47,    42,
     0,    19,    41,    20,     0,    26,     0,     3,    37,     0,
    26,     0,     4,     5,    33,    46,     0,    26,     0,     9,
    37,     0,    36,    19,    33,    20,     0,    36,    34,     0,
    36,    33,     0,    36,    19,    33,    20,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    72,    75,    76,    79,    82,    83,    84,    87,    88,    91,
    92,    95,    98,    99,   102,   103,   106,   107,   110,   117,
   118,   119,   120,   123,   124,   125,   133,   134,   135,   136,
   137,   138,   139,   144,   156,   167,   178,   186,   187,   190,
   196,   199,   200,   203,   204,   207,   208,   211,   212,   215,
   216
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","WHERE",
"GROUP","BY","FROM","SELECT","AS","HAVING","YYSTRING","YYID","YYNEQ","YYLEQ",
"YYGEQ","YYSTOP","YYLIKE","YYOR","YYAND","'('","')'","','","'='","'>'","'<'",
"'|'","null","word","string","id","arg","argument","function","args","spacedargs",
"moreargs","verb","boolean","test","op","queryline","query","from","source",
"where","group","having","pverbphrase","verbphrase", NULL
};
#endif

static const short yyr1[] = {     0,
    26,    27,    27,    28,    29,    29,    29,    30,    30,    31,
    31,    32,    33,    33,    34,    34,    35,    35,    36,    37,
    37,    37,    37,    38,    38,    38,    39,    39,    39,    39,
    39,    39,    39,    40,    41,    41,    41,    42,    42,    43,
    43,    44,    44,    45,    45,    46,    46,    47,    47,    48,
    48
};

static const short yyr2[] = {     0,
     0,     1,     1,     1,     1,     1,     1,     1,     3,     1,
     4,     1,     4,     1,     1,     2,     1,     3,     1,     3,
     3,     3,     1,     1,     3,     4,     1,     1,     1,     1,
     1,     1,     1,     2,     4,     5,     2,     1,     2,     2,
     3,     1,     2,     1,     4,     1,     2,     4,     2,     2,
     4
};

static const short yydefact[] = {     1,
     0,     5,     6,     7,    38,    19,     1,     0,     1,     1,
     1,     1,    39,     1,     4,     1,    15,    10,     3,     2,
     1,     8,     0,    50,    14,    34,     0,     0,    42,    37,
     1,     0,     1,     1,    49,    40,     0,     0,    16,     0,
     1,     1,     0,    24,     2,     0,    43,    23,     1,    41,
     0,    51,     1,     9,     2,     0,     1,     0,    32,    31,
    30,    33,    27,    28,    29,     0,     1,     0,     0,     0,
    44,    35,    48,     0,    17,    13,    11,    36,    20,    25,
     0,    21,    22,     1,     1,    26,     1,    18,     0,    46,
    45,    47,     0,     0,     0
};

static const short yydefgoto[] = {    17,
    18,    19,    20,    21,    22,    23,    24,    25,    76,    46,
    47,    48,    66,    93,     8,     9,    13,    30,    72,    91,
    14,    10
};

static const short yypact[] = {    85,
    30,-32768,-32768,-32768,-32768,-32768,    87,    -5,     2,    16,
    85,    90,-32768,    16,-32768,    63,-32768,-32768,-32768,     4,
    40,    36,    19,-32768,-32768,-32768,    59,   100,-32768,-32768,
     2,     5,    63,    63,-32768,-32768,    34,    63,-32768,    63,
    63,     2,   100,    70,    44,    48,    61,-32768,    71,-32768,
    67,-32768,    74,-32768,-32768,    79,    71,    95,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    63,    63,   100,   100,   109,
-32768,-32768,-32768,    63,-32768,-32768,-32768,-32768,-32768,-32768,
    96,-32768,-32768,    63,    74,-32768,   111,-32768,   100,-32768,
-32768,    61,   121,   122,-32768
};

static const short yypgoto[] = {     3,
    -4,-32768,     0,    -3,-32768,-32768,   -12,    25,    38,    15,
   -36,-32768,-32768,-32768,   113,    -8,-32768,   -23,    68,-32768,
-32768,    99
};


#define	YYLAST		126


static const short yytable[] = {     6,
     6,    31,     5,    37,    28,    36,    58,    49,    34,    26,
     6,    29,     5,     5,     7,    12,     5,    34,    57,    27,
    51,     1,   -12,    44,    50,     7,     6,    45,    56,    27,
    34,    82,    83,    29,    53,    54,    35,    41,    44,    55,
     2,     7,    45,    40,    29,    39,     3,     4,    11,    15,
     2,    71,    92,    52,    81,    75,     3,     4,    39,    71,
    38,    80,   -19,    44,    44,    55,    67,    45,    45,     2,
    85,    87,    15,     2,    70,     3,     4,    68,    69,     3,
     4,    59,    60,    61,    44,    62,    73,    75,    45,    90,
     1,    63,    64,    65,    74,     2,    15,     2,    77,    15,
     2,     3,     4,     3,     4,    16,     3,     4,    33,    15,
     2,    68,    69,    84,    79,    86,     3,     4,    43,    89,
    94,    95,    88,    32,    78,    42
};

static const short yycheck[] = {     0,
     1,    10,     0,    16,     3,    14,    43,    31,    12,    15,
    11,     9,    10,    11,     0,     1,    14,    21,    42,    25,
    33,     6,    19,    28,    20,    11,    27,    28,    41,    25,
    34,    68,    69,    31,    38,    40,    12,    19,    43,    40,
    11,    27,    43,     8,    42,    21,    17,    18,    19,    10,
    11,    49,    89,    20,    67,    53,    17,    18,    34,    57,
    21,    66,    19,    68,    69,    66,    19,    68,    69,    11,
    74,    84,    10,    11,     4,    17,    18,    17,    18,    17,
    18,    12,    13,    14,    89,    16,    20,    85,    89,    87,
     6,    22,    23,    24,    21,    11,    10,    11,    20,    10,
    11,    17,    18,    17,    18,    19,    17,    18,    19,    10,
    11,    17,    18,     5,    20,    20,    17,    18,    19,     9,
     0,     0,    85,    11,    57,    27
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

case 4:
#line 79 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.string = yystring; ;
    break;}
case 5:
#line 82 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.string = yystring; ;
    break;}
case 6:
#line 83 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.string = "or"; ;
    break;}
case 7:
#line 84 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.string = "and"; ;
    break;}
case 9:
#line 88 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.arglist->rename = yyvsp[0].string; ;
    break;}
case 10:
#line 91 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.arglist = newarg(yyvsp[0].string, 0, NULL); ;
    break;}
case 11:
#line 92 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.arglist = newarg(yyvsp[-3].string, 1, yyvsp[-1].arglist); ;
    break;}
case 13:
#line 98 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.arglist = yyvsp[-3].arglist; yyval.arglist->next = yyvsp[-1].arglist; yyvsp[-1].arglist->next = yyvsp[0].arglist; ;
    break;}
case 15:
#line 102 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.arglist = NULL; ;
    break;}
case 16:
#line 103 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; ;
    break;}
case 17:
#line 106 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.arglist = NULL; ;
    break;}
case 18:
#line 107 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.arglist = yyvsp[-1].arglist; yyval.arglist->next = yyvsp[0].arglist; ;
    break;}
case 20:
#line 117 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.comp = yyvsp[-1].comp; ;
    break;}
case 21:
#line 118 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, 1); ;
    break;}
case 22:
#line 119 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.comp = comp_join(yyvsp[-2].comp, yyvsp[0].comp, 0); ;
    break;}
case 24:
#line 123 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.comp = comp_new(yyvsp[0].string, EXIST, NULL, 0); ;
    break;}
case 25:
#line 124 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.comp = comp_new(yyvsp[-2].string, yyvsp[-1].op, &(yyvsp[0].string), 1); ;
    break;}
case 26:
#line 125 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{
					int argc; char ** argv;
					arglist2argv(yyvsp[-1].arglist, &argc, &argv);
					yyval.comp = comp_new(yyvsp[-3].string, FUNC, argv, argc);
					yyval.comp->arglist = yyvsp[-1].arglist;
				;
    break;}
case 27:
#line 133 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.op = EQ; ;
    break;}
case 28:
#line 134 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.op = GT; ;
    break;}
case 29:
#line 135 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.op = LT; ;
    break;}
case 30:
#line 136 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.op = GEQ; ;
    break;}
case 31:
#line 137 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.op = LEQ; ;
    break;}
case 32:
#line 138 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.op = NEQ; ;
    break;}
case 33:
#line 139 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.op = LIKE; ;
    break;}
case 34:
#line 145 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ 
#ifdef DEBUG
	   	smacq_graph_print(stderr, yyvsp[-1].graph.head, 0); 
#endif
		Graph = yyvsp[-1].graph.head;
		return 0;
	   ;
    break;}
case 35:
#line 157 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{
	   	yyval.graph.head = (yyval.graph.tail = NULL);
	   	graph_join(&(yyval.graph), yyvsp[-2].graph);
		graph_join(&(yyval.graph), yyvsp[-1].graph);
		if (yyvsp[0].group.args) {
			graph_join(&(yyval.graph), newgroup(yyvsp[0].group, yyvsp[-3].vphrase));
		} else {
			graph_join(&(yyval.graph), newmodule(yyvsp[-3].vphrase.verb, yyvsp[-3].vphrase.args));
		}
	   ;
    break;}
case 36:
#line 168 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{
	   	yyval.graph.head = (yyval.graph.tail = NULL);
	   	graph_join(&(yyval.graph), yyvsp[-4].graph);
		graph_join(&(yyval.graph), yyvsp[-1].graph);
		if (yyvsp[0].group.args) {
			graph_join(&(yyval.graph), newgroup(yyvsp[0].group, yyvsp[-2].vphrase));
		} else {
			graph_join(&(yyval.graph), newmodule(yyvsp[-2].vphrase.verb, yyvsp[-2].vphrase.args));
		}
	   ;
    break;}
case 37:
#line 179 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{
	   	yyval.graph.head = (yyval.graph.tail = NULL);
	   	graph_join(&(yyval.graph), yyvsp[-1].graph);
		graph_join(&(yyval.graph), yyvsp[0].graph);
	   ;
    break;}
case 38:
#line 186 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.graph.head = NULL; yyval.graph.tail = NULL; ;
    break;}
case 39:
#line 187 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.graph = yyvsp[0].graph; ;
    break;}
case 40:
#line 191 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{
	   			yyval.graph.head = (yyval.graph.tail = NULL);
	   			graph_join(&(yyval.graph), yyvsp[0].graph);
				graph_join(&(yyval.graph), yyvsp[-1].graph);
			;
    break;}
case 41:
#line 196 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.graph = yyvsp[-1].graph; ;
    break;}
case 42:
#line 199 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.graph = nullgraph; ;
    break;}
case 43:
#line 200 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.graph = optimize_bools(yyvsp[0].comp); ;
    break;}
case 44:
#line 203 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.group.args = NULL; yyval.group.having = NULL;;
    break;}
case 45:
#line 204 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.group.args = yyvsp[-1].arglist; yyval.group.having = newarg(print_comparison(yyvsp[0].comp), 0, NULL); ;
    break;}
case 46:
#line 207 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.comp = NULL; ;
    break;}
case 47:
#line 208 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.comp = yyvsp[0].comp; ;
    break;}
case 48:
#line 211 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.graph = newmodule(yyvsp[-3].string, yyvsp[-1].arglist); ;
    break;}
case 49:
#line 212 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.graph = newmodule(yyvsp[-1].string, yyvsp[0].arglist); ;
    break;}
case 50:
#line 215 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.vphrase = newvphrase(yyvsp[-1].string, yyvsp[0].arglist); ;
    break;}
case 51:
#line 216 "/Users/mfisk/smacq/libsmacq/sql/parser.y"
{ yyval.vphrase = newvphrase(yyvsp[-3].string, yyvsp[-1].arglist); ;
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
#line 219 "/Users/mfisk/smacq/libsmacq/sql/parser.y"


extern void yy_scan_string(char*);

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

