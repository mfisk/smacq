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
  static struct graph newgroup(struct group, struct vphrase);
  static void arglist2argv(struct arglist * al, int * argc, char *** argv);
  static char * arglist2str(struct arglist * al);
  static struct arglist * newarg(char * arg, int isfunc, struct arglist * func_args);
  static struct arglist * arglist_append(struct arglist * tail, struct arglist * addition);
  static struct vphrase newvphrase(char * verb, struct arglist * args);
  static struct graph optimize_bools(dts_comparison *);
  static dts_comparison * comp_join(dts_comparison *, dts_comparison *, int isor);
  static dts_comparison * comp_new(char * field, dts_compare_operation op, char * value);

  struct graph nullgraph = { head: NULL, tail: NULL };
  
  smacq_graph * Graph;

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
%token HAVING

%token YYSTOP YYLIKE YYOR YYAND
%left YYAND YYOR

%type <arglist> joins having arg argument boolarg boolargs args moreargs moreboolargs
%type <group> group 
%type <graph> where query from source pverbphrase  
%type <vphrase> verbphrase
%type <string> function verb word string id as
%type <op> op
%type <comp> boolean test

%start queryline

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

queryline: query STOP	
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

string:	STRING 		{ $$ = yystring; };

id:	ID 		{ $$ = yystring; }
                  | YYOR                         { $$ = "or"; }
                  | YYAND                       { $$ = "and"; }
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
	| '>'				{ $$ = newarg(">", 0, NULL); }
	| '='				{ $$ = newarg("=", 0, NULL); }
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

test : word			{ $$ = comp_new($1, EXIST, NULL); }
	| word op word		{ $$ = comp_new($1, $2, $3); }
	| verb '(' args ')'		{ $$ = comp_new($1, FUNC, arglist2str($3)); }
	;

op : '=' 		{ $$ = EQUALITY; }
	| '>'		{ $$ = GT; }
	| '<' 		{ $$ = LT; }
	| YYLIKE 	{ $$ = LIKE; }		 
	;

%%


static dts_environment * Tenv;

smacq_graph * smacq_build_query(dts_environment * tenv, int argc, char ** argv) {
  int size = 0;
  int i;
  char * qstr; 
  smacq_graph * graph;
  int res;

  Tenv = tenv;

  for (i=0; i<argc; i++) {
  	size += strlen(argv[i]);
  }
  size += argc;

  qstr = (char*)malloc(size);
  	
  for (i=0; i<argc; i++) {
  	strcatn(qstr, size, argv[i]);
  	strcatn(qstr, size, " ");
  }

  /* LOCK */

  yy_scan_string(qstr);
  /* fprintf(stderr, "parsing buffer: %s\n", qstr); */

  res = yyparse();

  graph = Graph;

  /* UNLOCK */

  if (res) {
    perror("parse error");
    return NULL;
  }

  if (!graph) {
    fprintf(stderr, "parse error\n");
  }

  return graph;
}

void yyerror(char * msg) {
  fprintf(stderr, "Error: %s near %s\n", msg, yytext);
  exit(-1);
}

static struct vphrase newvphrase(char * verb, struct arglist * args) {
  	struct vphrase vphrase;
	vphrase.verb = verb;
	vphrase.args = args;
	if (!strcmp(verb, "select")) {
     		/* SQL's select is really a projection operation */
     		vphrase.verb = "project";
	}
	return vphrase;
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

static char * arglist2str(struct arglist * alist) {
	char * argstr;
	struct arglist * al;
	int len = 1;

	for(al=alist; al; al=al->next) 
		len += strlen(al->arg) + 1;

	argstr = malloc(len);

	for(al=alist; al; al=al->next) {
		strcatn(argstr, len, al->arg);
		strcatn(argstr, len, " ");
	}

	return argstr;
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

	/* fprintf(stderr, "Adding %s after %s\n", newg.head->name, graph->tail->name); */

	/* Splice them together */
	assert(graph->tail);

	smacq_add_child(graph->tail, newg.head); 
	graph->tail = newg.tail;
}
	
static void graph_append(struct graph * graph, smacq_graph * newmod) {
	if (graph->tail) 
		smacq_add_child(graph->tail, newmod); 
	graph->tail = newmod;
	if (! graph->head) 
		graph->head = newmod;
}

static struct arglist * arglist_append(struct arglist * tail, struct arglist * addition) {
	for (; tail->next; tail=tail->next) ;
	tail->next = addition;
	for (; tail->next; tail=tail->next) ;

	return tail;
}
	
static struct graph newgroup(struct group group, struct vphrase vphrase) {
  /*
   * This function violates some abstractions by knowing the 
   * calling syntax for "groupby" and constructing arguments for it.
   */
  struct arglist * atail;
  struct graph g = { NULL, NULL};
  struct arglist * ap;
  int argcont = 0;
  
  if (!group.args) { 
    /* Do nothing if this was "group by" NULL */
    return newmodule(vphrase.verb, vphrase.args);
  }
  
  atail = arglist_append(group.args, newarg("--", 0, NULL));
  
  /* Insert function operations */
  for(ap=vphrase.args; ap; ap=ap->next) {
    /* fprintf(stderr, "group arg %s isfunc = %d\n", ap->arg, ap->isfunc); */
    if (ap->isfunc) {
      if (argcont) 
	atail = arglist_append(atail, newarg("|", 0, NULL));
      atail = arglist_append(atail, newarg(ap->arg, 0, NULL));
      atail = arglist_append(atail, ap->func_args);
      ap->isfunc = 0;
      argcont = 1;
    }
  }
  if (argcont) 
    g = newmodule("groupby", group.args);
  
  if (group.having) 
    graph_join(&g, newmodule("filter", group.having));
  
  graph_join(&g, newmodule(vphrase.verb, vphrase.args));
  
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

static char * opstr(dts_comparison * comp) {
  switch (comp->op) {
  case LT:
    return "<";

  case GT:
    return ">";

  case EQUALITY:
    return "==";

  case INEQUALITY:
    return "!=";

  case LIKE:
    return "like";

  case EXIST:
    return "";

  case FUNC:
    return "[FUNC]";

 case AND:
    return "[AND]";

 case OR:
    return "[OR]";
  }

  return "[ERR]";
}


static char * print_comparison(dts_comparison * comp) {
  int size = 20;
  char * buf;
  dts_comparison * c;
  char * b;

  if (comp->fieldname) 
    size += strlen(comp->fieldname);

  if (comp->valstr)
    size += strlen(comp->valstr);

  buf = malloc(size);

  if (comp->op == EXIST) {
    snprintf(buf, size, "(%s)", comp->fieldname);
  } else if (comp->op == FUNC) {
    snprintf(buf, size, "%s(%s)", comp->fieldname, comp->valstr);
  } else if (comp->op == OR) {
    buf[0] = '\0';

    for (c = comp->group; c; c=c->next) {
      assert(c);
      b = print_comparison(c);
      size += strlen(b) + 5;
      buf = realloc(buf, size);
      strcatn(buf, size, b);
      free(b);

      if (c->next) {
	strcatn(buf, size, " or ");
      }
    }
  } else {
    snprintf(buf, size, "(%s %s \"%s\")", comp->fieldname, opstr(comp), comp->valstr);
  }

  return(buf);
}

static struct graph optimize_bools(dts_comparison * comp) {
  dts_comparison *c;
  struct arglist * arglist;
  struct graph g;

  g.head = NULL; g.tail = NULL;
 
  for(c=comp; c; c = c->next) {
    if (c->op == AND) {
      assert (! "untested");
      graph_join(&g, optimize_bools(c->group));
    } else if (c->op == FUNC) {
      arglist = newarg(c->valstr, 0, NULL);
      graph_join(&g, newmodule(c->fieldname, arglist));
    } else if (c->op == OR) {
      arglist = newarg(print_comparison(c), 0, NULL);
      graph_join(&g, newmodule("filter", arglist)); 
    } else {
      arglist = newarg(print_comparison(c), 0, NULL);
      graph_join(&g, newmodule("filter", arglist));
    }
  }

  return g;
}

static dts_comparison * comp_join(dts_comparison * lh, dts_comparison * rh, int isor) {
  dts_comparison * ret = NULL;

  if (!lh) return rh;
  if (!rh) return lh;

  if (!isor) {
   	ret = lh;
	for (; lh->next; lh=lh->next) ;

	lh->next = rh;
  } else if (isor) {
        dts_comparison * left, * right;
        if (lh->next) {
		left = calloc(1,sizeof(dts_comparison));
		left->op = AND;
		left->group = lh;
	} else {
		left = lh;
	}
	if (rh->next) {
		right = calloc(1,sizeof(dts_comparison));
		right->op = AND;
		right->group = rh;
	} else {
		right = rh;
	}

        ret = calloc(1,sizeof(dts_comparison));
	ret->op = OR;
	ret->group = left;
	left->next = right;
  } else {
  	assert(!"Shouldn't get here!");
  }
  	
  return ret;
}


static dts_comparison * comp_new(char * field, dts_compare_operation op, char * value) {
     dts_comparison * comp = calloc(1,sizeof(dts_comparison));

     comp->op = op;
     comp->valstr = value;
     comp->fieldname = field;

     if (comp->op != FUNC) {
       comp->field = Tenv->requirefield(Tenv, field);
     }

     return comp;
}

