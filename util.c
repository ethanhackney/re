#include "util.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

void
die(const char *fmt, ...)
{
        va_list va;
        int error = errno;

        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
        fprintf(stderr, ": %s\n", strerror(error));
        exit(EXIT_FAILURE);
}
