#ifndef LIBSMACQ_INTERNAL_H
#define LIBSMACQ_INTERNAL_H

BEGIN_C_DECLS
const dts_object* _smacq_alloc(int size, int type);
END_C_DECLS

#include <gmodule.h>
#include <glib.h>

#define RING_EOF ((void*)-1)

struct smacq_module_ops {
  smacq_init_fn * init;
  smacq_shutdown_fn * shutdown;
  smacq_consume_fn * consume;
  smacq_produce_fn * produce;
  smacq_constructor_fn * constructor;
  smacq_thread_fn * thread_fn;
};

BEGIN_C_DECLS
/* 
 * Method entry points 
 */
/*
dts_object * type_getfield_virtual(const dts_object * datum, char * name, int * dtype, void **data, int * len);
int type_fromstring_virtual(int type, char * datastr, void ** tdata, int * tlen);
int type_lt_virtual(int type, void * p1, int len1, void * p2, int len2);
*/

/* 
 *Interfaces to type system 
 */
dts_environment * dts_init();
/*
int dts_requiretype(dts_environment *, char * name);
int type_newtype(dts_environment *, char * name, struct dts_field_spec * d);
int type_typenum_byname(dts_environment *, char*);
char * type_typename_bynum(dts_environment *, int);
*/

void * smacq_find_module(GModule ** gmodulep, char * envvar, char * envdefault, char * modformat, char * symformat, char * sym);

/*
 * Interfaces to msg system
 */
const dts_object * msg_check(dts_environment * tenv, const dts_object * d, dts_field_element field);

/*
 * Scheduler intefaces 
 */
struct runq;
void sched_mono(smacq_graph *);
smacq_result smacq_sched_iterative(smacq_graph * startf, const dts_object * din, const dts_object ** dout , struct runq **, int produce_first);
void smacq_sched_iterative_init(smacq_graph * startf, struct runq ** runqp, int produce_first);
smacq_result smacq_sched_iterative_busy(smacq_graph * startf, const dts_object ** dout, struct runq * runq, int produce_first);
void smacq_sched_iterative_input(smacq_graph * startf, const dts_object * din, struct runq * runq);

void smacq_sched_iterative_shutdown(smacq_graph * startf, struct runq * runq);

void smacq_start_threads(smacq_graph *);

END_C_DECLS

/*
 * Interace to buffer system 
 */
#include <buffer-inline.c>

#endif

