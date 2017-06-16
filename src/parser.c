#include "include/parser.h"

#include <stdlib.h>

struct bnf_component {
    int type;
    void* args;
};

struct bnf_variable {
    int name;
    BnfStatement* values;
};

struct bnf_grammar {
    BnfVariable *vars;
    BnfParser **parsers;
};

Exp parse(char* str, BnfGrammar grammar, int var) {
    // Get the parser for the given variable
    BnfParser *parsers = grammar->parsers[var];
    
    int i;
    Exp result = NULL;
    for (i = 0; parsers[i] && !result; i++)
        result = parsers[i](str, grammar, var);

    return result;
}

