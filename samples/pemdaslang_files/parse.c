#include "bnf_parser.h"
#include "parser.h"

#include "expressionset.h"
#include "linkedlist.h"
#include "langio.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PARSE_OPERATION             0
#define PARSE_VALUE                 1
#define PARSE_ADDITIVE_EXP          2
#define PARSE_ADDITIVE_PRM          3
#define PARSE_MULTIPLICATIVE_EXP    4
#define PARSE_MULTIPLICATIVE_PRM    5
#define PARSE_PARENTHESES           6


Exp parse(char *str) {
    return parse_bnf(str);
}

void* progFilter(int type, void **args) {
    void *res = args[0];
    free(args);
    
    return res;
}

void* valueFilter(int type, void **args) {
    return build_exp(CONST_EXP, args);
}

void* opConsFilter(int type, void **args) {
    // Initial condition
    Exp val = args[0];

    LinkedList list = args[1];
    
    // Apply each part of the operation
    while (sizeOfLinkedList(list)) {
        int *opPtr = pop(list);
        Exp item = pop(list);

        void **items = (void**) malloc(2*sizeof(void*));
        items[0] = val;
        items[1] = item;

        val = build_exp(*opPtr, items);

        free(opPtr);
    }
    disposeLinkedList(list);
    
    // Provide the result
    return val;
}

void disposeOpList(void* arg) {
    LinkedList list = (LinkedList) arg;

    while (sizeOfLinkedList(list)) {
        free(pop(list));
        dispose_exp(pop(list));
    }
}

void* additiveFilter(int type, void **args) {
    if (type == 2) {
        // Base case: no more to report
        free(args);
        return makeLinkedList();
    } else {
        LinkedList list = args[1];
        
        int* ptr = (int*) malloc(sizeof(int));
        *ptr = type == 0 ? ADD_EXP : SUBTRACT_EXP;

        push(list, args[0]);
        push(list, ptr);

        return list;

    }
}

void* multiplicativeFilter(int type, void **args) {
    if (type == 2) {
        // Base case: no more to report
        free(args);
        return makeLinkedList();
    } else {
        LinkedList list = args[1];
        
        int* ptr = (int*) malloc(sizeof(int));
        *ptr = type == 0 ? MULTIPLY_EXP : DIVIDE_EXP;

        push(list, args[0]);
        push(list, ptr);

        return list;

    }
}

BnfGrammar generate_bnf_grammar() {

    BnfVariable vars[] = {
        bnfVariable(PARSE_OPERATION,
            bnfUnion(1,
                bnfSeq(2,
                    bnfVar(PARSE_ADDITIVE_EXP),
                    bnfTrm()
                )
            ),
            progFilter,
            free
        ),

        bnfVariable(PARSE_VALUE,
            bnfUnion(1,
                bnfSeq(1,
                    bnfNum()
                )
            ),
            valueFilter,
            free
        ),

        bnfVariable(PARSE_ADDITIVE_EXP,
            bnfUnion(1,
                bnfSeq(2, bnfVar(PARSE_MULTIPLICATIVE_EXP), bnfVar(PARSE_ADDITIVE_PRM))
            ),
            opConsFilter,
            (void (*)(void*)) dispose_exp
        ),

        bnfVariable(PARSE_ADDITIVE_PRM,
            bnfUnion(3,
                bnfSeq(3,
                    bnfLit("+"), 
                    bnfVar(PARSE_MULTIPLICATIVE_EXP), 
                    bnfVar(PARSE_ADDITIVE_PRM)),
                bnfSeq(3,
                    bnfLit("-"), 
                    bnfVar(PARSE_MULTIPLICATIVE_EXP), 
                    bnfVar(PARSE_ADDITIVE_PRM)),
                bnfSeq(0)
            ),
            additiveFilter,
            disposeOpList
        ),

        bnfVariable(PARSE_MULTIPLICATIVE_EXP,
            bnfUnion(1,
                bnfSeq(2, bnfVar(PARSE_PARENTHESES), bnfVar(PARSE_MULTIPLICATIVE_PRM))
            ),
            opConsFilter,
            (void (*)(void*)) dispose_exp
        ),

        bnfVariable(PARSE_MULTIPLICATIVE_PRM,
            bnfUnion(3,
                bnfSeq(3, bnfLit("*"), bnfVar(PARSE_PARENTHESES), bnfVar(PARSE_MULTIPLICATIVE_PRM)),
                bnfSeq(3, bnfLit("/"), bnfVar(PARSE_PARENTHESES), bnfVar(PARSE_MULTIPLICATIVE_PRM)),
                bnfSeq(0)
            ),
            multiplicativeFilter,
            disposeOpList
        ),

        bnfVariable(PARSE_PARENTHESES,
            bnfUnion(2,
                bnfSeq(3, bnfLit("("), bnfVar(PARSE_ADDITIVE_EXP), bnfLit(")")),
                bnfSeq(1, bnfVar(PARSE_VALUE))
            ),
            progFilter,
            (void (*) (void*)) dispose_exp
        ),

        NULL
        
    };

    int numVars = 0;
    while (vars[numVars])
        numVars++;

    BnfVariable *vs = (BnfVariable*) malloc((1 + numVars) * sizeof(BnfVariable*));
    memcpy(vs, vars, (1 + numVars) * sizeof(BnfVariable*));
    
    return bnfGrammar(vs);

}

