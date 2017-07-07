#include "expvalset.h"

#include "environmentset.h"
#include "expressions.h"
#include "environments.h"

#include <stdio.h>
#include <stdlib.h>

ExpVal intVal(int val) {
    int *ptr = (int*) malloc(sizeof(int));
    *ptr = val;
    return build_expval(INT_VAL, ptr);
}
void disposeIntVal(ExpVal exp) {
    free(exp->value);
    free(exp);
}
ExpVal cloneIntVal(ExpVal val) {
    return intVal(*((int*) val->value));
}


ExpVal boolVal(int b) {
    int *ptr = (int*) malloc(sizeof(int));
    *ptr = b;
    return build_expval(BOOL_VAL, ptr);
}
void disposeBoolVal(ExpVal exp) {
    free(exp->value);
    free(exp);
}
ExpVal cloneBoolVal(ExpVal val) {
    return boolVal(*((int*) val->value));
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

ExpVal valueof_varExp(Exp exp, Env env) {
    // The result is stored in the environment; get it.
    return apply_env(env, (char*) exp->args[0]);
}

ExpVal valueof_sumExp(Exp exp, Env env) {
    // Get the two values, and compute their difference.
    Exp exp1 = (Exp) ((void**) exp->args)[0];
    Exp exp2 = (Exp) ((void**) exp->args)[1];
    
    ExpVal val1 = value_of(exp1, env);
    ExpVal val2 = value_of(exp2, env);

    if (!val1 || !val2)
        return NULL; // One of the two values returned an error
    else if (val1->type != INT_VAL) {
        printf("ValueError: expected left operand to be int-val\n");
        return NULL;
    } else if (val1->type != INT_VAL) {
        printf("ValueError: expected right operand to be int-val\n");
        return NULL;
    } else {
        // Garbage collect, then return result
        int a = *((int*) val1->value);
        int b = *((int*) val2->value);
        
        disposeIntVal(val1);
        disposeIntVal(val2);

        return intVal(a + b);

    }
}

ExpVal valueof_diffExp(Exp exp, Env env) {
    // Get the two values, and compute their difference.
    Exp exp1 = (Exp) ((void**) exp->args)[0];
    Exp exp2 = (Exp) ((void**) exp->args)[1];
    
    ExpVal val1 = value_of(exp1, env);
    ExpVal val2 = value_of(exp2, env);

    if (!val1 || !val2)
        return NULL; // One of the two values returned an error
    else if (val1->type != INT_VAL) {
        printf("ValueError: expected left operand to be int-val\n");
        return NULL;
    } else if (val1->type != INT_VAL) {
        printf("ValueError: expected right operand to be int-val\n");
        return NULL;
    } else {
        // Garbage collect, then return result
        int a = *((int*) val1->value);
        int b = *((int*) val2->value);
        
        disposeIntVal(val1);
        disposeIntVal(val2);

        return intVal(a - b);

    }
}

ExpVal valueof_multExp(Exp exp, Env env) {
    // Get the two values, and compute their difference.
    Exp exp1 = (Exp) ((void**) exp->args)[0];
    Exp exp2 = (Exp) ((void**) exp->args)[1];
    
    ExpVal val1 = value_of(exp1, env);
    ExpVal val2 = value_of(exp2, env);

    if (!val1 || !val2)
        return NULL; // One of the two values returned an error
    else if (val1->type != INT_VAL) {
        printf("ValueError: expected left operand to be int-val\n");
        return NULL;
    } else if (val1->type != INT_VAL) {
        printf("ValueError: expected right operand to be int-val\n");
        return NULL;
    } else {
        // Garbage collect, then return result
        int a = *((int*) val1->value);
        int b = *((int*) val2->value);
        
        disposeIntVal(val1);
        disposeIntVal(val2);

        return intVal(a * b);

    }
}

ExpVal valueof_letExp(Exp exp, Env env) {
    // Parsing let X = Y in Z
    void **args = (void**) exp->args;
    char *X = (char*) args[0];
    Exp Y = (Exp) args[1];
    Exp Z = (Exp) args[2];
    
    // We let X = Y
    Env newEnv = extendEnv(X, value_of(Y, env), env);
    
    // Now, find Z
    ExpVal result = value_of(Z, newEnv);

    //Garbage collection
    newEnv->subenv = NULL;
    disposeExtendEnv(newEnv);

    return result;
}

ExpVal valueof_ifExp(Exp exp, Env env) {
    
    void **args = (void**) exp->args;

    Exp condExp = (Exp) args[0];
    
    ExpVal cond = value_of(condExp, env);
    
    if (cond) {
        ExpVal res = NULL;

        if (cond->type != BOOL_VAL)
            printf("ValueError: expected conditional to be bool-val\n");
        else if (*((int*) cond->value))
            res = value_of((Exp) args[1], env);
        else
            res = value_of((Exp) args[2], env);
        
        // Garbage collection
        dispose_expval(cond);

        return res;

    } else
        return NULL;
}

ExpVal valueof_iszeroExp(Exp exp, Env env) {
    
    Exp exp1 = (Exp) exp->args[0];
    
    // The value to compare against zero.
    ExpVal val1 = value_of(exp1, env);
    
    if (val1) {
        ExpVal res;

        if (val1->type == INT_VAL)
            res = boolVal(*((int*) val1->value) == 0);
        else
            res = NULL;
            
        // Garbage collection
        dispose_expval(val1);
        
        return res;

    } else
        return NULL;
}

ExpVal valueof_parenthExp(Exp exp, Env env) {
    // The result is equal to the value of the contents.
    Exp exp1 = (Exp) exp->args[0];

    return value_of(exp1, env);

}

ExpEvaluator expEvaluators[] = {
    valueof_constExp,
    valueof_varExp,
    valueof_sumExp,
    valueof_diffExp,
    valueof_multExp,
    valueof_letExp,
    valueof_ifExp,
    valueof_iszeroExp,
    valueof_parenthExp
};
ExpVal value_of(Exp exp, Env env) {
    return exp ? expEvaluators[exp->type](exp, env) : NULL;
}

