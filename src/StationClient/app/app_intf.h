#ifndef APP_INTF_H
#define APP_INTF_H

#include "sys/platform.h"

#include "station_def.h"

typedef void (*client_online_function)(stationclient_t *);

typedef struct _app_intf
{
    client_online_function client_online;
} app_intf_t;

app_intf_t *app_intf_instance();

#endif // APP_INTF_H
