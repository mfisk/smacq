#ifndef SMACQ_PARSER_H
#define SMACQ_PARSER_H

#include <smacq.h>
#include <pthread.h>

BEGIN_C_DECLS
  
extern int yylex();
extern char * yytext;
extern char * yystring;
extern void yyerror(char*);

struct arglist {
    char * arg;
    char * rename;
    struct arglist * next;
    struct arglist * func_args;
    struct dts_operand * expr;
    int isfunc;
};

struct graph {
      smacq_graph * head;
      smacq_graph * tail;
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
struct graph newmodule(char * module, struct arglist * alist);
void graph_join(struct graph * graph, struct graph newg);
struct graph newgroup(struct group, struct graph vphrase);
void arglist2argv(struct arglist * al, int * argc, char *** argv);
char * arglist2str(struct arglist * al);
struct arglist * newarg(char * arg, enum argtype argtype, struct arglist * func_args);
struct arglist * arglist_append(struct arglist * tail, struct arglist * addition);
struct vphrase newvphrase(char * verb, struct arglist * args);
struct graph optimize_bools(dts_comparison *);
dts_comparison * comp_join(dts_comparison *, dts_comparison *, dts_compare_operation);
struct dts_operand * comp_operand(enum dts_operand_type type, char * str);
struct dts_operand * comp_arith(dts_environment *, enum dts_arith_operand_type op, struct dts_operand * op1, struct dts_operand * op2);
dts_comparison * comp_new(dts_compare_operation op, struct dts_operand *, struct dts_operand *);
dts_comparison * comp_new_func(char *, int, char **, struct arglist *);
char * print_comparison(dts_comparison * comp);
char * print_operand(struct dts_operand * op);

extern dts_environment * parse_tenv;

END_C_DECLS

#endif
