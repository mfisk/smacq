#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <fields.h>
#include <bytehash.h>

/* Programming constants */

#define KEYBYTES 128

//enum mode {  };

static struct smacq_options options[] = {
  {"add", {boolean_t:0}, "Add the random value to the field (instead of replacing i)", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  struct fieldset fieldset;
  int changefield;
  GHashTableofBytes * hashtable;
  const dts_object * datum;

  int add;
}; 

static inline int min(int a, int b) {
  return (a<b ? a : b);
}

static smacq_result encrypt_consume(struct state * state, const dts_object * datum, int * outchan) {
  dts_object field;
  struct iovec * keyv;
  unsigned long hval;

  if (! flow_getfield(state->env, datum, state->changefield, &field)) {
    fprintf(stderr, "Skipping packet without field\n");
    return SMACQ_PASS;
  }

  keyv = fields2vec(state->env, datum, &state->fieldset);
  if (!keyv) {
    fprintf(stderr, "Encryption basis fields not present\n");
    return SMACQ_PASS;
  }
  
 state->datum = dts_writable(state->env, datum);

  if (! flow_getfield(state->env, state->datum, state->changefield, &field)) {
    fprintf(stderr, "Skipping packet without field\n");
    return SMACQ_PASS;
  }

  hval = bytes_hash_valuev(state->hashtable, state->fieldset.num, keyv);

  if (!state->add) {
    memcpy(field.data, &hval, min(field.len, sizeof(unsigned long)));
  } else {
    if (field.len == sizeof(unsigned long)) {
      *(unsigned long*)field.data += hval;
    } else {
      fprintf(stderr, "Don't know how to use -add with %d byte fields\n", field.len);
      assert(0);
    }
  }
      
  return SMACQ_FREE|SMACQ_PRODUCE;
}

static int encrypt_init(struct flow_init * context) {
  int argc = 0;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
    smacq_opt add;
    
    struct smacq_optval optvals[] = {
     {"add", &add},
      {NULL, NULL}
    };
    flow_getoptsbyname(context->argc-1, context->argv+1,
		       &argc, &argv,
		       options, optvals);
    
    state->add = add.boolean_t;
  }

  // Consume rest of arguments as fieldnames
  if (argc) {
    state->changefield = flow_requirefield(state->env, argv[0]);
    if (!state->changefield) {
      fprintf(stderr, "No field to modify\n");
      return SMACQ_END;
    }
  }

  argc--, argv++;

  if (argc) {
    fields_init(state->env, &state->fieldset, argc, argv);
    state->hashtable = bytes_hash_table_new(KEYBYTES, chain);
  } else {
    state->hashtable = NULL;
  }

  return 0;
}

static int encrypt_shutdown(struct state * state) {
  return SMACQ_END;
}


static smacq_result encrypt_produce(struct state * state, const dts_object ** datump, int * outchan) {
  if (state->datum) {
    *datump = state->datum;
    state->datum = NULL;
    return SMACQ_PASS;
  } else {
    return SMACQ_END;
  }
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_encrypt_table = {
  &encrypt_produce, 
  &encrypt_consume,
  &encrypt_init,
  &encrypt_shutdown
};
