typedef union {
  struct graph graph;
  struct arglist * arglist;
  struct vphrase vphrase;
  char * string;
  struct group group;
  dts_compare_operation op;
  dts_comparison * comp;
} YYSTYPE;
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


extern YYSTYPE yylval;
