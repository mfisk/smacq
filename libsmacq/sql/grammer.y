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
//static struct graph nullgraph = { head: NULL, tail: NULL };
//static smacq_graph * Graph;

%}

%token WHERE
%token GROUP
%token BY
%token FROM
%token SELECT
%token AS
%token HAVING
%token YYSTRING YYID YYNUMBER
%token YYNEQ YYLEQ YYGEQ

%token YYSTOP YYLIKE YYOR YYAND
%left YYAND YYOR
%right FROM

%type <arglist> arg argument args moreargs spacedargs
%type <string> function verb word string id number
%type <op> op
%type <comp> boolean test 
%type <operand> operand expression
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

argument : word 			{ $$ = newarg($1, 0, NULL); } 
	| function '(' args ')' 	{ $$ = newarg($1, 1, $3); }
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

verb :  id
	;


/************* From boolean parser: **********************/


boolean : '(' boolean ')'	{ $$ = $2; }
	| boolean YYOR boolean	{ $$ = comp_join($1, $3, 1); }
	| boolean YYAND boolean	{ $$ = comp_join($1, $3, 0); }
	| test 		
	;

operand : id			{ $$ = comp_operand(FIELD, $1); }
	| string 		{ $$ = comp_operand(CONST, $1); }
	| number 		{ $$ = comp_operand(CONST, $1); }
	;

expression : '(' expression arithop expression ')' {
				  $$ = comp_arith(parse_tenv, $3, $2, $4); 
				}
	| operand
	;

test : operand			{ $$ = comp_new(EXIST, $1, $1); }
	| expression op expression { $$ = comp_new($2, $1, $3); }
	| verb '(' args ')'	{ 
				  int argc; char ** argv;
				  struct dts_operand op;

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
