#ifndef LIBSMACQ_INTERNAL_H
#define LIBSMACQ_INTERNAL_H
#include <smacq.h>
#include <smacq_args.h>

EXTERN const dts_object* _smacq_alloc(int size, int type);
#include <gmodule.h>
#include <glib.h>

#define RING_EOF ((void*)-1)

struct filter {
  // Ring buffer mgmt
  dts_object ** q;
  int ringsize;
  int ring_produce;
  int ring_consume;
  pthread_mutex_t qlock;
  pthread_cond_t ring_notfull;
  pthread_cond_t ring_notempty;

  pthread_t thread;

  char * name;
  char ** argv;
  int argc;

  GModule * module;
  void * state;
  int status;

  flow_init_fn * init;
  flow_shutdown_fn * shutdown;
  flow_consume_fn * consume;
  flow_produce_fn * produce;
  smacq_thread_fn * thread_fn;
  struct smacq_options * options;

  struct filter * previous;

  struct filter ** next;
  struct filter ** parent;
  int numchildren, numparents;

  struct smacq_optval * optvals;
};


/* 
 * Method entry points 
 */
/*
EXTERN int type_presentdata_virtual(int type, char * transform, void * data, int len, void ** tdata, int * tlen);
int type_getfield_virtual(const dts_object * datum, char * name, int * dtype, void **data, int * len);
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
const dts_object * msg_check(dts_environment * tenv, const dts_object * d, int field, dts_object *);

/*
 * Scheduler intefaces 
 */
void sched_mono(struct filter *);
EXTERN int flow_sched_iterative(struct filter * startf, const dts_object * din, const dts_object ** dout , void ** state, int produce_first);
EXTERN void flow_sched_iterative_shutdown(struct filter * startf, void ** state);


void flow_start_threads(struct filter *);

/*
 * Interace to buffer system 
 */
#include <buffer-inline.c>

#endif

