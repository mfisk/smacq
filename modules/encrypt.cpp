#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <fields.h>
#include <bytehash.h>

#define KEYBYTES 128

static struct smacq_options options[] = {
  {"add", {boolean_t:0}, "Add the random value to the field (instead of replacing i)", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};


SMACQ_MODULE(encrypt, 
  PROTO_CTOR(encrypt);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  struct fieldset fieldset;
  dts_field changefield;
  struct iovec_hash * hashtable;
  DtsObject * datum;

  int add;
); 

static inline int min(int a, int b) {
  return (a<b ? a : b);
}

smacq_result encryptModule::consume(DtsObject * datum, int * outchan) {
  DtsObject * field;
  struct iovec * keyv;
  unsigned long hval;

  if (!(field = datum->getfield(changefield))) {
    fprintf(stderr, "Skipping packet without field\n");
    return SMACQ_PASS;
  }
  field->decref();

  keyv = datum->fields2vec(&fieldset);
  if (!keyv) {
    fprintf(stderr, "Encryption basis fields not present\n");
    return SMACQ_PASS;
  }
  
  datum = datum->make_writable();

  if (!(field = datum->getfield(changefield))) {
    fprintf(stderr, "Skipping packet without field\n");
    return SMACQ_PASS;
  }

  hval = bytes_hashv(keyv, fieldset.num);

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
     
  field->decref();
  return (smacq_result)(SMACQ_FREE|SMACQ_PRODUCE);
}

encryptModule::encryptModule(struct smacq_init * context) : SmacqModule(context) {
  int argc = 0;
  char ** argv;
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
    dts->fields_init(&fieldset, argc, argv);
    hashtable = bytes_hash_table_new(KEYBYTES, CHAIN|NOFREE);
  } else {
    hashtable = NULL;
  }

}

smacq_result encryptModule::produce(DtsObject ** datump, int * outchan) {
  if (datum) {
    *datump = datum;
    datum = NULL;
    return SMACQ_PASS;
  } else {
    return SMACQ_END;
  }
}

