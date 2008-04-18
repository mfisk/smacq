#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <dts-module.h>

static int parse_string(const char* buf,  DtsObject d) {
  d->setsize(strlen(buf));
  memcpy((char*)d->getdata(), buf, d->getsize());
    
  return 1;
}

static int smacqtype_string_get_escaped(DtsObject o, DtsObject field) {
  field->setsize((4 * o->getsize()) +1); // Worst case is all hex plus a NULL terminator
  
  char * p = (char*)o->getdata();
  char * end = p + o->getsize();
  char * dp = (char*)field->getdata();

  while (p < end) {
     if (*p < 0x20 || *p > 0x7E || *p == '\\') { // Not human-readable
	// Hex escape it
	dp += sprintf(dp, "\\x%.2hhx", *p);
	p++;
     } else {
	*dp++ = *p++;
     }
  }

  /* Make sure it's NULL terminated */
  field->setsize(dp - (char*)field->getdata());

  return 1;
}

static int smacqtype_string_get_hexdecode(DtsObject o, DtsObject field) {
  unsigned char * p, * dp, * end;
  int shrink = 0;

  field->setsize(o->getsize());
  p = o->getdata();
  end = p + o->getsize();
  dp = field->getdata();

  if ( (o->getsize() % 2) != 0) {
	fprintf(stderr, "Odd length hex string.\n");
  }

  while (p < end) {
	if (0 < sscanf((const char*)p, "%2hhx", dp)) {
		p += 2;
		dp++;
		shrink += 1;
	} else {
		fprintf(stderr, "Invalid hex data in string.\n");
  		*dp++ = *p++;
	}
  }

  field->setsize(o->getsize() - shrink);

  return 1;
}
static int smacqtype_string_get_urldecode(DtsObject o, DtsObject field) {
  unsigned char * p, * dp, * end;
  int shrink = 0;

  field->setsize(o->getsize());
  p = o->getdata();
  end = p + o->getsize();
  dp = field->getdata();

  while (p < end) {
	  if (*p == '%' && (p+2 < end)) {
		shrink += 2;
		if (0 < sscanf((const char*)p+1, "%2hhx", dp)) {
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

struct dts_field_spec dts_type_string_fields[] = {
  { "string",   "urldecode", smacqtype_string_get_urldecode },
  { "string",   "hexdecode", smacqtype_string_get_hexdecode },
  { "string",   "string", smacqtype_string_get_escaped },
  { NULL,        NULL }
};

struct dts_type_info dts_type_string_table = {
  size: -1,
  fromstring: parse_string
};


