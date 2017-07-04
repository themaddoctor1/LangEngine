#include "expression_tests.h"

#include <stdio.h>
#include <stdlib.h>

#include "expressions.h"


int test_constructor1() {

    Exp exp;

    printf("TEST: new_blank_exp(void*)\n");
    
    printf("Creating blank Exp with non-null argument...\n");
    exp = new_blank_exp((void*) 64);
    if (!exp) {
        printf("Failed to create Exp!\n");
        return 1;
    } else if (exp->args != (void*) 64) {
        printf("Failed to properly allocate Exp!");
        return 1;
    } else {
        printf("Created Exp with non-null argument.\n");
    }

    printf("Creating blank Exp with null argument...\n");
    exp = new_blank_exp(NULL);
    if (!exp) {
        printf("Failed to create Exp!\n");
        return 1;
    } else if (exp->args) {
        printf("Failed to properly allocate Exp!\n");
        return 1;
    } else {
        printf("Created Exp with null argument.\n");
    }
    
    
    return 0;
}


int test_constructor2() {

    Exp exp;

    printf("TEST: build_exp(int, void*)\n");
    
    printf("Building Exp with non-null argument...\n");
    exp = build_exp(3, (void*) 64);
    if (!exp) {
        printf("Failed to create Exp!\n");
        return 1;
    } else if (exp->args != (void*) 64) {
        printf("Failed to properly allocate Exp!\n");
        return 1;
    } else {
        printf("Created Exp with non-null argument.\n");
    }

    printf("Building Exp with null argument...\n");
    exp = build_exp(0, NULL);
    if (!exp) {
        printf("Failed to create Exp!\n");
        return 1;
    } else if (exp->args) {
        printf("Failed to properly allocate Exp!\n");
        return 1;
    } else {
        printf("Created Exp with null argument.\n");
    }
    
    
    return 0;
}

int test_destructor() {
    Exp exp;
    
    printf("TEST: dispose_exp(Exp)\n");
    exp = build_exp(7, malloc(sizeof(char)));

    dispose_exp(exp);

    return 0;
}

