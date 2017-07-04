#include "expvals.h"

#include "environments.h"
#include "parser.h"

#include <stdlib.h>

ExpVal build_expval(int type, void *val) {
    ExpVal expval = (ExpVal) malloc(sizeof(struct expval));
    expval->value = val;
    expval->type = type;
    return expval;
}

ExpVal evaluate(char *prog) {
    
    Exp exp = parse(prog);
    if (!exp)
        return NULL;
    else {
        ExpVal val = value_of(exp, init_env());
        dispose_exp(exp);
        return val;
    }

}


