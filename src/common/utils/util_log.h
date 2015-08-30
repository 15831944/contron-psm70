#ifndef UTIL_LOG_H
#define UTIL_LOG_H

#include "sys/platform.h"

#ifdef  __cplusplus
extern "C" {
#endif

void jk_log(const char *fmt, ...);

#ifdef  __cplusplus
}
#endif

#endif // UTIL_LOG_H
