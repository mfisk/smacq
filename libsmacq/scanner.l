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
toksymbols	[\|\,\=\<\>\(\)\[\]]|{arithop}
number		{sign}{digits}+
id		[^\!\'\"\,\=\<\>\(\)[:space:]\|]+

%x xdquote
%x xquote

%%

where|WHERE	return WHERE;
union|UNION	return UNION;
from|FROM	return FROM;
group|GROUP	return GROUP;
by|BY		return BY;
as|AS		return AS;
having|HAVING	return HAVING;
"&&"|and|AND    return YYAND;
"||"|or|OR      return YYOR;
!|not|NOT       return YYNOT;
like|LIKE       return YYLIKE;
">="		return YYGEQ;
"<="		return YYLEQ;
"!="		return YYNEQ;
"=="		return '=';

{quote}		BEGIN(xquote);
<xquote>{
	{quote}	BEGIN(0);
	[^']+	{
			yystring = strdup(yytext);
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
				*yystring++ = yytext[0]; 
				*yystring++ = yytext[1];
			}
	\\{dquote}	*yystring++ = yytext[1];
	{dquote}	{
				*yystring = '\0'; 
			 	yystring = strdup(string_buf);
			 	BEGIN(0); 
				return YYSTRING;
			}
	.		*yystring++ = yytext[0];
	}

{space}	{ ; }

<<EOF>>		{ return YYSTOP; }

{toksymbols}    	return yytext[0];

{number}	{ 
			yystring = strdup((char *)yytext); 
			return YYNUMBER; 
	}

{id}	{ 
			yystring = strdup((char *)yytext); 
			return YYID; 
	}

.	{ fprintf(stderr, "Unexpected character \"%s\"\n", yytext); return -1; }

%%
