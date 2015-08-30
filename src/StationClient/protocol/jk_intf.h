#ifndef JK_INTF_H
#define JK_INTF_H

#include "sys/platform.h"

#include "jk_def.h"
#include "station_def.h"


typedef void (*device_function)(int,jk_device_list_t);
typedef void (*device_result_function)(int,jk_device_list_t);
typedef void (*device_all_function)(int);
typedef jk_device_list_t *(*get_station_device_function)(int);
typedef void (*device_update_function)(int,jk_device_list_t);
typedef void (*client_update_function)(stationclient_t *,jk_device_list_t);
typedef void (*request_data_function)(stationclient_t *);
typedef void (*reply_sync_time_function)(stationclient_t *, jk_time_t *time);
typedef void (*device_all_result_function)(stationclient_t *, int result);

typedef struct _jk_intf
{
    device_function             client_unlock_device;
    device_function             client_lock_device;
    device_result_function      server_unlock_device_result;
    device_result_function      server_lock_device_result;
    device_all_function         client_unlock_all_device;
    device_all_function         client_lock_all_device;
    get_station_device_function client_device_get;
    client_update_function      device_update;
    client_update_function      device_update_state;
    client_update_function      device_update_lock_state;
    device_update_function      server_send_ticket;
    request_data_function       request_sync_time;
    reply_sync_time_function    reply_sync_time;
    device_function             server_yk_device_state;
    request_data_function       client_yk_device_state;
    device_all_result_function  server_lock_all_device_result;
    device_all_result_function  server_unlock_all_device_result;
} jk_intf_t;

jk_intf_t *jk_intf_instance();

#endif // JK_INTF_H
