#include "langio.h"

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
    printf("const-exp %i\n", *((int*) exp->args[0]));
}

void printVarExp(Exp exp, int tabs) {
    printTabs(tabs, "+-");
    printf("var-exp '%s'\n", (char*) exp->args[0]);
}

void printSumExp(Exp exp, int tabs) {
    Exp exp1 = ((Exp*) exp->args)[0];
    Exp exp2 = ((Exp*) exp->args)[1];

    printTabs(tabs, "+-");
    printf("sum-exp\n");
    printTabs(tabs+1, "|\n");
    
    printExpTabbed(exp1, tabs+1);
    printExpTabbed(exp2, tabs+1);
}

void printDiffExp(Exp exp, int tabs) {
    Exp exp1 = ((Exp*) exp->args)[0];
    Exp exp2 = ((Exp*) exp->args)[1];

    printTabs(tabs, "+-");
    printf("diff-exp\n");
    printTabs(tabs+1, "|\n");
    
    printExpTabbed(exp1, tabs+1);
    printExpTabbed(exp2, tabs+1);
}

void printMultExp(Exp exp, int tabs) {
    Exp exp1 = ((Exp*) exp->args)[0];
    Exp exp2 = ((Exp*) exp->args)[1];

    printTabs(tabs, "+-");
    printf("mult-exp\n");
    printTabs(tabs+1, "|\n");
    
    printExpTabbed(exp1, tabs+1);
    printExpTabbed(exp2, tabs+1);
}

void printLetExp(Exp exp, int tabs) {
    void **args = exp->args;
    
    char *id = (char*) args[0];
    Exp exp1 = (Exp) args[1];
    Exp exp2 = (Exp) args[2];

    printTabs(tabs, "+-");
    printf("let-exp\n");
    printTabs(tabs+1, "|\n");
    
    // Print the variable name.
    printTabs(tabs+1, "+-");
    printf("'%s'\n", id);
    
    printExpTabbed(exp1, tabs+1);
    printExpTabbed(exp2, tabs+1);
}

void printIfExp(Exp exp, int tabs) {
    void **args = exp->args;
    
    Exp cond = (Exp) args[0];
    Exp exp1 = (Exp) args[1];
    Exp exp2 = (Exp) args[2];

    printTabs(tabs, "+-");
    printf("if-exp\n");
    printTabs(tabs+1, "|\n");
     
    printExpTabbed(cond, tabs+1);
    printExpTabbed(exp1, tabs+1);
    printExpTabbed(exp2, tabs+1);
}

void printIsZeroExp(Exp exp, int tabs) {
    void **args = exp->args;
    
    Exp cond = (Exp) args[0];
    
    printTabs(tabs, "+-");
    printf("zero?-exp\n");
    
    printTabs(tabs+1, "|\n");
    printExpTabbed(cond, tabs+1);
}

void printParenthExp(Exp exp, int tabs) {
    void **args = exp->args;
    
    Exp cond = (Exp) args[0];
    
    printTabs(tabs, "+-");
    printf("parenth-exp\n");
    
    printTabs(tabs+1, "|\n");
    printExpTabbed(cond, tabs+1);
}

void (*expPrinters[])(Exp, int) = {
    printConstExp,
    printVarExp,
    printSumExp,
    printDiffExp,
    printMultExp,
    printLetExp,
    printIfExp,
    printIsZeroExp
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
void printBoolVal(ExpVal val, int tabs) {
    printTabs(tabs, "+-");
    printf("bool-val %s\n", *((int*) val->value) ? "true" : "false");
}
void (*expvalPrinters[])(ExpVal, int) = {
    printIntVal,
    printBoolVal
};
void printExpValTabbed(ExpVal val, int tabs) {
    expvalPrinters[val->type](val, tabs);
}
void print_expval(ExpVal val) {
    printExpValTabbed(val, 0);
}



