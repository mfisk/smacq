#include <smacq-internal.h>
#include <stdio.h>

int smacq_start(struct filter * objs, enum smacq_scheduler scheduler, dts_environment * tenv) {
  smacq_environment * env = g_new0(smacq_environment, 1);

  if (!tenv) {
    tenv = dts_init();
  }

  env->types = tenv;
  env->alloc = _smacq_alloc;

  smacq_init_modules(objs, env);

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
    while (smacq_sched_iterative(objs, NULL, &record, &runq, 1)) ;
  } else {
    smacq_start_threads(objs);
    pthread_exit(0);
  }

  return 0;

  exit(-99); /* Should never get here */
}


