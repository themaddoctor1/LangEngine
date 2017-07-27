#include "expvalset.h"

#include "environmentset.h"
#include "expressions.h"
#include "environments.h"

#include "langio.h"

#include <stdio.h>
#include <stdlib.h>

ExpVal intVal(int val) {
    int *ptr = (int*) malloc(sizeof(int));
    *ptr = val;
    ExpVal result = build_expval(INT_VAL, ptr);

    return result;
}
void disposeIntVal(ExpVal exp) {
    free(exp->value);
    free(exp);
}
ExpVal cloneIntVal(ExpVal val) {
    return intVal(*((int*) val->value));
}

void (*expvalDisposers[])(ExpVal) = {
    disposeIntVal
};
void dispose_expval(ExpVal val) {
    if (val)
        expvalDisposers[val->type](val);
}


ExpVal (*expvalCloners[])(ExpVal) = {
    cloneIntVal
};
ExpVal clone_expval(ExpVal val) {
    if (val)
        return expvalCloners[val->type](val);
    else
        return NULL;
}

ExpVal valueof_constExp(Exp exp, Env env) {
    // The result is simply the stored number.
    return intVal(*((int*) exp->args[0]));
}

ExpVal valueof_addExp(Exp exp, Env env) {

    Exp exp1 = (Exp) exp->args[0];
    Exp exp2 = (Exp) exp->args[1];

    ExpVal val1 = value_of(exp1, env);
    if (!val1)
        return NULL;

    ExpVal val2 = value_of(exp2, env);
    if (!val2) {
        dispose_expval(val1);
        return NULL;
    }

    int res = *((int*) val1->value) + *((int*) val2->value);

    dispose_expval(val1);
    dispose_expval(val2);

    ExpVal result = intVal(res);

    return result;

}

ExpVal valueof_subtractExp(Exp exp, Env env) {
    
    Exp exp1 = (Exp) exp->args[0];
    Exp exp2 = (Exp) exp->args[1];
    
    ExpVal val1 = value_of(exp1, env);
    if (!val1)
        return NULL;
    
    ExpVal val2 = value_of(exp2, env);
    if (!val2) {
        dispose_expval(val1);
        return NULL;
    }

    int res = *((int*) val1->value) - *((int*) val2->value);
    
    dispose_expval(val1);
    dispose_expval(val2);

    return intVal(res);

}

ExpVal valueof_multiplyExp(Exp exp, Env env) {
    Exp exp1 = (Exp) exp->args[0];
    Exp exp2 = (Exp) exp->args[1];

    ExpVal val1 = value_of(exp1, env);
    if (!val1)
        return NULL;

    ExpVal val2 = value_of(exp2, env);
    if (!val2) {
        dispose_expval(val1);
    }

    int res = *((int*) val1->value) * *((int*) val2->value);
    dispose_expval(val1);
    dispose_expval(val2);

    return intVal(res);

}

ExpVal valueof_divideExp(Exp exp, Env env) {
    
    Exp exp1 = (Exp) exp->args[0];
    Exp exp2 = (Exp) exp->args[1];

    ExpVal val1 = value_of(exp1, env);
    if (!val1)
        return NULL;

    ExpVal val2 = value_of(exp2, env);
    if (!val2) {
        dispose_expval(val1);
    }
    
    // Gather the denominator
    int den = *((int*) val2->value);
    int num = *((int*) val1->value);

    // Garbage collection
    dispose_expval(val1);
    dispose_expval(val2);
    
    // Error check the denominator
    if (den) { 
        return intVal(num / den);
    } else {
        printf("ValueError: cannot divide expression by zero\n");
        return NULL;
    }
}

ExpVal (*expEvals[])(Exp, Env) = {
    valueof_constExp,
    valueof_addExp,
    valueof_subtractExp,
    valueof_multiplyExp,
    valueof_divideExp
};
ExpVal value_of(Exp exp, Env env) {
    if (exp)
        return expEvals[exp->type](exp, env);
    else
        return NULL;
}


