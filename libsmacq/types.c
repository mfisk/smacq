#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <dlfcn.h>
#include "smacq-internal.h"

struct smacq_engine_field {
  int type;
  struct dts_field_descriptor desc;
  int offset;
};

struct dts_type {
  GHashTable * fields;
  GHashTable * transform_names;
  int num_transforms;
  struct dts_transform_descriptor ** transforms;
  struct smacq_engine_field ** field_cache;
  int max_field_cache;
  char * name;
  int num;
  GModule * module;
  struct dts_field_descriptor * description;
  struct dts_transform_descriptor * transform_description;
  struct dts_type_info info;
};

struct dts_type * dts_type_bynum(dts_environment * tenv, int num) {
  assert(num <= tenv->max_type);
  return tenv->types[num];
}

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

	dobj = (dts_object*)_smacq_alloc(t->info.size, type);
	memcpy(dts_getdata(dobj), data, t->info.size);
	return dobj;
}

int type_getfieldoffset(dts_environment * tenv, const dts_object * datum, int fnum, int * dtype, int * offset, int * len) {
  struct dts_type * t = dts_type_bynum(tenv, dts_gettype(datum));
  struct smacq_engine_field * d;

  if (!t) return 0;

  d = g_hash_table_lookup(t->fields, (void*)fnum);
  if (!d) return 0;

  *dtype = d->type;
  *len = dts_type_bynum(tenv, d->type)->info.size;

  *offset = d->offset;

  return 1;
}

/*
static int field_byname(dts_environment * tenv, char * fname) {
  return ((int)g_hash_table_lookup(tenv->fields_byname, fname));
}
*/

int type_getfield_virtual(dts_environment * tenv, const dts_object * datum, int fnum, dts_object * data) {
  struct dts_type * t = dts_type_bynum(tenv, dts_gettype(datum));
  int offset;
  struct smacq_engine_field * d;

  assert(t);

  if (fnum >= t->max_field_cache) {
    t->field_cache = crealloc(t->field_cache, (fnum+1)*sizeof(void*),
		    	(t->max_field_cache+1) * sizeof(void*));
    t->max_field_cache = fnum;
  }
  d = t->field_cache[fnum];
  if (!d) {
    //XXX: doesn't do negative caching
    d = g_hash_table_lookup(t->fields, (void*)fnum);
    t->field_cache[fnum] = d;
  }
  if (d) {
    data->type = d->type;
    data->len = dts_type_bynum(tenv, d->type)->info.size;
    offset = d->offset;
    
    if (offset >= 0) {
      //fprintf(stderr, "%s offset %d (constant), len = %d\n", name, offset, *len);
      data->data = dts_getdata(datum) + offset;
      return 1;
    } else {
      int retval;
      
      //fprintf(stderr, "offset %d; calling getfunc()%p for %s\n", offset, d->desc.getfunc, name);
      
      assert(d->desc.getfunc);
      retval = d->desc.getfunc(datum, &data->data, &data->len);
      
      assert(data->len >= 0);
      
      return retval;
    }
  } else {
    const dts_object * field_data = msg_check(tenv, datum, fnum, data);
    if (!field_data) return 0;

    *data = *field_data;
    
    return 1;
  }
}

int dts_transform(dts_environment * tenv, char * transform) {
  int t = (int)g_hash_table_lookup(tenv->transform_names, transform);
  if (!t) {
	t = ++tenv->max_transform;
	g_hash_table_insert(tenv->transform_names, strdup(transform), (gpointer)t);
  }
  //fprintf(stderr, "transform %s is %d\n", transform, t-1);
  return t-1;
}

int type_presentdata_virtual(dts_environment * tenv, dts_object * data, int transform, void ** tdata, int * tlen) {
  struct dts_type * t = dts_type_bynum(tenv, data->type);
  struct dts_transform_descriptor * d;

  assert(transform >= 0);

  if (transform >= t->num_transforms) return 0;
  d = t->transforms[transform];

  assert(t->info.size < 0 || data->len == t->info.size);

  assert(d->getfunc);
  return d->getfunc(data->data, data->len, tdata, tlen);
}

int type_fromstring_virtual(dts_environment * tenv, int type, char * datastr, dts_object * data) {
  struct dts_type * t = dts_type_bynum(tenv, type);

  if (!t) return 0;
  assert(t->info.fromstring);

  data->type = type;
  return t->info.fromstring(datastr, &data->data, &data->len);
}

int type_requirefield(dts_environment * tenv, char * name) {
  int t = (int)g_hash_table_lookup(tenv->fields_byname, name);
  if (t) {
    return t;
  } else {
    ++tenv->max_field;
    g_hash_table_insert(tenv->fields_byname, strdup(name), (void*)tenv->max_field);
    return tenv->max_field;
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
    t->transform_description = smacq_find_module(&gmod, "DTS_HOME", "types", "%s/dts_%s", "dts_type_%s_transforms", t->name);

    if (infop || t->description || t->transform_description) {
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
  t->name=strdup(name);
  t->info.size = -1; // Variable

  if (!type_load_module(t)) {
    fprintf(stderr, "Cannot load type %s\n", t->name);
    return 0;
  }

  t->num = ++tenv->max_type;
  t->fields=g_hash_table_new(g_direct_hash, g_direct_equal);
  
  g_hash_table_insert(tenv->types_byname, t->name, t);
  //fprintf(stderr, "added type %s as %d, up from %d\n", t->name, t->num, tenv->max_type);
  tenv->types = crealloc(tenv->types, (t->num+1) * sizeof(void*), tenv->max_type* sizeof(void*));
  tenv->types[t->num] = t;

  if (t->description) {
    int offset = 0;
    struct dts_field_descriptor * d = t->description;

    for (d=t->description; (d && d->type != END); d++) {
      int dnum = type_requirefield(tenv, d->name);
      struct smacq_engine_field * f = g_new(struct smacq_engine_field, 1);
      // fprintf(stderr,"\tAdding %s (%s) to %s structure\n", d->name, d->type, t->name);
      f->offset = offset;
      f->type = type_requiretype(tenv, d->type);
      f->desc = *d;
      g_hash_table_insert(t->fields, (void*)dnum, f);
      if (offset >= 0) {
	int size = dts_type_bynum(tenv, f->type)->info.size;
	//fprintf(stderr, "\t\tsize of %s(%s) is %d\n", d->name, d->type, size);
	if (d->getfunc) size = -1;

	if (size >= 0) {
	  offset += size;
	} else { // Must be quieried dynamically 
	  offset = size;
	}
      }
    }
  } else {
    //fprintf(stderr,"Debug: opaque data type: %s\n", name);
  }

  if (t->transform_description) {
    struct dts_transform_descriptor * d = t->transform_description;

    for (d=t->transform_description; (d && d->name != END); d++) {
      int tnum = dts_transform(tenv, d->name);
      //fprintf(stderr,"\tAdding %s transform to %s type\n", d->name, t->name);
      if (tnum >= t->num_transforms) {
	      t->transforms = crealloc(t->transforms, 
			      	(tnum+1) * sizeof(void*), 
				t->num_transforms * sizeof(void*));
	      t->num_transforms = tnum+1;
      } 
      t->transforms[tnum] = d;
    }
  }

  return t->num;
}

int type_lt_virtual(dts_environment * tenv, int type, void * p1, int len1, void * p2, int len2) {
    struct dts_type * t = tenv->types[type];
    assert(t->info.lt);
    return t->info.lt(p1, len1, p2, len2);
}

dts_environment * dts_init() {
  dts_environment * tenv = g_new0(dts_environment, 1);

  tenv->types_byname = g_hash_table_new(g_str_hash, g_str_equal);
  tenv->fields_byname = g_hash_table_new(g_str_hash, g_str_equal);
  tenv->messages_byfield = g_hash_table_new(g_direct_hash, g_direct_equal);
  tenv->transform_names = g_hash_table_new(g_str_hash, g_str_equal);
  tenv->max_type = 0;
  tenv->max_field = 0;
  tenv->max_transform = 0;

  tenv->typenum_byname = type_typenum_byname;
  tenv->typename_bynum = type_typename_bynum;
  tenv->requiretype = type_requiretype;
  tenv->requirefield = type_requirefield;
  tenv->getfield = type_getfield_virtual;
  tenv->presentdata = type_presentdata_virtual;
  tenv->fromstring = type_fromstring_virtual;
  tenv->lt = type_lt_virtual;

  return tenv;
}

