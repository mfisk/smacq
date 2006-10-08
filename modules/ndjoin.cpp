#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <SmacqModule.h>
#include <FieldVec.h>
#include <SmacqScheduler.h>

struct join {
  SmacqGraph graph;
  DtsField field;
  DtsField left_key, right_key;
  double next_val;
  DtsObject next_dobj;
};

SMACQ_MODULE(ndjoin,
  PROTO_CTOR(ndjoin);
  PROTO_CONSUME();

  int whereargc;
  char ** whereargv;
  dts_comparison * comp;

  struct join join;

  SmacqScheduler * sched;
);

smacq_result ndjoinModule::consume(DtsObject datum, int & outchan) {
  double left_val;

  struct join * j = &join;
  DtsObject left = datum->getfield(j->left_key);
  if (!left) return SMACQ_PASS;

  left_val = dts_data_as(left, double);

  while (1) {
    while (!j->next_dobj) {
      j->next_dobj = sched->get();
      if (!j->next_dobj) {
	return SMACQ_END;
      }

      if (j->next_dobj) {
	DtsObject next_val = j->next_dobj->getfield(j->right_key);
	if (!next_val) {

	  j->next_dobj = NULL;
	} else {
	  j->next_val = dts_data_as(next_val, double);
	  /* Success: found the next key */

	}
      }
    }


    if (left_val > j->next_val) {
      /* We skipped over this object */

      j->next_dobj = NULL;
      //fprintf(stderr, "skipped over %g to %g\n", j->next_val, left_val);
    } else if (left_val == j->next_val) {
      datum->attach_field(j->field, j->next_dobj);
      j->next_dobj = NULL;
      //fprintf(stderr, "attaching %p to %p field %d\n", j->next_dobj, datum, j->field);
      break;
    }
  }

  return SMACQ_PASS;
}

ndjoinModule::ndjoinModule(struct SmacqModule::smacq_init * context)
  : SmacqModule(context), sched(context->scheduler)
{
  int argc = context->argc-1;
  char ** argv = context->argv+1;

  assert(argc > 3);

  join.left_key = dts->requirefield(dts_fieldname_append(argv[0], "double"));
  join.right_key = dts->requirefield(dts_fieldname_append(argv[1], "double"));
  join.field = dts->requirefield(argv[2]);

  join.graph.addQuery(dts, sched, argv2string(argc-3, argv+3));
  join.graph.init(dts, sched);
  sched->seed_produce(&join.graph);
}

