#ifndef _ENVIRONMENTS_H_
#define _ENVIRONMENTS_H_

#include "structures.h"

Env build_env(int, void*, Env);

/**
 * Creates the default environment.
 */
Env init_env();

void dispose_env(Env);

/**
 * A type that represent functions that
 * search an environment for a variable.
 */
typedef ExpVal (*ApplyEnv)(Env, char*);

/**
 * Looks up the value of the variable associated with a given
 * identifier in a given environment.
 *
 * Type: {char*, Env} -> ExpVal
 */
ExpVal apply_env(Env, char*);

#endif
