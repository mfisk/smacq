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

class dts_arith_operand {
 public:
  enum dts_arith_operand_type type;
  struct dts_operand * op1;
  struct dts_operand * op2;
};

class dts_literal_operand {
 public:
  dts_literal_operand() : str(NULL) {}

  char * str;
  DtsField field;
};

class dts_operand {
 public:
  enum dts_operand_type type;
  DtsObject valueo;

  struct { 
    dts_arith_operand arith;
    dts_literal_operand literal;
  } origin;

};

struct dts_comp_func {
  char * name;
  struct arglist * arglist;
  int argc;
  char ** argv;
};

class dts_comparison {
 public:
  dts_comparison() : op1(NULL), op2(NULL), next(NULL), group(NULL) {}

  dts_compare_operation op;

  dts_operand * op1, * op2;

  dts_comparison * next;
  dts_comparison * group;
  dts_comp_func func;
};

#endif
