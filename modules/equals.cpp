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
#include <bytehash.h>
#include <produceq.h>

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

#define SMACQ_MODULE_IS_VECTOR 1
#define SMACQ_MODULE_IS_STATELESS 1

SMACQ_MODULE(equals,
  PROTO_CTOR(equals);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  char ** argv;
  int argc;

  struct set_element * sets;
  int numsets;

  struct smacq_outputq * outputq;

  void init_sets();
  struct iovec_hash * get_table(struct set_element * set, DtsObject * datum);
);

struct value {
  char * string;
  int outchan;
};

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

void equalsModule::init_sets() {
  char * value = NULL;
  char * field = NULL;
  int outchan = 0;

  while(1) {
    if (!argc || (value && !strcmp(";", argv[0]))) {
      if (field && value) {
	int i;
	struct set_element * set = NULL;
	
	for (i=0; i < numsets; i++) {
	  if (!strcmp(sets[i].fieldset.fields[0].name, field)) {
	    set = &sets[i];
	    break;
	  }
	}
	if (!set) {
	  i = numsets++;
	  sets = (set_element*)realloc(sets, sizeof(struct set_element) * (i+1));
	  set = &sets[i];
	  memset(set, 0, sizeof(struct set_element));
	  darray_init(&set->values,0);
	  darray_init(&set->tables,0);
	  dts->fields_init(&set->fieldset, 1, &field);
	}
	
	{
	  struct value * v = (struct value*)calloc(sizeof(struct value), 1);
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
struct iovec_hash * equalsModule::get_table(struct set_element * set, DtsObject * datum) {
	int i;
	int type = set->fieldset.currentdata[0]->gettype();
	struct iovec_hash * table = (struct iovec_hash*)darray_get(&set->tables, type);

	if (!table) {
  		table = bytes_hash_table_new(KEYBYTES, CHAIN|NOFREE);
		darray_set(&set->tables, type, table);

		for(i=0; i < darray_size(&set->values); i++) {
			struct value * v = (struct value*)darray_get(&set->values, i);
			DtsObject * k;
			assert(v);

			k = dts->construct_fromstring(set->fieldset.currentdata[0]->gettype(), v->string);
			if (!k) {
			  fprintf(stderr, "equals: could not parse value %s as type %s\n", v->string, 
				  dts->typename_bynum(set->fieldset.currentdata[0]->gettype()));
			  continue;
			}
			set->fieldset.currentvecs[0].iov_base = ((DtsObject*)k)->getdata();
			bytes_hash_table_setv(table, set->fieldset.currentvecs, set->fieldset.num, (gpointer)v->outchan+1);
			/*
			fprintf(stderr, "inserting %lx(len %d), output = %d\n", 
				*(unsigned long*)set->fieldset.currentvecs[0].iov_base, 
				set->fieldset.currentvecs[0].iov_len, 
				v->outchan+1);
			*/
			//k->decref();
		}
	}
	return table;
}

/*
 * Check presense in set.
 */
smacq_result equalsModule::consume(DtsObject * datum, int * outchan) {
  struct iovec * domainv;
  int val;
  int found = 0;
  struct iovec_hash * table;
  int t;
  int i;

  for (i=0; i < numsets; i++) {
    domainv = datum->fields2vec(&sets[i].fieldset);

    if (!domainv || !sets[i].fieldset.currentdata[0]) {
      //fprintf(stderr, "Skipping datum\n");
      return SMACQ_FREE;
    }

    table = get_table(&sets[i], datum);

    t = sets[i].fieldset.currentdata[0]->gettype();
    domainv = datum->fields2vec(&sets[i].fieldset);
    if ((val = (int)bytes_hash_table_lookupv(table, domainv, sets[i].fieldset.num))) {
      // In match set
      //fprintf(stderr, "searching for %lx(len %d), output = %d\n", *(unsigned long*)domainv[0].iov_base, domainv[0].iov_len, val);
      if (found) { 
	smacq_produce_enqueue(&outputq, datum, val - 1);
	datum->incref();
      } else {
	found = 1;
      	*outchan = val - 1;
      }
    }
  }
    
  if (found) 
	return (smacq_result)(SMACQ_PASS|smacq_produce_canproduce(&outputq));

  return SMACQ_FREE;
}

equalsModule::equalsModule(struct smacq_init * context) : SmacqModule(context) {
  argc = context->argc-1;
  argv = context->argv+1;
  init_sets();
}

smacq_result equalsModule::produce(DtsObject ** datum, int * outchan) {
  return smacq_produce_dequeue(&outputq, datum, outchan);
}
                                                                                                                                 
