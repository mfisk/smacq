#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <smacq.h>
#include <fields.h>
#include <bytehash.h>

/* Programming constants */

#define KEYBYTES 128

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

struct join {
  smacq_graph * graph;
  dts_field field;
  dts_field left_key, right_key;
  struct runq * runq;
  double next_val;
  const dts_object * next_dobj; 
};

struct state {
  smacq_environment * env;
  int whereargc;
  char ** whereargv;
  const dts_object * product;
  dts_comparison * comp;

  struct join join;
}; 

static smacq_result join_consume(struct state * state, const dts_object * datum, int * outchan) {
  int more;
  double left_val;

	struct join * j = &state->join;
	const dts_object * left = smacq_getfield(state->env, datum, j->left_key, NULL);
	if (!left) return SMACQ_PASS;

	left_val = dts_data_as(left, double);
	dts_decref(left);

	while (1) {
	  while (!j->next_dobj) {
  		more = smacq_sched_iterative(j->graph, NULL, &j->next_dobj, &j->runq, 1);
		if (SMACQ_END & more) 
			return SMACQ_END;

		if (j->next_dobj) {
	  		const dts_object * next_val = smacq_getfield(state->env, j->next_dobj, j->right_key, NULL);
	    		if (!next_val) {
				dts_decref(j->next_dobj);
				j->next_dobj = NULL;
			} else {
	  			j->next_val = dts_data_as(next_val, double);
				/* Success: found the next key */
	  			dts_decref(next_val);
			}
		}
          }


	  if (left_val > j->next_val) {
		/* We skipped over this object */
		dts_decref(j->next_dobj);
		j->next_dobj = NULL;
		//fprintf(stderr, "skipped over %g to %g\n", j->next_val, left_val);
	  } else if (left_val == j->next_val) {
		dts_attach_field(datum, j->field, j->next_dobj);
		j->next_dobj = NULL;
		//fprintf(stderr, "attaching %p to %p field %d\n", j->next_dobj, datum, j->field);
		break;
	  }
        }

  return SMACQ_PASS;
}

static smacq_result join_init(struct smacq_init * context) {
  int argc = 0;
  char ** argv;

  struct state * state = context->state = calloc(1, sizeof(struct state));
  state->env = context->env;

  {
  	struct smacq_optval optvals[] = {
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

  }

  assert(argc > 3);

  state->join.left_key = smacq_requirefield(state->env, dts_fieldname_append(argv[0], "double"));
  state->join.right_key = smacq_requirefield(state->env, dts_fieldname_append(argv[1], "double"));
  state->join.field = smacq_requirefield(state->env, argv[2]);
  state->join.graph = smacq_build_query(state->env->types, argc-3, argv+3);
  assert(state->join.graph);
  if (0 != smacq_start(state->join.graph, ITERATIVE, state->env->types)) {
	return SMACQ_ERROR;
  }

  return 0;
}

static smacq_result join_shutdown(struct state * state) {
  return SMACQ_END;
}


static smacq_result join_produce(struct state * state, const dts_object ** datump, int * outchan) {
  int status;

  if (state->product) {
    *datump = state->product;
    status = SMACQ_PASS;
  } else {
    status = SMACQ_FREE;
  }

  return status | (state->product ? SMACQ_PRODUCE : 0);
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_ndjoin_table = {
  &join_produce, 
  &join_consume,
  &join_init,
  &join_shutdown
};
