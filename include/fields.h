#include <dts.h>

struct field {
  char * name;
  dts_field num;
};

struct fieldset {
  int num;
  struct iovec * currentvecs;
  DtsObject ** currentdata;
  struct field * fields;

  //Private:
  int lasttype;
};


static inline int smacq_nextfielddata(struct fieldset * fieldset, DtsObject **d, int i) {
     if (i >= fieldset->num) 
	     	return 0;

     *d = fieldset->currentdata[i];
     return (i+1);
}

extern struct iovec * fields2vec(DTS * env, DtsObject * datum, struct fieldset * fieldset);
void fields_init(DTS * env, struct fieldset * fieldset, int argc, char ** argv);
int iovec_has_undefined(struct iovec *, int nvecs);
void fieldset_destroy(struct fieldset * fieldset);
