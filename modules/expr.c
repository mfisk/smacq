#include <smacq.h>
#include <smacq-parser.h>

#define __USE_ISOC99 1
#include <math.h>

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;

  int argc;
  char ** argv;

  dts_field expr_field;
  dts_field as_field;
  struct dts_operand * expr;

  int double_type;
};

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
				fprintf(stderr, "%g + %g\n", val1, val2);
		  		return val1 + val2;
		  		break;
	  		case SUB:
				fprintf(stderr, "%g - %g\n", val1, val2);
		  		return val1 - val2;
		  		break;
	  		case DIVIDE:
				fprintf(stderr, "%g / %g\n", val1, val2);
		  		return val1 / val2;
		  		break;
	  		case MULT:
				fprintf(stderr, "%g * %g\n", val1, val2);
	  	  		return val1 * val2;
		  		break;
  		}
		break;
  }
  return NAN;
}

static smacq_result expr_init(struct smacq_init * context) {
  struct state * state;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->env = context->env;
  {
    struct smacq_optval optvals[] = {
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &state->argc, &state->argv,
				 options, optvals);
    assert(state->argv);
  }

  state->double_type = smacq_requiretype(state->env, "double");

  if ((state->argc > 2) && (!strcmp(state->argv[state->argc-2], "as"))) {
    fprintf(stderr, "expr got as %s\n", state->argv[state->argc-1]);
    state->as_field = smacq_requirefield(state->env, state->argv[state->argc-1]);
    assert(state->as_field);
    state->argc -= 2;
  }

  state->expr = dts_parse_expr(state->env->types, state->argc, state->argv);
  if (!state->expr) 
	  return SMACQ_ERROR|SMACQ_END;

  {
    char * expr_str = "expr";
      //expression2fieldname(state->expr);
    state->expr_field = smacq_requirefield(state->env, expr_str);
    assert(state->expr_field);
    //free(expr_str);
  }

  return 0; 
}

static smacq_result expr_consume(struct state * state, const dts_object * datum, int * outchan) {
  dts_object * msgdata;
  double val;

  assert(datum);

  val = eval_arith_operand(datum, state->expr);

  msgdata = smacq_dts_construct(state->env, state->double_type, &val);

  //dts_data_as(msgdata, double) = eval_arith_operand(datum, op);

  dts_attach_field(datum, state->expr_field, msgdata); 

  if (state->as_field) {
    dts_attach_field(datum, state->as_field, msgdata); 
    dts_incref(msgdata, 1);
  }

  return SMACQ_PASS;
}


/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_expr_table = {
  consume: &expr_consume,
  init: &expr_init
};
