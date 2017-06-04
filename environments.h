#ifndef _ENVIRONMENTS_H_
#define _ENVIRONMENTS_H_

#include "structures.h"

/**
 * Looks up the value of the variable associated with a given
 * identifier in a given environment.
 *
 * Type: {char*, Env} -> ExpVal
 */
ExpVal apply_env(char*, Env);

#endif
