#include "expvals.h"

#include "environments.h"
#include "expressions.h"
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
        // If the expression fails to parse, it should trivially
        // fail to evaluate.
        return NULL;
    else {
        Env env = init_env();
        ExpVal val = value_of(exp, env);

        // Garbage collection
        dispose_exp(exp);
        dispose_env(env);

        return val;
    }

}


