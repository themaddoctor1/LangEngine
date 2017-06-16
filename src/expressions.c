#include "include/expressions.h"

#include <string.h>
#include <stdlib.h>

Exp newBlankExp(void* args) {
    Exp exp = (Exp) malloc(sizeof(struct expression));
    exp->args = args;
    return exp;
}

Exp buildExp(int type, void* args) {
    Exp exp = (Exp) malloc(sizeof(struct expression));
    exp->args = args;
    exp->type = type;
    return exp;
}

void disposeExp(Exp exp) {
    free(exp->args);
    free(exp);
}

