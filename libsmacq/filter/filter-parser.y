/*
 * BUGS:
 *
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <smacq.h>
#include <smacq-internal.h>
#include "filter.h"
#include "filter-parser.h"
  
  extern int yylex();
  extern void yy_scan_string(const char *);
  void yyerror(char *);
  extern char * yytext;
  extern char * yystring;
  void yyfilter_scan_string(char *);

  static struct list list_join(struct list list, struct list newl, int isor);
  static struct list newlist(char * field, dts_compare_operation op, char * value);

  static dts_comparison * Comp;

%}

%token YYSTOP YYLIKE YYOR YYAND YYSTRING YYID

%left YYAND YYOR

%type <op> op
%type <list> boolean booleanline test
%type <string> word string id

%union {
  char * string;
  struct list list;
  dts_compare_operation op;
}
%%

booleanline: boolean YYSTOP 	{ 
					if ($1.isor) {
						Comp = calloc(1,sizeof(dts_comparison));

						fprintf(stderr, "parsed a top-level OR to %p\n", Comp);

						Comp->op = OR;
						Comp->group = $1.head;
						Comp->next = NULL;
					} else {
						Comp = $1.head; 
					}
					return 0;
				}
	;

boolean : '(' boolean ')'	{ $$ = $2; }
	| boolean YYOR boolean	{ $$ = list_join($1, $3, 1); }
	| boolean YYAND boolean	{ $$ = list_join($1, $3, 0); }
	| test 		
	;

test : word			{ $$ = newlist($1, EXIST, NULL); }
	| word op word		{ $$ = newlist($1, $2, $3); }
	;

op : '=' 		{ $$ = EQUALITY; }
	| '>'		{ $$ = GT; }
	| '<' 		{ $$ = LT; }
	| YYLIKE 	{ $$ = LIKE; }		 
	;

word:	id 		
	| string  
	;

string:	YYSTRING 		{ $$ = yystring; };

id:	YYID 		{ $$ = yystring; };


%%

static dts_environment * tenv;

static void print_comp(dts_environment * tenv, dts_comparison * c) {
	char * op;

	if (!c) {
		// fprintf(stderr,". (%p)\n", c);
		return;
	}

	switch(c->op) {
		case AND: op = "and"; break;
		case OR: op = "or"; break;
		case GT: op = "<"; break;
		case LT: op = ">"; break;
		case EQUALITY: op = "="; break;
		case INEQUALITY: op = "!="; break;
		case EXIST: op = "exist"; break;
		case LIKE: op = "like"; break;
	}

	//fprintf(stderr, "Comparison %p: field %d op %s %s, next %p, group %p\n", c, c->field, op, c->valstr, c->next, c->group);
	print_comp(tenv, c->group);
	print_comp(tenv, c->next);

}

dts_comparison * dts_parse_tests(dts_environment * localtenv, int argc, char ** argv) {
  dts_comparison * retval;
  int size = 0;
  int i;
  char * qstr;

  /* LOCK */
  tenv = localtenv;

  for (i=0; i<argc; i++) {
  	size += strlen(argv[i]);
  }
  size += argc;

  qstr = (char*)malloc(size);
  	
  for (i=0; i<argc; i++) {
  	strcatn(qstr, size, argv[i]);
  	strcatn(qstr, size, " ");
  }
  yyfilter_scan_string(qstr);
  //fprintf(stderr, "parsing filter buffer: %s\n", qstr); 

  if (yyfilterparse()) {
  	/* Should free the comparisons? */
        fprintf(stderr, "got nonzero return\n"); 
  	return NULL;
  }

  retval = Comp;

  /* UNLOCK */
  print_comp(localtenv, retval);

  return retval;
}

static struct list list_join(struct list list, struct list newl, int isor) {
	struct list retval;

	assert(newl.head);
	assert(list.head);

	if ((list.head == list.tail || list.isor == isor) && (newl.head == newl.tail || newl.isor == isor)) {
		/* Splice them together */
		assert(list.tail);
		list.tail->next = newl.head;

		retval.head = list.head;
		retval.tail = newl.head;
		retval.isor = isor;

		return retval;
	} else if (list.head == list.tail || list.isor == isor) {
		/* The right list (newl) is a different type, so make it a sublist */
		dts_comparison * comp = calloc(1,sizeof(dts_comparison));

		comp->op = newl.isor ? OR : AND;
		comp->group = newl.head;
		comp->next = NULL;

		list.tail = (list.tail->next = comp);
		list.isor = isor;

		return list;
	} else if (newl.head == newl.tail || newl.isor == isor) {
		/* The left list (list) is a different type, so make it a sublist */
		dts_comparison * comp = calloc(1,sizeof(dts_comparison));

		comp->op = list.isor ? OR : AND;
		comp->group = list.head;
		comp->next = newl.head;

		list.head = comp;
		list.tail = newl.tail;
		list.isor = isor;

		return list;
	} else {
		/* Both sublists are a different type, so make a new meta-list */
		dts_comparison * comp = calloc(1,sizeof(dts_comparison));
		dts_comparison * comp2 = calloc(1,sizeof(dts_comparison));

		comp->op = list.isor ? OR : AND;
		comp->group = list.head;
		comp->next = comp2;

		comp2->op = newl.isor ? OR : AND;
		comp2->group = newl.head;
		comp2->next = NULL;

		list.head = comp;
		list.tail = comp2;
		list.isor = isor;

		return list;
	}
}

static struct list newlist(char * field, dts_compare_operation op, char * value) {
     dts_comparison * comp = calloc(1,sizeof(dts_comparison));
     struct list list;

     comp->op = op;
     comp->valstr = value;
     comp->field = tenv->requirefield(tenv, field);

     list.head = comp;
     list.tail = comp;

     return list;
}

void yyerror(char * msg) {
  fprintf(stderr, "Error: %s near %s\n", msg, yytext);
  exit(-1);
}


