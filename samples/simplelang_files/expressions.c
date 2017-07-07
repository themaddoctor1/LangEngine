#include "expressions.h"
#include "expressionset.h"

#include <stdlib.h>
#include <string.h>

Exp constExp(int x) {
    int **ptr = (int**) malloc(sizeof(int*));
    *ptr = (int*) malloc(sizeof(int));
    **ptr = x;
    return build_exp(CONST_EXP, ptr);
}

Exp varExp(char* args) {
    void** arg = (void**) malloc(sizeof(char*));
    *arg = strdup(args);
    return build_exp(VAR_EXP, arg);
}

Exp sumExp(Exp exp1, Exp exp2) {
    void **args = (void**) malloc(2*sizeof(Exp));
    args[0] = exp1;
    args[1] = exp2;
    return build_exp(SUM_EXP, args);
}


Exp diffExp(Exp exp1, Exp exp2) {
    void **args = (void**) malloc(2*sizeof(Exp));
    args[0] = exp1;
    args[1] = exp2;
    return build_exp(DIFF_EXP, args);
}

Exp multExp(Exp exp1, Exp exp2) {
    void **args = (void**) malloc(2*sizeof(Exp));
    args[0] = exp1;
    args[1] = exp2;
    return build_exp(MULT_EXP, args);
}

Exp letExp(char *id, Exp exp1, Exp exp2) {
    void **args = (void**) malloc(3*sizeof(void*));
    args[0] = strdup(id);
    args[1] = exp1;
    args[2] = exp2;
    return build_exp(LET_EXP, args);
}

Exp ifExp(Exp exp1, Exp exp2, Exp exp3) {
    void **args = (void**) malloc(3*sizeof(void*));
    args[0] = exp1;
    args[1] = exp2;
    args[2] = exp3;
    return build_exp(LET_EXP, args);
}

Exp iszeroExp(Exp exp) {
    void **args = (void**) malloc(sizeof(void*));
    args[0] = exp;
    return build_exp(IS_ZERO_EXP, args);
}

Exp parenthExp(Exp exp) {
    void **args = (void**) malloc(sizeof(void*));
    args[0] = exp;
    return build_exp(PARENTH_EXP, args);
}

void disposeConstExp(Exp exp) {
    free(exp->args[0]);
    base_dispose_exp(exp);
}

void disposeVarExp(Exp exp) {
    disposeConstExp(exp);
}

void disposeSumExp(Exp exp) {
    disposeDiffExp(exp);
}

void disposeMultExp(Exp exp) {
    disposeDiffExp(exp);
}

void disposeDiffExp(Exp exp) {
    Exp *args = (Exp*) exp->args;
    
    // Dispose of the children
    dispose_exp(args[0]);
    dispose_exp(args[1]);
    
    // Use the default destructor
    base_dispose_exp(exp);

}

void disposeLetExp(Exp exp) {
    void** args = exp->args;

    free(args[0]);
    dispose_exp((Exp) args[1]);
    dispose_exp((Exp) args[2]);
    
    // The rest uses default operations
    base_dispose_exp(exp);
}

void disposeIfExp(Exp exp) {
    void **args = exp->args;

    dispose_exp((Exp) args[0]);
    dispose_exp((Exp) args[1]);
    dispose_exp((Exp) args[2]);

    base_dispose_exp(exp);
}

void disposeIszeroExp(Exp exp) {
    void **args = exp->args;

    dispose_exp((Exp) args[0]);

    base_dispose_exp(exp);
}

void disposeParenthExp(Exp exp) {
    void **args = exp->args;

    dispose_exp((Exp) args[0]);

    base_dispose_exp(exp);

}

void (*dispose_exps[])(Exp) = {
    disposeConstExp,
    disposeVarExp,
    disposeSumExp,
    disposeDiffExp,
    disposeMultExp,
    disposeLetExp,
    disposeIfExp,
    disposeIszeroExp,
    disposeParenthExp
};

void dispose_exp(Exp exp) {
    if (exp) dispose_exps[exp->type](exp);
}



