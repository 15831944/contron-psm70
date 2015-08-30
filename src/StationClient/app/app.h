#ifndef APP_H
#define APP_H

#include "sys/platform.h"

#include "station_def.h"

typedef struct
{
    stationserver_t *server;            //主站
    stationclient_t *client;            //子站
    pthread_t guard_thread[1];          //监护线程，实现app主线程循环
} app_t;

void app_start();
void app_close(app_t *app);

#endif // APP_H
