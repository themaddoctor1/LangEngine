#ifndef _EXPRESSIONS_H_
#define _EXPRESSIONS_H_

#include "structures.h"

/**
 * Defines ExpConstructor to be a function type for
 * creating Exps.
 *
 * void** -> Exp
 */
typedef Exp (*ExpConstructor)(void**);

/**
 * Defines a type that evaluates an expression.
 * Type: {Exp, Env} -> void*
 */
typedef ExpVal (*ExpEvaluator)(Exp, Env);

/**
 * Evaluates a given expression.
 *
 * Type: ExpEvaluator
 */
ExpVal value_of(Exp, Env);

#endif
