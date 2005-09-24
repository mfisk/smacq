#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
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

void print_help(struct smacq_options * opt) {
	char defbuf[256];
	double t;
	for (; opt->name; opt++) {
		char * defval = defbuf;
		switch (opt->type) {
		case SMACQ_OPT_TYPE_STRING:
			defval = opt->default_value.string_t;
			break;
		case SMACQ_OPT_TYPE_BOOLEAN:
			defval = opt->default_value.boolean_t ? "on" : "off";
			break;
		case SMACQ_OPT_TYPE_DOUBLE:
			sprintf(defval, "%g", opt->default_value.double_t);
			break;
		case SMACQ_OPT_TYPE_UINT32:
			sprintf(defval, "%lu", opt->default_value.uint32_t);
			break;
		case SMACQ_OPT_TYPE_INT:
			sprintf(defval, "%i", opt->default_value.int_t);
			break;
	    case SMACQ_OPT_TYPE_TIMEVAL:
			t = opt->default_value.timeval_t.tv_sec +  
					opt->default_value.timeval_t.tv_usec / 1e6;
			sprintf(defval, "%g", t);
			break;
		case END:
			return;
		}

		fprintf(stderr, "\t-%s%s\t%s (default=%s)\n", strlen(opt->name) > 1 ? "-" : "",
					opt->name, opt->description, defval);
	}
}

int parse_opt(struct smacq_options * options, struct smacq_optval * optvals, 
	char * arg, char * nextarg) {
	struct smacq_options * opt = get_optstruct_byname(options, arg);

	if (!opt) {
		// XXX: handle --docbook option here

		fprintf(stderr, "Unknown option %s:\n", arg);
		print_help(options);
	   	exit(-1);
	}

	struct smacq_optval * val = get_optval_byname(optvals, arg);
	assert(val);

	if (!nextarg && opt->type != SMACQ_OPT_TYPE_BOOLEAN) {
	    fprintf(stderr, "Option %s needs argument:\n", arg);
		print_help(options);
	    exit(-1);
	}
	  
	switch(opt->type) {
	  case SMACQ_OPT_TYPE_BOOLEAN:
	    val->location->boolean_t++;
		return 0;
	    break;
	  case SMACQ_OPT_TYPE_STRING:
	    val->location->string_t = nextarg;
		return 1;
	    break;
	  case SMACQ_OPT_TYPE_INT:
	    val->location->int_t = strtol(nextarg, NULL, 0);
		return 1;
	    break;
	  case SMACQ_OPT_TYPE_UINT32:
	    val->location->uint32_t = strtoul(nextarg, NULL, 0);
		return 1;
	    break;
	  case SMACQ_OPT_TYPE_DOUBLE:
	    val->location->double_t = strtod(nextarg, NULL);
		return 1;
	    break;
	  case SMACQ_OPT_TYPE_TIMEVAL:
	    val->location->timeval_t.tv_sec = floor(strtod(nextarg, NULL));
	    val->location->timeval_t.tv_usec = floor(1000000 * (strtod(nextarg, NULL) - val->location->timeval_t.tv_sec));
		return 1;
	    break;
	  case END:
	    assert(0);
		break;
	}

	assert(0);
	return 0;
}


int smacq_getoptsbyname( int argc, char ** argv,
			int * argc_left, char *** argv_left,
			struct smacq_options * options, 
			struct smacq_optval * optvals) {
	int i;

	assert(options);
	assert(optvals);

	if (argc_left) *argc_left = 0;

	set_opts_to_default(options, optvals);

	assert (argc>=0);

	for (i = 0; i < argc; i++) {
	  if ((argv[i][0] != '-') || (argv[i][1]=='\0')) {
	    if (argc_left && argv_left) {
	   	  *argv_left = argv+i;
		  *argc_left = argc - i;
		  return 0;
	    } else {
	      fprintf(stderr, "Unknown option %s", argv[i]);
		  print_help(options);
	      exit(-1);
	    }
	  } else if (argv[i][0] == '-' && argv[i][1] == '-') {
		char * nextarg;
		if (i < (argc-1)) {
			nextarg = argv[i+1];
		} else {
			nextarg = NULL;
		}
		i += parse_opt(options, optvals, argv[i]+2, i < (argc-1) ? argv[i+1] : NULL);
	  } else {
		char * o;
		char opt[2] = "\0\0";

		for(o = argv[i]+1; *o; o++) {
			opt[0] = *o;
			i += parse_opt(options, optvals, opt, i < argc ? argv[i+1] : NULL);
		}
	  }
	}

	return 0;
}
