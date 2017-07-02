#ifndef _EXPRESSIONS_H_
#define _EXPRESSIONS_H_

#include "structures.h"

/**
 * Defines ExpConstructor to be a function type for
 * creating Exps.
 *
 * void* -> Exp
 */
typedef Exp (*ExpConstructor)(void*);

/**
 * Defines destructor function type for deleting Exps
 *
 * Exp -> void
 */
typedef void (*ExpDestructor)(Exp);

/**
 * Creates a blank expression.
 *
 * note: type should be defined by the user.
 */
Exp newBlankExp(void*);

/**
 * Full constructor that creates an expression.
 *
 * type - The type of expression represented
 * args - The arguments to be provided to the expression.
 */
Exp build_exp(int type, void* args);

/**
 * Destructs a basic Exp structure.
 */
void dispose_exp(Exp);

/**
 * Function that should be written to automatically run the
 * correct destructor for the given Exp.
 */
void basedispose_exp(Exp);

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
