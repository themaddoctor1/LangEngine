#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    else switch(exp1->type) {
        case CONST_EXP:
            return *((int*) exp1->args[0]) != *((int*) exp2->args[0]);
        case VAR_EXP:
            return strcmp(exp1->args[0], exp2->args[0]);
        case SUM_EXP:
            return expcmp(exp1->args[0], exp2->args[0]) || expcmp(exp1->args[1], exp2->args[1]);
        case DIFF_EXP:
            return expcmp(exp1->args[0], exp2->args[0]) || expcmp(exp1->args[1], exp2->args[1]);
        case MULT_EXP:
            return expcmp(exp1->args[0], exp2->args[0]) || expcmp(exp1->args[1], exp2->args[1]);
        case LET_EXP:
            return strcmp(exp1->args[0], exp2->args[0])
                || expcmp(exp1->args[1], exp2->args[1])
                || expcmp(exp1->args[2], exp2->args[2]);
        case IF_EXP:
            return expcmp(exp1->args[0], exp2->args[0])
                || expcmp(exp1->args[1], exp2->args[1])
                || expcmp(exp1->args[2], exp2->args[2]);
        case IS_ZERO_EXP:
            return expcmp(exp1->args[0], exp2->args[0]);
        case PARENTH_EXP:
            return expcmp(exp1->args[0], exp2->args[0]);
        default:
            return 1;
    }
}


int test_env1() {

    Env env;
    ExpVal res;

    printf("TEST: empty-env\n");
    
    printf("Creating empty environment...\n");
    env = emptyEnv(NULL, NULL);
    
    printf("Running apply_env(env, 'x')...\n");
    res = apply_env(env, "x");
    if (res) {
        // Something was found, and that is incorrect behavior
        printf("apply_env(env) returned %p when NULL expected!\n", (void*) res);
        return 1;
    }

    return 0;
}

int test_env2() {
    
    Env env;
    ExpVal res;

    printf("TEST: extend-env\n");
    printf("ASSUMES: empty-env works correctly\n");

    printf("Creating extend-env('x', (ExpVal) 42, empty-env())...\n");
    env = extendEnv("x", intVal(42), emptyEnv());

    printf("Running applyEnv(env, 'x')...\n");
    res = apply_env(env, "x");
    if (*((int*) res->value) != 42) {
        if (!res)
            printf("apply_env(env) failed to find value of 'x'!\n");
        else
            printf("apply_env(env) returned unexpected value!\n");

        return 1;
    }

    printf("Running applyEnv(env, 'y')...\n");
    res = apply_env(env, "y");
    if (res) {
        printf("apply_env found value when NULL expected!\n");
        return 1;
    }

    printf("Performing garbage collection...\n");
    ((void**) env->args)[1] = NULL;
    dispose_env(env);
    
    return 0;

}

/**
 * Evaluates an expression given an environment, and returns the accuracy when done.
 */
int evaluation_test(Exp exp, Env env, ExpVal expected) {
    ExpVal result;
    int res;

    printf("INPUT:\n");
    printf("exp:\n");
    print_exp(exp);

    printf("\nenv:\n");
    print_env(env);

    printf("\nEXPECTED:\n");
    print_expval(expected);

    result = value_of(exp, env);

    if (result->type != expected->type) {
        printf(ANSI_COLOR_RED "Failed to return correct type!\n" ANSI_COLOR_RESET);
        res = 1;
    } else if (*((int*) result->value) != *((int*) expected->value)) {
        printf(ANSI_COLOR_RED "Failed to return correct value!\n" ANSI_COLOR_RESET);
        res = 1;
    } else
        res = 0;
    
    if (res) {
        printf("\nRETURNED:\n");
        print_expval(result);
        printf("\n");
    }
   
    dispose_expval(result);
    dispose_expval(expected);
    dispose_exp(exp);
    dispose_env(env);

    return res;
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

int parse_test(char *str, Exp expected) {
    Exp res;
    
    printf("TEST: parse(\"%s\")\n", str);
    printf("EXPECTED:\n");
    if (expected)
        print_exp(expected);
    else
        printf("NULL\n");

    res = (Exp) parse(str);

    if (expected && res) {
        int n = expcmp(expected, res);
        dispose_exp(expected);

        if (n) {
            
            printf("RESULT:\n");
            if (res)
                print_exp(res);
            else
                printf("NULL\n");

            printf(ANSI_COLOR_RED "Failed!\n\n" ANSI_COLOR_RESET);

        } else
            printf(ANSI_COLOR_GREEN "Passed!\n\n" ANSI_COLOR_RESET);

        dispose_exp(res);

        return n;
    } else if (expected || res) {

        if (expected)
            dispose_exp(expected);

        printf("RESULT:\n");
        if (res) {
            print_exp(res);
            dispose_exp(res);
        } else
            printf("NULL\n");

        printf(ANSI_COLOR_RED "Failed!\n\n" ANSI_COLOR_RESET);
        return 1;
    } else {
        printf(ANSI_COLOR_GREEN "Passed!\n\n" ANSI_COLOR_RESET);
        return 0;
    }
}

int test_exp1() {
    Env env = emptyEnv();
    Exp exp = diffExp(varExp("x"), varExp("y"));
    
    int i = 0;
    for (i = 0; i < 5; i++) {
        char buff[2] = "z";
        buff[0] -= i;
        env = extendEnv(buff, intVal(i), env);
    }

    return evaluation_test(exp, env, intVal(1));

}

int test_int1() {
    printf(ANSI_COLOR_BOLD ANSI_COLOR_ULINE "CONST STATEMENTS INT TEST" ANSI_COLOR_RESET "\n");

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

int test_int2() {
    printf(ANSI_COLOR_BOLD ANSI_COLOR_ULINE "VAR STATEMENTS INT TEST" ANSI_COLOR_RESET "\n");

    return
        full_test(
            // Should parse and evaluate x to be int-val 2
            "x", 
            varExp("x"), 
            intVal(2), 
            extendEnv("x", intVal(2), emptyEnv()))
    |   full_test(
            // Should parse, but fail to evaluate (no value abc).
            "abc", 
            varExp("abc"),
            NULL,
            emptyEnv())
    |   full_test(
            // Should fail to parse.
            "a b c d e", 
            NULL,
            NULL,
            NULL)
    ;
}

int test_int3() { 
    printf(ANSI_COLOR_BOLD ANSI_COLOR_ULINE "MATH STATEMENTS INT TEST" ANSI_COLOR_RESET "\n");

    return
        full_test(
            // Will parse and lookup x and y for calculation.
            "+ x y",
            sumExp(varExp("x"), varExp("y")),
            intVal(5),
            extendEnv("x", intVal(2),
                extendEnv("y", intVal(3),
                    emptyEnv())))
    |   full_test(
            // Will parse and lookup x and y for calculation.
            "- x y",
            diffExp(varExp("x"), varExp("y")),
            intVal(-1),
            extendEnv("x", intVal(2),
                extendEnv("y", intVal(3),
                    emptyEnv())))
    |   full_test(
            // Should be able to parse substatement
            "- 5 + 7 3",
            diffExp(constExp(5), sumExp(constExp(7), constExp(3))),
            intVal(-5),
            emptyEnv())
    |   full_test(
            // Should not subtract with three arguments
            "- x x x",
            NULL, NULL, NULL)
    ;

}

int test_int4() {
    printf(ANSI_COLOR_BOLD ANSI_COLOR_ULINE "LET STATEMENT INT TEST" ANSI_COLOR_RESET "\n");

    return
        full_test(
            // Should be able to add to environment and extract.
            "let x be 2 in x",
            letExp("x", constExp(2), varExp("x")),
            intVal(2),
            emptyEnv())
    |   full_test(
            // Subcalculations should be possible.
            "let x be - 4 2 in + x 2",
            letExp("x", diffExp(constExp(4), constExp(2)), sumExp(varExp("x"), constExp(2))),
            intVal(4),
            emptyEnv())
    |   full_test(
            // Let expression inside a let expression.
            "let x be 2 in let y be 3 in + x y",
            letExp("x", constExp(2),
                letExp("y", constExp(3),
                    sumExp(varExp("x"), varExp("y")))),
            intVal(5),
            emptyEnv())
    |   full_test(
            // Recognize bad usage of let
            "letx=2in2",
            NULL, NULL, NULL)
    ;

}

int test_int5() {
    printf(ANSI_COLOR_BOLD ANSI_COLOR_ULINE "ZERO STATEMENT INT TEST" ANSI_COLOR_RESET "\n");

    return
        full_test(
            " zero? ( 0 )",
            iszeroExp(constExp(0)),
            boolVal(1),
            emptyEnv())
    |   full_test(
            "zero?(+ 1 2)",
            iszeroExp(sumExp(constExp(1), constExp(2))),
            boolVal(0),
            emptyEnv())
    |   full_test(
            // A vailid program, but should not be allowed to evaluate.
            "zero?(zero?(1))",
            iszeroExp(iszeroExp(constExp(1))),
            NULL,
            NULL)
    ;

}

int test_int6() {
    printf(ANSI_COLOR_BOLD ANSI_COLOR_ULINE "PARENTHETICAL STATEMENTS INT TEST" ANSI_COLOR_RESET "\n");
    int res = full_test(
            "(let x be 2 in (+ x 2))",
            parenthExp(letExp("x", constExp(2), parenthExp(sumExp(varExp("x"), constExp(2))))),
            intVal(4),
            emptyEnv());

    printf("Part 2\n");
    res |=  full_test(
            "(let x = 2 in x",
            NULL,
            NULL,
            NULL);

    return res;
}

/* Collection of test scripts */
int (*tests[])() = {
    test_env1,
    test_env2,
    test_exp1,
    test_int1,
    test_int2,
    test_int3,
    test_int4,
    test_int5,
    test_int6,
    NULL
};

int test(int choice) {

    int res = 0;

    if (choice <= 0) {
        int i;
        for (i = 0; tests[i]; i++) {
            printf("Running test %i...\n", i+1);
            if (tests[i]()) {
                printf(ANSI_COLOR_BOLD ANSI_COLOR_ULINE ANSI_COLOR_RED "Failed test %i!\n" ANSI_COLOR_RESET, i+1);
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

