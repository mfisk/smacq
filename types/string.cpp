#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <dts-module.h>

static int smacqtype_string_get_string(DtsObject o, DtsObject data) {
  // XXX this needlessly makes a copy of the string
  data->setsize(o->getsize()); 
  strcpy((char*)data->getdata(), (char*)o->getdata());
  return 1;
}

static int parse_string(const char* buf,  DtsObject d) {
  d->setsize(strlen(buf)+1);
  strcpy((char*)d->getdata(), buf);
    
  return 1;
}

struct dts_field_spec dts_type_string_fields[] = {
  { "string",   "string",  smacqtype_string_get_string },
  { NULL,        NULL }
};

struct dts_type_info dts_type_string_table = {
  size: -1,
  fromstring: parse_string
};

