#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <smacq.h>

SMACQ_MODULE(head,
	PROTO_CTOR(head);
	PROTO_CONSUME();
	int left;
);

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

smacq_result headModule::consume(DtsObject * datum, int * outchan) {
  assert(datum);

  if (left-- > 0) return SMACQ_PASS;

  return SMACQ_END;
}

headModule::headModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = context->argc - 1;
  char ** argv = context->argv + 1;

  assert(argc);

  left = atoi(argv[0]);
}

