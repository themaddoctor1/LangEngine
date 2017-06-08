#include "test/expression_tests.h"

#include <stdio.h>
#include <stdlib.h>

#include "expressions.h"


int test_constructor1() {

    Exp exp;

    printf("TEST: newBlankExp(void*)\n");
    
    printf("Creating blank Exp with non-null argument...\n");
    exp = newBlankExp((void*) 64);
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
    exp = newBlankExp(NULL);
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

    printf("TEST: buildExp(int, void*)\n");
    
    printf("Building Exp with non-null argument...\n");
    exp = buildExp(3, (void*) 64);
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
    exp = buildExp(0, NULL);
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
    
    printf("TEST: disposeExp(Exp)\n");
    exp = buildExp(7, malloc(sizeof(char)));

    disposeExp(exp);

    return 0;
}

