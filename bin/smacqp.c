#include <stdio.h>
#include <glib.h>
#include <gmodule.h>
#include <smacq.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <smacq-internal.h>

struct filter * parse_stmt(int, char **);

struct thread_args {
  struct filter * f;
  struct smacq_init * context;
};

int main(int argc, char ** argv) {
  struct filter * objs;

  assert(argc > 1);

  if (strcmp(argv[1], "-s") == 0) {
    objs = parse_stmt(argc-1, argv+1);
  }
  else {
    objs = smacq_build_pipeline(argc-1, argv+1);
  }
  return smacq_start(objs, RECURSIVE, NULL);
}

