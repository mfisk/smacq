#include <smacq.h>

struct field {
  char * name;
  dts_field num;
};

struct fieldset {
  int num;
  struct iovec * currentvecs;
  dts_object * currentdata;
  struct field * fields;

  //Private:
  int lasttype;
};

static inline int smacq_nextfielddata(struct fieldset * fieldset, dts_object *d, int i) {
     if (i >= fieldset->num) 
	     	return 0;

     *d = fieldset->currentdata[i];
     return (i+1);
}

extern dts_object * fields2data(smacq_environment * env, const dts_object * datum, struct fieldset * fieldset);
extern struct iovec * fields2vec(smacq_environment * env, const dts_object * datum, struct fieldset * fieldset);
void fields_init(smacq_environment * env, struct fieldset * fieldset, int argc, char ** argv);


