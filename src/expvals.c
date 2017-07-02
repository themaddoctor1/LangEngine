#include "expvals.h"

#include "environments.h"

#include <stdlib.h>

ExpVal buildExpVal(int type, void *val) {
    ExpVal expval = (ExpVal) malloc(sizeof(struct expval));
    expval->value = val;
    expval->type = type;
    return expval;
}

ExpVal evaluate(char *prog, BnfGrammar grammar) {
    
    Exp exp = parse(prog, grammar);
    if (!exp)
        return NULL;
    else {
        ExpVal val = value_of(exp, init_env());
        dispose_exp(exp);
        return val;
    }

}


