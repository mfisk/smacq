#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

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
# define	WHERE	257
# define	UNTIL	258
# define	GROUP	259
# define	BY	260
# define	FROM	261
# define	SELECT	262
# define	AS	263
# define	HAVING	264
# define	UNION	265
# define	YYSTRING	266
# define	YYID	267
# define	YYNUMBER	268
# define	YYNEQ	269
# define	YYLEQ	270
# define	YYGEQ	271
# define	YYSTOP	272
# define	YYLIKE	273
# define	YYOR	274
# define	YYAND	275
# define	YYNOT	276


extern YYSTYPE yylval;

#endif /* not BISON_Y_TAB_H */
