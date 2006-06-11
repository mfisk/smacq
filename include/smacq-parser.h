#ifndef SMACQ_PARSER_H
#define SMACQ_PARSER_H

#include <smacq.h>
#include <SmacqGraph.h>
#include <pthread.h>
#include <dts-filter.h>

BEGIN_C_DECLS
 
extern char * yytext;
extern char * yystring;
extern void yyerror(char * msg);

struct arglist {
    char * arg;
    char * rename;
    struct arglist * next;
    struct arglist * func_args;
    struct dts_operand * expr;
    int isfunc;
};

class DatalogNamespace : public std::map<char*,SmacqGraph *> {
  public:
    SmacqGraphContainer graphs;

    void add_graph(SmacqGraph * g) {
	graphs.add_graph(g);
    }
};

class joinlist {
 public:
        joinlist(char * n, SmacqGraphContainer * g, SmacqGraphContainer * u)
                : next(NULL), graph(g), name(n), until(u) 
		{}

        void append(struct joinlist * b) {
                joinlist * a = this;
                while(a->next) a=a->next;
                a->next = b;
        }

        joinlist * next;
        SmacqGraphContainer * graph;
        char * name;
	SmacqGraphContainer * until;	
};

struct reflist_element {
      char * refname;
      char * aliasname;
      SmacqGraph * until; 
};

struct TermList {
    dts_comparison * bools;
    joinlist * joins;
    std::vector<reflist_element> refs;
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
SmacqGraphContainer * newmodule(char * module, struct arglist * alist);
void graph_join(SmacqGraphContainer * & graph, SmacqGraphContainer * newg);
SmacqGraphContainer * newgroup(struct group, SmacqGraphContainer * vphrase);
void arglist2argv(struct arglist * al, int * argc, char *** argv);
char * arglist2str(struct arglist * al);
struct arglist * newarg(char * arg, enum argtype argtype, struct arglist * func_args);
struct arglist * arglist_append(struct arglist * tail, struct arglist * addition);
struct vphrase newvphrase(char * verb, struct arglist * args);
SmacqGraphContainer * optimize_bools(dts_comparison *);
dts_comparison * comp_join(dts_comparison *, dts_comparison *, dts_compare_operation);
struct dts_operand * comp_operand(enum dts_operand_type type, char * str);
dts_comparison * comp_new(dts_compare_operation op, struct dts_operand *, struct dts_operand *);
dts_comparison * comp_new_func(char *, int, char **, struct arglist *);
char * print_comparison(dts_comparison * comp);
char * print_operand(struct dts_operand * op);

extern DTS * parse_dts;

SmacqGraphContainer * joinlist2graph(joinlist * joinlist, SmacqGraphContainer * where);
END_C_DECLS

#endif
