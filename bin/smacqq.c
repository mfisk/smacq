#include <stdio.h>
#include <glib.h>
#include <gmodule.h>
#include <smacq.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <smacq-internal.h>

static int multiproc = 1;

struct thread_args {
  struct filter * f;
  struct flow_init * context;
};

int main(int argc, char ** argv) {
  struct filter * objs;
  dts_init();
  objs = smacq_build_query(argc-1, argv+1);
  return flow_start(objs, multiproc, NULL);
}

