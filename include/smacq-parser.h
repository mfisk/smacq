#ifndef SMACQ_PARSER_H
#define SMACQ_PARSER_H

#include <smacq.h>
#include <SmacqGraph.h>
#include <pthread.h>
#include <dts-filter.h>

extern char * yytext;
extern char * yystring;
/*
extern int yylex();
extern void yyerror(char*);
*/

BEGIN_C_DECLS
 
struct arglist {
    char * arg;
    char * rename;
    struct arglist * next;
    struct arglist * func_args;
    struct dts_operand * expr;
    int isfunc;
};

class joinlist {
 public:
        joinlist(char * n, SmacqGraph *g)
                : next(NULL), graph(g), name(n) 
		{}

        void append(struct joinlist * b) {
                joinlist * a = this;
                while(a->next) a=a->next;
                a->next = b;
        }

        joinlist * next;
        SmacqGraph * graph;
        char * name;
};

struct vphrase {
      char * verb;
      struct arglist * args;
};

struct group {
      struct arglist * args;
      dts_comparison * having;
};

enum argtype { WORD, FUNCTION };

char * expression2fieldname(struct dts_operand * expr);
SmacqGraph * newmodule(char * module, struct arglist * alist);
void graph_join(SmacqGraph ** graph, SmacqGraph * newg);
SmacqGraph * newgroup(struct group, SmacqGraph * vphrase);
void arglist2argv(struct arglist * al, int * argc, char *** argv);
char * arglist2str(struct arglist * al);
struct arglist * newarg(char * arg, enum argtype argtype, struct arglist * func_args);
struct arglist * arglist_append(struct arglist * tail, struct arglist * addition);
struct vphrase newvphrase(char * verb, struct arglist * args);
SmacqGraph * optimize_bools(dts_comparison *);
dts_comparison * comp_join(dts_comparison *, dts_comparison *, dts_compare_operation);
struct dts_operand * comp_operand(enum dts_operand_type type, char * str);
dts_comparison * comp_new(dts_compare_operation op, struct dts_operand *, struct dts_operand *);
dts_comparison * comp_new_func(char *, int, char **, struct arglist *);
char * print_comparison(dts_comparison * comp);
char * print_operand(struct dts_operand * op);

extern DTS * parse_dts;

SmacqGraph * newjoin(struct joinlist * joinlist, SmacqGraph * where);
void joinlist_append(struct joinlist * a, struct joinlist * b);
void joinlist_create(struct joinlist * a, char * name, SmacqGraph * g);

END_C_DECLS

#endif
