%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <smacq-internal.h>
  
  extern int yylex();
  extern void yy_scan_string(const char *);
  void yyerror(char *);
  extern char * yytext;
  extern char * yystring;

  struct vphrase {
    int argc;
    char ** argv;
    struct filter * mod;
  };
 
  struct arglist {
    char * arg;
    struct arglist * next;
  };
 
  static struct vphrase * newmodule(char * module, struct arglist * alist);
  static void arglist2argv(struct arglist * al, int * argc, char *** argv);
  static struct arglist * newarg(char * arg);
  
%}

%token WHERE
%token GROUP
%token BY
%token FROM
%token SELECT
%token STRING
%token ID
%token STOP

%type <arglist> arg args arglist moreargs
%type <graph> query from source
%type <vphrase> pverbphrase verbphrase where group
%type <string> function verb word string id 

%start queryline

%union {
  struct {
  	struct filter * head;
	struct filter * tail;
  } graph;
  struct vphrase * vphrase;
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
			smacq_add_child($2.tail, $1->mod);
			$$.head = $2.head;
	   		fprintf(stderr, "from head was %p, %s\n", 
				$2.head, $2.head ? $2.head->name : ""); 
		} else {
			$$.head = $1->mod;
		}
		$$.tail = $1->mod;
		assert($$.head);
		assert($$.tail);
	   	fprintf(stderr, "Got a full query! (%s thru %s)\n", 
			$$.head->name, $$.tail->name); 
	   }
	;

where : null 		{ $$ = NULL; }
	| WHERE args 	{ $$ = newmodule("filter", $2); }
	;

group : null 		{ $$ = NULL; }
	| GROUP BY args { $$ = newmodule("groupby", $3); }
	;

word:	id 		
	| string  
	;

string:	STRING 		{ $$ = yystring; };

id:	ID 		{ $$ = yystring; };

arg : 	word 				{ $$ = newarg($1); } 
	| function '(' arglist ')' 	{ $$ = newarg($1); }
	;

function : id 
	;

from :  null 		{ $$.head = NULL; $$.tail = NULL; } 
	| FROM source where group 
           {
	   	if ($3) {
			if ($4) {
				smacq_add_child($2.tail, $3->mod);
				smacq_add_child($3->mod, $4->mod);
				$$.tail = $4->mod;
			} else {
				smacq_add_child($2.tail, $3->mod);
				$$.tail = $3->mod;
			}
		} else if ($4) {
			smacq_add_child($2.tail, $4->mod);
			$$.tail = $4->mod;
		} else {
			$$.tail = $2.tail;
		}
		$$.head = $2.head;
	   }
	;

source : pverbphrase		{ $$.tail = ($$.head = $1->mod);} 
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

static struct vphrase * newmodule(char * module, struct arglist * alist) {
     struct arglist anew;
     struct vphrase * vp = malloc(sizeof(struct vphrase));

     anew.arg = module;
     if (!strcmp(module, "select")) {
     	/* SQL's select is really a projection operation */
     	anew.arg = "project";
     }
     anew.next = alist;

     arglist2argv(&anew, &(vp->argc), &(vp->argv));

     /* fprintf(stderr,"new module: %s,%s\n", module, vp->argv[0]); */
     vp->mod = smacq_new_module(vp->argc, vp->argv);

     return vp;
}

static struct arglist * newarg(char * arg) {
     struct arglist * al = malloc(sizeof(struct arglist));
     al->arg = arg;
     al->next = NULL;
     return(al);
}


