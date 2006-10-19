#include <stdio.h>
#include <SmacqModule.h>
#include <string>
#include <iostream>
#include <dts.h>

static struct smacq_options options[] = {
  {"l", {boolean_t:false}, "Read list of files from stdin", SMACQ_OPT_TYPE_BOOLEAN},
  {"F", {boolean_t:false}, "Follow files", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(file, 
  PROTO_CTOR(file);
  PROTO_PRODUCE();

  int argc;
  char ** argv;

  bool use_stdin;
  DtsObject followObject;
  DtsField followField; 
  dts_typeid string_type;
);


fileModule::fileModule(smacq_init * context) : SmacqModule(context) {
  smacq_opt list, follow;

  //fprintf(stderr, "Loading pcapfile (%d,%d)\n", context->isfirst, context->islast);
  dts->requiretype("packet");

  {
    struct smacq_optval optvals[] = {
      { "l", &list}, 
      { "F", &follow}, 
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
    use_stdin = list.boolean_t;
  }
  string_type = dts->requiretype("string");
  if (follow.boolean_t) {
 	followObject = dts->newObject(dts->requiretype("empty"));
	followField = dts->requirefield("doFollow");
  } 

  if (!argc && !use_stdin) {
	/* Read directly from stdin */
   	DtsObject o = dts->construct_fromstring(string_type, "-");
	enqueue(o);
  }

#ifdef USE_GASNET
  context->self->distribute_children(dts);
#endif
}

smacq_result fileModule::produce(DtsObject & o, int & outchan) {
	if (use_stdin) {
		char filename[4096];
		filename[0] = '\0';
		std::cin.getline(filename, 4096); 
		if (filename[0] != '\0') {
			o = dts->construct_fromstring(string_type, filename);
			assert(o);
			if (followObject) { o->attach_field(followField, followObject); }
			return SMACQ_PASS|SMACQ_PRODUCE;
		} else {
			return SMACQ_END;
		}
	} else if (argc) {
		o = dts->construct_fromstring(string_type, argv[0]);
		if (followObject) { o->attach_field(followField, followObject); }
		argv++;
		argc--;
		return SMACQ_PASS|SMACQ_PRODUCE;
	}
	return SMACQ_FREE|SMACQ_END;
}

