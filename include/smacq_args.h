#ifndef SMACQ_ARGS_H
#define SMACQ_ARGS_H
#include <ccpp.h>
#include <sys/time.h>

union _smacq_opt { 
  const char * string_t;
  int int_t;
  double double_t;
  unsigned long uint32_t;
  struct timeval timeval_t;
  char boolean_t;
};

typedef union _smacq_opt smacq_opt;

struct smacq_optval {
  const char * name;
  smacq_opt * location;
};


typedef enum smacq_opt_type {
  END, SMACQ_OPT_TYPE_INT, SMACQ_OPT_TYPE_TIMEVAL, SMACQ_OPT_TYPE_UINT32, SMACQ_OPT_TYPE_STRING, SMACQ_OPT_TYPE_DOUBLE, SMACQ_OPT_TYPE_BOOLEAN
} smacq_opt_type;

struct smacq_options {
  const char * name;
  smacq_opt default_value;
  const char * description;
  smacq_opt_type type;
  int flags;
};

#define END_SMACQ_OPTIONS {NULL, {int_t:0}, NULL, END}

BEGIN_C_DECLS

int smacq_getoptsbyname(int argc, const char ** argv, 
			      int * , const char***, 
			      struct smacq_options *, 
			      struct smacq_optval *);

void smacq_print_help(struct smacq_options *);

END_C_DECLS

#endif
