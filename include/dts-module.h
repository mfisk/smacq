#ifndef DTS_MODULE_H
#define DTS_MODULE_H

#include <dts-types.h>

/* This is the minimal header file included by DTS modules */
typedef int field_getfunc_fn(DtsObject, DtsObject);

struct dts_field_spec {
  char * type;
  char * name;
  field_getfunc_fn * getfunc;
};

/* Unfortunately, we currently need almost everything */
#include <dts.h>

#endif
