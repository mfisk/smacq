#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <dts-module.h>

static int smacqtype_bytes_get_string(DtsObject o, DtsObject field) {
  field->setsize(o->getsize()+1);
  memcpy(field->getdata(), o->getdata(), o->getsize());

  /* Make sure it's NULL terminated */
  memset(field->getdata() + o->getsize(), '\0', 1); 

  return 1;
}

static int smacqtype_bytes_url_decode(DtsObject o, DtsObject field) {
  unsigned char * p, * dp, * end;
  int shrink = 0;
  char hex[3];

  hex[2] = '\0';
  field->setsize(o->getsize());
  p = (unsigned char*)o->getdata();
  end = p + o->getsize();
  dp = (unsigned char*)field->getdata();

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
