#ifndef _EXPVALSET_H_
#define _EXPVALSET_H_

#include "structures.h"
#include "expvals.h"

#define INT_VAL  0
#define BOOL_VAL 1

ExpVal intVal(int val);
void disposeIntVal(ExpVal);
ExpVal cloneIntVal(ExpVal);

ExpVal boolVal(int b);
void disposeBoolVal(ExpVal);
ExpVal cloneBoolVal(ExpVal);

/**
 * Evaluators of expressions.
 */
ExpVal valueof_constExp(Exp, Env);
ExpVal valueof_varExp(Exp, Env);
ExpVal valueof_sumExp(Exp, Env);
ExpVal valueof_diffExp(Exp, Env);
ExpVal valueof_multExp(Exp, Env);
ExpVal valueof_letExp(Exp, Env);
ExpVal valueof_ifExp(Exp, Env);
ExpVal valueof_iszeroExp(Exp, Env);
ExpVal valueof_parenthExp(Exp, Env);

#endif
