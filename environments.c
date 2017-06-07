#include "environments.h"

Env newBlankEnv(void* args) {
    Env env = (Env) malloc(sizeof(struct environment));
    env->args = args;
    return exp;
}

Env buildEnv(int type, void* args) {
    Env env = (Env) malloc(sizeof(struct environment));
    env->args = args;
    env->type = type;
    return exp;
}

void disposeEnv(Env env) {
    free(env->args);
    free(env);
}


