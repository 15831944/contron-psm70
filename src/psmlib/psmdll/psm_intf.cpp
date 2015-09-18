#include "psm_intf.h"

#include "sys/platform.h"
#include "utils/utils.h"

#ifdef HAS_PSM
#include "stationlibrary.h"
#endif

static int inited = NUMBER_FALSE;
static psm_intf_t self;

void _log(char *text)
{
    jk_log(text);
}

void psm_intf_init()
{
#ifdef HAS_PSM
    dll_init();

    //服务
    set_log(_log);
//    set_sub2jk_device_unlock_result(self.client_send_device_unlock_result);
//    set_sub2jk_device_lock_result(self.client_send_device_lock_result);
    set_sub2jk_request_data(self.client_request_data);
    set_sub2jk_send_request_sync_time(self.client_send_request_sync_time);
    set_sub2jk_send_reply_sync_time(self.client_send_reply_sync_time);
    set_sub2jk_send_yk_device_state(self.client_send_yk_device_state);
    set_sub2jk_send_lock_device_result(self.client_send_lock_device_result);
    set_sub2jk_send_unlock_device_result(self.client_send_unlock_device_result);
    set_sub2jk_send_lock_all_device_result(self.client_send_lock_all_device_result);
    set_sub2jk_send_unlock_all_device_result(self.client_send_unlock_all_device_result);

    set_main2jk_request_data(self.server_request_data);
    set_main2jk_send_request_sync_time(self.server_send_request_sync_time);
    set_main2jk_send_reply_sync_time(self.server_send_reply_sync_time);
    set_main2jk_send_ticket(self.server_send_ticket);
    set_main2jk_send_lock_device(self.server_send_lock_device);
    set_main2jk_send_unlock_device(self.server_send_unlock_device);
    set_main2jk_send_lock_all_device(self.server_send_lock_all_device);
    set_main2jk_send_unlock_all_device(self.server_send_unlock_all_device);

    //外部系统
//    psm_intf_instance()->client_device_unlock = client_device_unlock;
//    psm_intf_instance()->server_send_ticket = s_receive_ticket;
    psm_intf_instance()->server_request_device_unlock = jk2sub_unlock_device;
    psm_intf_instance()->server_request_device_lock = jk2sub_lock_device;

    psm_intf_instance()->server_request_sync_time = jk2main_request_sync_time;
    psm_intf_instance()->client_request_sync_time = jk2sub_request_sync_time;
    psm_intf_instance()->server_reply_sync_time = jk2main_reply_sync_time;
    psm_intf_instance()->client_reply_sync_time = jk2sub_reply_sync_time;
    psm_intf_instance()->jk2sub_send_ticket = jk2sub_send_ticket;
    psm_intf_instance()->serve_lock_device_result = jk2main_lock_device_result;
    psm_intf_instance()->server_unlock_device_result = jk2main_unlock_device_result;
    psm_intf_instance()->client_lock_all_device = jk2sub_lock_all_device;
    psm_intf_instance()->client_unlock_all_device = jk2sub_unlock_all_device;
    psm_intf_instance()->server_lock_all_device_result = jk2main_lock_all_device_result;
    psm_intf_instance()->server_unlock_all_device_result = jk2main_unlock_all_device_result;
    set_value(5);
#endif

}

psm_intf_t *psm_intf_instance()
{
    if(!inited)
    {
        memset(&self, 0, sizeof(psm_intf_t));
        inited = NUMBER_TRUE;
    }
    return &self;
}
