#ifndef LIBSMACQ_INTERNAL_H
#define LIBSMACQ_INTERNAL_H

EXTERN const dts_object* _smacq_alloc(int size, int type);
#include <gmodule.h>
#include <glib.h>

#define RING_EOF ((void*)-1)

struct smacq_module_ops {
  smacq_init_fn * init;
  smacq_shutdown_fn * shutdown;
  smacq_consume_fn * consume;
  smacq_produce_fn * produce;
  smacq_thread_fn * thread_fn;
};

struct _smacq_module {
  // Ring buffer mgmt
  dts_object ** q;
  int ringsize;
  int ring_produce;
  int ring_consume;
#ifndef SMACQ_OPT_NOPTHREADS
  pthread_mutex_t qlock;
  pthread_cond_t ring_notfull;
  pthread_cond_t ring_notempty;

  pthread_t thread;
#endif

  char * name;
  char ** argv;
  int argc;

  struct smacq_module_ops ops;

  GModule * module;
  void * state;
  int status;

  struct smacq_options * options;

  smacq_graph * previous;

  smacq_graph ** child;
  smacq_graph ** parent;
  int numchildren, numparents;

  struct smacq_optval * optvals;

  smacq_graph * next_graph;
};

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
EXTERN dts_environment * dts_init();
/*
EXTERN int type_requiretype(dts_environment *, char * name);
EXTERN int type_newtype(dts_environment *, char * name, struct dts_field_descriptor * d);
EXTERN int type_typenum_byname(dts_environment *, char*);
EXTERN char * type_typename_bynum(dts_environment *, int);
*/

void * smacq_find_module(GModule ** gmodulep, char * envvar, char * envdefault, char * modformat, char * symformat, char * sym);

/*
 * Interfaces to msg system
 */
const dts_object * msg_check(dts_environment * tenv, const dts_object * d, dts_field_element field, dts_object *);

/*
 * Scheduler intefaces 
 */
void sched_mono(smacq_graph *);
EXTERN int smacq_sched_iterative(smacq_graph * startf, const dts_object * din, const dts_object ** dout , void ** state, int produce_first);
EXTERN void smacq_sched_iterative_shutdown(smacq_graph * startf, void ** state);


void smacq_start_threads(smacq_graph *);

/*
 * Interace to buffer system 
 */
#include <buffer-inline.c>

#endif

