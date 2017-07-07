#ifndef _EXPRESSIONSET_H_
#define _EXPRESSIONSET_H_

#include "expressions.h"

#define CONST_EXP       0
#define VAR_EXP         1
#define SUM_EXP         2
#define DIFF_EXP        3
#define MULT_EXP        4
#define LET_EXP         5
#define IF_EXP          6
#define IS_ZERO_EXP     7
#define PARENTH_EXP     8

/**
 * name: const-exp
 * args:
 *  int x
 */
Exp constExp(int);
void disposeConstExp(Exp);

/**
 * name: var-exp
 * args:
 *  identifier id
 */
Exp varExp(char*);
void disposeVarExp(Exp);

Exp sumExp(Exp, Exp);
void disposeSumExp(Exp);

/**
 * name: diff-exp
 * args:
 *  int x
 *  int y
 */
Exp diffExp(Exp, Exp);
void disposeDiffExp(Exp);

Exp multExp(Exp, Exp);
void disposeMultExp(Exp);

/**
 * name: let-exp
 * args:
 *  identifier id - Variable name
 *  Exp exp1      - Value to assign
 *  Exp body      - Expression to evaluate
 */
Exp letExp(char*, Exp, Exp);
void disposeLetExp(Exp);

/**
 * name: if-exp
 * args:
 *  Exp exp1 - The condition to check
 *  Exp exp2 - The true condition
 *  Exp exp2 - The false condition
 */
Exp ifExp(Exp, Exp, Exp);
void disposeIfExp(Exp);

/**
 * name: zero?-exp
 * aegs:
 *  Exp exp1 - Number that should equal zero
 */
Exp iszeroExp(Exp);
void disposeIszeroExp(Exp);

/**
 * name: parenth-exp
 * args:
 *  Exp exp - Expression inside of the parentheses
 */
Exp parenthExp(Exp);
void disposeIszeroExp(Exp);

#endif
