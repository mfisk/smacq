/*
 * BUGS:
 *
 *    - Use of renamed arguments later in the argument list is not supported:
 *		select counter() as count, count ....
 *      This is because renames are done as a batch just before the VERB
 */
 
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <smacq-internal.h>
#include "smacq-parser.h"
#define DEBUG
  
  extern int yylex();
  extern void yy_scan_string(const char *);
  void yyerror(char *);
  extern char * yytext;
  extern char * yystring;

  struct arglist {
    char * arg;
    char * rename;
    struct arglist * next;
    struct arglist * func_args;
    int isfunc;
  };

  static struct graph newmodule(char * module, struct arglist * alist);
  static void graph_join(struct graph * graph, struct graph newg);
  static struct graph newgroup(struct arglist *, struct vphrase);
  static void arglist2argv(struct arglist * al, int * argc, char *** argv);
  static struct arglist * newarg(char * arg, int isfunc, struct arglist * func_args);
  void print_graph(struct filter * f);

  struct graph nullgraph = { head: NULL, tail: NULL };
  
%}

%token WHERE
%token GROUP
%token BY
%token FROM
%token SELECT
%token STRING
%token ID
%token STOP
%token AS

%type <arglist> arg argument group args arglist moreargs
%type <graph> where query from source pverbphrase  
%type <vphrase> verbphrase
%type <string> function verb word string id 

%start queryline

%union {
  struct graph graph;
  struct arglist * arglist;
  struct vphrase vphrase;
  char * string;
}
%%

queryline: query STOP	
	   { 
#ifdef DEBUG
	   	print_graph($1.head); 
#endif
		return smacq_start($1.head, RECURSIVE, NULL); 
	   }
	;

null:   /* empty */ ;

query : verbphrase from where group
           {
	   	$$.head = ($$.tail = NULL);
	   	graph_join(&($$), $2);
		graph_join(&($$), $3);
		if ($4) {
			graph_join(&($$), newgroup($4, $1));
		} else {
			graph_join(&($$), newmodule($1.verb, $1.args));
		}
	   }
	;

from :  null 		{ $$.head = NULL; $$.tail = NULL; } 
	| FROM source 	{ $$ = $2; }
	;

source : pverbphrase		
	| '(' query ')'	{ $$ = $2; }
	;


where : null 		{ $$ = nullgraph; }
	| WHERE args 	{ $$ = newmodule("filter", $2); }
	;

group : null 		{ $$ = NULL; }
	| GROUP BY args { $$ = $3; }
	;

word:	id 		
	| string  
	;

string:	STRING 		{ $$ = yystring; };

id:	ID 		{ $$ = yystring; };

arg: argument 
	| argument AS word		{ $$->rename = $3; }
	;

argument : word 			{ $$ = newarg($1, 0, NULL); } 
	| function '(' arglist ')' 	{ $$ = newarg($1, 1, $3); }
	;

function : id 
	;

pverbphrase: verb 		{ $$ = newmodule($1, NULL); }
	| verb '(' arglist ')' 	{ $$ = newmodule($1, $3); }
	;

verbphrase : verb args 		{ $$.verb = $1; $$.args = $2; }
	;

args : 	'(' arglist ')' 	{ $$ = $2; }
	| arglist 
	;

/* argset : null | arg argset ; */

arglist : null 			{ $$ = NULL; }
	| arg moreargs 		{ $$ = $1; $$->next = $2; }
	;

moreargs : null			{ $$ = NULL; }
	| ',' arg moreargs 	{ $$ = $2; $$->next = $3; }
	;

/* arg : STRING ; */

verb :  id
	;

%%

int smacq_execute_query(int argc, char ** argv) {
  int size = 0;
  int i;
  char * qstr;

  for (i=0; i<argc; i++) {
  	size += strlen(argv[i]);
  }
  size += argc;

  qstr = (char*)malloc(size);
  	
  for (i=0; i<argc; i++) {
  	strcat(qstr, argv[i]);
  	strcat(qstr, " ");
  }
  yy_scan_string(qstr);
  /* fprintf(stderr, "parsing buffer: %s\n", qstr); */
  if (yyparse()) {
  	return 1;
  }

  return 0;
}

void yyerror(char * msg) {
  fprintf(stderr, "Error: %s near %s\n", msg, yytext);
  exit(-1);
}

static void arglist2argv(struct arglist * alist, int * argc, char *** argvp) {
	int i;
	char ** argv;
	struct arglist * al;

	for(i=0, al=alist; al; i++) {
		al=al->next;
	}
	argv = malloc(sizeof(char **) * i);
	*argc = i;
	*argvp = argv;

	for(i=0, al=alist; i<*argc; i++, al=al->next) {
		argv[i] = al->arg;
	}
}

static void graph_join(struct graph * graph, struct graph newg) {
	if (!newg.head) 
		return; /* Do nothing */

	assert(graph);

	if (!graph->head) {
		graph->head = newg.head;
		graph->tail = newg.tail;
		return;
	}

	fprintf(stderr, "Adding %s after %s\n", newg.head->name, graph->tail->name);
	/* Splice them together */
	assert(graph->tail);

	smacq_add_child(graph->tail, newg.head); 
	graph->tail = newg.tail;
}
	
static void graph_append(struct graph * graph, struct filter * newmod) {
	if (graph->tail) 
		smacq_add_child(graph->tail, newmod); 
	graph->tail = newmod;
	if (! graph->head) 
		graph->head = newmod;
}

/*
static struct arglist * arglist_alloc(struct arglist * addition) {
	struct arglist * newa = malloc(sizeof(struct arglist));
	newa->arg = addition;
	newa->NULL;
	return newa;
}
*/
	
static struct arglist * arglist_append(struct arglist * tail, struct arglist * addition) {
	for (; tail->next; tail=tail->next) ;
	tail->next = addition;
	for (; tail->next; tail=tail->next) ;

	return tail;
}
	
static struct graph newgroup(struct arglist * alist, struct vphrase vphrase) {
	/*
	 * This function violates some abstractions by knowing the 
	 * calling syntax for "groupby" and constructing arguments for it.
	 */
	struct arglist * atail;
	struct graph g;
	struct arglist * ap;

	if (!alist) { 
		/* Do nothing if this was "group by" NULL */
		return newmodule(vphrase.verb, vphrase.args);
	}

	atail = arglist_append(alist, newarg("--", 0, NULL));

	/* Insert function operations */
        for(ap=vphrase.args; ap; ap=ap->next) {
	   fprintf(stderr, "group arg %s isfunc = %d\n", ap->arg, ap->isfunc);
     	   if (ap->isfunc) {
	        atail = arglist_append(atail, newarg(ap->arg, 0, NULL));
	        atail = arglist_append(atail, ap->func_args);
	        atail = arglist_append(atail, newarg("|", 0, NULL));
		ap->isfunc = 0;
 	   }
	}

	atail = arglist_append(atail, newarg(vphrase.verb, 0, NULL));
	atail = arglist_append(atail, vphrase.args);

	g = newmodule("groupby", alist);

	return g;
}


static struct graph newmodule(char * module, struct arglist * alist) {
     struct arglist * anew;
     struct graph graph = { head: NULL, tail: NULL };

     int argc;
     char ** argv;

     int rename_argc = 1;
     char ** rename_argv = NULL;
     struct arglist * ap;

     if (!strcmp(module, "select")) {
     	/* SQL's select is really a projection operation */
     	module = "project";
     }
     anew = newarg(module, 0, NULL);
     arglist_append(anew, alist);

     for(ap=anew; ap; ap=ap->next) {
        /* Check for rename options on arguments */
     	if (ap->rename) {
		rename_argc += 2;
		rename_argv = realloc(rename_argv, rename_argc * sizeof(char*));
		rename_argv[rename_argc - 2] = ap->arg;
		rename_argv[rename_argc - 1] = ap->rename;
		ap->arg = ap->rename;
	}

	/* Check for function arguments */
	if (ap->isfunc) 
		graph_join(&graph, newmodule(ap->arg, ap->func_args));
     }

     if (rename_argc > 1) {
        /* We need to splice in a rename module before this module */
     	rename_argv[0] = "rename";
        graph_append(&graph, smacq_new_module(rename_argc, rename_argv));
     }

     arglist2argv(anew, &argc, &argv);
     graph_append(&graph, smacq_new_module(argc, argv));

     return graph;
}

static struct arglist * newarg(char * arg, int isfunc, struct arglist * func_args) {
     struct arglist * al = calloc(1, sizeof(struct arglist));
     al->arg = arg;
     if (isfunc) {
     	al->func_args = func_args;
	al->isfunc = 1;
     }

     return(al);
}

void print_graph(struct filter * f) {
	int i;
	if (!f) return;

	printf("Graph node %s (%p):\n", f->name, f);
	for (i=0; i<f->argc; i++) {
		printf("\tArgument %s\n", f->argv[i]);
	}
	for (i=0; i<f->numchildren; i++) {
		printf("\tChild %d is %s (%p)\n", i, f->next[i]->name, f->next[i]);
		print_graph(f->next[i]);
	}
}


