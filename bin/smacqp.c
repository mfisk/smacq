#include <stdio.h>
#include <glib.h>
#include <gmodule.h>
#include <smacq.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <smacq-internal.h>

char ** parse_stmt(int, char **, int *);

struct thread_args {
  struct filter * f;
  struct smacq_init * context;
};

int main(int argc, char ** argv) {
  struct filter * objs;
  int new_argc;
  char ** new_argv;

  assert(argc > 1);

  if (strcmp(argv[1], "-s") == 0) {
    new_argv = parse_stmt(argc-1, argv+1, &new_argc);
    objs = smacq_build_pipeline(new_argc, new_argv);
  }
  else {
    objs = smacq_build_pipeline(argc-1, argv+1);
  }
  return smacq_start(objs, RECURSIVE, NULL);
}

