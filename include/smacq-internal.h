#ifndef LIBSMACQ_INTERNAL_H
#define LIBSMACQ_INTERNAL_H

BEGIN_C_DECLS
DtsObject _smacq_alloc(int size, int type);
END_C_DECLS

#include <gmodule.h>
#include <glib.h>

#define RING_EOF ((void*)-1)

struct smacq_module_ops {
  smacq_constructor_fn * constructor;
};

BEGIN_C_DECLS

void * smacq_find_module(GModule ** gmodulep, char * envvar, char * envdefault, char * modformat, char * symformat, char * sym);


/*
 * Scheduler intefaces 
 */
struct runq;
void sched_mono(smacq_graph *);
smacq_result smacq_sched_iterative(smacq_graph * startf, DtsObject din, DtsObject * dout , struct runq **, int produce_first);
void smacq_sched_iterative_init(smacq_graph * startf, struct runq ** runqp, int produce_first);
smacq_result smacq_sched_iterative_busy(smacq_graph * startf, DtsObject * dout, struct runq * runq, int produce_first);
void smacq_sched_iterative_input(smacq_graph * startf, DtsObject din, struct runq * runq);

void smacq_sched_iterative_shutdown(smacq_graph * startf, struct runq * runq);

void smacq_start_threads(smacq_graph *);

END_C_DECLS

/*
 * Interace to buffer system 
 */
#include <buffer-inline.c>

#endif

