#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_int_get_string(void * data, int dlen, void ** transform, int * tlen) {
  char buf[64]; // Only has to hold log10(2**32)

  snprintf(buf, 64, "%d", *(int*)data);
  *transform = strdup(buf);
  *tlen = strlen(data);

  return 1;
}

static int smacqtype_int_get_double(void * data, int dlen, void ** transform, int * tlen) {
  double * d = g_new(double, 1);
  int * i = (int*)data;
  *d = *i;
  *transform = d;
  *tlen = sizeof(double);

  return 1;
}

static int parse_string(char * buf, void ** resp, int * reslen) {
	int * val = malloc(sizeof(int));
	char * left = NULL;
	*val = strtol(buf, &left, 10);
 	if (left == buf) {
		free(*resp);
		return(0);
	}
	*resp = val;
	*reslen = sizeof(int);
	return 1;
}

int int_lt(void * num1, int size1, void * num2, int size2) {
	int * a = num1;
	int * b = num2;

	assert(size1 == sizeof(int));
	assert(size2 == sizeof(int));

	return(*a < *b);
}
	
struct dts_transform_descriptor dts_type_int_transforms[] = {
	{ "string",   smacqtype_int_get_string },
	{ "double",   smacqtype_int_get_double },
        { END,        NULL }
};

struct dts_type_info dts_type_int_table = {
        size: sizeof(int),
  	fromstring: parse_string,
	lt: int_lt
};

