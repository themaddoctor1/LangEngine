#ifndef _EXPVALSET_H_
#define _EXPVALSET_H_

#include "structures.h"
#include "expvals.h"

#define INT_VAL  0

ExpVal intVal(int val);
void disposeIntVal(ExpVal);
ExpVal cloneIntVal(ExpVal);

ExpVal valueof_constExp(Exp, Env);
ExpVal valueof_addExp(Exp, Env);
ExpVal valueof_subtractExp(Exp, Env);
ExpVal valueof_multiplyExp(Exp, Env);
ExpVal valueof_divideExp(Exp, Env);

#endif
