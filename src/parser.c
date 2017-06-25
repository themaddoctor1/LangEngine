#include "parser.h"

#include "linkedlist.h"

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

void** parseArbno(char*, BnfGrammar, BnfStatement elem, BnfStatement delim);
void** parseIdentifier(char*);
void** parseLiteral(char*, char*);
void** parseSequence(char*, BnfGrammar, BnfStatement);
void** parseUnion(char*, BnfGrammar, BnfStatement*);

struct bnf_statement {
    int type;
    void* args;
};

struct bnf_variable {
    int name;
    BnfStatement values;
    BnfParser parse;
};
BnfVariable bnfVariable(int name, BnfStatement states, BnfParser parse) {
    BnfVariable var = (BnfVariable) malloc(sizeof(struct bnf_variable));
    var->name = name;
    var->values = states;
    var->parse = parse;
    return var;
}

struct bnf_grammar {
    BnfVariable *vars;
    BnfParser *parsers;
    void (**disposers)(void*);
};
BnfGrammar bnfGrammar(BnfVariable *vars, BnfParser *parsers, void (**disposers)(void*)) {
    BnfGrammar grammar = (BnfGrammar) malloc(sizeof(struct bnf_grammar));
    
    grammar->vars = vars;
    grammar->parsers = parsers;
    grammar->disposers = disposers;

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

BnfStatement bnfArbno(BnfStatement statement, BnfStatement delim) {
    BnfStatement state = (BnfStatement) malloc(sizeof(struct bnf_statement));
    state->type = BNF_ARBNO;
    state->args = malloc(2 * sizeof(BnfStatement));
    ((BnfStatement*) state->args)[0] = statement;
    ((BnfStatement*) state->args)[1] = delim;

    return state;
}

void disposeBnfSequence(BnfGrammar grammar, BnfStatement state, LinkedList argList) {
    BnfStatement *comps = (BnfStatement*) state->args;

    int i;
    for(i = 0; sizeOfLinkedList(argList); i++) {
        BnfStatement s = comps[i];
        int type = s->type;
        
        if (type == BNF_NUMBER || type == BNF_IDENTIFIER) {
            void *val = dequeue(argList);
            free(val);
        } else if (type == BNF_VARIABLE)
            grammar->disposers[*((int*) s->args)](dequeue(argList));
        else if (type == BNF_ARBNO) {
            void **arbVals = dequeue(argList);
            // Dispose of each sequence's results
            int j;
            for (j = 0; arbVals[j]; j++) {
                disposeBnfSequence(grammar, s->args, buildLinkedList(arbVals[j]));
                free(arbVals[j]);
            }
            free(arbVals);
        } else if (type == BNF_SEQUENCE) {
            void *seqArgs = dequeue(argList);
            disposeBnfSequence(grammar, s, buildLinkedList(s->args));
            free(seqArgs);
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
    int num;
    if (sscanf(str, " %i", &num)) {

        // Update the string and store the number
        int i = 0;
        while (str[i] == ' ' || str[i] == '\n' || str[i] == '\t')
            i++;

        if (str[i] == '-') i++;

        while (str[i] >= '0' && str[i] <= '9')
            i++;
        
        void **ptr = (void**) malloc(2 * sizeof(int));
        ptr[0] = (void*) malloc(sizeof(int));
        *((int*) ptr[0]) = num;
        ptr[1] = (void*) malloc(sizeof(int));
        *((int*) ptr[1]) = i;
        
        return ptr;
        
    } else return NULL;
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
    
    id = (char*) malloc((j - i + 1) * sizeof(char));
    strncpy(id, &str[i], j-i);
    id[j-i] = '\0';
    
    int *pos = (int*) malloc(sizeof(int));
    *pos = j;

    void **res = (void**) malloc(2 * sizeof(void*));
    res[0] = id;
    res[1] = pos;

    return res;
}

void** parseSequence(char *str, BnfGrammar grammar, BnfStatement state) {
    char *strt = str;
    BnfStatement *comps = (BnfStatement*) state->args;

    //printf("Seq: '%s'\n", str);
    
    // The arguments will be stored in a list so that when needed, they can
    // be extracted into an argument array or sequentially disposed of.
    LinkedList argList = makeLinkedList();
    int err = 0;
    
    // Iterate through all of the substatements
    int i;
    for (i = 0; comps[i]; i++) {

        int type = comps[i]->type;
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
            
            int j;
            for (j = 0; vuStates[j]; j++) {
                // First, generate the result of the variable.
                res = var->parse(str, grammar, var, j);

                if (res) {
                    // Extract the result
                    void *varRes = res[0];
                    char *strn = &str[*((int*) res[1])];
                    free(res[1]);
                    free(res);
                    
                    // Parse the rest of the statement, given the variable parsed correctly.
                    BnfStatement substate = bnfSeq2(&comps[i+1]);
                    void **strRes = parseSequence(strn, grammar, substate);
                    free(substate);

                    if (strRes) {
                        // The variable parsed correctly, prep the variable for return.
                        int k;
                        enqueue(argList, varRes);
                        str = &strn[*((int*) strRes[1])];
                        
                        res = strRes[0];
                        
                        free(strRes[1]);
                        free(strRes);

                        for (k = 0; res[k]; k++)
                            enqueue(argList, res[k]);
                        
                        break;
                    } else {
                        // Garbage collection
                        grammar->disposers[varId](varRes);
                    }
                }
                //else printf("    found nothing\n}\n");
            }

            if (vuStates[j]) {
                // A result was found, so proceed to end state
                while (comps[i])
                    i++;
                break;
            } else
                // String does not match
                res = NULL;

        } else if (type == BNF_ARBNO) {
            // Parse a collection of items
            res = parseArbno(str, grammar, ((BnfStatement*) comps[i]->args)[0], ((BnfStatement*) comps[i]->args)[1]);
        } else if (type == BNF_TERMINATOR) {
            // The given string should have no more items to parse.
            int j = 0;
            while (str[j] == ' ' || str[j] == '\t' || str[j] == '\n') j++;

            if (str[j])
                res = NULL;
            else {
                // There is nothing left to parse
                res= (void**) malloc(2 * sizeof(void**));
                res[0] = NULL;
                res[1] = (int*) malloc(sizeof(int));
                *((int*) res[1]) = i;
            }
        } else {
            res = NULL;
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
        } else {
            err = 1;
            break;
        }
    }

    if (comps[i] || err) {
        // Clean the argument list
        disposeBnfSequence(grammar, state, argList);
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
    //printf("inserted %i\n", j);
    
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
void** parseArbno(char *str, BnfGrammar grammar, BnfStatement elem, BnfStatement delim) {
    char *strt = str;
    LinkedList vals = makeLinkedList();
    
    // If the delimiter is an empty string, use NULL for simplicity.
    if (!strcmp((char*) delim->args, ""))
        delim = NULL;
    
    int n;
    while (1) {
        void **res;

        // First, check for a lack of delimiter if one is required.
        if (delim) {
            res = parseLiteral(str, (char*) delim->args);
            if (!res)
                break;
            else {
                // Proceed through the string
                str = &str[*((int*) res[1])];
                free(res[1]);
                free(res);
            }
        }

        res = parseSequence(str, grammar, elem);

        if (!res) {
            // No more statements were found.
            if (delim) {
                // Dispose of the previously determined values.
                while (sizeOfLinkedList(vals))
                    disposeBnfSequence(grammar, elem, dequeue(vals));
                return NULL;
            } else break;
        }
        
        // Continue through the string
        str = &str[*((int*) res[1])];
        enqueue(vals, res);
    }
    enqueue(vals, NULL);
    
    // The result
    void **result = (void**) malloc(2 * sizeof(void*));
    
    // Store each of the subresults
    result[0] = (void**) malloc(sizeOfLinkedList(vals) * sizeof(void*));
    for (n = 0; sizeOfLinkedList(vals); n++) {
        void *res = dequeue(vals);
        ((void**) result[0])[n] = res;
        if (!res)
            break;
    }

    disposeLinkedList(vals);
    
    // Provide the length of the statement parsed.
    result[1] = malloc(sizeof(int));
    *((int*) result[1]) = (int) (str - strt);

    return result;

}

void** parseUnion(char *str, BnfGrammar grammar, BnfStatement *states) {
    
    void **res = NULL;

    int i;
    for (i = 0; states[i] && !res; i++)
        res = parseSequence(str, grammar, states[i]);
    
    if (!res)
        return NULL;

    void **result = (void**) malloc(3 * sizeof(void*));
    result[0] = res[0];
    result[1] = res[1];
    result[2] = malloc(sizeof(int));
    *((int*) result[2]) = i;
    free(res);

    return result;
}

void** parsePtrVar(char *str, BnfGrammar grammar, BnfVariable var, int type) {
    BnfStatement values = var->values;
    void **res = NULL;

    res = parseSequence(str, grammar, ((BnfStatement*)values->args)[type]);

    if (res) {
        free(res[2]);

        return res;
    } else
        return NULL;
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
void** parseExpVar(char *str, BnfGrammar grammar, BnfVariable var, int type) {
    BnfStatement values = var->values;
    void **res = NULL;

    res = parseSequence(str, grammar, ((BnfStatement*)values->args)[type]);

    if (res) {
        res[0] = buildExp(type, res[0]);
        
        free(res[2]);

        return res;
    } else
        return NULL;

}

/**
 * Given a potential program and a BNF grammar, creates a concrete
 * syntax tree representing the program if one exists, else NULL.
 */
Exp parse(char* str, BnfGrammar grammar) {
    
    void **res = NULL;
    int i;
    for (i = 0; !res && ((void**) grammar->vars[0]->values->args)[i]; i++)
        res = grammar->vars[0]->parse(str, grammar, grammar->vars[0], i);

    if (res) {
        return ((Exp*) res[0])[0];
    } else
        return NULL;
}

