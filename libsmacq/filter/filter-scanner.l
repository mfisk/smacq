%{
#define YY_NO_UNPUT
#include "filter.h"
#include "filter-parser.h"
#include <string.h>
#define MAX_STR_CONST 1024

extern void yyerror(const char *);

char string_buf[MAX_STR_CONST];
char *yystring;

%}

%option noyywrap


space		[ \t\n\r\f]
quote		\'
dquote		\"
id		[a-zA-Z0-9\-_\.\:]+

%x xdquote
%x xquote

%%

like		return YYLIKE;
and		return YYAND;
"&&"		return YYAND;
or		return YYOR;
"||"		return YYOR;

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
	\\(.)		*yystring++ = yytext[1];
	{dquote}		{
				*yystring = '\0'; 
			 	yystring = strdup(string_buf);
			 	BEGIN(0); 
				return YYSTRING;
			}
	.		*yystring++ = yytext[0];
	}

[\ \n\t]	{ ; }

[\!\<\>\(\)]    	return yytext[0];

=+		{ return yytext[0]; }

{id}		{ 
			yystring = strdup((char *)yytext); 
			return YYID; 
		}

<<EOF>>		{ return YYSTOP; }

.		{ yyerror("Unexpected character"); }

%%

