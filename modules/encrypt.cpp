#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <SmacqModule.h>
#include <FieldVec.h>
#include <FieldVec.h>

#define KEYBYTES 128

static struct smacq_options options[] = {
  {"add", {boolean_t:0}, "Add the random value to the field (instead of replacing i)", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};


SMACQ_MODULE(encrypt, 
  PROTO_CTOR(encrypt);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  FieldVec fieldvec;
  DtsField changefield;
  DtsObject datum;

  int add;
); 

static inline int min(int a, int b) {
  return (a<b ? a : b);
}

smacq_result encryptModule::consume(DtsObject datum, int & outchan) {
  DtsObject field;
  unsigned long hval;

  if (!(field = datum->getfield(changefield))) {
    fprintf(stderr, "Skipping packet without field\n");
    return SMACQ_PASS;
  }
  

  fieldvec.getfields(datum);
  
  datum = datum->make_writable();

  if (!(field = datum->getfield(changefield))) {
    fprintf(stderr, "Skipping packet without field\n");
    return SMACQ_PASS;
  }

  hval = fieldvec.getobjs().hash();

  if (!add) {
    memcpy(field->getdata(), &hval, min(field->getsize(), sizeof(unsigned long)));
  } else {
    if (field->getsize() == sizeof(unsigned long)) {
      *(unsigned long*)field->getdata() += hval;
    } else {
      fprintf(stderr, "Don't know how to use -add with %d byte fields\n", field->getsize());
      assert(0);
    }
  }
     
  
  return (smacq_result)(SMACQ_FREE|SMACQ_PRODUCE);
}

encryptModule::encryptModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  int argc = 0;
  const char ** argv;
  {
    smacq_opt add_opt;
    
    struct smacq_optval optvals[] = {
     {"add", &add_opt},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
		       &argc, &argv,
		       options, optvals);
    
    add = add_opt.boolean_t;
  }

  // Consume rest of arguments as fieldnames
  if (argc) {
    changefield = dts->requirefield(argv[0]);
    if (!changefield) {
     fprintf(stderr, "No field to modify\n");
     assert(0);
    }
  }

  argc--, argv++;

  if (argc) {
    fieldvec.init(dts, argc, argv);
  }

}

smacq_result encryptModule::produce(DtsObject & datump, int & outchan) {
  if (datum) {
    datump = datum;
    datum = NULL;
    return SMACQ_PASS;
  } else {
    return SMACQ_END;
  }
}

