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
# define	UNIQ	273
# define	SPLIT	274
# define	COUNT	275
# define	PCAPFILE	276
# define	PCAPLIVE	277
# define	IDENT	278
# define	FCONST	279
# define	SCONST	280
# define	BITCONST	281
# define	Op	282
# define	ICONST	283
# define	PARAM	284
# define	OP	285
# define	POSTFIXOP	286
# define	UMINUS	287
# define	TYPECAST	288


extern YYSTYPE yylval;

#endif /* not BISON_GRAM_H */
