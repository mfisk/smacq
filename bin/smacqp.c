#include <stdio.h>
#include <glib.h>
#include <smacq.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <smacq.h>

struct thread_args {
  smacq_graph * f;
  struct smacq_init * context;
};

int main(int argc, char ** argv) {
  smacq_graph * objs;

  assert(argc > 1);
  objs = smacq_build_pipeline(argc-1, argv+1);

  return smacq_start(objs, RECURSIVE, NULL);
}

