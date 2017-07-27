#ifndef _EXPRESSIONSET_H_
#define _EXPRESSIONSET_H_

#include "expressions.h"

#define CONST_EXP       0
#define ADD_EXP         1
#define SUBTRACT_EXP    2
#define MULTIPLY_EXP    3
#define DIVIDE_EXP      4

Exp constExp(int);
void disposeConstExp(Exp);

Exp addExp(Exp, Exp);
Exp subtractExp(Exp, Exp);
Exp multiplyExp(Exp, Exp);
Exp divideExp(Exp, Exp);
void disposeOpExp(Exp);

#endif
