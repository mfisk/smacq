#ifndef SMACQ_PARSER_H
#define SMACQ_PARSER_H

#include <smacq.h>
#include <pthread.h>
  
  extern int yylex();
  extern char * yytext;
  extern char * yystring;
  extern void yyerror(char*);
  //extern void yy_scan_string(char*);

#ifdef PTHREAD_MUTEX_INITIALIZER
  pthread_mutex_t local_lock = PTHREAD_MUTEX_INITIALIZER;
#else
  pthread_mutex_t local_lock;
  #warning "No PTHREAD_MUTEX_INITIALIZER"
#endif

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

  extern struct graph newmodule(char * module, struct arglist * alist);
  extern void graph_join(struct graph * graph, struct graph newg);
  extern struct graph newgroup(struct group, struct vphrase);
  extern void arglist2argv(struct arglist * al, int * argc, char *** argv);
  extern char * arglist2str(struct arglist * al);
  extern struct arglist * newarg(char * arg, int isfunc, struct arglist * func_args);
  extern struct arglist * arglist_append(struct arglist * tail, struct arglist * addition);
  extern struct vphrase newvphrase(char * verb, struct arglist * args);
  extern struct graph optimize_bools(dts_comparison *);
  extern dts_comparison * comp_join(dts_comparison *, dts_comparison *, int isor);
  extern dts_comparison * comp_new(char * field, dts_compare_operation op, char **, int);

  extern dts_environment * parse_tenv;
#endif
