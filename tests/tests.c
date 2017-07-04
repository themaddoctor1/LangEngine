#include <stdio.h>
#include <stdlib.h>

#include "expression_tests.h"

#include "parser.h"

/* Collection of test scripts */
int (**tests)() = NULL;

// Suppress warnings from implementation-dependent functions
BnfGrammar generate_bnf_grammar() {return NULL;}
Env init_env() {return NULL;}
ExpVal value_of(Exp exp, Env env) {return NULL;}
void dispose_exp(Exp exp) {}

void initTests() {
    tests = (int (**)()) malloc(4 * sizeof(int (*)()));
    tests[0] = test_constructor1;
    tests[1] = test_constructor2;
    tests[2] = test_destructor;

    tests[3] = NULL;
}


int main(int argc, char *argv[]) {
    
    initTests();

    int choice = argv[1] ? atoi(argv[1]) : -1;
    int res = 0;

    if (choice <= 0) {
        int i;
        for (i = 0; tests[i]; i++) {
            printf("Running test %i...\n", i+1);
            if (tests[i]()) {
                printf("Failed test %i!\n\n", i+1);
                res++;
            } else
                printf("Passed test %i\n\n", i+1);
        }

        printf("Pased %i out of %i tests\n", i-res, i);
    } else {
        printf("Running test %i...\n", choice);
        res =  tests[choice-1]();
        printf(res ? "Failed test %i!\n" : "Passed test %i\n", choice);
    }

    return res;

}

