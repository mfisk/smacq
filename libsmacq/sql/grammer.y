%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <smacq-internal.h>
#include "smacq-parser.h"
  
  extern int yylex();
  extern void yy_scan_string(const char *);
  void yyerror(char *);
  extern char * yytext;
  extern char * yystring;

  struct arglist {
    char * arg;
    char * rename;
    struct arglist * next;
  };

  static struct graph newmodule(char * module, struct arglist * alist);
  static void arglist2argv(struct arglist * al, int * argc, char *** argv);
  static struct arglist * newarg(char * arg);

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

%type <arglist> arg argument args arglist moreargs
%type <graph> where group query from source pverbphrase verbphrase 
%type <string> function verb word string id 

%start queryline

%union {
  struct graph graph;
  struct arglist * arglist;
  char * string;
}
%%


queryline: query STOP	{ return smacq_start($1.head, RECURSIVE, NULL); }
	;

null:   /* empty */ ;

query : verbphrase from 
           {
	   	if ($2.head) {
			smacq_add_child($2.tail, $1.head);
			$$.head = $2.head;
	   		fprintf(stderr, "from head was %p, %s\n", 
				$2.head, $2.head ? $2.head->name : ""); 
		} else {
			$$.head = $1.head;
		}
		$$.tail = $1.tail;
		assert($$.head);
		assert($$.tail);
	   	fprintf(stderr, "Got a full query! (%s thru %s)\n", 
			$$.head->name, $$.tail->name); 
	   }
	;


where : null 		{ $$ = nullgraph; }
	| WHERE args 	{ $$ = newmodule("filter", $2); }
	;

group : null 		{ $$ = nullgraph; }
	| GROUP BY args { $$ = newmodule("groupby", $3); }
	;

word:	id 		
	| string  
	;

string:	STRING 		{ $$ = yystring; };

id:	ID 		{ $$ = yystring; };

arg: argument 
	| argument AS word		{ $$->rename = $3; }
	;

argument : word 			{ $$ = newarg($1); } 
	| function '(' arglist ')' 	{ $$ = newarg($1); }
	;

function : id 
	;

from :  null 		{ $$.head = NULL; $$.tail = NULL; } 
	| FROM source where group 
           {
	   	if ($3.head) {
			if ($4.head) {
				smacq_add_child($2.tail, $3.head);
				smacq_add_child($3.tail, $4.head);
				$$.tail = $4.tail;
			} else {
				smacq_add_child($2.tail, $3.head);
				$$.tail = $3.tail;
			}
		} else if ($4.head) {
			smacq_add_child($2.tail, $4.head);
			$$.tail = $4.tail;
		} else {
			$$.tail = $2.tail;
		}
		$$.head = $2.head;
	   }
	;

source : pverbphrase		
	| '(' query ')' 	{ $$ = $2; }
	;

pverbphrase: verb 		{ $$ = newmodule($1, NULL); }
	| verb '(' arglist ')' 	{ $$ = newmodule($1, $3); }
	;

verbphrase : verb args 		{ $$ = newmodule($1, $2); }
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

static struct graph newmodule(char * module, struct arglist * alist) {
     struct arglist anew;
     struct graph graph;

     int argc;
     char ** argv;

     int rename_argc = 1;
     char ** rename_argv = NULL;
     struct arglist * ap;

     anew.arg = module;
     anew.rename = NULL;
     if (!strcmp(module, "select")) {
     	/* SQL's select is really a projection operation */
     	anew.arg = "project";
     }
     anew.next = alist;

     /* Check for rename options on arguments */
     for(ap=&anew; ap; ap=ap->next) {
     	if (ap->rename) {
		rename_argc += 2;
		rename_argv = realloc(rename_argv, rename_argc * sizeof(char*));
		rename_argv[rename_argc - 2] = ap->arg;
		rename_argv[rename_argc - 1] = ap->rename;
		ap->arg = ap->rename;
	}
     }

     arglist2argv(&anew, &argc, &argv);

     /* fprintf(stderr,"new module: %s,%s\n", module, argv[0]); */
     graph.tail = smacq_new_module(argc, argv);

     if (rename_argc > 1) {
        /* We need to splice in a rename module before this module */
     	int i;
     	rename_argv[0] = "rename";
	for(i=0; i<(rename_argc); i++) {
		printf("rename arg: %s\n", rename_argv[i]);
	}
	graph.head = smacq_new_module(rename_argc, rename_argv);

	smacq_add_child(graph.head, graph.tail);
     } else {
     	graph.head = graph.tail;
     }

     return graph;
}

static struct arglist * newarg(char * arg) {
     struct arglist * al = malloc(sizeof(struct arglist));
     al->arg = arg;
     al->next = NULL;
     al->rename = NULL;
     return(al);
}


