#ifndef _EXPVALS_H_
#define _EXPVALS_H_

#include "structures.h"

ExpVal build_expval(int, void*);
void dispose_expval(ExpVal);
ExpVal clone_expval(ExpVal);

/**
 * Evaluates an expression.
 */
ExpVal evaluate(char*);

#endif
