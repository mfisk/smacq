#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

#ifndef YYSTYPE
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
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
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


extern YYSTYPE yylval;

#endif /* not BISON_Y_TAB_H */
