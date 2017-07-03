#ifndef _EXPVALS_H_
#define _EXPVALS_H_

#include "structures.h"
#include "parser.h"

ExpVal build_expval(int, void*);
void dispose_expval(ExpVal);
ExpVal clone_expval(ExpVal);

ExpVal evaluate(char*, BnfGrammar);

#endif
