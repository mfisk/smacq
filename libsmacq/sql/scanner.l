%{

#include "grammer.h"
#include <string.h>
#define MAX_STR_CONST 1024

extern void yyerror(const char *);

char string_buf[MAX_STR_CONST];
char *yystring;

%}

space		[ \t\n\r\f]
quote		\'
dquote		\"
number		-?[0-9.]+
id		[a-zA-Z][a-zA-Z0-9_\.\:]*

%x xdquote
%x xquote

%%

where|WHERE	return WHERE;
from|FROM	return FROM;
group|GROUP	return GROUP;
by|BY		return BY;

{quote}		BEGIN(xquote);
<xquote>{
	{quote}	BEGIN(0);
	[^']+	{
			yystring = strdup(yytext);
			return STRING;
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
				return STRING;
			}
	.		*yystring++ = yytext[0];
	}

[\ \n\t]	{ ; }

[,>()]    	return yytext[0];

{id}		{ 
			yystring = strdup((char *)yytext); 
			return ID; 
		}

<<EOF>>		{ return STOP; }

.		{ yyerror("Unexpected character"); }

%%

