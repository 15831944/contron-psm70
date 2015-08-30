#ifndef LOG_H
#define LOG_H

void log_init();
void log_free();
void log_enter();
void log_leave();
void _log(const char *fmt, ...);

#endif // LOG_H
