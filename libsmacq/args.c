#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <smacq_args.h>

struct smacq_options * get_optstruct_byname(struct smacq_options * opts, char * name) {
	assert(opts);
	for(; opts->name; opts++) {
		if (strcmp(opts->name, name)) continue;
	
		return(opts);
	}
	return(NULL);
}


/* Return the pointer to the option value as specirfied in the optval list */
struct smacq_optval * get_optval_byname(struct smacq_optval * opts, char * name) {
  int i;
  assert(opts);
  assert(name);
  for(i=0; &opts[i] && opts[i].name; i++) {
    if (strcmp(opts[i].name, name)) continue;
    
    return(opts+i);
  }
  return(NULL);
}

void set_opts_to_default(struct smacq_options * smacq_options, 
		    struct smacq_optval * smacq_optval) {
	struct smacq_options * opts;
	struct smacq_optval * val;

	for (opts = smacq_options; opts->name; opts++) {
		val = get_optval_byname(smacq_optval, opts->name);
		if (!val) {
			fprintf(stderr, "No storage for option %s\n", opts->name);
		}
		*(val->location) = opts->default_value;
	}
}

int smacq_getoptsbyname( int argc, char ** argv,
			int * argc_left, char *** argv_left,
			struct smacq_options * options, 
			struct smacq_optval * optvals) {
	struct smacq_options * opt;
	struct smacq_optval * val;
	int c;
	char ** v;

	assert(options);
	assert(optvals);

	if (argc_left) *argc_left = 0;

	set_opts_to_default(options, optvals);

	assert (argc>=0);

	for(c = argc, v=argv; c; c--, v++) {
	  if ((v[0][0] != '-') || v[0][1]=='\0' || !(opt = get_optstruct_byname(options, v[0]+1))) {
	    if (argc_left) {
	      if (!*argc_left) {
		*argv_left = g_new(char *, argc);
	      }
	      (*argv_left)[*argc_left] = v[0];
	      (*argc_left)++;
	      continue;
	    } else {
	      fprintf(stderr, "Unknown option %s", v[0]);
	      exit(-1);
	    }
	  }
				 
	  val = get_optval_byname(optvals, v[0]+1);
	  assert(val);

	  if (c < 2 && opt->type != SMACQ_OPT_TYPE_BOOLEAN) {
	    fprintf(stderr, "Need argument to %s\n", v[0]);
	    exit(-1);
	  }
	  v++; c--;
	  
	  switch(opt->type) {
	  case SMACQ_OPT_TYPE_BOOLEAN:
	    val->location->boolean_t++;
	    v--; c++;
	    break;
	  case SMACQ_OPT_TYPE_STRING:
	    val->location->string_t = v[0];
	    break;
	  case SMACQ_OPT_TYPE_INT:
	    val->location->int_t = atoi(v[0]);
	    break;
	  case SMACQ_OPT_TYPE_UINT32:
	    val->location->uint32_t = atol(v[0]);
	    break;
	  case SMACQ_OPT_TYPE_USHORT:
	    val->location->ushort_t = atoi(v[0]);
	    break;
	  case SMACQ_OPT_TYPE_DOUBLE:
	    val->location->double_t = atof(v[0]);
	    break;
	  case SMACQ_OPT_TYPE_TIMEVAL:
	    val->location->timeval_t.tv_sec = floor(atof(v[0]));
	    val->location->timeval_t.tv_usec = floor(1000000 * (atof(v[0]) - val->location->timeval_t.tv_sec));
	    break;
	  default:
	    assert(0);
	  }
	}

	return 0;
}
