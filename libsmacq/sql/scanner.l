%{
#define YY_NO_UNPUT
#include <smacq-parser.h>
#include <parser.h>
#include <string.h>
#define MAX_STR_CONST 1024

char string_buf[MAX_STR_CONST];
char *yystring;

%}

%option noyywrap

quote		\'
dquote		\"
space		[ \t\n\r\f]+
toksymbols	[\,\=\<\>\(\)]
id		[^\!\'\"\,\=\<\>\(\) \t\n\r\f]+

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

{toksymbols}    	return yytext[0];

<<EOF>>		{ return YYSTOP; }

{id}	{ 
			yystring = strdup((char *)yytext); 
			return YYID; 
	}

.	{ yyerror("Unexpected character in query"); return -1; }

%%

