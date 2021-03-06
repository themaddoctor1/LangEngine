#include "bnf_parser.h"

#include "langio.h"
#include "linkedlist.h"

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

BnfGrammar grammar = NULL;

void init_bnf_grammar(int verbose) {
    if (grammar)
        return;

    grammar = generate_bnf_grammar();

    if (verbose) {
        printf("BNF Grammar:\n");
        printBnfGrammar(grammar);
    }
}

void** parseArbno(char*, BnfGrammar grammar, BnfStatement arbState, BnfStatement state);
void** parseIdentifier(char*);
void** parseLiteral(char*, char*);
void** parseSequence(char*, BnfStatement);
void** parseUnion(char*, BnfStatement*);

struct bnf_statement {
    int type;
    void* args;
};

struct bnf_variable {
    int name;
    BnfStatement values;
    BnfFilter filter;
    void (*dispose)(void*);
};
BnfVariable bnfVariable(int name, BnfStatement states, BnfFilter filter, void (*dispose)(void*)) {
    BnfVariable var = (BnfVariable) malloc(sizeof(struct bnf_variable));
    var->name = name;
    var->values = states;
    var->filter = filter;
    var->dispose = dispose;
    return var;
}

struct bnf_grammar {
    BnfVariable *vars;
};
BnfGrammar bnfGrammar(BnfVariable *vars) {
    BnfGrammar grammar = (BnfGrammar) malloc(sizeof(struct bnf_grammar));
    
    grammar->vars = vars;

    return grammar;
}

BnfStatement bnfUnion(int num, ... ) {
    va_list args;
    va_start(args, num);
    
    void **states = (void**) malloc((num+1) * sizeof(void*));
    int i;
    for (i = 0; i < num; i++)
        states[i] = va_arg(args, void*);
    states[i] = NULL;

    va_end(args);

    BnfStatement state = (BnfStatement) malloc(sizeof(struct bnf_statement));
    state->type = BNF_UNION;
    state->args = states;

    return state;
}

BnfStatement bnfLit(char *lit) {
    BnfStatement state = (BnfStatement) malloc(sizeof(struct bnf_statement));
    state->type = BNF_LITERAL;
    state->args = strdup(lit);

    return state;
}

BnfStatement bnfNum() {
    BnfStatement state = (BnfStatement) malloc(sizeof(struct bnf_statement));
    state->type = BNF_NUMBER;
    state->args = NULL;

    return state;
}

BnfStatement bnfId() {
    BnfStatement state = (BnfStatement) malloc(sizeof(struct bnf_statement));
    state->type = BNF_IDENTIFIER;
    state->args = NULL;

    return state;
}

BnfStatement bnfVar(int var) {
    BnfStatement state = (BnfStatement) malloc(sizeof(struct bnf_statement));
    state->type = BNF_VARIABLE;
    state->args = malloc(sizeof(int));
    *((int*) state->args) = var;

    return state;
}

BnfStatement bnfSeq(int num, ... ) {
    va_list args;
    va_start(args, num);
    
    void **states = (void**) malloc((num+1) * sizeof(void*));
    int i;
    for (i = 0; i < num; i++)
        states[i] = va_arg(args, void*);
    states[i] = NULL;

    return bnfSeq2((BnfStatement*) states);
}

BnfStatement bnfSeq2(BnfStatement *states) {
    BnfStatement state = (BnfStatement) malloc(sizeof(struct bnf_statement));
    state->type = BNF_SEQUENCE;
    state->args = states;

    return state;
}

BnfStatement bnfTrm() {
    BnfStatement state = (BnfStatement) malloc(sizeof(struct bnf_statement));
    state->type = BNF_TERMINATOR;
    state->args = NULL;

    return state;
}

BnfStatement bnfArb(BnfStatement statement, BnfStatement delim) {
    BnfStatement state = (BnfStatement) malloc(sizeof(struct bnf_statement));
    state->type = BNF_ARBNO;
    state->args = malloc(2 * sizeof(BnfStatement));
    ((BnfStatement*) state->args)[0] = statement;
    ((BnfStatement*) state->args)[1] = delim;

    return state;
}

void disposeBnfSequence(BnfStatement state, LinkedList argList) {
    // The set of components of the statement.
    BnfStatement *comps = (BnfStatement*) state->args;

    int i;
    for(i = 0; sizeOfLinkedList(argList); i++) {
        // Get the current component and its type
        BnfStatement s = comps[i];
        int type = s->type;
        
        if (type == BNF_NUMBER || type == BNF_IDENTIFIER) {
            // Type is simply a single pointer that can be removed and freed.
            void *val = dequeue(argList);
            free(val);
        } else if (type == BNF_VARIABLE) {
            // Dispose of the variable outcome using the given destructor.
            BnfVariable v = grammar->vars[*((int*) s->args)];
            v->dispose(dequeue(argList));
        } else if (type == BNF_ARBNO) {
            void **arbVals = dequeue(argList);
            // Dispose of each sequence's results
            int j;
            for (j = 0; arbVals[j]; j++) {
                disposeBnfSequence(s->args, buildLinkedList(arbVals[j]));
                free(arbVals[j]);
            }
            free(arbVals);
        } else if (type == BNF_SEQUENCE) {
            void *seqArgs = dequeue(argList);
            disposeBnfSequence(s, buildLinkedList(s->args));
            free(seqArgs);
        } else if (type == BNF_ARBNO) {
            void **arbArgs = dequeue(argList);
            BnfStatement elem = ((BnfStatement*) s->args)[0];
            
            int j;
            for (j = 0; arbArgs[j]; j++) {
                LinkedList tmp = buildLinkedList(arbArgs[j]);
                free(arbArgs[j]);
                disposeBnfSequence(elem, tmp);
            }
            free(arbArgs);
        }
    }
    
    disposeLinkedList(argList);
}

/**
 * Finds a string literal in the given string.
 * return - A pointer to an int representing the index of the literal.
 */
void** parseLiteral(char *str, char *lit) {
    char *pos = strstr(str, lit);

    if (pos) {
        // The string is present
        int p = (int) (pos - str);

        // Ensure it's the first string. It is if i == p
        int i;
        for (i = 0; i < p; i++)
            if (str[i] != ' ' && str[i] != '\n' && str[i] != '\t')
                break;

        // If it is a literal, ensure it is bounded by the target's length.
        int j;
        for (j = i; (str[j] >= 'a' && str[j] <= 'z') || (str[j] >= 'A' && str[j] <= 'Z'); j++);

        if (i == p && j - i <= strlen(lit)) {
            // Build the result
            void **res = (void**) malloc(2 * sizeof(int));
            int *dist = (int*) malloc(sizeof(int));;
            res[0] = NULL;
            res[1] = dist;
            *dist = p + strlen(lit);
            return res;
        } else return NULL;
    } else return NULL;
}

void** parseNumber(char *str) {
    int i = 0;
    int num = 0, sgn = 1;
    while (str[i] == ' ' || str[i] == '\n' || str[i] == '\t')
            i++;
   
    // Permit a negation
    if (str[i] == '-') {
        sgn *= -1;
        i++;
    }
    
    // The first string must be a number
    if (str[i] < '0' || str[i] > '9')
        return NULL;

    // Compute the number
    while (str[i] >= '0' && str[i] <= '9') {
        num = 10 * num + str[i] - '0';
        i++;
    }
    
    // Build the result.
    void **ptr = (void**) malloc(2 * sizeof(int));
    ptr[0] = (void*) malloc(sizeof(int));
    *((int*) ptr[0]) = num * sgn;
    ptr[1] = (void*) malloc(sizeof(int));
    *((int*) ptr[1]) = i;
    
    return ptr;
}

void** parseIdentifier(char *str) {
    char *id;
    
    // Find the start of an identifier
    int i;
    for (i = 0; str[i] == ' ' || str[i] == '\n' || str[i] == '\t'; i++);
    
    int j;
    for (j = i; (str[j] >= 'a' && str[j] <= 'z') || (str[j] >= 'A' && str[j] <= 'Z'); j++);

    if (i == j)
        return NULL;
    
    // Make a copy of the identifier for future use.
    id = (char*) malloc((j - i + 1) * sizeof(char));
    strncpy(id, &str[i], j-i);
    id[j-i] = '\0';
    
    // Store the id position in the string.
    int *pos = (int*) malloc(sizeof(int));
    *pos = j;
    
    // Build the bundle.
    void **res = (void**) malloc(2 * sizeof(void*));
    res[0] = id;
    res[1] = pos;

    return res;
}

void** parseSequence(char *str, BnfStatement state) {
    char *strt = str;
    BnfStatement *comps = (BnfStatement*) state->args;

    // The arguments will be stored in a list so that when needed, they can
    // be extracted into an argument array or sequentially disposed of.
    LinkedList argList = makeLinkedList();
    int err = 0;
    
    // Iterate through all of the substatements
    int i;
    for (i = 0; comps[i]; i++) {
        
        // The type of expression
        int type = comps[i]->type;

        // Will hold the correct bundle after the if chain.
        void **res;

        if (type == BNF_LITERAL) {
            res = parseLiteral(str, (char*) comps[i]->args);

        } else if (type == BNF_NUMBER) {
            res = parseNumber(str);

        } else if (type == BNF_IDENTIFIER) {

            // Find an identifier and its location.
            res = parseIdentifier(str);

        } else if (type == BNF_VARIABLE) {
            int varId = *((int*) comps[i]->args);
            BnfVariable var = grammar->vars[varId];
            BnfStatement *vuStates = (BnfStatement*) var->values->args;
            
            BnfStatement substate = bnfSeq2(&comps[i+1]);

            int j;
            for (j = 0; vuStates[j]; j++) {
                // First, generate the result of the variable.
                res = parse_bnfstring(str, var, j);

                if (res) {
                    // Extract the result
                    void *varRes = res[0];
                    char *strn = &str[*((int*) res[1])];
                    free(res[1]);
                    free(res);
                    
                    // Parse the rest of the statement, given the variable parsed correctly.

                    void **strRes = parseSequence(strn, substate);

                    if (strRes) {
                        // The variable parsed correctly, prep the variable for return.
                        int k;
                        enqueue(argList, varRes);
                        str = &strn[*((int*) strRes[1])];
                        
                        // The values to add to handle.
                        res = strRes[0];
                        
                        free(strRes[1]);
                        free(strRes);
                        
                        // Add the tail end of the chain.
                        for (k = 0; res[k]; k++)
                            enqueue(argList, res[k]);
                        
                        break;
                    } else {
                        // Garbage collection
                        grammar->vars[varId]->dispose(varRes);
                    }
                }
            }

            free(substate);

            if (vuStates[j]) {
                // A result was found, so proceed to end state
                while (comps[i])
                    i++;
                break;
            } else
                // String does not match
                res = NULL;

        } else if (type == BNF_ARBNO) {
            BnfStatement poststate = bnfSeq2(&comps[i+1]);
            
            res = parseArbno(str, grammar, comps[i], poststate);

            if (!res) {
                err = 1;
            } else {
                // Add the rest of the arguments
                void **args = res[0];

                int k;
                for (k = 0; args[k]; k++) {
                    enqueue(argList, args[k]);
                }
                
                /*
                for (k = 0; ((void**) args[0])[k]; k++)
                    print_exp(((Exp*) args[0])[k]);
                */
                
                // Progress the string
                str = &str[*((int*) res[1])];
                
                // Garbage collection
                free(res[0]);
                free(res[1]);
                free(res);
                
                // Terminate the string
                while (comps[i])
                    i++;
                
                break;
            }

        } else if (type == BNF_TERMINATOR) {
            // The given string should have no more items to parse.
            int j = 0;
            while (str[j] == ' ' || str[j] == '\t' || str[j] == '\n') j++;

            if (str[j]) {
                err = 1;
                res = NULL;
            } else {
                // There is nothing left to parse
                res= (void**) malloc(2 * sizeof(void**));
                res[0] = NULL;
                res[1] = (int*) malloc(sizeof(int));
                *((int*) res[1]) = i;
            }
        } else {
            res = NULL;
            err = 1;
        }

        if (res) {
            // Add the argument
            if (res[0])
                enqueue(argList, res[0]);
            
            // Traverse
            int dist = *((int*) res[1]);
            str = &str[dist];

            // Garbage collection
            free(res[1]);
            free(res);

            res = NULL;
        } else {
            err = 1;
            break;
        }
    }

    if (comps[i] || err) {
        // Clean the argument list
        disposeBnfSequence(state, argList);
        return NULL;
    }

    enqueue(argList, NULL);

    void **res = (void**) malloc(2 * sizeof(void*));
    void **vals = (void**) malloc(sizeOfLinkedList(argList) * sizeof(void*));
    res[0] = vals;

    // Insert the arguments
    int j;
    for (j = 0; sizeOfLinkedList(argList) > 0; j++) {
        void *tmp = dequeue(argList);
        vals[j] = tmp;

        if (!tmp)
            break;
    }
    
    disposeLinkedList(argList);
    
    // Provide statement length
    res[1] = (void*) malloc(sizeof(int));
    *((int*) res[1]) = (int) (str - strt);
    
    return res;
}

/**
 * Parses the arbno sequence.
 *
 * note: delim MUST be a literal, or NULL if no delimiter is to be used.
 */
void** parseArbno(char *str, BnfGrammar grammar, BnfStatement arbState, BnfStatement state) {
    BnfStatement elem = ((BnfStatement*) arbState->args)[0];
    BnfStatement delim = ((BnfStatement*) arbState->args)[1];

    char *strt = str;
    LinkedList vals = makeLinkedList();
    
    void **output = parseSequence(str, elem);
    if (output) {

        if (output[0])
            enqueue(vals, output[0]);

        str = &str[*((int*) output[1])];

        free(output[1]);
        free(output);
        
        int k = 0;
        while ((output = parseSequence(str, delim))) {
            k++;
            
            // Progress the string
            str = &str[*((int*) output[1])];
            free(output[0]);
            free(output[1]);
            free(output);

            // Parse the next element
            if (!(output = parseSequence(str, elem))) {
                // There can be no more elements
                break;
            }

            if (output[0])
                enqueue(vals, output[0]);
            
            // Progress the string
            str = &str[*((int*) output[1])];
            free(output[1]);
            free(output);
        }
    }

    // First, try to parse the rest of the string
    if (!(output = parseSequence(str, state))) {

        // Failed to parse
        while (sizeOfLinkedList(vals)) {
            void **del = (void**) dequeue(vals);
            
            LinkedList list = buildLinkedList(del);
            free(del);
                
            disposeBnfSequence(elem, list);
        }
        
        // Final garbage collection
        disposeLinkedList(vals);

        return NULL;
    } else {
        // values holds the args for the rest of the sequence
        void **values = (void**) output[0];

        int i;
        
        // Progress the string
        str = &str[*((int*) output[1])];
        free(output[1]);
        free(output);
        
        // Compute the length of the argument array
        int len = 0;
        while (values[len])
            len++;
        
        // Build enough space for the arbno values, the following
        // values, and the NULL terminator.
        output = (void**) malloc((len+2) * sizeof(void*));

        // Provide the arbno values. We use value to store them.
        values = (void**) malloc((sizeOfLinkedList(vals) + 1) * sizeof(void*));
        for (i = 0; sizeOfLinkedList(vals); i++) {
            values[i] = dequeue(vals);
        }
        values[i] = NULL;

        output[0] = values;

        // Add the followers
        for (i = 0; i < len; i++) {
            output[i+1] = values[i];
        }
        output[len+1] = NULL;

        // Dispose of the linked list
        disposeLinkedList(vals);

        values = output;

        output = (void**) malloc(2 * sizeof(void*));
        output[0] = values;

        output[1] = (void*) malloc(sizeof(int));
        *((int*) output[1]) = (int) (str - strt);

        return output;
    }

}

void** parseUnion(char *str, BnfStatement *states) {
    
    void **res = NULL;
    
    // Find an outcome that works.
    int i;
    for (i = 0; states[i] && !res; i++)
        res = parseSequence(str, states[i]);
    
    return res;
}

/**
 * Parses for an expression within a string. The expression may
 * or may not use the entire input program.
 *
 * return - If successful, a pointer with the following contents:
 *              result[0]: An Exp representing the parsed values.
 *              result[1]: The length of the string used.
 *          Otherwise, returns NULL.
 */
void** parse_bnfstring(char *str, BnfVariable var, int type) {
    BnfStatement values = var->values;
    BnfStatement state = ((BnfStatement*) values->args)[type];
    void **res = NULL;

    res = parseSequence(str, state);

    if (res) {
        // Something was returned, see if it can be filtered.
        if (res[0]) {
            // Filter if requested, then return.
            if (var->filter)
                res[0] = var->filter(type, res[0]);
            return res;
        } else {
            // Dispose of the result; it is invalid.
            free(res[1]);
            free(res);
            return NULL;
        }
    } else
        // Nothing came back; indicate that fact.
        return NULL;

}

/**
 * Given a potential program and a BNF grammar, creates a concrete
 * syntax tree representing the program if one exists, else NULL.
 */
Exp parse_bnf(char* str) {

    if (!grammar)
        grammar = generate_bnf_grammar();
    
    void **res = NULL;
    int i;
    for (i = 0; !res && ((void**) grammar->vars[0]->values->args)[i]; i++)
        res = parse_bnfstring(str, grammar->vars[0], i);

    if (res) {
        return ((Exp*) res)[0];
    } else
        return NULL;
}

void printBnfGrammar(BnfGrammar grammar) {
    BnfVariable *vars = grammar->vars;

    int i = 0;
    for (i = 0; vars[i]; i++) {
        BnfVariable var = vars[i];
        BnfStatement *values = (BnfStatement*) var->values->args;
        
        printf("<%i> :=\n", i);

        int j;
        for (j = 0; values[j]; j++) {
            printf("  %s ", j ? "|" : " ");
            printBnfSequence(values[j]);
            printf("\n");
        }
    }
}

void printBnfSequence(BnfStatement statement) {
    BnfStatement *states = (BnfStatement*) statement->args;

    printf("(");

    int i, j;
    for (i = 0; states[i]; i++) {
        BnfStatement state = states[i];

        switch (state->type) {
            case BNF_UNION:
                for (j = 0; ((BnfStatement*) state->args)[j]; j++) {
                    if (j) printf(" | ");
                    printBnfSequence(((BnfStatement*) state->args)[j]);
                }
                break;
            case BNF_LITERAL:
                printf(" '%s'", (char*) state->args);
                break;
            case BNF_NUMBER:
                printf(" num");
                break;
            case BNF_IDENTIFIER:
                printf(" id");
                break;
            case BNF_VARIABLE:
                printf(" <%i>", *((int*) state->args));
                break;
            case BNF_SEQUENCE:
                printf(" (");
                printBnfSequence(state);
                printf(" )");
                break;
            case BNF_ARBNO:
                printf(" arbno [");
                printBnfSequence(((BnfStatement*) state->args)[0]);
                printf(" ][");
                printBnfSequence(((BnfStatement*) state->args)[1]);
                printf(" ]");
                break;
            case BNF_TERMINATOR:
                printf(" ;");
                break;
        }
    }

    printf(" )");
    
    return;
}


