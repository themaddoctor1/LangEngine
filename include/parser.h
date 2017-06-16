#ifndef _PARSER_H_
#define _PARSER_H_

#include "structures.h"

/**
 * Defines a BNF statement. Can consist of smaller BNF statements.
 */
struct bnf_statement;
typedef struct bnf_statement* BnfStatement;

#define BNF_UNION 0
#define BNF_LITERAL 1
#define BNF_IDENTIFIER 2
#define BNF_VARIABLE 3

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

/**
 * Defines a parsing function type. Given a string,
 * a grammar, and a variable in the grammar that the
 * string should equal, attempt to build the syntax
 * tree for the string.
 *
 * return - A syntax tree if successful, otherwise NULL.
 */
typedef Exp (*BnfParser)(char*, BnfGrammar, int);

/**
 * Parses a string given a grammar and initial variable.
 */
Exp parse(char*, BnfGrammar, int);

#endif
