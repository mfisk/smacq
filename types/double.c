#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_double_get_double(const dts_object * o, dts_object * field) {
  double * dbl = malloc(sizeof(double));
  *dbl = dts_data_as(o, double);
  field->data= dbl;
  field->len= sizeof(double);

  return 1;
}

static int smacqtype_double_get_string(const dts_object * o, dts_object * field) {
  char buf[64]; 

  snprintf(buf, 64, "%g", dts_data_as(o, double));

  field->data= strdup(buf);
  field->len= strlen(buf);
  field->free_data = 1;

  return 1;
}

static int parse_string(char * buf, void ** resp, int * reslen) {
  double * dbl = malloc(sizeof(double));
  char * left = NULL;
  *dbl = strtod(buf, &left);
  if (left == buf) {
	free(dbl);
	return 0;
  }  
  *resp = dbl;
  *reslen = sizeof(double);
    
  return 1;
}

struct dts_field_descriptor dts_type_double_fields[] = {
  { "string",   "string",	smacqtype_double_get_string },
  { "double",   "double",	smacqtype_double_get_double },
  { END,        NULL }
};

struct dts_type_info dts_type_double_table = {
        size: sizeof(double),
  	fromstring: parse_string
};

