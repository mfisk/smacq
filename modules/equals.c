/* 
   This is a vectorized implementation of a equality comparisons
   between a field and a constant value.  We build a hash table of all
   the given constants and then look-up each object's field value in
   the table.  If there's a match, the hash table value tells us what
   output channel to use.

  The trick is that we may have to compare fields of different types
  and the vector may specify multiple fields.  So we have a table for
  each type of each field name.  The table gets built when we first
  see a given type for each field.

*/

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <smacq.h>
#include <fields.h>
#include <bloom.h>
#include "bytehash.h"

/* Programming constants */

#define VECTORSIZE 21
#define ALARM_BITS 15
#define KEYBYTES 128

struct set_element {
  //struct iovec_hash *drset;
  struct darray tables; // darray of struct iovec_hash* 
  struct darray values; // darray of struct value 
  struct fieldset fieldset;
};

struct state {
  smacq_environment * env;
  char ** argv;
  int argc;

  struct set_element * sets;
  int numsets;

  struct smacq_outputq * outputq;
};

struct value {
  char * string;
  int outchan;
};

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

static inline void init_sets(struct state * state) {
  int argc = state->argc;
  char ** argv = state->argv;
  char * value = NULL;
  char * field = NULL;
  int outchan = 0;

  while(1) {
    if (!argc || (value && !strcmp(";", argv[0]))) {
      if (field && value) {
	int i;
	struct set_element * set = NULL;
	
	for (i=0; i < state->numsets; i++) {
	  if (!strcmp(state->sets[i].fieldset.fields[0].name, field)) {
	    set = &state->sets[i];
	    break;
	  }
	}
	if (!set) {
	  i = state->numsets++;
	  state->sets = realloc(state->sets, sizeof(struct set_element) * (i+1));
	  set = &state->sets[i];
	  memset(set, 0, sizeof(struct set_element));
	  darray_init(&set->values,0);
	  darray_init(&set->tables,0);
	  fields_init(state->env, &set->fieldset, 1, &field);
	}
	
	{
	  struct value * v = calloc(sizeof(struct value), 1);
	  v->outchan = outchan++;
	  v->string = value;
	  darray_append(&set->values, v);
	}
      }
      if (!argc) break;

      value = NULL;
      field = NULL;
    } else if (field) {
      value = argv[0];
    } else {
      field = argv[0];
    }

    argc--, argv++;
  }
}

/* fields2vec must have been called before get_table() */
static inline struct iovec_hash * get_table(struct state * state, struct set_element * set, const dts_object * datum) {
	int i;
	int type = set->fieldset.currentdata[0]->type;
	struct iovec_hash * table = darray_get(&set->tables, type);

	if (!table) {
  		table = bytes_hash_table_new(KEYBYTES, CHAIN|NOFREE);
		darray_set(&set->tables, type, table);

		for(i=0; i < darray_size(&set->values); i++) {
			struct value * v = darray_get(&set->values, i);
			const dts_object * k;
			assert(v);

			k = dts_construct_fromstring(state->env->types, dts_gettype(set->fieldset.currentdata[0]), v->string);
			if (!k) {
			  fprintf(stderr, "equals: could not parse value %s as type %s\n", v->string, 
				  dts_typename_bynum(state->env, dts_gettype(set->fieldset.currentdata[0])));
			  continue;
			}
			set->fieldset.currentvecs[0].iov_base = ((dts_object*)k)->data;
			bytes_hash_table_setv(table, set->fieldset.currentvecs, set->fieldset.num, (gpointer)v->outchan+1);
			/*
			fprintf(stderr, "inserting %lx(len %d), output = %d\n", 
				*(unsigned long*)set->fieldset.currentvecs[0].iov_base, 
				set->fieldset.currentvecs[0].iov_len, 
				v->outchan+1);
			*/
			//dts_decref(k);
		}
	}
	return table;
}

/*
 * Check presense in set.
 */
static smacq_result equals_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct iovec * domainv;
  int val;
  int found = 0;
  struct iovec_hash * table;
  int t;
  int i;

  for (i=0; i < state->numsets; i++) {
    domainv = fields2vec(state->env, datum, &state->sets[i].fieldset);

    if (!domainv || !state->sets[i].fieldset.currentdata[0]) {
      //fprintf(stderr, "Skipping datum\n");
      return SMACQ_FREE;
    }

    table = get_table(state, &state->sets[i], datum);

    t = state->sets[i].fieldset.currentdata[0]->type;
    domainv = fields2vec(state->env, datum, &state->sets[i].fieldset);
    if ((val = (int)bytes_hash_table_lookupv(table, domainv, state->sets[i].fieldset.num))) {
      // In match set
      //fprintf(stderr, "searching for %lx(len %d), output = %d\n", *(unsigned long*)domainv[0].iov_base, domainv[0].iov_len, val);
      if (found) { 
	smacq_produce_enqueue(&state->outputq, datum, val - 1);
	dts_incref(datum, 1);
      } else {
	found = 1;
      	*outchan = val - 1;
      }
    }
  }
    
  if (found) return SMACQ_PASS|smacq_produce_canproduce(&state->outputq);

  return SMACQ_FREE;
}

static smacq_result equals_init(struct smacq_init * context) {
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
  	struct smacq_optval optvals[] = {
    		{NULL, NULL}
  	};
  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &state->argc, &state->argv,
			       options, optvals);
  }

  init_sets(state);

  return 0;
}

static smacq_result equals_produce(struct state* state, const dts_object ** datum, int * outchan) {
  return smacq_produce_dequeue(&state->outputq, datum, outchan);
}
                                                                                                                                 
/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_equals_table = {
	init: &equals_init,
	consume: &equals_consume,
	produce: &equals_produce,
	algebra: {vector: 1, boolean: 1},
};
