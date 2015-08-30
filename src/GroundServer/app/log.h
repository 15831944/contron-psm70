#ifndef LOG_H
#define LOG_H

void ground_log_init();
void ground_log_free();
void ground_log_enter();
void ground_log_leave();
void ground_log(const char *fmt, ...);

#endif // LOG_H
