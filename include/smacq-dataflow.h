#ifndef LIBSMACQ_DATAFLOW_H
#define LIBSMACQ_DATAFLOW_H

#include <SmacqModule.h>

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
  struct smacq_module_algebra algebra;

  SmacqModule * instance;

  GModule * module;
  int status;

  struct smacq_options * options;

  smacq_graph * previous;

  smacq_graph ** child;
  smacq_graph ** parent;
  int numchildren, numparents;

  struct smacq_optval * optvals;

  smacq_graph * next_graph;
};

#endif

