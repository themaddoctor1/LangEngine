#include "langio.h"

#include <stdio.h>

int debug_printf(char *str, ...) {
    va_list args;
    int n = 0;

    va_start(args, str);
    
    n += printf("\x1b[33m");
    n += vprintf(str, args);
    n += printf("\x1b[0m");

    va_end(args);

    return n;
}


