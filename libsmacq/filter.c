#include <smacq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int type_match_andor(dts_environment * tenv, const dts_object * datum, 
	   dts_comparison * comps, int same_types, int op);

int type_parsetest (dts_environment * tenv, dts_comparison * comp, 
		    char * test) {
    int offset = strcspn(test,  "=<>!");
    comp->valstr = test+offset+1;

    if (offset == strlen(test)) {
      comp->op = EXIST;

    } else if (test[offset] == '=') {
      comp->op = EQUALITY;

    } else if (!strncmp(test+offset, "!=", 2)) {
      comp->op = INEQUALITY;
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

int type_match_one(dts_environment * tenv, const dts_object * datum, 
	   dts_comparison * c, int same_types) {
  dts_object test_data;
  int dtype;

  if ((c->op != AND) && (c->op != OR)) {
    if ((!same_types) && c->valstr) {
      /* The types of these fields may have changed, 
	 reinitialize the values we're matching against */

      if (! tenv->getfield(tenv, datum, c->field, &test_data)) 
		return 0;
      dtype = test_data.type;
      // fprintf(stderr, "Got field %d bytes %p == %p (%d)\n", len, *(unsigned long*)data, *(unsigned long*)c->data, c->size);

      if (!dtype) {
	// This datum doesn't even have a field by this name.  give up now
	//fprintf(stderr, "Warning: object does not have field number %d\n", c->field);
	return 0;
      }
      
      if ((c->op != EXIST) && (dtype != c->field_data.type)) {
	free(c->field_data.data); // XXX: Doesn't work for mmapped data.
	if (! tenv->fromstring(tenv, dtype, c->valstr, &c->field_data)) {
	  fprintf(stderr,"Error: value %s is not valid for type %s \n", 
		  c->valstr, tenv->typename_bynum(tenv, dtype));
	  assert(0);
	}
      }
    }
  }

      switch (c->op) {
      case EQUALITY:
	if ((c->field_data.type == test_data.type) && 
	    (c->field_data.len == test_data.len) && 
	    (!memcmp(c->field_data.data, test_data.data, test_data.len)))  
	  return 1;
	break;

      case INEQUALITY:
	if ((c->field_data.type == test_data.type) &&
	    ((c->field_data.len != test_data.len) || (memcmp(c->field_data.data, test_data.data, test_data.len))))  
	  return 1;
	break;

      case LT:
	if ((c->field_data.type == test_data.type) && 
	    (dts_lt(tenv, c->field_data.type, test_data.data, test_data.len, c->field_data.data, c->field_data.len)))  
	  return 1;
	// fprintf(stderr, "%d <? %d: %d\n", *(ushort*)test_data.data, *(ushort*)c->field_data.data, match);
	break;

      case GT:
	if ((c->field_data.type == test_data.type) && 
	    (!dts_lt(tenv, c->field_data.type, test_data.data, test_data.len, c->field_data.data, c->field_data.len)) &&  
	    ((c->field_data.len != test_data.len) || (memcmp(c->field_data.data, test_data.data, test_data.len))))  
	  return 1;
	break;

      case EXIST:
	return 1;
	break;

      case LIKE:
	assert("LIKE not supported yet" && 0);
	break;

      case AND:
      case OR:
        // fprintf(stderr, "criterion check %s\n", c->op == AND ? "and" : "or");
	return type_match_andor(tenv, datum, c->group, same_types, c->op);
	break;
      }

      
    return 0;
}
      

int type_match_andor(dts_environment * tenv, const dts_object * datum, 
	   dts_comparison * comps, int same_types, int op) {
  dts_comparison * c;

  for (c = comps; c; c = c->next) {
	if (!type_match_one(tenv, datum, c, same_types)) {
		// fprintf(stderr,"no\n");
		if (op == AND) 
		  	return 0;
	} else {
		// fprintf(stderr,"yes\n");
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

