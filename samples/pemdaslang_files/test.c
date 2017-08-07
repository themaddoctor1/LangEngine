#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>

#include "expvalset.h"
#include "expressionset.h"
#include "environmentset.h"

#include "langio.h"
#include "parser.h"

// For color formatting
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BOLD    "\x1b[1m"
#define ANSI_COLOR_ULINE   "\x1b[4m"

int expcmp(Exp exp1, Exp exp2) {
    if (!exp1 || !exp2)
        // If one is null, but not the other
        return exp1 != exp2;
    else if (exp1->type != exp2->type)
        // If their formats do not match
        return 1;
    else {
        int type = exp1->type;
        if (type == CONST_EXP)
            return *((int*) exp1->args[0]) != *((int*) exp2->args[0]);
        else {
            int i;
            for (i = 0; exp1->args[i] && exp2->args[i]; i++) {
                Exp e1 = exp1->args[i];
                Exp e2 = exp2->args[i];
                int n = expcmp(e1, e2);
                if (n) return n;
            }

            return 0;
        }
    }
}


int full_test(char *str, Exp expExp, ExpVal expVal, Env env) {

    printf("TEST: evaluate('%s')\n", str);
    printf("EXPECTED EXPRESSION:\n");
    if (expExp) print_exp(expExp); else printf("NULL\n");

    printf("EXPECTED EXPVAL:\n");
    if (expVal) print_expval(expVal); else printf("NULL\n");

    if (env) {
        printf("GIVEN ENVIRONMENT:\n");
        print_env(env);
    }

    printf("\n");

    Exp resExp = parse(str);

    if (expcmp(expExp, resExp)) {
        printf("ACTUAL EXP:\n");
        if (resExp) print_exp(resExp); else printf("NULL\n");
        
        printf(ANSI_COLOR_RED "Parsing failed!" ANSI_COLOR_RESET "\n");

        if (expExp) dispose_exp(expExp);
        if (resExp) dispose_exp(resExp);
        if (env) dispose_env(env);
        if (expVal) dispose_expval(expVal);

        return 1;
    } else
        if (resExp) dispose_exp(resExp);
    
    if (!env)
        env = emptyEnv();
    ExpVal resVal = value_of(expExp, env);

    dispose_env(env);
    dispose_exp(expExp);

    if (expVal && !resVal) {
        printf("EXPECTED:\n");
        print_expval(expVal);
        printf("ACTUAL EXPVAL:\nNULL\n");
        
        printf(ANSI_COLOR_RED "Evaluation failed!\n\n" ANSI_COLOR_RESET);
        
        dispose_expval(expVal);
        
        return 1;
        
    } else if (!expVal && resVal) {
        printf("EXPECTED:\nNULL\n");
        printf("ACTUAL EXPVAL:\n");
        print_expval(resVal);
        
        printf(ANSI_COLOR_RED "Evaluation failed!\n\n" ANSI_COLOR_RESET);

        dispose_expval(resVal);
        
        return 1;

    } else if (!expVal) {
        printf(ANSI_COLOR_GREEN "Correctly fails to evaluate\n\n" ANSI_COLOR_RESET);

        return 0;
    } else if (expVal->type != resVal->type) {

        printf("ACTUAL EXPVAL:\n");
        print_expval(resVal);
        
        printf(ANSI_COLOR_RED "Evaluation returned value of wrong type\n\n" ANSI_COLOR_RESET);

        dispose_expval(expVal);
        dispose_expval(resVal);

        return 1;

    } else if (*((int*) expVal->value) != *((int*) resVal->value)) {

        printf("EXPECTED:\n");
        print_expval(expVal);
        printf("ACTUAL EXPVAL:\n");
        print_expval(resVal);
        
        printf(ANSI_COLOR_RED "Evaluation returned incorrect value\n\n" ANSI_COLOR_RESET);

        dispose_expval(expVal);
        dispose_expval(resVal);

        return 1;

    } else {
        printf(ANSI_COLOR_GREEN "Evaluation returned correct answer\n\n" ANSI_COLOR_RESET);

        dispose_expval(expVal);
        dispose_expval(resVal);

        return 0;
    }


}


int test1() {
    printf(ANSI_COLOR_BOLD ANSI_COLOR_ULINE "CONST STATEMENTS TEST" ANSI_COLOR_RESET "\n");

    return 
        full_test(
            "2", 
            constExp(2), 
            intVal(2), 
            NULL)
    |   full_test(
            "    67    ",
            constExp(67),
            intVal(67),
            emptyEnv())
    |   full_test(
            "22dhsfkjd",
            NULL,
            NULL,
            NULL)
    ;
}

int test2() {
    printf(ANSI_COLOR_BOLD ANSI_COLOR_ULINE "ADDITIVE STATEMENTS TEST" ANSI_COLOR_RESET "\n");

    return 
        full_test(
            "2+2", 
            addExp(
                constExp(2),
                constExp(2)
            ), 
            intVal(4), 
            NULL)
    |   full_test(
            "17-2+5",
            addExp(
                subtractExp(
                    constExp(17),
                    constExp(2)
                ),
                constExp(5)
            ),
            intVal(20),
            NULL)
    |   full_test(
            "2+",
            NULL,
            NULL,
            NULL)
    ;
}

int test3() {
    printf(ANSI_COLOR_BOLD ANSI_COLOR_ULINE "MULTIPLICATIVE STATEMENTS TEST" ANSI_COLOR_RESET "\n");

    return 
        full_test(
            "2*3", 
            multiplyExp(
                constExp(2),
                constExp(3)
            ), 
            intVal(6), 
            NULL)
    |   full_test(
            "13 * 4 / 5",
            divideExp(
                multiplyExp(
                    constExp(13),
                    constExp(4)
                ),
                constExp(5)
            ),
            intVal(10),
            NULL)
    |   full_test(
            "1/0",
            divideExp(
                constExp(1),
                constExp(0)
            ),
            NULL,
            NULL)
    ;
}

int test4() {
    printf(ANSI_COLOR_BOLD ANSI_COLOR_ULINE "PARENTHETICAL STATEMENTS TEST" ANSI_COLOR_RESET "\n");

    return 
        full_test(
            "(2*3)", 
            multiplyExp(
                constExp(2),
                constExp(3)
            ), 
            intVal(6), 
            NULL)
    |   full_test(
            "(((((42)))))",
            constExp(42),
            intVal(42),
            NULL)
    |   full_test(
            "6 / (3 * 2)",
            divideExp(
                constExp(6),
                multiplyExp(
                    constExp(3),
                    constExp(2)
                )
            ),
            intVal(1),
            NULL)
    ;
}

int test5() {
    printf(ANSI_COLOR_BOLD ANSI_COLOR_ULINE "OPERATION ORDERING TEST" ANSI_COLOR_RESET "\n");

    return 
        full_test(
            "(4 * 4 + 2) - (2 + 4 * 4)",
            subtractExp(
                addExp(
                    multiplyExp(
                        constExp(4),
                        constExp(4)
                    ),
                    constExp(2)
                ),
                addExp(
                    constExp(2),
                    multiplyExp(
                        constExp(4),
                        constExp(4)
                    )
                )
            ), 
            intVal(0), 
            NULL)
    ;
}

/* Collection of test scripts */
int (*tests[])() = {
    test1,
    test2,
    test3,
    test4,
    test5,
    NULL
};

int test(int choice) {

    int res = 0;

    if (choice <= 0) {
        int i;
        for (i = 0; tests[i]; i++) {
            int n, pid;
            printf("Running test %i...\n", i+1);
            
            pid = fork();
            if (pid > 0)
                // Parent process
                waitpid(pid, &n, 0);
            else if (!pid) {
                // Child process will only run the test, then exit with its error code.
                exit(tests[i]());
            } else {
                // Could not run the test
                printf("error: could not create test subprocess!\n\n");
                exit(1);
            }
            
            if (n) {
                printf(ANSI_COLOR_BOLD ANSI_COLOR_ULINE ANSI_COLOR_RED "Failed test %i!\n" ANSI_COLOR_RESET, i+1);
                if (n == 139)
                    printf("reason: segmentation fault\n");
                res++;
            } else
                printf(ANSI_COLOR_BOLD ANSI_COLOR_ULINE ANSI_COLOR_GREEN "Passed test %i\n" ANSI_COLOR_RESET, i+1);
            printf("\n");
        }

        printf("Passed %i out of %i tests\n", i-res, i);
    } else {
        printf("Running test %i...\n", choice);
        res =  tests[choice-1]();
        printf(res ? "Failed test %i!\n" : "Passed test %i\n", choice);
    }

    return res;

}

