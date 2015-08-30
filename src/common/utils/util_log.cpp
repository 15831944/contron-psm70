#include "util_log.h"

void jk_log(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    char logstr[10240+1];
    vsnprintf(logstr, 10240, fmt, argp);
    printf(logstr);
    va_end(argp);
}
