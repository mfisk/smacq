/*-------------------------------------------------------------------------
 *
 * keywords.c
 *	  lexical token lookup for reserved words in PostgreSQL
 *
 * Portions Copyright (c) 1996-2001, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  $Header: /home/cvs/flow/libsmacq/sql/keywords.c,v 1.1 2002/10/30 17:39:26 wbarber Exp $
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include <ctype.h>

#include "nodes/parsenodes.h"
#include "parser/keywords.h"
//#include "parser/parse.h"
#include "gram.h"

/*
 * List of (keyword-name, keyword-token-value) pairs.
 *
 * !!WARNING!!: This list must be sorted, because binary
 *		 search is used to locate entries.
 */
static ScanKeyword ScanKeywords[] = {
	/* name, value */
	{"all", ALL},
	{"and", AND},
	{"any", ANY},
	{"asc", ASC},
	{"by", BY},
	{"desc", DESC},
	{"distinct", DISTINCT},
	{"from", FROM},
	{"group", GROUP},
	{"last", LAST},
	{"not", NOT},
	{"or", OR},
	{"order", ORDER},
	{"print", PRINT},
	{"select", SELECT},
	{"top", TOP},
	{"uniq", UNIQ},
	{"using", USING},
	{"where", WHERE},
};

/*
 * ScanKeywordLookup - see if a given word is a keyword
 *
 * Returns a pointer to the ScanKeyword table entry, or NULL if no match.
 *
 * The match is done case-insensitively.  Note that we deliberately use a
 * dumbed-down case conversion that will only translate 'A'-'Z' into 'a'-'z',
 * even if we are in a locale where tolower() would produce more or different
 * translations.  This is to conform to the SQL99 spec, which says that
 * keywords are to be matched in this way even though non-keyword identifiers
 * receive a different case-normalization mapping.
 */
ScanKeyword *
ScanKeywordLookup(char *text)
{
	int			len,
				i;
	char		word[NAMEDATALEN];
	ScanKeyword *low;
	ScanKeyword *high;

	len = strlen(text);
	/* We assume all keywords are shorter than NAMEDATALEN. */
	if (len >= NAMEDATALEN)
		return NULL;

	/*
	 * Apply an ASCII-only downcasing.	We must not use tolower() since it
	 * may produce the wrong translation in some locales (eg, Turkish),
	 * and we don't trust isupper() very much either.  In an ASCII-based
	 * encoding the tests against A and Z are sufficient, but we also
	 * check isupper() so that we will work correctly under EBCDIC.  The
	 * actual case conversion step should work for either ASCII or EBCDIC.
	 */
	for (i = 0; i < len; i++)
	{
		char		ch = text[i];

		if (ch >= 'A' && ch <= 'Z' && isupper((unsigned char) ch))
			ch += 'a' - 'A';
		word[i] = ch;
	}
	word[len] = '\0';

	/*
	 * Now do a binary search using plain strcmp() comparison.
	 */
	low = &ScanKeywords[0];
	high = endof(ScanKeywords) - 1;
	while (low <= high)
	{
		ScanKeyword *middle;
		int			difference;

		middle = low + (high - low) / 2;
		difference = strcmp(middle->name, word);
		if (difference == 0)
			return middle;
		else if (difference < 0)
			low = middle + 1;
		else
			high = middle - 1;
	}

	return NULL;
}
