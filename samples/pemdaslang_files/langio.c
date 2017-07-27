#include "langio.h"

#include "expressionset.h"

#include <stdio.h>

void printExpTabbed(Exp, int);
void printEnvTabbed(Env, int);
void printExpValTabbed(ExpVal, int);


void printTabs(int tabs, char *tip) {
    int i = tabs;
    while (--i > 0)
        printf(" |");
    if (tabs)
        printf(" %s", tip);
}

void printConstExp(Exp exp, int tabs) {
    printTabs(tabs, "+-");
    printf("const-exp %i\n", ((int*) exp->args[0])[0]);
}

void printOpExp(Exp exp, int tabs) {

    printTabs(tabs, "+-");
    printf(
            exp->type == ADD_EXP
            ? "add-exp"
        :   exp->type == SUBTRACT_EXP
            ? "subtract-exp"
        :   exp->type == MULTIPLY_EXP
            ? "multiply-exp"
        :   "divide-exp");
    printf("\n");
    printTabs(tabs+1, "|\n");

    Exp exp1 = (Exp) exp->args[0];
    printExpTabbed(exp1, tabs+1);
    Exp exp2 = (Exp) exp->args[1];
    printExpTabbed(exp2, tabs+1);

}

void (*expPrinters[])(Exp, int) = {
    printConstExp,
    printOpExp,
    printOpExp,
    printOpExp,
    printOpExp
};
void printExpTabbed(Exp exp, int tabs) {
    expPrinters[exp->type](exp, tabs);
}
void print_exp(Exp exp) {
    printExpTabbed(exp, 0);
}


void printEmptyEnv(Env env, int tabs) {
    printTabs(tabs, "+-");
    printf("empty-env\n");
}

void printExtendEnv(Env env, int tabs) {
    void **args = (void**) env->args;

    printTabs(tabs, "+-");
    printf("extend-env\n");

    printTabs(tabs+1, "|\n");

    // Variable name
    printTabs(tabs+1, "+-");
    printf("'%s'\n", (char*) args[0]);
    
    printExpValTabbed((ExpVal) args[1], tabs+1);
    printEnvTabbed(env->subenv, tabs+1);
}

void (*envPrinters[])(Env, int) = {
    printEmptyEnv,
    printExtendEnv
};
void printEnvTabbed(Env env, int tabs) {
    envPrinters[env->type](env, tabs);
}
void print_env(Env env) {
    printEnvTabbed(env, 0);
}


void printIntVal(ExpVal val, int tabs) {
    printTabs(tabs, "+-");
    printf("int-val %i\n", *((int*) val->value));
}
void (*expvalPrinters[])(ExpVal, int) = {
    printIntVal
};
void printExpValTabbed(ExpVal val, int tabs) {
    expvalPrinters[val->type](val, tabs);
}
void print_expval(ExpVal val) {
    printExpValTabbed(val, 0);
}



