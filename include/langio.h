#ifndef _LANGIO_H_
#define _LANGIO_H_

#include "structures.h"
#include "bnf_parser.h"

#include <stdarg.h>

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

void VERBOSE_SET(int);
int VERBOSE_MODE();

int debug_printf(char*, ...);

#endif
