#include <smacq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define __USE_ISOC99 1
#include <math.h>

static inline int type_match_andor(dts_environment * tenv, const dts_object * datum, 
	   dts_comparison * comps, int op);

static inline int compat(const dts_comparison * c) {
	return (c->op1->valueo && c->op2->valueo && (c->op1->valueo->type == c->op2->valueo->type));
}

static inline int eq(dts_environment * tenv, const dts_comparison * c) {
	return 
	    (c->op1->valueo->len == c->op2->valueo->len) && 
	    (!memcmp(c->op1->valueo->data, c->op2->valueo->data, c->op1->valueo->len));
}

static inline int lt(dts_environment * tenv, const dts_comparison * c) {
	//fprintf(stderr, "%g <? %g\n", dts_data_as(c->op1->valueo, double), dts_data_as(c->op2->valueo, double));
	
	return (compat(c) && 
	    (dts_lt(tenv, c->op1->valueo->type, c->op1->valueo->data, c->op1->valueo->len, c->op2->valueo->data, c->op2->valueo->len)));
}

static double eval_arith_operand(const dts_object * datum, struct dts_operand * op) {
  double val1, val2;

  switch (op->type) {
	  case FIELD:
  	  	 fetch_operand(datum->tenv, datum, op, -1);
		 if (op->valueo) {
		 	 assert(op->valueo->type == datum->tenv->double_type);
			 return dts_data_as(op->valueo, double);
		 } else {
			 fprintf(stderr, "Warning: no field %s to eval, using NaN for value\n", op->origin.literal.str);
			 return NAN;
		 };
		 break;

	  case CONST:
  	  	 fetch_operand(datum->tenv, datum, op, datum->tenv->double_type);
		 if (op->valueo) {
			 return dts_data_as(op->valueo, double);
		 } else {
			 return NAN;
		 };
		 break;

	  case ARITH:
  		val1 = eval_arith_operand(datum, op->origin.arith.op1);
  		val2 = eval_arith_operand(datum, op->origin.arith.op2);
  		switch (op->origin.arith.type) {
	  		case ADD:
		  		return val1 + val2;
		  		break;
	  		case SUB:
		  		return val1 - val2;
		  		break;
	  		case DIVIDE:
		  		return val1 / val2;
		  		break;
	  		case MULT:
	  	  		return val1 * val2;
		  		break;
  		}
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
	retval = compat(c) && eq(tenv, c);
	break;

      case NEQ:
	retval = compat(c) && !eq(tenv, c);
	break;

      case LT:
	retval = compat(c) && lt(tenv, c);
	break;

      case GEQ:
	retval = (compat(c) && !lt(tenv, c));
	break;

      case GT:
	retval = (compat(c) && !lt(tenv, c) && !eq(tenv, c));
	break;

      case LEQ:
	retval = compat(c) && (eq(tenv, c) || lt(tenv, c));
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

