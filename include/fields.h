#include <smacq.h>

struct field {
  char * name;
  dts_field num;
};

struct fieldset {
  int num;
  struct iovec * currentvecs;
  const dts_object ** currentdata;
  struct field * fields;

  //Private:
  int lasttype;
};

extern struct iovec * fields2vec(smacq_environment * env, const dts_object * datum, struct fieldset * fieldset);
void fields_init(smacq_environment * env, struct fieldset * fieldset, int argc, char ** argv);


