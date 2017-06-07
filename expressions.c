#include "expressions.h"

#include <string.h>

Exp newBlankExp(void* args) {
    Exp exp = (Exp) malloc(sizeof(struct expression));
    exp->args = args;
    return exp;
}

Exp buildBlankExp(char* type, void* args) {
    Exp exp = (Exp) malloc(sizeof(struct expression));
    exp->args = args;
    strcpy(exp->type, type);
}

void disposeExp(Exp exp) {
    free(exp->args);
    free(exp);
}

