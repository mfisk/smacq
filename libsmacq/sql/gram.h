#ifndef BISON_GRAM_H
# define BISON_GRAM_H

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


extern YYSTYPE yylval;

#endif /* not BISON_GRAM_H */
