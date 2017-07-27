#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expvalset.h"
#include "expressionset.h"
#include "environmentset.h"

#include "linkedlist.h"

#include "langio.h"
#include "parser.h"

#include "test.h"

int main(int argc, char *argv[]) {

    LinkedList characters = makeLinkedList();

    // Command line arguments
    int lang_mode = -1; // Modes: -1: interpret (default), 0: test all, n > 0: test specific item.
    int verbose = 0; // Whether or not to print extra information

    int i;
    for (i = 1; argv[i]; i++) {

        // Optionally run a test.
        if (!strcmp("-t", argv[i])) {
            if (!argv[i+1] || !strcmp(argv[i+1], "all"))
                lang_mode = 0;
            else
                lang_mode = atoi(argv[i+1]);
        } else if (!strcmp("-v", argv[i]))
            verbose = 1;

    }

    // Copyright information.
    printf(
    "PemdasLang v. 0.1.0\n"
    "Developed using LangEngine\n"
    "Designed by Christopher Hittner\n\n"
    );
    
    init_bnf_grammar(verbose);
    
    if (lang_mode >= 0) {
        return test(lang_mode);
    }

    // Indicate the means of termination.
    printf(
    "Press Ctrl-C to terminate program\n"
    );
    
    // In the case of this language, it is executed
    // by providing statements on the command line.

    while (1) {
        printf("> ");

        // Get input from stdin
        int ch;
        while ((ch = fgetc(stdin)) != '\n') {
            int *ptr = (int*) malloc(sizeof(int));
            *ptr = ch;
            enqueue(characters, ptr);
        }
        
        // Build the input string.
        char *program = (char*) malloc((1 + sizeOfLinkedList(characters)) * sizeof(char));
        int i;
        for (i = 0; sizeOfLinkedList(characters); i++) {
            int *ptr = dequeue(characters);
            program[i] = (char) *ptr;
            free(ptr);
        }
        program[i] = '\0';
        
        // Compute the result of the program
        ExpVal result = NULL;
        
        if (verbose) {
            Exp res = parse(program);

            if (res) {
                printf("Expression:\n");
                printf("\x1b[2m\x1b[3m");
                print_exp(res);

                printf("\x1b[0m");

                Env env = init_env();
                result = value_of(res, env);

                dispose_exp(res);
                dispose_env(env);
            }
            
            if (result)
                printf("Result:");
            printf("\n");


        } else
            result = evaluate(program);
        
        // Provide the result
        if (result) {
            print_expval(result);
        } else
            printf("Could not evaluate statement '%s'\n", program);

        free(program);

    }

    return 0;

}

