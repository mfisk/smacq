/*
 * BUGS:
 *
 *    - Cannot call the same function twice with different "as" names
 *      This is because renames are done as a batch just before the VERB
 *
 *    - Cannot use nested functions as arguments
 *      FIX: we-write arguments recursively or something
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
static struct graph nullgraph = { head: NULL, tail: NULL };
static smacq_graph * Graph;

%}

%token WHERE
%token GROUP
%token BY
%token FROM
%token SELECT
%token AS
%token HAVING
%token YYSTRING YYID
%token YYNEQ YYLEQ YYGEQ

%token YYSTOP YYLIKE YYOR YYAND
%left YYAND YYOR
%right FROM

%type <arglist> joins having arg argument boolarg boolargs args moreargs moreboolargs
%type <group> group 
%type <graph> where query from source pverbphrase  
%type <vphrase> verbphrase
%type <string> function verb word string id as
%type <op> op
%type <comp> boolean test 

%union {
  struct graph graph;
  struct arglist * arglist;
  struct vphrase vphrase;
  char * string;
  struct group group;
  dts_compare_operation op;
  dts_comparison * comp;
}
%%

queryline: query YYSTOP	
	   { 
#ifdef DEBUG
	   	smacq_graph_print(stderr, $1.head, 0); 
#endif
		Graph = $1.head;
		return 0;
	   }
	;

null:   /* empty */ ;

query : verbphrase from where group
           {
	   	$$.head = ($$.tail = NULL);
	   	graph_join(&($$), $2);
		graph_join(&($$), $3);
		if ($4.args) {
			graph_join(&($$), newgroup($4, $1));
		} else {
			graph_join(&($$), newmodule($1.verb, $1.args));
		}
	   }
	;

from :  null 			{ $$.head = NULL; $$.tail = NULL; } 
	| FROM source joins 	
	   { 
		$$ = $2; 
	   	if ($3) {
			graph_join(&($$), newmodule("join", $3));
		}
	   }
	;

joins : null			{ $$ = NULL; }
	| ',' boolargs as joins	
	   {
		struct arglist * atail;
		$$ = newarg(arglist2str($2), 0, NULL);

	   	fprintf(stderr, "got a join with '%s' as %s.\n", $$->arg, $3); 

		if (!$3) {
			yyerror("Joins must be aliased with \"as <alias>\"");
		}

		atail = arglist_append($$, newarg($3, 0, NULL));

	   	if ($4) {
			atail = arglist_append(atail, $4);
		} 
	   }
	;

as : null		{ $$ = NULL; }
	| AS word	{ $$ = $2; }
	;

source : pverbphrase		
	| '(' query ')'	{ $$ = $2; }
	;

where : null 		{ $$ = nullgraph; }
        | WHERE boolean { $$ = optimize_bools($2); }
	;

group : null 			{ $$.args = NULL; $$.having = NULL;}
	| GROUP BY args having 	{ $$.args = $3; $$.having = $4; }
	;

having : null			{ $$ = NULL; }
	| HAVING boolargs	{ $$ = $2; }
	;

word:	id 		
	| string  
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

boolarg : id				{ $$ = newarg($1, 0, NULL); }
	| string			
	    { 
	    	char * str = malloc(sizeof(char *) * (strlen($1)+2)); 
		sprintf(str,"\"%s\"", $1); 
		$$ = newarg(str, 0, NULL); 
	    }
	| '<'				{ $$ = newarg("<", 0, NULL); }
	| YYLEQ				{ $$ = newarg("<=", 0, NULL); }
	| '>'				{ $$ = newarg(">", 0, NULL); }
	| YYGEQ				{ $$ = newarg(">=", 0, NULL); }
	| '='				{ $$ = newarg("=", 0, NULL); }
	| YYNEQ				{ $$ = newarg("!=", 0, NULL); }
	| '('				{ $$ = newarg("(", 0, NULL); }
	| ')'				{ $$ = newarg(")", 0, NULL); }
	| '!'				{ $$ = newarg("!", 0, NULL); }
	;

function : id 
	;

pverbphrase: verb 		{ $$ = newmodule($1, NULL); }
	| verb '(' args ')' 	{ $$ = newmodule($1, $3); }
	;

verbphrase : verb args 		{ $$ = newvphrase($1, $2); }
	| verb '(' args ')'	{ $$ = newvphrase($1, $3); }
	;

boolargs : boolarg moreboolargs	{ $$ = $1; $$->next = $2; }
	;

moreboolargs : null		{ $$ = NULL; }
	| boolarg moreboolargs	{ $$ = $1; $$->next = $2; }
	;

args : null 			{ $$ = NULL; }
	| arg moreargs 		{ $$ = $1; $$->next = $2; }
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

test : word			{ $$ = comp_new($1, EXIST, NULL, 0); }
	| word op word		{ $$ = comp_new($1, $2, &($3), 1); }
	| verb '(' args ')'	{
					int argc; char ** argv;
					arglist2argv($3, &argc, &argv);
					$$ = comp_new($1, FUNC, argv, argc);
					$$->arglist = $3;
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

