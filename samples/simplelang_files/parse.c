#include "bnf_parser.h"
#include "parser.h"

#include "expressionset.h"

#include <stdlib.h>
#include <stdio.h>

#define PARSE_PROGRAM       0
#define PARSE_EXPRESSION    1

Exp parse(char *str) {
    return parse_bnf(str);
}

void* progFilter(int type, void **args) {
    void *res = args[0];
    free(args);
    return res;
}

void* expFilter(int type, void **args) {
    // The established order of operations
    static int types[] = {
        PARENTH_EXP,
        SUM_EXP,
        DIFF_EXP,
        MULT_EXP,
        LET_EXP,
        IF_EXP,
        IS_ZERO_EXP,
        CONST_EXP,
        VAR_EXP
    };
    return build_exp(types[type], args);
}

BnfGrammar generate_bnf_grammar() {
    
    /**
    <Program> := <Expression> NULL
     */
    BnfVariable prog_var = bnfVariable(PARSE_PROGRAM,
        bnfUnion(1,
            bnfSeq(2,
                bnfVar(PARSE_EXPRESSION),
                bnfTrm()
            )
        ),
        progFilter,
        free
    );

    /**
    <Expression> := <Number>
                  | <Identifier>
                  | '-' '(' <Expression> ',' <Expression> ')'
                  | 'let' <Identifier> '=' <Expression> 'in' <Expression>
                  | 'if' <Expression> 'then' <Expression> 'else' <Expression>
     */
    BnfVariable exp_var = bnfVariable(PARSE_EXPRESSION,
        bnfUnion(9,
            bnfSeq(3,
                bnfLit("("),
                bnfVar(PARSE_EXPRESSION),
                bnfLit(")")
            ),
            bnfSeq(3,
                bnfLit("+"),
                bnfVar(PARSE_EXPRESSION),
                bnfVar(PARSE_EXPRESSION)
            ),
            bnfSeq(3,
                bnfLit("-"),
                bnfVar(PARSE_EXPRESSION),
                bnfVar(PARSE_EXPRESSION)
            ),

            bnfSeq(3,
                bnfLit("*"),
                bnfVar(PARSE_EXPRESSION),
                bnfVar(PARSE_EXPRESSION)
            ),
            bnfSeq(6,
                bnfLit("let"),
                bnfId(),
                bnfLit("be"),
                bnfVar(PARSE_EXPRESSION),
                bnfLit("in"),
                bnfVar(PARSE_EXPRESSION)
            ),
            bnfSeq(6,
                bnfLit("if"),
                bnfVar(PARSE_EXPRESSION),
                bnfLit("then"),
                bnfVar(PARSE_EXPRESSION),
                bnfLit("else"),
                bnfVar(PARSE_EXPRESSION)
            ),
            bnfSeq(4,
                bnfLit("zero?"),
                bnfLit("("),
                bnfVar(PARSE_EXPRESSION),
                bnfLit(")")
            ),
            bnfSeq(1,
                bnfNum()
            ),
            bnfSeq(1,
                bnfId()
            )

        ),
        expFilter,
        (void (*)(void*)) dispose_exp
    );

    BnfVariable *vars = (BnfVariable*) malloc(3 * sizeof(BnfVariable));
    vars[0] = prog_var;
    vars[1] = exp_var;
    vars[2] = NULL;
    
    BnfGrammar grammar =  bnfGrammar(vars);
    
    /*
    printf("Generated BNF grammar:\n");
    printBnfGrammar(grammar);
    printf("\n");
    */

    return grammar;


}

