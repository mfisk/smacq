#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "smacq.h"

static int smacqtype_bytes_get_string(const dts_object * o, dts_object * field) {
  dts_setsize(field, o->len+1);
  memcpy(dts_getdata(field), dts_getdata(o), o->len);

  /* Make sure it's NULL terminated */
  memset(field->data + o->len, '\0', 1); 

  return 1;
}

static int smacqtype_bytes_url_decode(const dts_object * o, dts_object * field) {
  unsigned char * p, * dp, * end;
  int shrink = 0;
  char hex[3];

  hex[2] = '\0';
  dts_setsize(field, o->len);
  p = dts_getdata(o);
  end = p + dts_getsize(o);
  dp = dts_getdata(field);

  while (p < end) {
	  if (*p == '%' && (p+2 < end)) {
		shrink += 2;
		memcpy(hex, p+1, 2);
		if (0 < sscanf(hex, "%2hhx", dp)) {
			p += 3;
			dp++;
		} else {
	  		*dp++ = *p++;
		}
	  } else {
	  	*dp++ = *p++;
	  }
  }

  dts_setsize(field, o->len - shrink);

  return 1;
}


struct dts_field_spec dts_type_bytes_fields[] = {
  { "string",   "string",	smacqtype_bytes_get_string },
  { "bytes",    "urldecode",	smacqtype_bytes_url_decode },
  { END,        NULL }
};

struct dts_type_info dts_type_bytes_table = {
	size: -1
};
