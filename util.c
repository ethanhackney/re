#include "util.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

void
die(const char *fmt, ...)
{
        va_list args;
        int error = 0;

        error = errno;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
        fprintf(stderr, ": %s\n", strerror(error));
        exit(1);
}
