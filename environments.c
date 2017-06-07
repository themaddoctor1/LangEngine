#include "environments.h"
#include "structures.h"

#include <stdlib.h>

Env newBlankEnv(void* args, Env subenv) {
    Env env = (Env) malloc(sizeof(struct environment));
    env->args = args;
    env->subenv = subenv;
    return env;
}

Env buildEnv(int type, void* args, Env subenv) {
    Env env = (Env) malloc(sizeof(struct environment));
    env->args = args;
    env->type = type;
    env->subenv = subenv;
    return env;
}

void disposeEnv(Env env) {
    free(env->args);
    free(env);
}


