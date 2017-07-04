#ifndef _PARSER_H_
#define _PARSER_H_

#include "expressions.h"

#include <stdarg.h>

/**
 * Defines a BNF statement. Can consist of smaller BNF statements.
 */
struct bnf_statement;
typedef struct bnf_statement* BnfStatement;

#define BNF_UNION       0
#define BNF_LITERAL     1
#define BNF_NUMBER      2
#define BNF_IDENTIFIER  3
#define BNF_VARIABLE    4
#define BNF_SEQUENCE    5
#define BNF_ARBNO       6
#define BNF_TERMINATOR  7

BnfStatement bnfLit(char*);
BnfStatement bnfNum();
BnfStatement bnfId();
BnfStatement bnfVar(int);
BnfStatement bnfUnion(int num, ...);
BnfStatement bnfSeq(int num, ...);
BnfStatement bnfSeq2(BnfStatement*);
BnfStatement bnfArb(BnfStatement, BnfStatement);
BnfStatement bnfTrm();

/**
 * Defines a BNF variable.
 */
struct bnf_variable;
typedef struct bnf_variable* BnfVariable;

/**
 * Defines a BNF grammar.
 */
struct bnf_grammar;
typedef struct bnf_grammar* BnfGrammar;

typedef void** (*BnfParser)(char*, BnfGrammar, BnfVariable, int);
typedef void* (*BnfFilter)(int, void**);

/**
 * Creates a BNF variable.
 *
 * precondition - The BnfStatement is a union statement.
 *                The disposer will completely dispose of any value that is
 *                  returned by the BnfFilter parameter.
 */
BnfVariable bnfVariable(int, BnfStatement, BnfFilter, void (*disposer)(void*));

BnfGrammar bnfGrammar(BnfVariable*);
BnfGrammar generateGrammar();

void** parseString(char*, BnfGrammar, BnfVariable, int);
Exp parse(char*, BnfGrammar);

#endif
