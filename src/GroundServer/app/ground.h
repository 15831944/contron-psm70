#ifndef GROUND_H
#define GROUND_H

#include "config.h"
#include "global.h"
#include "intf.h"

typedef struct
{
    int ready;  //启动标志
    int last_state_time;  //上次汇报地线状态时间
    CRITICAL_SECTION ground_critical;
    pthread_t state_thread[1];
    pthread_t console_thread[1];
    pthread_t guard_thread[1];
    config_t config[1];
} ground_t;

ground_t *ground_app();

ground_t *ground_new(config_t *config);

void ground_start(ground_t *ground);
void ground_close(ground_t *ground);
void ground_state_report(ground_t *ground);
int ground_ready_get(ground_t *ground);
void ground_ready_set(ground_t *ground, int ready);
void ground_enter(ground_t *ground);
void ground_leave(ground_t *ground);


THREAD_API ground_state_thread(void *param);
THREAD_API ground_console_thread(void *param);
THREAD_API ground_guard_thread(void *param);


#endif // GROUND_H
