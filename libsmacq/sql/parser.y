%start queryline
%name-prefix="yysmacql_"

%{
void yysmacql_error(char*);

#include <smacq-parser.h>
static struct graph nullgraph = { head: NULL, tail: NULL };
static smacq_graph * Graph;
%}

%type <graph> pverbphrase from source query where 
%type <vphrase> verbphrase
%type <group> group
%type <comp> having

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


queryline: query YYSTOP	
	   { 
#ifdef DEBUG
	   	smacq_graph_print(stderr, $1.head, 0); 
#endif
		Graph = $1.head;
		return 0;
	   }
	;

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
	| query '|' verbphrase where group
           {
	   	$$.head = ($$.tail = NULL);
	   	graph_join(&($$), $1);
		graph_join(&($$), $4);
		if ($5.args) {
			graph_join(&($$), newgroup($5, $3));
		} else {
			graph_join(&($$), newmodule($3.verb, $3.args));
		}
	   }
	| from where 
           {
	   	$$.head = ($$.tail = NULL);
	   	graph_join(&($$), $1);
		graph_join(&($$), $2);
	   }
	;

from :  null 			{ $$.head = NULL; $$.tail = NULL; } 
	| FROM source 		{ $$ = $2; }
	;

source : pverbphrase from
		        {
	   			$$.head = ($$.tail = NULL);
	   			graph_join(&($$), $2);
				graph_join(&($$), $1);
			}
	| '(' query ')'	{ $$ = $2; }
	;

where : null 		{ $$ = nullgraph; }
        | WHERE boolean { $$ = optimize_bools($2); }
	;

group : null 			{ $$.args = NULL; $$.having = NULL;}
	| GROUP BY args having 	{ $$.args = $3; $$.having = newarg(print_comparison($4), 0, NULL); }
	;

having : null			{ $$ = NULL; }
	| HAVING boolean	{ $$ = $2; }
	;

pverbphrase: verb '(' args ')' 	{ $$ = newmodule($1, $3); }
	| verb spacedargs  	{ $$ = newmodule($1, $2); }
	;

verbphrase : verb args 		{ $$ = newvphrase($1, $2); }
	| verb '(' args ')'	{ $$ = newvphrase($1, $3); }
	;

%%

extern void yysmacql_scan_string(char*);
extern char * yysmacql_text;

#ifdef PTHREAD_MUTEX_INITIALIZER
  static pthread_mutex_t local_lock = PTHREAD_MUTEX_INITIALIZER;
#else
  static pthread_mutex_t local_lock;
  #warning "No PTHREAD_MUTEX_INITIALIZER"
#endif

smacq_graph * smacq_build_query(dts_environment * tenv, int argc, char ** argv) {
  int size = 0;
  int i;
  char * qstr; 
  smacq_graph * graph;
  int res;

  parse_tenv = tenv;

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

  /* LOCK */
  pthread_mutex_lock(&local_lock);

  yysmacql_scan_string(qstr);
  //fprintf(stderr, "parsing buffer: %s\n", qstr); 

  res = yysmacql_parse();

  graph = Graph;

  /* UNLOCK */
  pthread_mutex_unlock(&local_lock);

  if (res) {
    fprintf(stderr, "smacq_build_query: error parsing query: %s\n", qstr);
    return NULL;
  }

  if (!graph) {
    fprintf(stderr, "unknown parse error\n");
  }

  return graph;
}

void yysmacql_error(char * msg) {
  fprintf(stderr, "%s near %s\n", msg, yysmacql_text-1);
}

