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

int DtsObject::match_one(dts_comparison * c) {
  int retval = 0;

  switch (c->op) {
  case EXIST:
    this->fetch_operand(c->op1, -1);
    break;

  case EQ:
  case NEQ:
  case LT:
  case GT:
  case LEQ:
  case GEQ:

    if (c->op1->type != CONST) {
      this->fetch_operand(c->op1, -1);
      if (c->op1->valueo)
	this->fetch_operand(c->op2, c->op1->valueo->gettype());

    } else if (c->op2->type != CONST) {

      this->fetch_operand(c->op2, -1);
      if (c->op2->valueo)
	this->fetch_operand(c->op1, c->op2->valueo->gettype());

    } else {
    
      this->fetch_operand(c->op2, dts->requiretype("string"));
      if (c->op2->valueo)
	this->fetch_operand(c->op1, c->op2->valueo->gettype());
    }
    
    break;

  case NOT:
  case OR:
  case AND:
  case FUNC:
  case LIKE:
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
	fprintf(stderr, "Error: function tests are unsupported\n");
	break;
      case LIKE:
	fprintf(stderr, "Error: like tests are unsupported\n");
	break;
  }

  return retval;
}
      

