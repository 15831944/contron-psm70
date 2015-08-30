#include "app.h"

#include "station_server.h"
#include "station_client.h"

#include "protocol/jk_function.h"
#include "protocol/jk_intf.h"
#include "psm_intf.h"
#include "psm_sys.h"

#include "config.h"
#include "app_intf.h"

typedef struct _jk_ticket_item
{
    UCHAR       device_len;
    char        *device;
    UCHAR       before_state;
    UCHAR       after_state;
} jk_ticket_item_t;

typedef struct _jk_ticket
{
    USHORT                  item_count;
    jk_ticket_item_t       *item;
} jk_ticket_t;

static int inited = 0;
static app_t app;

#define ERROR_CLIENT_OFFLINE -1;
int jk_server_ticket_send(stationclient_t *client, jk_ticket_t *ticket)
{
    return NUMBER_TRUE;
}

int app_server_ticket_send(int station, jk_ticket_t *ticket)
{
    stationclient_t *client = stationserver_find(app.server, station);
    if(NULL==client)
    {
        return ERROR_CLIENT_OFFLINE;
    }
    int ret = jk_server_ticket_send(client, ticket);
    return ret;
}

void app_server_device_unlock(int station, int device_len, char *device, int operate)
{
    stationclient_t *client = stationserver_find(app.server, station);
    jk_server_device_unlock(client, device_len, device, operate);
}

void sub2jk_send_unlock_device_result(int station_no, jk_device_list_t *devices)
{
    stationclient_t *client = stationclient_find(app.client, station_no);
    jk_send_device_unlock_result(client, devices);
}

void sub2jk_send_lock_device_result(int station_no, jk_device_list_t *devices)
{
    stationclient_t *client = stationclient_find(app.client, station_no);
    jk_send_device_lock_result(client, devices);
}

void app_client_request_data_all(int station)
{
    stationclient_t *client = stationclient_find(app.client, station);
    jk_request_data_count(client);
}

void app_device_update(stationclient_t *client, jk_device_list_t list)
{
    int station = stationclient_id(client);
    int count = list.count;
    if(stationclient_in_server(client))
    {
        psm_fix_lock_device(station, list);
    }
    for(int i=0;i<count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list.device+i);
        psm_device_update_state(station, d->device_len, d->device, d->state_1);
        psm_device_update_lock_state(station, d->device_len, d->device, d->state_2);
    }
}

/*!
 * \brief app_device_update_lock_state
 * \param client
 * \param list
 * \note  优特要求，主站向子站发送设备闭锁个数为0时，子站将主站所有已闭锁的设备设置为解锁状态
 */
void app_device_update_lock_state(stationclient_t *client, jk_device_list_t list)
{
    int station = stationclient_id(client);
    int count = list.count;
    if(stationclient_in_server(client))
    {
        if(0==count)
        {
            psm_clear_lock_device(station);
            return;
        }
        else
        {
            psm_fix_lock_device(station, list);
        }
    }
    for(int i=0;i<count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list.device+i);
        psm_device_update_lock_state(station, d->device_len, d->device, d->state_2);
    }

}

void app_device_update_state(stationclient_t *client, jk_device_list_t list)
{
    int station = stationclient_id(client);
    int count = list.count;
    for(int i=0;i<count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list.device+i);
        psm_device_update_state(station, d->device_len, d->device, d->state_1);
    }

}

jk_device_list_t *app_get_station_device(int station_no)
{
    jk_device_list_t *result = psm_get_device(station_no);
    /*
    device_t *device = psm_device_get(station_no);
    if(NULL!=device)
    {
        int count = psm_device_count(station_no);
        if(0<count)
        {
            result = (jk_device_list_t *)malloc(sizeof(jk_device_list_t));
            if(NULL!=result)
            {
                memset(result, 0, sizeof(jk_device_list_t));

                result->count = count;

                int size = count*sizeof(jk_device_t);
                result->device = (jk_device_t *)malloc(size);
                if(NULL!=result->device)
                {
                    memset(result->device, 0, size);
                    jk_device_t *d = result->device;
                    device_t *n = device;
                    while(NULL!=n)
                    {
                        size = MIN(sizeof(n->name), sizeof(d->device));
                        memcpy(d->device, n->name, size);
                        d->device_len =  strlen(d->device);
                        d->state_1 = n->value;
                        d->state_2 = n->lock_value;
                        d++;
                        n = (device_t *)n->next;
                    }
                }

            }
        }//count
//        device_free(&device);
    }//device
    */

    return result;
}

void app2jk_device_state_change(int station_no, int device_len, char *device, int state)
{
    stationclient_t *client = stationclient_find(app.client, station_no);
    jk_client_device_state_upload(client, device_len, device, state);


    client = NULL;
    do
    {
        client = stationserver_find(app.server, station_no, client);
        jk_client_device_state_upload(client, device_len, device, state);
    }while(NULL!=client);
}

void app2jk_device_lock_state_change(int station_no, int device_len, char *device, int state)
{
    stationclient_t *client = stationclient_find(app.client, station_no);
    jk_client_device_lock_state_upload(client, device_len, device, state);


    client = NULL;
    do
    {
        client = stationserver_find(app.server, station_no, client);
        jk_client_device_lock_state_upload(client, device_len, device, state);
    }while(NULL!=client);
}

void app2jk_client_request_data(int station_no)
{
    stationclient_t *client = stationclient_find(app.client, station_no);
    jk_request_data_count(client);
}

void jk2app_server_send_ticket(int station_no, jk_device_list_t devices)
{
    stationclient_t *client = stationclient_find(app.client, station_no);
    if(NULL==client)
    {
        return;
    }
    psm_receive_tikcet(station_no, &devices);
    if(NULL==psm_intf_instance()->jk2sub_send_ticket)
    {
        return;
    }
    psm_intf_instance()->jk2sub_send_ticket(station_no, &devices);

}

void jk2app_client_device_unlock(int station_no, jk_device_list_t operate)
{
    if(1>operate.count)
    {
        return;
    }
    jk_device_t *d = operate.device;
    //20150603, lgh, 王飞跃要求每次只处理一个设备解锁,不在遥控收表中的设备直接回失败
    {
        operate.count = 1;
        int in_table = psm_device_in_yk_table(station_no, d->device, d->device_len, 0);
        int can_operate = psm_device_can_operate(station_no, d->device, d->device_len, d->state_1);
        if((!in_table)||(!can_operate))
        {
            jk_device_t n;
            jk_device_list_t list;
            memset(&n, 0, sizeof(jk_device_t));
            list.count = 1;
            list.device = &n;
            memcpy(&n, d, sizeof(jk_device_t));
            n.state_2 = 2;

            sub2jk_send_unlock_device_result(station_no, &list);
            return;
        }

    }
    //直接处理设备解锁
    int ret = psm_device_operate(station_no, d->device, d->device_len, d->state_1);
    if(!ret)
    {
        jk_device_t n;
        jk_device_list_t list;
        memset(&n, 0, sizeof(jk_device_t));
        list.count = 1;
        list.device = &n;
        memcpy(&n, d, sizeof(jk_device_t));
        n.state_2 = 2;

        sub2jk_send_unlock_device_result(station_no, &list);
    }
    //外部系统处理设备解锁,目前只用于输出信息
    if(NULL==psm_intf_instance()->server_request_device_unlock)
    {
        return;
    }
    psm_intf_instance()->server_request_device_unlock(station_no, &operate);
}

void jk2app_client_device_lock(int station_no, jk_device_list_t operate)
{
    if(1>operate.count)
    {
        return;
    }
    jk_device_t *d = operate.device;
    //20150603, lgh, 王飞跃要求每次只处理一个设备解锁,不在遥控收表中的设备直接回失败
    {
        operate.count = 1;
        int in_table = psm_device_in_yk_table(station_no, d->device, d->device_len, 0);
        int can_operate = psm_device_can_operate(station_no, d->device, d->device_len, d->state_1);
        if((!in_table)||(!can_operate))
        {
            jk_device_t n;
            jk_device_list_t list;
            memset(&n, 0, sizeof(jk_device_t));
            list.count = 1;
            list.device = &n;
            memcpy(&n, d, sizeof(jk_device_t));
            n.state_2 = 2;

            sub2jk_send_lock_device_result(station_no, &list);
            return;
        }

    }
    //直接处理设备闭锁
    int ret = psm_device_operate(station_no, d->device, d->device_len, d->state_1);
    if(!ret)
    {
        jk_device_t n;
        jk_device_list_t list;
        memset(&n, 0, sizeof(jk_device_t));
        list.count = 1;
        list.device = &n;
        memcpy(&n, d, sizeof(jk_device_t));
        n.state_2 = 2;

        sub2jk_send_lock_device_result(station_no, &list);
    }
    if(NULL==psm_intf_instance()->server_request_device_lock)
    {
        return;
    }
    psm_intf_instance()->server_request_device_lock(station_no, &operate);
}

void jk2app_client_device_lock_result(int station_no, jk_device_list_t operate)
{
    if(NULL==psm_intf_instance()->serve_lock_device_result)
    {
        return;
    }
    psm_intf_instance()->serve_lock_device_result(station_no, &operate);
}

void jk2app_client_device_unlock_result(int station_no, jk_device_list_t operate)
{
    if(NULL==psm_intf_instance()->server_unlock_device_result)
    {
        return;
    }
    psm_intf_instance()->server_unlock_device_result(station_no, &operate);
}

void main2jk_send_request_sync_time(int station_no)
{
    stationclient_t *client = stationserver_find(app.server, station_no);
    jk_request_sync_time(client);
}

void sub2jk_send_request_sync_time(int station_no)
{
    stationclient_t *client = stationclient_find(app.client, station_no);
    jk_request_sync_time(client);
}

void jk2app_request_sync_time(stationclient_t *client)
{
    stationclient_todo_cmd(client, CMD_SYNC_TIME);
    int station_no = stationclient_id(client);
    psm_request_data func = psm_intf_instance()->server_request_sync_time;
    if(stationclient_in_server(client))
    {
        func = psm_intf_instance()->client_request_sync_time;
    }
    if(NULL==func)
    {
        return;
    }
    func(station_no);
}

void jk2app_reply_sync_time(stationclient_t *client, jk_time_t *time)
{
    int station_no = stationclient_id(client);
    psm_sync_time func = psm_intf_instance()->client_reply_sync_time;
    if(stationclient_in_server(client))
    {
        func = psm_intf_instance()->server_reply_sync_time;
    }
    if(NULL==func)
    {
        return;
    }
    func(station_no, time);
}

void sub2jk_send_reply_sync_time(int station_no, jk_time_t *time)
{
    stationclient_t *client = stationclient_find(app.client, station_no);
    jk_reply_sync_time(client, time);
}

void main2jk_send_reply_sync_time(int station_no, jk_time_t *time)
{
    stationclient_t *client = NULL;
    do
    {
        client = stationserver_find_todo(app.server, station_no, CMD_SYNC_TIME, client);
        jk_reply_sync_time(client, time);
        stationserver_clear_todo(app.server, client, CMD_SYNC_TIME);
    }while(NULL!=client);
}

void main2jk_send_ticket(int station_no, jk_device_list_t *devices)
{
    stationclient_t *client = NULL;
    do
    {
        client = stationserver_find(app.server, station_no, client);
        jk_send_ticket(client, devices);
    }while(NULL!=client);

}

void jk2app_yk_device_state(int station_no, jk_device_list_t devices)
{
    if(NULL==psm_intf_instance()->jk2main_yk_device_state)
    {
        return;
    }
    psm_intf_instance()->jk2main_yk_device_state(station_no, &devices);
}

void jk2app_lock_all_device(int station_no)
{
    if(NULL==psm_intf_instance()->client_lock_all_device)
    {
        return;
    }
    psm_intf_instance()->client_lock_all_device(station_no);
}

void jk2app_unlock_all_device(int station_no)
{
    if(NULL==psm_intf_instance()->client_unlock_all_device)
    {
        return;
    }
    psm_intf_instance()->client_unlock_all_device(station_no);
}

void sub2jk_send_yk_device_state(int station_no, jk_device_list_t *devices)
{
    stationclient_t *client = stationclient_find(app.client, station_no);
    jk_send_yk_device_state(client, devices);
}

void sub2jk_send_lock_all_device_result(int station_no, int result)
{
    stationclient_t *client = stationclient_find(app.client, station_no);
    jk_send_lock_all_device_result(client, result);
}

void sub2jk_send_unlock_all_device_result(int station_no, int result)
{
    stationclient_t *client = stationclient_find(app.client, station_no);
    jk_send_unlock_all_device_result(client, result);
}

void main2jk_send_lock_device(int station_no, jk_device_list_t *devices)
{
    stationclient_t *client = NULL;
    do
    {
        client = stationserver_find(app.server, station_no, client);
        jk_send_lock_device(client, devices);
    }while(NULL!=client);
}

void main2jk_send_unlock_device(int station_no, jk_device_list_t *devices)
{
    stationclient_t *client = NULL;
    do
    {
        client = stationserver_find(app.server, station_no, client);
        jk_send_unlock_device(client, devices);
    }while(NULL!=client);
}

void main2jk_send_lock_all_device(int station_no)
{
    stationclient_t *client = NULL;
    do
    {
        client = stationserver_find(app.server, station_no, client);
        jk_send_lock_all_device(client);
    }while(NULL!=client);
}

void main2jk_send_unlock_all_device(int station_no)
{
    stationclient_t *client = NULL;
    do
    {
        client = stationserver_find(app.server, station_no, client);
        jk_send_unlock_all_device(client);
    }while(NULL!=client);
}

void jk2app_lock_all_device_result(stationclient_t *client, int result)
{
    int id = stationclient_id(client);
    if(NULL==psm_intf_instance()->server_lock_all_device_result)
    {
        return;
    }
    psm_intf_instance()->server_lock_all_device_result(id, result);
}

void jk2app_unlock_all_device_result(stationclient_t *client, int result)
{
    int id = stationclient_id(client);
    if(NULL==psm_intf_instance()->server_unlock_all_device_result)
    {
        return;
    }
    psm_intf_instance()->server_unlock_all_device_result(id, result);
}

void app_client_online(stationclient_t *client)
{
    int id = stationclient_id(client);
    if(stationclient_in_server(client))
    {
        jk_server_request_data_count(client);
    }
    else
    {
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        if(!psm_get_kbq(id, &list))
        {
            return;
        }
        client->yk_send_index = 1;
        jk_send_kbq(client, &list);
    }
}

void jk2app_send_kbq_next(stationclient_t *client)
{
    int id = stationclient_id(client);
    jk_device_list_t list;
    memset(&list, 0, sizeof(jk_device_list_t));
    if(!psm_get_kbq(id, &list))
    {
        return;
    }
    client->yk_send_index++;
    jk_send_kbq(client, &list);
}

THREAD_API app_guard_thread(void *param)
{
    app_t *app = (app_t *)param;
    if(NULL==app)
    {
        return NULL;
    }
    int idle = 500;
    while(true)
    {
        //FIXME:判断程序退出
        Sleep(idle);
    }
    printf("app exit...\n");
    return NULL;
}

void app_create_server(app_t *app)
{
    if(!config_instance()->server_enable)
    {
        return;
    }

    app->server = stationserver_new();
    if(NULL != app->server)
    {
        app->server->station_no = config_instance()->server_station_no;
        sprintf(app->server->conn[0].tcp[0].hostname, "%s", "0.0.0.0");
        app->server->conn[0].tcp[0].port = config_instance()->server_port;
        app->server->conn[0].tcp[0].local_if = NUMBER_TRUE;
    }
}

void app_create_client(app_t *app)
{
    if(!config_instance()->client_enable)
    {
        return;
    }
    stationclient_t *client = NULL;
    int count=config_instance()->client_count;
    for(int i=0;i<count;i++)
    {
        stationclient_t *c = stationclient_new();
        if(NULL==client)
        {
            app->client = c;
            client = c;
        }
        else
        {
            client->next = c;
            client = c;
        }

        c->server_no = config_instance()->client_server_id[i];
        c->station_no = config_instance()->client_id[i];
        sprintf(c->conn[0].tcp[0].hostname, "%s", config_instance()->client_ip[i]);
        c->conn[0].tcp[0].port = config_instance()->client_port[i];//5555;
    }
}

void app_start()
{
    if(!inited)
    {
        memset(&app, 0, sizeof(app_t));
        inited = NUMBER_TRUE;
    }

    if(!config_start())
    {
        return;
    }

    psm_config_t config;
    memset(&config, 0, sizeof(psm_config_t));
//    char ip[] = "127.0.0.1";
    config.rt_id = config_instance()->rt_id;
    config.wf_ip = config_instance()->wf_server_ip;
    config.wf_port = config_instance()->wf_server_port;
    psm_start(&config);

    app_intf_instance()->client_online = app_client_online;

    //初始化PSM系统接口
    psm_intf_instance()->client_send_device_lock_result         = sub2jk_send_lock_device_result;
//    psm_intf_instance()->server_device_unlock = app_server_device_unlock;
    psm_intf_instance()->device_state_change                    = app2jk_device_state_change;
    psm_intf_instance()->device_lock_state_change               = app2jk_device_lock_state_change;
    psm_intf_instance()->client_request_data                    = app2jk_client_request_data;
    psm_intf_instance()->client_send_request_sync_time          = sub2jk_send_request_sync_time;
    psm_intf_instance()->server_send_request_sync_time          = main2jk_send_request_sync_time;
    psm_intf_instance()->client_send_reply_sync_time            = sub2jk_send_reply_sync_time;
    psm_intf_instance()->server_send_reply_sync_time            = main2jk_send_reply_sync_time;
    psm_intf_instance()->server_send_ticket                     = main2jk_send_ticket;
    psm_intf_instance()->client_send_yk_device_state            = sub2jk_send_yk_device_state;
    psm_intf_instance()->server_send_lock_device                = main2jk_send_lock_device;
    psm_intf_instance()->server_send_unlock_device              = main2jk_send_unlock_device;
    psm_intf_instance()->client_send_lock_device_result         = sub2jk_send_lock_device_result;
    psm_intf_instance()->client_send_device_unlock_result       = sub2jk_send_unlock_device_result;
    psm_intf_instance()->server_send_lock_all_device            = main2jk_send_lock_all_device;
    psm_intf_instance()->server_send_unlock_all_device          = main2jk_send_unlock_all_device;
    psm_intf_instance()->client_send_lock_all_device_result     = sub2jk_send_lock_all_device_result;
    psm_intf_instance()->client_send_unlock_all_device_result   = sub2jk_send_unlock_all_device_result;
    psm_intf_init();

    //初始化集控接口
    jk_intf_instance()->client_unlock_device        = jk2app_client_device_unlock;
    jk_intf_instance()->client_lock_device          = jk2app_client_device_lock;
    jk_intf_instance()->client_device_get           = app_get_station_device;
    jk_intf_instance()->device_update               = app_device_update;
    jk_intf_instance()->device_update_state         = app_device_update_state;
    jk_intf_instance()->device_update_lock_state    = app_device_update_lock_state;
    jk_intf_instance()->server_send_ticket          = jk2app_server_send_ticket;
    jk_intf_instance()->request_sync_time           = jk2app_request_sync_time;
    jk_intf_instance()->reply_sync_time             = jk2app_reply_sync_time;
    jk_intf_instance()->server_yk_device_state      = jk2app_yk_device_state;
    jk_intf_instance()->client_lock_all_device      = jk2app_lock_all_device;
    jk_intf_instance()->client_unlock_all_device    = jk2app_unlock_all_device;
    jk_intf_instance()->server_lock_all_device_result = jk2app_lock_all_device_result;
    jk_intf_instance()->server_unlock_all_device_result = jk2app_unlock_all_device_result;
    jk_intf_instance()->client_yk_device_state      = jk2app_send_kbq_next;

    int ret,success;
    THREAD_CREATE(&app.guard_thread[0], app_guard_thread, &app, ret);
    success = ret;
    if(success)
    {
        app_create_server(&app);
        if(NULL != app.server)
        {
            stationserver_start(app.server);
        }
        app_create_client(&app);
        stationclient_t *client = app.client;
        while(NULL!=client)
        {
            stationclient_start(client);
            client = (stationclient_t *)client->next;
        }
        THREAD_RUN(app.guard_thread[0], true);
    }
    app_close(&app);
}


void app_close(app_t *app)
{
    stationclient_close(app->client);
}
