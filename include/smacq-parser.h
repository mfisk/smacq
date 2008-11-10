#ifndef SMACQ_PARSER_H
#define SMACQ_PARSER_H

#include <smacq.h>
#include <SmacqGraph.h>
#include <pthread.h>
#include <dts-filter.h>
#include <map>
#include <string>

extern char * yytext;
BEGIN_C_DECLS

extern char * yystring;
extern SmacqGraph * Graph;
extern std::map<std::string, SmacqGraph *> Graphs;

struct arglist {
    const char * arg;
    const char * rename;
    struct arglist * next;
    struct arglist * func_args;
    struct dts_operand * expr;
    int isfunc;
};

class DatalogNamespace : public std::map<char*,SmacqGraphNode *> {
  public:
    SmacqGraph graphs;

    void add_graph(SmacqGraphNode * g) {
	graphs.add_graph(g);
    }
};

class joinlist {
 public:
        joinlist(const char * n, SmacqGraph * g, SmacqGraph * u)
                : next(NULL), graph(g), name(n), until(u) 
		{}

        void append(struct joinlist * b) {
                joinlist * a = this;
                while(a->next) a=a->next;
                a->next = b;
        }

        joinlist * next;
        SmacqGraph * graph;
        const char * name;
	SmacqGraph * until;	
};

struct reflist_element {
      char * refname;
      char * aliasname;
      SmacqGraphNode * until; 
};

struct TermList {
    dts_comparison * bools;
    joinlist * joins;
    std::vector<reflist_element> refs;
};

struct vphrase {
      const char * verb;
      struct arglist * args;
};

struct group {
      struct arglist * args;
      dts_comparison * having;
};

enum argtype { WORD, FUNCTION };

char * expression2fieldname(struct dts_operand * expr);
SmacqGraph * newmodule(const char * module, struct arglist * alist);
void graph_join(SmacqGraph * & graph, SmacqGraph * newg);
SmacqGraph * newgroup(struct group, SmacqGraph * vphrase);
void arglist2argv(struct arglist * al, int * argc, const char *** argv);
char * arglist2str(struct arglist * al);
struct arglist * newarg(const char * arg, enum argtype argtype, struct arglist * func_args);
struct arglist * arglist_append(struct arglist * tail, struct arglist * addition);
struct vphrase newvphrase(const char * verb, struct arglist * args);
SmacqGraph * optimize_bools(dts_comparison *);
dts_comparison * comp_join(dts_comparison *, dts_comparison *, dts_compare_operation);
struct dts_operand * comp_operand(enum dts_operand_type type, const char * str);
dts_comparison * comp_new(dts_compare_operation op, struct dts_operand *, struct dts_operand *);
dts_comparison * comp_new_func(const char *, int, const char **, struct arglist *);
char * print_comparison(dts_comparison * comp);
char * print_operand(struct dts_operand * op);

extern DTS * parse_dts;

SmacqGraph * joinlist2graph(joinlist * joinlist, SmacqGraph * where);
END_C_DECLS

#endif
