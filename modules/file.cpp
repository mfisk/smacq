#include <stdio.h>
#include <SmacqModule.h>
#include <string>
#include <iostream>
#include <dts.h>

static struct smacq_options options[] = {
  {"l", {boolean_t:false}, "Read list of files from stdin", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(file, 
  PROTO_CTOR(file);
  PROTO_PRODUCE();

  int argc;
  char ** argv;

  bool use_stdin;
  dts_typeid string_type;
);


fileModule::fileModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  smacq_opt list;

  //fprintf(stderr, "Loading pcapfile (%d,%d)\n", context->isfirst, context->islast);
  dts->requiretype("packet");

  {
    struct smacq_optval optvals[] = {
      { "l", &list}, 
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
    use_stdin = list.boolean_t;
  }
  string_type = dts->requiretype("string");

  if (!argc && !use_stdin) {
	/* Read directly from stdin */
   	DtsObject o = dts->construct_fromstring(string_type, "-");
	enqueue(o);
  }
}

smacq_result fileModule::produce(DtsObject & o, int & outchan) {
	if (use_stdin) {
		char filename[4096];
		filename[0] = '\0';
		std::cin.getline(filename, 4096); 
		if (filename[0] != '\0') {
			o = dts->construct_fromstring(string_type, filename);
			assert(o);
			return SMACQ_PASS|SMACQ_PRODUCE;
		} else {
			return SMACQ_END;
		}
	} else if (argc) {
		o = dts->construct_fromstring(string_type, argv[0]);
		argv++;
		argc--;
		return SMACQ_PASS|SMACQ_PRODUCE;
	}
	return SMACQ_FREE|SMACQ_END;
}

