%start booleanline
%type <comp> booleanline

%{
     #define    yymaxdepth yyfiltermaxdepth
     #define    yyparse yyfilterparse
     #define    yylex   yyfilterlex
     #define    yyerror yyfiltererror
     #define    yylval  yyfilterlval
     #define    yychar  yyfilterchar
     #define    yydebug yyfilterdebug
     #define    yypact  yyfilterpact
     #define    yyr1    yyfilterr1
     #define    yyr2    yyfilterr2
     #define    yydef   yyfilterdef
     #define    yychk   yyfilterchk
     #define    yypgo   yyfilterpgo
     #define    yyact   yyfilteract
     #define    yyexca  yyfilterexca
     #define yyerrflag yyfiltererrflag
     #define yynerrs    yyfilternerrs
     #define    yyps    yyfilterps
     #define    yypv    yyfilterpv
     #define    yys     yyfilters
     #define    yy_yys  yyfilteryys
     #define    yystate yyfilterstate
     #define    yytmp   yyfiltertmp
     #define    yyv     yyfilterv
     #define    yy_yyv  yyfilteryyv
     #define    yyval   yyfilterval
     #define    yylloc  yyfilterlloc
     #define yyreds     yyfilterreds
     #define yytoks     yyfiltertoks
     #define yylhs      yyfilteryylhs
     #define yylen      yyfilteryylen
     #define yydefred yyfilteryydefred
     #define yydgoto    yyfilteryydgoto
     #define yysindex yyfilteryysindex
     #define yyrindex yyfilteryyrindex
     #define yygindex yyfilteryygindex
     #define yytable     yyfilteryytable
     #define yycheck     yyfilteryycheck
     #define yyname   yyfilteryyname
     #define yyrule   yyfilteryyrule
#include <smacq.h>
static dts_comparison * Comp;
#define yyfilterlex yysmacql_lex
%}

/*
 * BUGS:
 *
 *    - Cannot call the same function twice with different "as" names
 *      This is because renames are done as a batch just before the VERB
 *
 *    - Cannot use nested functions as arguments
 *      FIX: re-write arguments recursively or something
 *
 */

/*
 * Examples:
 *
 *	print srcip from (uniq srcip, dstip from pcapfile("/hog/traces/aa.80")) group by srcip having "counter>10"
 *	print srcip, counter(), sum(len) from (uniq srcip, dstip from pcapfile("/hog/traces/aa.80")) group by srcip having "counter>10"
 *
 */
 
%{
#include <smacq-parser.h>
%}

%token WHERE
%token GROUP
%token BY
%token FROM
%token SELECT
%token AS
%token HAVING
%token UNION
%token YYSTRING YYID YYNUMBER
%token YYNEQ YYLEQ YYGEQ

%token YYSTOP YYLIKE YYOR YYAND YYNOT

%left YYAND YYOR
%right YYNOT

%left '+' '-'
%left '*' '/'

%type <arglist> arg argument args moreargs spacedargs
%type <string> function word string id number
%type <op> op
%type <comp> boolean test 
%type <operand> operand expression subexpression
%type <arithop> arithop

%union {
  struct graph graph;
  struct arglist * arglist;
  struct vphrase vphrase;
  char * string;
  struct group group;
  dts_compare_operation op;
  enum dts_arith_operand_type arithop;
  dts_comparison * comp;
  struct dts_operand * operand;
}
%%

null:   /* empty */ ;


word:	id 		
	| string
	| number 
	;

number:	YYNUMBER 	{ $$ = yystring; }
	;

string:	YYSTRING 	{ $$ = yystring; }
	;

id:	YYID 		{ $$ = yystring; }
        | YYOR          { $$ = "or"; }
        | YYAND         { $$ = "and"; }
        ;

arg: argument 
	| argument AS word		{ $$->rename = $3; }
	;

argument : word 			{ $$ = newarg($1, WORD, NULL); } 
	| function '(' args ')' 	                  { $$ = newarg($1, FUNCTION, $3); }
	| '[' expression ']'	                  { $$ = newarg("expr", FUNCTION, 
					       newarg(print_operand($2), WORD, NULL)); 
			                  }
	;

function : id 
	;

args :  arg ',' arg moreargs 	{ $$ = $1; $$->next = $3; $3->next = $4; }
	| spacedargs	
	;

spacedargs : null		{ $$ = NULL; }
	| arg spacedargs 	{ $$ = $1; $$->next = $2; }
	;

moreargs : null			{ $$ = NULL; }
	| ',' arg moreargs 	{ $$ = $2; $$->next = $3; }
	;



/************* From boolean parser: **********************/


boolean : '(' boolean ')'	{ $$ = $2; }
	| boolean YYOR boolean	{ $$ = comp_join($1, $3, OR); }
	| boolean YYAND boolean	{ $$ = comp_join($1, $3, AND); }
	| YYNOT boolean 	{ $$ = comp_join($2, NULL, NOT); }
	| test 		
	;

operand : id			{ $$ = comp_operand(FIELD, $1); }
	| string 		{ $$ = comp_operand(CONST, $1); }
	| number 		{ $$ = comp_operand(CONST, $1); }
	;


expression :   
	'(' expression ')' 
				{	  
				  $$ = $2;  
				} 
	| subexpression arithop subexpression  
				{
				  $$ = comp_arith(parse_tenv, $2, $1, $3); 
				}
	;

subexpression : 
	expression
	| operand
	;

test : 	
	operand		{  $$ = comp_new(EXIST, $1, $1); }
	| subexpression op subexpression      { $$ = comp_new($2, $1, $3); }
	| function '(' args ')'	{ 
				  int argc; char ** argv;
				  arglist2argv($3, &argc, &argv);
				  $$ = comp_new_func($1, argc, argv, $3);
				}
	;

op : '=' 		{ $$ = EQ; }
	| '>'		{ $$ = GT; }
	| '<' 		{ $$ = LT; }
	| YYGEQ		{ $$ = GEQ; }
	| YYLEQ		{ $$ = LEQ; }
	| YYNEQ		{ $$ = NEQ; }
	| YYLIKE 	{ $$ = LIKE; }		 
	;

arithop : '+'		{ $$ = ADD; }
	| '-'		{ $$ = SUB; }
	| '/'		{ $$ = DIVIDE; }
	| '*'		{ $$ = MULT; }
	;


booleanline: boolean YYSTOP 	{
					Comp = $1;
					return 0;
				}
	;

%%

static dts_environment * tenv;
extern void yysmacql_scan_string(char *);

#ifndef SMACQ_OPT_NOPTHREADS
#ifdef PTHREAD_MUTEX_INITIALIZER
  static pthread_mutex_t local_lock = PTHREAD_MUTEX_INITIALIZER;
#else
  static pthread_mutex_t local_lock;
  #warning "No PTHREAD_MUTEX_INITIALIZER"
#endif
#endif

static void print_comp(dts_environment * tenv, dts_comparison * c) {
	char * op;
	op = "UNDEFINED";

	if (!c) {
		// fprintf(stderr,". (%p)\n", c);
		return;
	}

	switch(c->op) {
		case AND: op = "AND"; break;
		case OR: op = "OR"; break;
		case GT: op = "<"; break;
		case LT: op = ">"; break;
		case EQ: op = "="; break;
		case NEQ: op = "!="; break;
		case GEQ: op = ">="; break;
		case LEQ: op = "<="; break;
		case EXIST: op = "exist"; break;
		case LIKE: op = "like"; break;
		case FUNC: op = "FN"; break;
		case NOT: op = "!"; break;
	}

#if DEBUG
	switch(c->op) {
		case AND:
		case OR:
			fprintf(stderr, "Comparison %p: op %s, next %p, group %p\n", c, op, c->next, c->group);
			break;

		case FUNC:
			fprintf(stderr, "Comparison %p: FN %s(), next %p, group %p\n", c, c->valstr, c->next, c->group);
			break;

		default:
			fprintf(stderr, "Comparison %p: field %d... %s %s, next %p, group %p\n", c, c->field[0], op, c->valstr, c->next, c->group);
			break;
			
	}
#endif

	print_comp(tenv, c->group);
	print_comp(tenv, c->next);

}

dts_comparison * dts_parse_tests(dts_environment * localtenv, int argc, char ** argv) {
  dts_comparison * retval;
  int size = 1;
  int i;
  char * qstr;

  /* LOCK */
  tenv = localtenv;

  for (i=0; i<argc; i++) {
  	size += strlen(argv[i]);
  }
  size += argc;

  qstr = (char*)malloc(size);
  qstr[0] = '\0';
  	
  for (i=0; i<argc; i++) {
  	strcatn(qstr, size, argv[i]);
  	strcatn(qstr, size, " ");
  }
  yysmacql_scan_string(qstr);
  //fprintf(stderr, "parsing filter buffer: %s\n", qstr); 

  if (yyfilterparse()) {
  	/* Should free the comparisons? */
        fprintf(stderr, "got nonzero return parsing boolean %s\n", qstr); 
  	return NULL;
  }

  retval = Comp;

  /* UNLOCK */
  print_comp(localtenv, retval);

  return retval;
}

void yyfiltererror(char * msg) {
  fprintf(stderr, "Error: %s near %s\n", msg, yytext);
  //exit(-1);
}



