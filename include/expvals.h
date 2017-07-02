#ifndef _EXPVALS_H_
#define _EXPVALS_H_

#include "structures.h"
#include "parser.h"

ExpVal buildExpVal(int, void*);
void disposeExpVal(ExpVal);
ExpVal cloneExpVal(ExpVal);

ExpVal evaluate(char*, BnfGrammar);

#endif
