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
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(cflow,
  PROTO_CTOR(cflow);
  PROTO_DTOR(cflow);
  PROTO_PRODUCE();

  int argc;
  char ** argv;

  Strucio * rdr;
  int cflow_type;
  int cflow_type_size;
);

smacq_result cflowModule::produce(DtsObject & datump, int * outchan) {
  DtsObject datum;

  datum = dts->newObject(cflow_type, cflow_type_size);
  datum->setdata(rdr->read(datum->getdata(), cflow_type_size));

  if (!datum->getdata()) {
	
	return SMACQ_END;
  }

  datump = datum;
  return (smacq_result)(SMACQ_PASS|SMACQ_PRODUCE);
}

cflowModule::~cflowModule() {
  delete rdr;
}

cflowModule::cflowModule(struct smacq_init * context) : SmacqModule(context) {
  dts->requiretype("packet");

  rdr = new Strucio();
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
				 &argc, &argv,
				 options, optvals);
   
    if (start.double_t) {
      assert(!list.boolean_t);
      assert(argc);
      rdr->register_filelist_bounded(argv[0], start.double_t, end.double_t);
    } else if (list.boolean_t) {
      rdr->register_filelist_stdin();
    } else {
      rdr->register_filelist_args(argc, argv);
    }

    rdr->set_use_gzip(gzip.boolean_t);

    if (avoid_mmap.boolean_t) {
      rdr->set_read_type(COPY);
    }
  }

  if (!context->isfirst) {
    fprintf(stderr, "cflow module must be at beginning of dataflow\n");
    exit(-1);
  }

  cflow_type = dts->requiretype("cflow");
  cflow_type_size = dts->type_size(cflow_type);
}

