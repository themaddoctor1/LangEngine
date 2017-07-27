#include "expressionset.h"

#include <stdlib.h>
#include <string.h>

Exp constExp(int val) {
    void **args = (void**) malloc(sizeof(void*));
    int *ptr = (int*) malloc(sizeof(int));

    *ptr = val;
    *args = ptr;

    return build_exp(CONST_EXP, args);
}
void disposeConstExp(Exp exp) {
    free(exp->args[0]);
    base_dispose_exp(exp);
}

void disposeOperationExp(Exp exp) {
    dispose_exp(exp->args[0]);
    dispose_exp(exp->args[1]);
    
    // Use the default destructor
    base_dispose_exp(exp);
}

Exp opExp(int type, Exp exp1, Exp exp2) {
    void **args = (void**) malloc(2 * sizeof(void*));
    
    args[0] = exp1;
    args[1] = exp2;

    return build_exp(type, args);
}
Exp addExp(Exp exp1, Exp exp2) {
    return opExp(ADD_EXP, exp1, exp2);
}
Exp subtractExp(Exp exp1, Exp exp2) {
    return opExp(SUBTRACT_EXP, exp1, exp2);
}
Exp multiplyExp(Exp exp1, Exp exp2) {
    return opExp(MULTIPLY_EXP, exp1, exp2);
}
Exp divideExp(Exp exp1, Exp exp2) {
    return opExp(DIVIDE_EXP, exp1, exp2);
}

void (*disposeExps[])(Exp) = {
    disposeConstExp,
    disposeOperationExp,
    disposeOperationExp,
    disposeOperationExp,
    disposeOperationExp
};
void dispose_exp(Exp exp) {
    if (exp) disposeExps[exp->type](exp);
}


