#ifndef DTS_MODULE_H
#define DTS_MODULE_H

#include <dts.h>

typedef int field_getfunc_fn(DtsObject*, DtsObject*);

struct dts_field_spec {
  char * type;
  char * name;
  field_getfunc_fn * getfunc;
};

struct dts_field_info {
  int type;
  int offset;
  struct dts_field_spec desc;
};


#endif
