#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <dts-module.h>

static int smacqtype_bytes_get_string(DtsObject o, DtsObject field) {
  field->setsize((4*o->getsize())+1);
  
  char * p = (char*)o->getdata();
  char * end = p + o->getsize();
  char * dp = (char*)field->getdata();

  while (p < end) {
     if (*p < 0x20 || *p > 0x7E) { // Not human-readable
	// Hex escape it
	dp += sprintf(dp, "\\x%.2hhx", *p);
	p++;
     } else {
	*dp++ = *p++;
     }
  }

  /* Make sure it's NULL terminated */
  *dp++ = '\0';

  return 1;
}

static int smacqtype_bytes_url_decode(DtsObject o, DtsObject field) {
  unsigned char * p, * dp, * end;
  int shrink = 0;
  char hex[3];

  hex[2] = '\0';
  field->setsize(o->getsize());
  p = o->getdata();
  end = p + o->getsize();
  dp = field->getdata();

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

  field->setsize(o->getsize() - shrink);

  return 1;
}


struct dts_field_spec dts_type_bytes_fields[] = {
  { "string",   "string",	smacqtype_bytes_get_string },
  { "bytes",    "urldecode",	smacqtype_bytes_url_decode },
  { NULL,        NULL }
};

struct dts_type_info dts_type_bytes_table = {
	size: -1
};
