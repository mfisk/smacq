#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <dlfcn.h>
#include <smacq.h>
#include <dts-module.h>

#ifdef SMACQ_DEBUG_MEM
# define SMDEBUG(x) x
#else
# define SMDEBUG(x)
#endif

#define HashTemplate(keytype,valtype,fname) \
valtype fname(GHashTable * table, keytype key) { \
    return (valtype)g_hash_table_lookup(table, (gpointer)key);	\
}

static inline HashTemplate(int, struct dts_type *, lookup_type_byint);

DtsObject DTS::construct(dts_typeid type, void * data) {
	DtsObject dobj = newObject(type);
	dobj->setdatacopy(data);
	return dobj;
}

/*
int DTS::getfieldoffset(DtsObject datum, int fnum, int * dtype, int * offset, int * len) {
  struct dts_type * t = type_bynum(datum->gettype());
  struct dts_field_info * d;

  if (!t) return 0;

  *dtype = t->fields[fnum]->type;
  *len = type_bynum(d->type)->info.size;
  *offset = t->fields[fnum]->offset;

  return 1;
}
*/

char * DTS::field_getname(dts_field f) {
  char buf[1024];
  char * name;
  dts_field_element v;
  assert(f);
  v = dts_field_first(f);
  buf[0] = '\0';

  while (1) {
	  if (!v) {
		return strdup(buf);
	  }

	  name = (char*)fields_bynum[v];

	  if (!name) {
  		char elbuf[64];
	  	snprintf(elbuf, 64, "%d", v);
	  	name = elbuf;
	  }

	  strcatn(buf, 1024, name);

	  f = dts_field_next(f);
  	  v = dts_field_first(f);

	  if (v) {
	  	strcatn(buf, 1024, ".");
	  }
  }
}

dts_field_element DTS::requirefield_single(char * name) {
  int t = (int)g_hash_table_lookup(fields_byname, name);
  if (t) {
    return t;
  } else {
    ++max_field;
    g_hash_table_insert(fields_byname, strdup(name), (void*)max_field);
    fields_bynum[max_field] = strdup(name);
    return max_field;
  }
}

int dts_comparefields(dts_field a, dts_field b) {
	while (1) {
		if (!a && !b) return 1;
		if (!a || !b) return 0;
		if (dts_field_first(a) != dts_field_first(b)) return 0;
		a = dts_field_next(a);
		b = dts_field_next(b);
	}
}

dts_field DTS::requirefield(char * name) {
  char * p;
  int i = 0;
  dts_field f = NULL;

  //fprintf(stderr, "Parsing field %s\n", name);
  while (1) {
    f = (dts_field)realloc(f, (i+1) * sizeof(dts_field_element));

    if (!name) {
      //fprintf(stderr, "No more field components\n");
      f[i] = 0;
      return f;
    }
	
    p = strchr(name, '.');
    if (p) {
      p[0] = '\0';
    }
    
    f[i] = requirefield_single(name);
    //fprintf(stderr, "component (%d) %s is %d\n", i, name, f[i]);
    
    if (p) {
      p[0] = '.';
      name = p+1;
    } else {
      name = NULL;
    }
    i++;
  }
}

static int dts_load_module(struct dts_type * t) {
    struct dts_type_info * infop;
    GModule * gmod;

    infop = (dts_type_info*)smacq_find_module(&gmod, "DTS_HOME", "types", "%s/dts_%s", "dts_type_%s_table", t->name);
    if (infop) { 
	t->info = *infop;
    }
    t->description = (dts_field_spec*)smacq_find_module(&gmod, "DTS_HOME", "types", "%s/dts_%s", "dts_type_%s_fields", t->name);

    if (infop || t->description ) {
        return 1;
    } else {
   	fprintf(stderr, "Error: unable to find type %s (Need to set %s?)\n", t->name, "DTS_HOME");
    	return 0;
    }
}

dts_typeid DTS::requiretype(const char * name) {
  struct dts_type * t;

  t = (struct dts_type*)g_hash_table_lookup(types_byname, name);
  if (t)  return(t->num);

  t = g_new0(struct dts_type, 1);
  //darray_init(&t->fields, max_field);
  t->name=strdup(name);
  t->info.size = -1; // Variable

  if (!dts_load_module(t)) {
    fprintf(stderr, "Cannot load type %s\n", t->name);
    return 0;
  }

  t->num = ++max_type;
  
  g_hash_table_insert(types_byname, t->name, t);
  //fprintf(stderr, "added type %s as %d, up from %d\n", t->name, t->num, max_type);
  types[t->num] = t;

  if (t->description) {
    int offset = 0;
    struct dts_field_spec * d = t->description;

    for (d=t->description; (d && d->type != NULL); d++) {
      dts_field_element dnum = requirefield_single(d->name);
      dts_field_info * f = g_new0(struct dts_field_info, 1);
      t->fields[dnum] = f;
      // fprintf(stderr,"\tAdding %s (%s) to %s structure\n", d->name, d->type, t->name);
      f->offset = offset;
      f->type = requiretype(d->type);
      f->elementid = dnum;
      f->desc = *d;
      if (offset >= 0) {
	int size = type_bynum(f->type)->info.size;
	//fprintf(stderr, "\t\tsize of %s(%s) is %d\n", d->name, d->type, size);
	if (d->getfunc) size = -1;

	if (size >= 0) {
	  offset += size;
	} else { // Must be quieried dynamically 
	  f->offset = size;
	  offset = size;
	}
      }
    }
  } else {
    //fprintf(stderr,"Debug: opaque data type: %s\n", name);
  }

  return t->num;
}

int DTS::dts_lt(int type, void * p1, int len1, void * p2, int len2) {
  struct dts_type * t = (struct dts_type*)types[type];
  assert(t->info.lt);
  return t->info.lt(p1, len1, p2, len2);
}

DTS::DTS() : max_type(0), max_field(0) {
  //darray_init(&messages_byfield, max_field);
  //darray_init(&types, max_type);
  //darray_init(&fields_bynum, max_field);

  types_byname = g_hash_table_new(g_str_hash, g_str_equal);
  fields_byname = g_hash_table_new(g_str_hash, g_str_equal);
}

DtsObject DTS::construct_fromstring(dts_typeid type, char * data) {
  DtsObject o = this->newObject(type);

  if (o->set_fromstring(data)) {
    return o;
  } else {
    return NULL;
  }
}

DtsObject DTS::newObject(dts_typeid id) {
  struct dts_type * t = type_bynum(id);
  if (!t) return NULL;
  
  int size = t->info.size;
  if (size < 0) size = 0;

  return newObject(id, size);
}

#ifndef SMACQ_OPT_DTS_FREELIST
#warning "SMACQ_OPT_DTS_FREELIST not set"
DtsObject DTS::newObject(dts_typeid type, int size) {
  DtsObject o = new DtsObject_(this, size, type);
  return o;
}
#else

DtsObject DTS::newObject(dts_typeid type, int size) {
  DtsObject o;
  
  SMDEBUG(DtsObject_virtual_count++);
  
  if (! freelist.empty()) {
    o = freelist.top();
    freelist.pop();

    o->init(size, type);
  } else {
    o = new DtsObject_(this, size, type);
    //fprintf(stderr, "new object refcount = %d\n", o->getrefcount());
  }
  return o;
}
#endif

void DTS::send_message(DtsObject msgo, dts_field_element fieldnum, dts_comparison * comparisons) {
  dts_message * msg = (dts_message*)malloc(sizeof(dts_message));

  msg->field_data = msgo;
  msg->criteria = comparisons;

  dts_message * mlist = (dts_message*)this->messages_byfield[fieldnum];

  if (!mlist) {
    msg->next = NULL;
    this->messages_byfield[fieldnum] = msg;
  } else {
    msg->next = mlist->next;
    mlist->next = msg;
  }
}

DtsObject DTS::msg_check(DtsObject o, dts_field_element field) {
  dts_message * m;
  //fprintf(stderr, "Looking for possible messages\n");
 
  // Look for an unbound message that applies to this datum
  for (m = (dts_message*)messages_byfield[field]; m; m = m->next) {
      if (o->match(m->criteria)) {
	o->attach_field_single(field, m->field_data);

	// If message was ANYCAST, free it now.

	return m->field_data;
      }
  }

  return NULL;
}



