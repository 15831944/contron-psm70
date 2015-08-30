#ifndef PSM_INTF_H
#define PSM_INTF_H

#include "psm_def.h"

typedef struct _psm_intf
{
    psm_device          server_device_unlock;
    psm_device_list     serve_lock_device_result;
    psm_device_list     server_unlock_device_result;

    psm_device          client_device_unlock;
    psm_device_list     client_send_device_unlock_result;
    psm_device_list     client_send_device_lock_result;

    psm_device          device_state_change;
    psm_device          device_lock_state_change;

    psm_request_data    client_request_data;
    psm_request_data    server_request_data;

    psm_device_list     server_request_device_unlock;
    psm_device_list     server_request_device_lock;

    psm_request_data    server_send_request_sync_time;
    psm_request_data    client_send_request_sync_time;

    psm_request_data    server_request_sync_time;
    psm_request_data    client_request_sync_time;

    psm_sync_time       server_send_reply_sync_time;
    psm_sync_time       client_send_reply_sync_time;

    psm_sync_time       server_reply_sync_time;
    psm_sync_time       client_reply_sync_time;

    psm_device_list     jk2sub_send_ticket;
    psm_device_list     server_send_ticket;

    psm_device_list     client_send_yk_device_state;
    psm_device_list     jk2main_yk_device_state;

    psm_device_list     server_send_lock_device;
    psm_device_list     server_send_unlock_device;
    psm_device_list     client_send_lock_device_result;
    psm_device_list     client_send_unlock_device_result;

    psm_request_data    client_lock_all_device;
    psm_request_data    client_unlock_all_device;

    psm_request_data    server_send_lock_all_device;
    psm_request_data    server_send_unlock_all_device;
    psm_device_all      client_send_lock_all_device_result;
    psm_device_all      client_send_unlock_all_device_result;
    psm_device_all      server_lock_all_device_result;
    psm_device_all      server_unlock_all_device_result;

} psm_intf_t;

psm_intf_t *psm_intf_instance();
void psm_intf_init();

#endif // PSM_INTF_H
