#include <flow-internal.h>
#include <stdio.h>

int flow_start(struct filter * objs, enum flow_scheduler scheduler, dts_environment * tenv) {
  smacq_environment * env = g_new0(smacq_environment, 1);

  if (!tenv) {
    tenv = dts_init();
  }

  env->types = tenv;
  env->alloc = _flow_alloc;

  flow_init_modules(objs, env);

  if (! objs) {
    fprintf(stderr, "Nothing to do!\n");
    exit(-1);
  }

  if (scheduler == RECURSIVE) {
    sched_mono(objs);
  } else if (scheduler == ITERATIVE) {
    /* Caller must call sched_iterative() manually */
  } else if (scheduler == LOOP) {
    const dts_object * record;
    void * runq = NULL;
    while (flow_sched_iterative(objs, NULL, &record, &runq, 1)) ;
  } else {
    flow_start_threads(objs);
    pthread_exit(0);
  }

  return 0;

  exit(-99); /* Should never get here */
}


