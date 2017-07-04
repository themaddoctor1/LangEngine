#include "environments.h"
#include "structures.h"

#include <stdlib.h>

Env build_env(int type, void* args, Env subenv) {
    Env env = (Env) malloc(sizeof(struct environment));
    env->args = args;
    env->type = type;
    env->subenv = subenv;
    return env;
}


