#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <dlfcn.h>
#include <smacq.h>

#define DTS_FREELIST_SIZE 100

char * type_typename_bynum(dts_environment * tenv, int num) {
  assert(tenv);
  assert(dts_type_bynum(tenv, num));
  return dts_type_bynum(tenv, num)->name;
}

int type_typenum_byname(dts_environment * tenv, char * name) {
  struct dts_type * t = g_hash_table_lookup(tenv->types_byname, name);
  if (!t) return -1;
  return t->num;
}

#define HashTemplate(keytype,valtype,fname) \
valtype fname(GHashTable * table, keytype key) { \
   return g_hash_table_lookup(table, (gpointer)key); \
}

static inline HashTemplate(int, struct dts_type *, lookup_type_byint);

dts_object * dts_construct(dts_environment * tenv, int type, void * data) {
	dts_object * dobj;
	struct dts_type * t = dts_type_bynum(tenv, type);
  	if (!t) return NULL;

	assert((t->info.size >=0)  && "Cannot use dts_construct on variable sized types");

	dobj = (dts_object*)dts_alloc(tenv, t->info.size, type);
	memcpy(dts_getdata(dobj), data, t->info.size);
	return dobj;
}

int type_getfieldoffset(dts_environment * tenv, const dts_object * datum, int fnum, int * dtype, int * offset, int * len) {
  struct dts_type * t = dts_type_bynum(tenv, dts_gettype(datum));
  struct smacq_engine_field * d;

  if (!t) return 0;

  d = darray_get(&t->fields, fnum);
  if (!d) return 0;

  *dtype = d->type;
  *len = dts_type_bynum(tenv, d->type)->info.size;

  *offset = d->offset;

  return 1;
}

const dts_object* type_getfield_virtual(dts_environment * tenv, const dts_object * datum, dts_field fieldv, dts_object * scratch) {
  return dts_getfield(tenv, datum, fieldv, scratch);
}

char * dts_field_getname(dts_environment * tenv, dts_field f) {
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

	  name = darray_get(&tenv->fields_bynum, v);

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

int type_fromstring_virtual(dts_environment * tenv, int type, char * datastr, dts_object * data) {
  struct dts_type * t = dts_type_bynum(tenv, type);

  if (!t) return 0;
  assert(t->info.fromstring);

  data->type = type;
  data->free_data = 1;
  data->len = data->max_size;
  return t->info.fromstring(datastr, &data->data, &data->len);
}

static dts_field_element type_requirefield_single(dts_environment * tenv, char * name) {
  int t = (int)g_hash_table_lookup(tenv->fields_byname, name);
  if (t) {
    return t;
  } else {
    ++tenv->max_field;
    g_hash_table_insert(tenv->fields_byname, strdup(name), (void*)tenv->max_field);
    darray_set(&tenv->fields_bynum, tenv->max_field, strdup(name));
    return tenv->max_field;
  }
}

dts_field type_requirefield(dts_environment * tenv, char * name) {
  char * p;
  int i = 0;
  dts_field f = NULL;

  //fprintf(stderr, "Parsing field %s\n", name);
  while (1) {
	f = realloc(f, (i+1) * sizeof(dts_field_element));

	if (!name) {
		//fprintf(stderr, "No more field components\n");
		f[i] = 0;
		return f;
	}
	
	p = strchr(name, '.');
	if (p) {
		p[0] = '\0';
	}

	f[i] = type_requirefield_single(tenv, name);
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

static int type_load_module(struct dts_type * t) {
    struct dts_type_info * infop;
    GModule * gmod;

    infop = smacq_find_module(&gmod, "DTS_HOME", "types", "%s/dts_%s", "dts_type_%s_table", t->name);
    if (infop) { 
	t->info = *infop;
    }
    t->description = smacq_find_module(&gmod, "DTS_HOME", "types", "%s/dts_%s", "dts_type_%s_fields", t->name);

    if (infop || t->description ) {
        return 1;
    } else {
   	fprintf(stderr, "Error: unable to find type %s (Need to set %s?)\n", t->name, "DTS_HOME");
    	return 0;
    }
}


int type_requiretype(dts_environment * tenv, char * name) {
  struct dts_type * t;

  t = g_hash_table_lookup(tenv->types_byname, name);
  if (t)  return(t->num);

  t = g_new0(struct dts_type, 1);
  darray_init(&t->fields, tenv->max_field);
  t->name=strdup(name);
  t->info.size = -1; // Variable

  if (!type_load_module(t)) {
    fprintf(stderr, "Cannot load type %s\n", t->name);
    return 0;
  }

  t->num = ++tenv->max_type;
  
  g_hash_table_insert(tenv->types_byname, t->name, t);
  //fprintf(stderr, "added type %s as %d, up from %d\n", t->name, t->num, tenv->max_type);
  darray_set(&tenv->types, t->num, t);

  if (t->description) {
    int offset = 0;
    struct dts_field_descriptor * d = t->description;

    for (d=t->description; (d && d->type != END); d++) {
      int dnum = type_requirefield_single(tenv, d->name);
      struct smacq_engine_field * f = g_new(struct smacq_engine_field, 1);
      // fprintf(stderr,"\tAdding %s (%s) to %s structure\n", d->name, d->type, t->name);
      f->offset = offset;
      f->type = type_requiretype(tenv, d->type);
      f->desc = *d;
      darray_set(&t->fields, dnum, f);
      if (offset >= 0) {
	int size = dts_type_bynum(tenv, f->type)->info.size;
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

int type_lt_virtual(dts_environment * tenv, int type, void * p1, int len1, void * p2, int len2) {
    struct dts_type * t = darray_get(&tenv->types, type);
    assert(t->info.lt);
    return t->info.lt(p1, len1, p2, len2);
}

dts_environment * dts_init() {
  dts_environment * tenv = g_new0(dts_environment, 1);

  tenv->max_type = 0;
  tenv->max_field = 0;

  darray_init(&tenv->messages_byfield, tenv->max_field);
  darray_init(&tenv->types, tenv->max_type);
  darray_init(&tenv->fields_bynum, tenv->max_field);

  tenv->types_byname = g_hash_table_new(g_str_hash, g_str_equal);
  tenv->fields_byname = g_hash_table_new(g_str_hash, g_str_equal);

  tenv->typenum_byname = type_typenum_byname;
  tenv->typename_bynum = type_typename_bynum;
  tenv->requiretype = type_requiretype;
  tenv->requirefield = type_requirefield;
  tenv->getfield = type_getfield_virtual;
  tenv->fromstring = type_fromstring_virtual;
  tenv->lt = type_lt_virtual;

  tenv->freelist.start = calloc(DTS_FREELIST_SIZE, sizeof(dts_object *));
  tenv->freelist.p = tenv->freelist.start;
  tenv->freelist.end = tenv->freelist.start + DTS_FREELIST_SIZE - 1;

  return tenv;
}

