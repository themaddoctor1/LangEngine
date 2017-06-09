#include "expvals.h"

#include <stdlib.h>

ExpVal buildExpVal(int type, void *val) {
    ExpVal expval = (ExpVal) malloc(sizeof(struct expval));
    expval->value = val;
    expval->type = type;
    return expval;
}


