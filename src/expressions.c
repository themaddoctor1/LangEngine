#include "expressions.h"

#include <string.h>
#include <stdlib.h>

Exp new_blank_exp(void* args) {
    Exp exp = (Exp) malloc(sizeof(struct expression));
    exp->args = args;
    return exp;
}

Exp build_exp(int type, void* args) {
    Exp exp = (Exp) malloc(sizeof(struct expression));
    exp->args = args;
    exp->type = type;
    return exp;
}

void base_dispose_exp(Exp exp) {
    free(exp->args);
    free(exp);
}

