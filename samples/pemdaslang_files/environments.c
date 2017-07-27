#include "environments.h"
#include "expvals.h"
#include "environmentset.h"

#include <stdlib.h>
#include <string.h>

ApplyEnv applyEnvs[] = {
    applyEmptyEnv
};
ExpVal apply_env(Env env, char *id) {
    // Find and evaluate the correct environment applier.
    return applyEnvs[env->type](env, id);
}

Env emptyEnv() {
    return build_env(EMPTY_ENV, NULL, NULL);
}
void disposeEmptyEnv(Env env) {
    free(env);
}
ExpVal applyEmptyEnv(Env env, char* id) {
    // Variable not found
    return NULL;
}

void (*envDisposers[])(Env) = {
    disposeEmptyEnv
};
void dispose_env(Env env) {
    if (env)
        envDisposers[env->type](env);
}

Env init_env() {
    return emptyEnv();
}


