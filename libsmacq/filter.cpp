#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dts.h>

static inline int compat(const dts_comparison * c) {
	return (c->op1->valueo && c->op2->valueo && 
		(c->op1->valueo->gettype() == c->op2->valueo->gettype()));
}

static inline int eq(DTS * tenv, const dts_comparison * c) {
	return 
	    (c->op1->valueo->getsize() == c->op2->valueo->getsize()) && 
	    (!memcmp(c->op1->valueo->getdata(), c->op2->valueo->getdata(), c->op1->valueo->getsize()));
}

static inline int lt(DTS * tenv, const dts_comparison * c) {
	//fprintf(stderr, "%g <? %g\n", dts_data_as(c->op1->valueo, double), dts_data_as(c->op2->valueo, double));
	
	return (compat(c) && 
	    (tenv->dts_lt(c->op1->valueo->gettype(), c->op1->valueo->getdata(), c->op1->valueo->getsize(), c->op2->valueo->getdata(), c->op2->valueo->getsize())));
}

int DtsObject_::match_one(dts_comparison * c) {
  int retval = 0;

  switch (c->op) {
  case EXIST:
    // Use a non-warning getfield here since by filtering on 
    // the existance of a field, you must believe it won't always
    // be there. 
    this->fetch_operand(c->op1, true);
    break;

  case EQ:
  case NEQ:
  case LT:
  case GT:
  case LEQ:
  case GEQ:

    if (c->op1->type == CONST && c->op2->type == CONST) {
      fetch_const_operand(c->op2, dts->requiretype("string"));
      if (c->op2->valueo)
	fetch_const_operand(c->op1, c->op2->valueo->gettype());
    } else {
  	dts_operand * op1, * op2;
	if (c->op1->type != CONST) {
      		op1 = c->op1;
      		op2 = c->op2;
    	} else {
      		op1 = c->op2;
      		op2 = c->op1;
    	}
	fetch_operand(op1);
	if (op2->type == CONST) {
		fetch_const_operand(op2, op1->valueo->gettype());
	} else {
		fetch_operand(op2);
        	if (op1->valueo && op2->valueo) {
		        dts_typeid op1type = op1->valueo->gettype();
			dts_typeid op2type = op2->valueo->gettype();
			if (op1type != op2type) {
				//fprintf(stderr, "cast to %s to %s\n", op2->valueo->gettypename(), op1->valueo->gettypename());
				DtsObject cast = op2->valueo->getfield(op1->valueo->gettypename());
				if (cast) {
					op2->valueo = cast;
				} else {
					fprintf(stderr, "Warning cannot cast %s to %s; trying reverse\n", op2->valueo->gettypename(), op1->valueo->gettypename());
					op1->valueo = op1->valueo->getfield(op2->valueo->gettypename());
				}
			}
		}
	}
    }
    break;

  case NOT:
  case OR:
  case AND:
  case FUNC:
    break;

  }

  switch (c->op) {
      case EQ:
	retval = compat(c) && eq(dts, c);
	break;

      case NEQ:
	retval = compat(c) && !eq(dts, c);
	break;

      case LT:
	retval = compat(c) && lt(dts, c);
	break;

      case GEQ:
	retval = (compat(c) && !lt(dts, c));
	break;

      case GT:
	retval = (compat(c) && !lt(dts, c) && !eq(dts, c));
	break;

      case LEQ:
	retval = compat(c) && (eq(dts, c) || lt(dts, c));
	break;

      case EXIST:
	retval = (c->op1->valueo != NULL);
	break;

      case NOT:
	retval = !this->match_one(c->group);
	break;

      case AND:
      case OR:
        //fprintf(stderr, "criterion check %s\n", c->op == AND ? "and" : "or");
	retval = this->match_andor(c->group, c->op);
	break;

      case FUNC:
	assert(0);
	break;
  }

  return retval;
}
      

