#include <stdio.h>
#include <glib.h>
#include <smacq.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <smacq.h>

struct thread_args {
  SmacqGraph * f;
  struct SmacqModule::smacq_init * context;
};

char * printer_args[] = { "print", "count" };

int main(int argc, char ** argv) {
  struct runq * runq = NULL;
  struct runq * printq = NULL;
  DtsObject record = NULL;
  SmacqGraph * objs, *printer;

  DTS * tenv = dts_init();

  objs = smacq_build_pipeline(argc-1, argv+1);
  smacq_start(objs, ITERATIVE, tenv);

  printer = smacq_build_pipeline(2, printer_args);
  smacq_start(printer, ITERATIVE, tenv);

  while (! (SMACQ_END & smacq_sched_iterative(objs, NULL, &record, &runq, 1))) {
	DtsObject ignore = NULL;
	assert(record);
	fprintf(stderr, "Got object %p; printing it's count field\n", record);
	smacq_sched_iterative(printer, record, &ignore, &printq, 0);
	
  }

  fprintf(stderr, "Done\n");
  return 0;
}

