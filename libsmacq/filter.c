#include <smacq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static inline int type_match_andor(dts_environment * tenv, const dts_object * datum, 
	   dts_comparison * comps, int op);

#if 0
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
#endif

static inline int compat(const dts_comparison * c) {
	return (c->op1->valueo && c->op2->valueo && (c->op1->valueo->type == c->op2->valueo->type));
}

static inline int eq(dts_environment * tenv, const dts_comparison * c) {
	return (compat(c) && 
	    (c->op1->valueo->len == c->op2->valueo->len) && 
	    (!memcmp(c->op1->valueo->data, c->op2->valueo->data, c->op1->valueo->len)));
}

static inline int lt(dts_environment * tenv, const dts_comparison * c) {
	// fprintf(stderr, "%d <? %d: %d\n", *(ushort*)test_data.data, *(ushort*)c->field_data->data, match);
	return (compat(c) && 
	    (dts_lt(tenv, c->op1->valueo->type, c->op1->valueo->data, c->op1->valueo->len, c->op2->valueo->data, c->op2->valueo->len)));
}

static inline fetch_operand(dts_environment * tenv, const dts_object * datum, 
	   struct dts_operand * op, int const_type) {

  if (op->type == CONST && op->valueo && op->valueo->type == const_type) {
	  return; 
  }

  if (op->valueo) dts_decref(op->valueo);

  switch(op->type) {
	case FIELD: 
	  op->valueo = dts_getfield(tenv, datum, op->field);
	  //if (!op->valueo) fprintf(stderr, "Field %s not found in obj %p\n", op->str, datum);
	  break;

	case CONST: 
	  op->valueo = dts_construct_fromstring(tenv, const_type, op->str); 
	  if (!op->valueo) fprintf(stderr, "Could not parse %s\n", op->str);
	  break;
  }
}

static inline int type_match_one(dts_environment * tenv, const dts_object * datum, 
	   dts_comparison * c) {
  const dts_object * test_data = NULL;
  int retval = 0;

  switch (c->op) {
	case EXIST:
  		fetch_operand(tenv, datum, c->op1, -1);
		break;
		
	case EQ:
	case NEQ:
	case LT:
	case GT:
	case LEQ:
	case GEQ:
	  	if (c->op1->type != CONST) {
  			fetch_operand(tenv, datum, c->op1, -1);
			if (c->op1->valueo)
			  fetch_operand(tenv, datum, c->op2, c->op1->valueo->type);
		} else if (c->op2->type != CONST) {
  			fetch_operand(tenv, datum, c->op2, -1);
			if (c->op2->valueo)
			  fetch_operand(tenv, datum, c->op1, c->op2->valueo->type);
		} else {
  			fetch_operand(tenv, datum, c->op2, dts_requiretype(tenv, "string"));
			if (c->op2->valueo)
			  fetch_operand(tenv, datum, c->op1, c->op2->valueo->type);
		}

		break;

	case OR:
	case AND:
	case FUNC:
		break;
  }

  switch (c->op) {
      case EQ:
	retval = eq(tenv, c);
	break;

      case NEQ:
	retval = !eq(tenv, c);
	break;

      case LT:
	retval = lt(tenv, c);
	break;

      case GEQ:
	retval = (compat(c) && !lt(tenv, c));
	break;

      case GT:
	retval = (!lt(tenv, c) && !eq(tenv, c));
	break;

      case LEQ:
	retval = (eq(tenv, c) || lt(tenv, c));
	break;

      case EXIST:
	retval = (c->op1->valueo != NULL);
	break;

      case AND:
      case OR:
        //fprintf(stderr, "criterion check %s\n", c->op == AND ? "and" : "or");
	retval = type_match_andor(tenv, datum, c->group, c->op);
	break;

      case FUNC:
	fprintf(stderr, "Error: function tests are unsupported\n");
	break;
  }

  return retval;
}
      

static inline int type_match_andor(dts_environment * tenv, const dts_object * datum, 
	   dts_comparison * comps, int op) {
  dts_comparison * c;

  for (c = comps; c; c = c->next) {
	if (!type_match_one(tenv, datum, c)) {
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
	   dts_comparison * comps) {

  return type_match_andor(tenv, datum, comps, AND);
}

