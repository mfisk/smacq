#include <smacq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int type_match_andor(dts_environment * tenv, const dts_object * datum, 
	   dts_comparison * comps, int same_types, int op);

int dts_parsetest (dts_environment * tenv, dts_comparison * comp, 
		    char * test) {
    int offset = strcspn(test,  "=<>!");
    comp->valstr = test+offset+1;

    if (offset == strlen(test)) {
      comp->op = EXIST;

    } else if (test[offset] == '=') {
      comp->op = EQ;

    } else if (!strncmp(test+offset, "!=", 2)) {
      comp->op = NEQ;
      comp->valstr++;
    } else if (test[offset] == '<') {
      comp->op = LT;
    } else if (test[offset] == '>') {
      comp->op = GT;
    }

    test[offset] = '\0';
    comp->field = tenv->requirefield(tenv, test);
    //fprintf(stderr, "Field %s is number %d\n", test, comp->field);

    return 1;
}

static inline int compat(const dts_comparison * c, const dts_object * test_data) {
	return (c->field_data.type == test_data->type);
}

static inline int eq(dts_environment * tenv, const dts_comparison * c, const dts_object * test_data) {
	return (compat(c, test_data) && 
	    (c->field_data.len == test_data->len) && 
	    (!memcmp(c->field_data.data, test_data->data, test_data->len)));
}

static inline int lt(dts_environment * tenv, const dts_comparison * c, const dts_object * test_data) {
	// fprintf(stderr, "%d <? %d: %d\n", *(ushort*)test_data.data, *(ushort*)c->field_data.data, match);
	return (compat(c, test_data) && 
	    (dts_lt(tenv, c->field_data.type, test_data->data, test_data->len, c->field_data.data, c->field_data.len)));
}

int type_match_one(dts_environment * tenv, const dts_object * datum, 
	   dts_comparison * c, int same_types) {
  const dts_object * test_data = NULL;
  int retval = 0;

  if ((c->op != AND) && (c->op != OR)) {
    if (! (test_data = tenv->getfield(tenv, datum, c->field, NULL))) {
	//fprintf(stderr, "Warning: object does not have field number %d\n", c->field);
	return 0;
    }

    if ((!same_types) && c->valstr) {
      /* The types of these fields may have changed, 
	 reinitialize the values we're matching against */

      // fprintf(stderr, "Got field %d bytes %p == %p (%d)\n", len, *(unsigned long*)data, *(unsigned long*)c->data, c->size);

      if ((c->op != EXIST) && (test_data->type != c->field_data.type)) {
	if (c->field_data.free_data) 
		free(c->field_data.data);
	if (! tenv->fromstring(tenv, test_data->type, c->valstr, &c->field_data)) {
	  fprintf(stderr,"Error: value %s is not valid for type %s \n", 
		  c->valstr, tenv->typename_bynum(tenv, test_data->type));
	  assert(0);
	}
      }
    }
  }

      switch (c->op) {
      case EQ:
	retval = eq(tenv, c, test_data);
	break;

      case NEQ:
	retval = !eq(tenv, c, test_data);
	break;

      case LT:
	retval = lt(tenv, c, test_data);
	break;

      case GEQ:
	retval = (compat(c, test_data) && !lt(tenv, c, test_data));
	break;

      case GT:
	retval = (!lt(tenv, c, test_data) && !eq(tenv, c, test_data));
	break;

      case LEQ:
	retval = (eq(tenv, c, test_data) || lt(tenv, c, test_data));
	break;

      case EXIST:
	retval = 1;
	break;

      case LIKE:
	assert("LIKE not supported yet.  Use substr module" && 0);
	break;

      case AND:
      case OR:
        //fprintf(stderr, "criterion check %s\n", c->op == AND ? "and" : "or");
	retval = type_match_andor(tenv, datum, c->group, same_types, c->op);
	break;
      }

    if (test_data) 
    	dts_decref(test_data);

    return retval;
}
      

int type_match_andor(dts_environment * tenv, const dts_object * datum, 
	   dts_comparison * comps, int same_types, int op) {
  dts_comparison * c;

  for (c = comps; c; c = c->next) {
	if (!type_match_one(tenv, datum, c, same_types)) {
		//fprintf(stderr,"no (%d)\n", op);
		if (op == AND) 
		  	return 0;
	} else {
		//fprintf(stderr,"yes (%d)\n", op);
		if (op == OR)
			return 1;
	}
  }

  // fprintf(stderr, "total match\n");
  if (op == AND) 
	  return 1;
  else
	  return 0;
}

int type_match(dts_environment * tenv, const dts_object * datum, 
	   dts_comparison * comps, int same_types) {

  return type_match_andor(tenv, datum, comps, same_types, AND);
}

