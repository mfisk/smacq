#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <smacq.h>
#include <FieldVec.h>
#include <FieldVec.h>
#include <SmacqScheduler.h>

/* Programming constants */

#define KEYBYTES 128

struct join {
  SmacqGraph * graph;
  dts_field field;
  dts_field left_key, right_key;
  struct runq * runq;
  double next_val;
  DtsObject next_dobj;
};

SMACQ_MODULE(ndjoin,
  PROTO_CTOR(ndjoin);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  int whereargc;
  char ** whereargv;
  DtsObject product;
  dts_comparison * comp;

  struct join join;

  SmacqScheduler * sched;
);

smacq_result ndjoinModule::consume(DtsObject datum, int & outchan) {
  smacq_result more;
  double left_val;

  struct join * j = &join;
  DtsObject left = datum->getfield(j->left_key);
  if (!left) return SMACQ_PASS;

  left_val = dts_data_as(left, double);

  while (1) {
    while (!j->next_dobj) {
      more = sched->get(j->next_dobj);
      if ( (SMACQ_END|SMACQ_ERROR) & more)
	return more;

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
  : SmacqModule(context)
{
  int argc = context->argc-1;
  char ** argv = context->argv+1;

  assert(argc > 3);

  join.left_key = dts->requirefield(dts_fieldname_append(argv[0], "double"));
  join.right_key = dts->requirefield(dts_fieldname_append(argv[1], "double"));
  join.field = dts->requirefield(argv[2]);
  join.graph = SmacqGraph::newQuery(dts, argc-3, argv+3);
  assert(join.graph);

  sched = new SmacqScheduler(dts, join.graph, true);
}

smacq_result ndjoinModule::produce(DtsObject & datump, int & outchan) {
  smacq_result status;

  if (product) {
    datump = product;
    status = SMACQ_PASS;
  } else {
    status = SMACQ_FREE;
  }

  return (smacq_result)(status | (product ? SMACQ_PRODUCE : 0));
}
