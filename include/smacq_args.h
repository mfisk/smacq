#ifndef SMACQ_ARGS_H
#define SMACQ_ARGS_H

#include <sys/time.h>
#include <glib.h>
#include "smacq.h"

typedef union { 
  char * string_t;
  int int_t;
  double double_t;
  unsigned int uint_t;
  guint32 uint32_t;
  gushort ushort_t;
  gushort ubyte_t;
  struct timeval timeval_t;
  char boolean_t;
} smacq_opt;

struct smacq_optval {
  char * name;
  smacq_opt * location;
};


typedef enum smacq_opt_type {
  END, SMACQ_OPT_TYPE_INT, SMACQ_OPT_TYPE_USHORT, SMACQ_OPT_TYPE_TIMEVAL, SMACQ_OPT_TYPE_UINT32, SMACQ_OPT_TYPE_BYTES, SMACQ_OPT_TYPE_STRING, SMACQ_OPT_TYPE_UBYTE, SMACQ_OPT_TYPE_DOUBLE, SMACQ_OPT_TYPE_BOOLEAN
} smacq_opt_type;

struct smacq_options {
  char * name;
  smacq_opt default_value;
  char * description;
  smacq_opt_type type;
  int flags;
};

EXTERN int smacq_getoptsbyname(int argc, char ** argv, 
			      int * , char***, 
			      struct smacq_options *, 
			      struct smacq_optval *);


#endif
