#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <smacq.h>
#include <sys/stat.h>
#include <unistd.h>
#include <strucio.h>

static struct smacq_options options[] = {
  {"starttime", {double_t:0}, "Start time for input files", SMACQ_OPT_TYPE_DOUBLE},
  {"endtime", {double_t:0}, "End time for input files", SMACQ_OPT_TYPE_DOUBLE},
  {"l", {boolean_t:0}, "List of files to read is on STDIN", SMACQ_OPT_TYPE_BOOLEAN},
  {"z", {boolean_t:1}, "Use gzip compression", SMACQ_OPT_TYPE_BOOLEAN},
  {"M", {boolean_t:0}, "Disable memory-mapped I/O", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  dts_object * datum;	
  smacq_environment * env;
  int argc;
  char ** argv;

  struct strucio * rdr;
  int cflow_type;
  int cflow_type_size;
};

static smacq_result cflow_produce(struct state * state, const dts_object ** datump, int * outchan) {
  dts_object * datum;

  datum = (dts_object*)smacq_alloc(state->env, state->cflow_type_size, state->cflow_type);
  datum->data = strucio_read(state->rdr, dts_getdata(datum), state->cflow_type_size);

  if (!datum->data) {
	dts_decref(datum);
	return SMACQ_END;
  }

  *datump = datum;
  return SMACQ_PASS|SMACQ_PRODUCE;
}

static smacq_result cflow_shutdown(struct state * state) {
  strucio_close(state->rdr);
  free(state);

  return 0;
}

static smacq_result cflow_init(struct smacq_init * context) {
  struct state * state;

  smacq_requiretype(context->env, "packet");

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->rdr = strucio_init();
  state->env = context->env;
  {
    smacq_opt list, gzip, avoid_mmap, start, end;

    struct smacq_optval optvals[] = {
      { "starttime", &start}, 
      { "endtime", &end}, 
      { "l", &list}, 
      { "z", &gzip}, 
      { "M", &avoid_mmap}, 
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &state->argc, &state->argv,
				 options, optvals);
   
    if (start.double_t) {
      assert(!list.boolean_t);
      assert(state->argc);
      strucio_register_filelist_bounded(state->rdr, state->argv[0], start.double_t, end.double_t);
    } else if (list.boolean_t) {
      strucio_register_filelist_stdin(state->rdr);
    } else {
      strucio_register_filelist_args(state->rdr, state->argc, state->argv);
    }

    strucio_set_use_gzip(state->rdr, gzip.boolean_t);

    if (avoid_mmap.boolean_t) {
      strucio_set_read_type(state->rdr, COPY);
    }
  }

  if (!context->isfirst) {
    fprintf(stderr, "cflow module must be at beginning of dataflow\n");
    exit(-1);
  }

  state->cflow_type = smacq_requiretype(context->env, "cflow");
  state->cflow_type_size = dts_type_size(context->env->types, state->cflow_type);

  return 0;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_cflow_table = {
  produce: &cflow_produce, 
  init: &cflow_init,
  shutdown: &cflow_shutdown
};

