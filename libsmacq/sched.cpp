#include <smacq.h>
#include <smacq-dataflow.h>
#include <stdio.h>
#include <dts.h>

static int smacq_start_single(smacq_graph * objs, enum smacq_scheduler scheduler, DTS * tenv) {
  DtsObject record;
  struct runq * runq = NULL;

  if (!tenv) {
    tenv = new DTS;
  }

  smacq_init_modules(objs, tenv);

  if (! objs) {
    fprintf(stderr, "Nothing to do!\n");
    exit(-1);
  }

  switch (scheduler) {

    case RECURSIVE:
	assert(!"Recursive scheduler disabled");
    	/* sched_mono(objs); */
    	break;

    case ITERATIVE:
    	/* Caller must call sched_iterative() manually */
    	break;

    case LOOP:
    	while (smacq_sched_iterative(objs, NULL, &record, &runq, 1)) ;
    	break;

    case THREADED:
#ifndef SMACQ_OPT_NOPTHREADS
    	smacq_start_threads(objs);
	smacq_pthread_exit(0);
#else
	fprintf(stderr, "Threads not enabled\n");
	return -1;
#endif
    	break;
  }

  return 0;
}

int smacq_start(smacq_graph * g, enum smacq_scheduler scheduler, DTS * tenv) {
  while (g) {
	if (g->next_graph && (scheduler != ITERATIVE) && (scheduler != THREADED)) {
		fprintf(stderr, "Cannot start multiple graphs with this scheduler\n");
		return -1;
	}
  	if (0 != smacq_start_single(g, scheduler, tenv))
		return -1;

	g = g->next_graph;
  }

  return 0;
}


