#include "environments.h"
#include "expvals.h"
#include "environmentset.h"

#include <stdlib.h>
#include <string.h>

ApplyEnv applyEnvs[] = {
    applyEmptyEnv,
    applyExtendEnv
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

Env extendEnv(char *id, ExpVal val, Env subenv) {
    void **args = (void**) malloc(2 * sizeof(void*));
    args[0] = strdup(id);
    args[1] = val;
    return build_env(EXTEND_ENV, args, subenv);
}
void disposeExtendEnv(Env env) {
    void **args = env->args;
    // Dispose of the variable name
    free(args[0]);
    
    //Dispose of the saved expressed value
    dispose_expval((ExpVal) args[1]);

    free(args);
    dispose_env(env->subenv);
    free(env);
}
ExpVal applyExtendEnv(Env env, char* id) {
    void **args = (void*) env->args;
    char *record_id = (char*) args[0];
    
    // Return value if in this record, else continue the search.
    return strcmp(record_id, id)
            ? apply_env(env->subenv, id)
            : clone_expval((ExpVal) args[1]);
}

void (*envDisposers[])(Env) = {
    disposeEmptyEnv,
    disposeExtendEnv
};
void dispose_env(Env env) {
    if (env)
        envDisposers[env->type](env);
}

Env init_env() {
    return emptyEnv();
}


