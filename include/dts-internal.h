#ifndef DTS_INTERNAL_H
#define DTS_INTERNAL_H

#include <dts.h>
#include <dts-module.h>

typedef struct _dts_msg {
  DtsObject * field_data;
  dts_comparison * criteria;
  struct _dts_msg * next;
} dts_message;

#endif
