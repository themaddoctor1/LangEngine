#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

/**
 * To define our expressions, we need to have some form of structure
 * to form our syntax trees.
 */
struct expression {
    int type; /* A name used for parsing the type of expression */
    void *args; /* The subexpressions that make up the expression */
};
typedef struct expression* Exp;

/**
 * To remember the values of expressions, we need an environment that
 * tracks  the values of existing variables.
 */
struct environment {
    int type; /* A value to distinguish between the types of environments */
    void *args;
};
typedef struct environment* Env;

/**
 * To store values, we need to have a wrapper that stores the value.
 */
struct expval {
    int type; /* The name of the expval template used */
    void *value;
};
typedef struct expval* ExpVal;

#endif
