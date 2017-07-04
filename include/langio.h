#ifndef _LANGIO_H_
#define _LANGIO_H_

#include "structures.h"
#include "parser.h"

/**
 * Pretty printers for evaluation structures.
 */
void print_exp(Exp);
void print_env(Env);
void print_expval(ExpVal);

/**
 * Pretty printers for the BNF data.
 */
void printBnfSequence(BnfStatement);
void printBnfVariable(BnfVariable);
void printBnfGrammar(BnfGrammar);

#endif
