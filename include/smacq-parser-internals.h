extern int yylex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param );
extern void yyerror(YYLTYPE * yylloc, const char * msg);

#define YYLEX_PARAM &yylval, &yylloc
#define YYERROR_VERBOSE 1
#define yyerror(Msg) yyerror (&yylloc, Msg)

