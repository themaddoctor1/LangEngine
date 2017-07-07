#ifndef _ENVIRONMENTSET_H_
#define _ENVIRONMENTSET_H_

#include "environments.h"

#define EMPTY_ENV   0
#define EXTEND_ENV  1

/**
 * Empty environment is a form of null terminator for environments.
 */
Env emptyEnv();
void disposeEmptyEnv(Env);
ExpVal applyEmptyEnv(Env, char*);

/**
 * Extended environment indicates the value of a variable.
 */
Env extendEnv(char*, ExpVal, Env);
void disposeExtendEnv(Env);
ExpVal applyExtendEnv(Env, char*);

#endif
