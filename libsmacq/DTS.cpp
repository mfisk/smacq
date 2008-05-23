#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <dlfcn.h>
#include <smacq.h>
#include <dts-module.h>

#ifdef USE_GASNET
#include <smacq_am.h>
#endif

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

std::string DTS::field_getname(DtsField &f) {
  std::string buf;

  assert(f.size());

  DtsField::iterator v = f.begin();

  while (1) {
	  if (v == f.end()) {
		return buf;
	  } else if (buf[0] != '\0') {
	  	buf += ".";
	  }
		
	  std::string name = fields_bynum[*v];

	  if (!name.length()) {
  		char elbuf[64];
	  	snprintf(elbuf, 64, "%d", *v);
	  	name = elbuf;
	  }

	  buf += name;

  	  ++v;
  }
}

dts_field_element DTS::requirefield_single(const char * name) {
  dts_field_element el = 0;
  if (fields_byname.get(name, el)) {
	return (el);
  } else {
	name = strdup(name);
	int f;
#ifdef USE_GASNET
	if (isProxy) {
		f = 0;
		Gasnet.RequestMedium(0, AM_GETFIELD, name, strlen(name), 1, &f);
		GASNET_BLOCKUNTIL(f);
	} else {
#else
	{
#endif
		f = max_field.increment();
	}
	fields_byname[name] = f;
    	fields_bynum[f] = name;
    	return f;
  }
}

int dts_comparefields(DtsField &fa, DtsField &fb) {
	DtsField::iterator a, b;
	a = fa.begin();
	b = fb.begin();
	
	while (1) {
		if (a == fa.end() && b == fb.end()) return 1;
		if (a == fa.end() || b == fb.end()) return 0;
		if (*a != *b) return 0;

		++a; ++b;
	}
}

DtsField DTS::requirefield(const char * name) {
  char * p;
  int i = 0;
  DtsField f;

  //fprintf(stderr, "Parsing field %s\n", name);
  while (1) {
    if (!name) {
      //fprintf(stderr, "No more field components\n");
      return f;
    }
	
    p = strchr(name, '.');
    if (p) {
      p[0] = '\0';
    }
   
    f.push_back(requirefield_single(name));
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
    lt_dlhandle gmod;

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
  struct dts_type * type = NULL;

  if (types_byname.get(name, type)) {
	return type->num;
  }

  /// Didn't already exist, so do it the hard way
  RecursiveLock l(this);

  struct dts_type *& t = types_byname[name];

  // Else:
  t = new dts_type;

#if USE_GASNET
  if (isProxy) {
	t->num = 0;
	Gasnet.RequestMedium(0, AM_GETTYPE, (void*)name, strlen(name), 1, &t->num);
	GASNET_BLOCKUNTIL(t->num);
  } else {
#else
  {
#endif
  	t->num = max_type.increment();
  }
  types[t->num] = t;
  
  //darray_init(&t->fields, max_field);
  t->name=strdup(name);
  t->info.size = -1; // Variable

  if (!dts_load_module(t)) {
    fprintf(stderr, "Cannot load type %s\n", t->name);
    return 0;
  }

  //fprintf(stderr, "added type %s as %d, up from %d\n", t->name, t->num, max_type);

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
  struct dts_type * t = types[type];
  assert(t->info.lt);
  return t->info.lt(p1, len1, p2, len2);
}

DTS::DTS() : warnings(true), isProxy(false) {
}

DtsObject DTS::construct_fromstring(dts_typeid type, const char * data) {
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
 
  if (freelist.pop(o)) {
    o->init(size, type);

  } else {
    o = new DtsObject_(this, size, type);
    //fprintf(stderr, "new object refcount = %d\n", o->getrefcount());
  }
  return o;
}
#endif

void DTS::send_message(DtsObject msgo, dts_field_element fieldnum, dts_comparison * comparisons) {
  dts_message * msg = new dts_message;

  msg->field_data = msgo;
  msg->criteria = comparisons;

  RecursiveLock l(this);

  dts_message * mlist = (dts_message*)this->messages_byfield[fieldnum];

  if (!mlist) {
    msg->next = NULL;
    messages_byfield[fieldnum] = msg;
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
