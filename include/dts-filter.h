#ifndef DTS_FILTER_H
#define DTS_FILTER_H

#include <dts-types.h>

/* 
 * Comparisons & filtering
 */
enum _dts_comp_op { EXIST, EQ, LEQ, GEQ, NEQ, LT, GT, LIKE, AND, OR, FUNC, NOT };
typedef enum _dts_comp_op dts_compare_operation;

enum dts_operand_type { CONST, FIELD, ARITH };
enum dts_arith_operand_type { ADD, SUB, MULT, DIVIDE };

struct dts_operand;

struct dts_arith_operand {
  enum dts_arith_operand_type type;
  struct dts_operand * op1;
  struct dts_operand * op2;
};

struct dts_literal_operand{
  char * str;
  dts_field field;
};

struct dts_operand {
  enum dts_operand_type type;
  DtsObject valueo;

  union { 
    struct dts_arith_operand arith;
    struct dts_literal_operand literal;
  } origin;

};

struct dts_comp_func {
  char * name;
  struct arglist * arglist;
  int argc;
  char ** argv;
};

typedef struct _dts_comparison {
  dts_compare_operation op;

  struct dts_operand * op1, * op2;

  struct _dts_comparison * next;
  struct _dts_comparison * group;
  struct dts_comp_func func;
} dts_comparison;

#endif
