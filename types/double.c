#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_double_get_double(void * data, int dlen, void ** transform, int * tlen) {
  double * dbl = malloc(sizeof(double));
  *dbl = *(double*)data;
  *transform = dbl;
  *tlen = sizeof(double);

  return 1;
}

static int smacqtype_double_get_string(void * data, int dlen, void ** transform, int * tlen) {
  char buf[64]; 

  snprintf(buf, 64, "%g", *(double*)data);

  *transform = strdup(buf);
  *tlen = strlen(data);

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

struct dts_transform_descriptor dts_type_double_transforms[] = {
	{ "string",   smacqtype_double_get_string },
	{ "double",   smacqtype_double_get_double },
        { END,        NULL }
};

struct dts_type_info dts_type_double_table = {
        size: sizeof(double),
  	fromstring: parse_string
};

