%{
#define YY_NO_UNPUT
#include <smacq-parser.h>
#include <parser.h>
#include <string.h>
#define MAX_STR_CONST 1024

char string_buf[MAX_STR_CONST];
char *yystring;
#define yysmacql__scan_string yysmacql_scan_string
%}

%option noyywrap

quote		\'
dquote		\"
space		[[:space:]]+
arithop		[\-\+\*\/]
sign		[\-\+]?
digits		[0-9e\.]
toksymbols	[\|\,\=\<\>\(\)]|{arithop}
number		{sign}{digits}+
id		[^\!\'\"\,\=\<\>\(\)[:space:]\|]+

%x xdquote
%x xquote

%%

where|WHERE	return WHERE;
from|FROM	return FROM;
group|GROUP	return GROUP;
by|BY		return BY;
as|AS		return AS;
having|HAVING	return HAVING;
"&&"|and|AND    return YYAND;
"||"|or|OR      return YYOR;
like|LIKE       return YYLIKE;
">="		return YYGEQ;
"<="		return YYLEQ;
"!="		return YYNEQ;
"=="		return '=';

{quote}		BEGIN(xquote);
<xquote>{
	{quote}	BEGIN(0);
	[^']+	{
			yystring = strdup(yysmacql_text);
			return YYSTRING;
		}

	}

{dquote}		yystring = string_buf; BEGIN(xdquote);
<xdquote>{
	"\\n"		*yystring++ = '\n';
	"\\r"		*yystring++ = '\r';
	"\\t"		*yystring++ = '\t';
	"\\f"		*yystring++ = '\f';
	\\\\		{
				*yystring++ = yysmacql_text[0]; 
				*yystring++ = yysmacql_text[1];
			}
	\\{dquote}	*yystring++ = yysmacql_text[1];
	{dquote}	{
				*yystring = '\0'; 
			 	yystring = strdup(string_buf);
			 	BEGIN(0); 
				return YYSTRING;
			}
	.		*yystring++ = yysmacql_text[0];
	}

{space}	{ ; }

<<EOF>>		{ return YYSTOP; }

{toksymbols}    	return yysmacql_text[0];

{number}	{ 
			yystring = strdup((char *)yysmacql_text); 
			return YYNUMBER; 
	}

{id}	{ 
			yystring = strdup((char *)yysmacql_text); 
			return YYID; 
	}

.	{ fprintf(stderr, "Unexpected character \"%s\"\n", yysmacql_text); return -1; }

%%

