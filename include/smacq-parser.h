#ifndef SMACQ_PARSER_H
#define SMACQ_PARSER_H

#include <smacq.h>
#include <pthread.h>
  
  EXTERN int yylex();
  EXTERN char * yytext;
  EXTERN char * yystring;
  EXTERN void yyerror(char*);

struct arglist {
    char * arg;
    char * rename;
    struct arglist * next;
    struct arglist * func_args;
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
      struct arglist * having;
};

  EXTERN struct graph newmodule(char * module, struct arglist * alist);
  EXTERN void graph_join(struct graph * graph, struct graph newg);
  EXTERN struct graph newgroup(struct group, struct vphrase);
  EXTERN void arglist2argv(struct arglist * al, int * argc, char *** argv);
  EXTERN char * arglist2str(struct arglist * al);
  EXTERN struct arglist * newarg(char * arg, int isfunc, struct arglist * func_args);
  EXTERN struct arglist * arglist_append(struct arglist * tail, struct arglist * addition);
  EXTERN struct vphrase newvphrase(char * verb, struct arglist * args);
  EXTERN struct graph optimize_bools(dts_comparison *);
  EXTERN dts_comparison * comp_join(dts_comparison *, dts_comparison *, int isor);
  EXTERN struct dts_operand * comp_operand(enum dts_operand_type type, char * str);
  EXTERN struct dts_operand * comp_arith(dts_environment *, enum dts_arith_operand_type op, struct dts_operand * op1, struct dts_operand * op2);
  EXTERN dts_comparison * comp_new(dts_compare_operation op, struct dts_operand *, struct dts_operand *);
  EXTERN dts_comparison * comp_new_func(char *, int, char **, struct arglist *);
  EXTERN char * print_comparison(dts_comparison * comp);

  EXTERN dts_environment * parse_tenv;
#endif
