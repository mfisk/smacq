#include <stdio.h>
#include <glib.h>
#include <gmodule.h>
#include <smacq.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <smacq-internal.h>

struct thread_args {
  struct filter * f;
  struct smacq_init * context;
};

int main(int argc, char ** argv) {
  dts_init();
  smacq_execute_query(argc-1, argv+1);
  return 0;
}

