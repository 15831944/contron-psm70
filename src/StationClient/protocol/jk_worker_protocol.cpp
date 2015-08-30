#include "jk_worker_protocol.h"

#include "jk_core.h"
#include "jk_intf.h"

#include "jk_worker_client.cpp"
#include "jk_worker_server.cpp"

void jk_client_heartbeat(stationclient_t *client, jk_command_t *command)
{
    if(!jk_command_check_cmd(command, PRL_CMD_HEARTBEAT))
    {
        return;
    }
    int back = jk_command_check_back(command, PRL_CMD_HEARTBEAT);
    UCHAR error_no = PRL_ACK_ERROR_NONE;
    if(!jk_command_check_fnn(command, PRL_FNN_HEARTBEAT))
    {
        error_no = PRL_ACK_ERROR_FNN;
    }
    if(!back)
    {
        jk_command_t *ack = jk_command_ack(command, error_no);
        jk_command_send(client, ack);
        jk_command_free(&ack);
    }
    stationclient_heartbeat_update(client, back, command->source_address);
}

void jk_client_request_data_count(stationclient_t *client, jk_command_t *command)
{
    if(!jk_command_check_cmd(command, PRL_CMD_REQUEST_DATA))
    {
        return;
    }
    if(!jk_command_check_fnn(command, PRL_FNN_REQUEST_DATA_COUNT))
    {
        return;
    }
    if(!jk_command_check_data_struct(command))
    {
        return;
    }
    int back = jk_command_check_back(command, PRL_CMD_REQUEST_DATA);
    if(!back)
    {
        jk_client_data_count_send(client);
        return;
    }
    UCHAR count = (UCHAR)(*(command->data+3));
    if(count > 0)
    {
        jk_server_request_data(client, 1);
    }
}

void jk_client_request_data(stationclient_t *client, jk_command_t *command)
{
    if(!jk_command_check_cmd(command, PRL_CMD_REQUEST_DATA))
    {
        return;
    }
    if(!jk_command_check_fnn(command, PRL_FNN_REQUEST_DATA))
    {
        return;
    }
    if(!jk_command_check_data_struct(command))
    {
        return;
    }
    int back = jk_command_check_back(command, PRL_CMD_REQUEST_DATA);
    if(!back)
    {
        UCHAR index = (UCHAR)(*(command->data+3));
        jk_client_data_send(client, command, index);
        return;
    }
    jk_server_data_update(client, command, 0);
}


void jk_client_request_device_unlock_lock(stationclient_t *client, jk_command_t *command)
{
    if(!jk_command_check_cmd(command, PRL_CMD_REQUEST_DEVICE))
    {
        return;
    }
    bool unlock = jk_command_check_fnn(command, PRL_FNN_REQUEST_DEVICE_UNLOCK);
    bool lock = !jk_command_check_fnn(command, PRL_FNN_REQUEST_DEVICE_LOCK);
    if(!(unlock || lock))
    {
        return;
    }
    if(!jk_command_check_data_struct(command))
    {
        return;
    }
    int back = jk_command_check_back(command, PRL_CMD_REQUEST_DEVICE);
    if(!back)
    {
        //子站接收到请求设备解/闭锁
        device_function client_function = NULL;
        if(unlock)
        {
            client_function = jk_intf_instance()->client_unlock_device;
        }
        if(lock)
        {
            client_function = jk_intf_instance()->client_lock_device;
        }
        if(NULL==client_function)
        {
            return;
        }
        USHORT count = jk_get_ushort((char *)(command->data+3));
        char *p = command->data+5;
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        int size = count*sizeof(jk_device_t);
        jk_device_t *device = (jk_device_t *)malloc(size);
        if(NULL==device)
        {
            return;
        }
        memset(device, 0, size);
        for(int i=0; i<count; i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            UCHAR len = (UCHAR)(*(p++));
            d->device_len = len;
            memcpy(d->device, p, len);
            p += len;
            UCHAR op = (UCHAR)(*(p++));
            d->state_1 = op;
        }
        //先回ack
        UCHAR error_no = PRL_ACK_ERROR_NONE;
        jk_command_t *ack = jk_command_ack(command, error_no);
        jk_command_send(client, ack);
        jk_command_free(&ack);

        int station_no = command->source_address;
        client_function(station_no, list);

        free(list.device);

        return;
    }//back
    else
    {
        //主站接收到请求设备解锁处理结果
        device_result_function server_function = NULL;
        if(unlock)
        {
            server_function = jk_intf_instance()->server_unlock_device_result;
        }
        if(lock)
        {
            server_function = jk_intf_instance()->server_lock_device_result;
        }
        if(NULL==server_function)
        {
            return;
        }
        USHORT count = jk_get_ushort((char *)(command->data+3));
        char *p = command->data+5;
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        int size = count*sizeof(jk_device_t);
        jk_device_t *device = (jk_device_t *)malloc(size);
        if(NULL==device)
        {
            return;
        }
        memset(device, 0, size);
        for(int i=0; i<count; i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            UCHAR len = (UCHAR)(*(p++));
            d->device_len = len;
            memcpy(d->device, p, len);
            p += len;
            UCHAR op = (UCHAR)(*(p++));
            d->state_1 = op;
            UCHAR result = (UCHAR)(*(p++));
            d->state_2 = result;
        }
        //先回ack
        UCHAR error_no = PRL_ACK_ERROR_NONE;
        jk_command_t *ack = jk_command_ack(command, error_no);
        jk_command_send(client, ack);
        jk_command_free(&ack);
        //再由PSM系统处理请求设备解锁处理结果
        server_function(command->source_address, list);
        free(list.device);
    }
}

void jk_client_request_unlock_all_device(stationclient_t *client, jk_command_t *command)
{
    UN_USE(client);
    if(!jk_command_check_cmd(command, PRL_CMD_REQUEST_DEVICE))
    {
        return;
    }
    if(!jk_command_check_fnn(command, PRL_FNN_REQUEST_DEVICE_UNLOCK_ALL))
    {
        return;
    }
//    if(!jk_command_check_data_struct(command))
//    {
//        return;
//    }
    int back = jk_command_check_back(command, PRL_CMD_REQUEST_DEVICE);
    if(!back)
    {
        //子站收到请求设备全解锁
        if(NULL==jk_intf_instance()->client_unlock_all_device)
        {
            return;
        }
        int station = command->source_address;
        jk_intf_instance()->client_unlock_all_device(station);
    }
    else
    {
        //主站收到子站发送的设备全解锁结果
        UCHAR result = (UCHAR)(*(command->data+3));
        if(NULL==jk_intf_instance()->server_unlock_all_device_result)
        {
            return;
        }
        jk_intf_instance()->server_unlock_all_device_result(client, result);
    }
}


void jk_client_request_lock_all_device(stationclient_t *client, jk_command_t *command)
{
    UN_USE(client);
    if(!jk_command_check_cmd(command, PRL_CMD_REQUEST_DEVICE))
    {
        return;
    }
    if(!jk_command_check_fnn(command, PRL_FNN_REQUEST_DEVICE_LOCK_ALL))
    {
        return;
    }
//    if(!jk_command_check_data_struct(command))
//    {
//        return;
//    }
    int back = jk_command_check_back(command, PRL_CMD_REQUEST_DEVICE);
    if(!back)
    {
        //子站收到请求设备全解锁
        if(NULL==jk_intf_instance()->client_lock_all_device)
        {
            return;
        }
        int station = command->source_address;
        jk_intf_instance()->client_lock_all_device(station);
    }
    else
    {
        //主站收到子站发送的设备全闭锁结果
        UCHAR result = (UCHAR)(*(command->data+3));
        if(NULL==jk_intf_instance()->server_lock_all_device_result)
        {
            return;
        }
        jk_intf_instance()->server_lock_all_device_result(client, result);
    }
}


void jk_client_upload_device_state(stationclient_t *client, jk_command_t *command)
{
    if(!jk_command_check_cmd(command, PRL_CMD_REQUEST_DATA))
    {
        return;
    }
    if(!jk_command_check_fnn(command, PRL_FNN_UPLOAD_DEVICE_STATE))
    {
        return;
    }
    if(!jk_command_check_data_struct(command))
    {
        return;
    }
    int back = jk_command_check_back(command, PRL_CMD_REQUEST_DATA);
    if(!back)
    {
        jk_server_data_update(client, command, 1);
    }
}


void jk_client_upload_device_lock_state(stationclient_t *client, jk_command_t *command)
{
    if(!jk_command_check_cmd(command, PRL_CMD_REQUEST_DATA))
    {
        return;
    }
    if(!jk_command_check_fnn(command, PRL_FNN_UPLOAD_DEVICE_LOCK_STATE))
    {
        return;
    }
    if(!jk_command_check_data_struct(command))
    {
        return;
    }
    int back = jk_command_check_back(command, PRL_CMD_REQUEST_DATA);
    if(!back)
    {
        jk_server_data_update(client, command, 2);
    }
}


void jk_client_send_ticket(stationclient_t *client, jk_command_t *command)
{
    if(!jk_command_check_cmd(command, PRL_CMD_TICKET))
    {
        return;
    }
    if(!jk_command_check_fnn(command, PRL_FNN_SEND_TICKET))
    {
        return;
    }
    if(!jk_command_check_data_struct(command))
    {
        return;
    }
    int back = jk_command_check_back(command, PRL_CMD_TICKET);
    if(!back)
    {
        if(NULL==jk_intf_instance()->server_send_ticket)
        {
            return;
        }

        USHORT count = jk_get_ushort((char *)(command->data+3));
        int size = count*sizeof(jk_device_t);
        jk_device_t *device = (jk_device_t *)malloc(size);
        if(NULL==device)
        {
            return;
        }
        memset(device, 0, size);
        jk_device_list_t list;
        list.count = count;
        list.device = device;

        char *p = command->data+5;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            UCHAR len = (UCHAR)(*p++);
            memcpy(d->device, p, len);
            p += len;
            d->state_1 = (UCHAR)(*p++);
            d->state_2 = (UCHAR)(*p++);
        }

        //回ack
        UCHAR error_no = PRL_ACK_ERROR_NONE;
        jk_command_t *ack = jk_command_ack(command, error_no);
        jk_command_send(client, ack);
        jk_command_free(&ack);

        int station = command->source_address;
        jk_intf_instance()->server_send_ticket(station, list);
        free(list.device);
    }

}


void jk_client_request_sync_time(stationclient_t *client, jk_command_t *command)
{
    if(!jk_command_check_cmd(command, PRL_CMD_TIME))
    {
        return;
    }
    if(!jk_command_check_fnn(command, PRL_FNN_SYNC_TIME))
    {
        return;
    }
//    if(!jk_command_check_data_struct(command))
//    {
//        return;
//    }
    int back = jk_command_check_back(command, PRL_CMD_TIME);
    if(!back)
    {
        request_data_function func = jk_intf_instance()->request_sync_time;
        if(NULL!=func)
        {
            func(client);
        }
        return;
    }
    char *p = command->data+2;
    jk_time_t time;
    memset(&time, 0, sizeof(time));
    time.year = jk_get_ushort(p);
    p+=2;
    time.month = (UCHAR)(*(p++));
    time.day = (UCHAR)(*(p++));
    time.hour = (UCHAR)(*(p++));
    time.minute = (UCHAR)(*(p++));
    time.second = (UCHAR)(*(p++));

    reply_sync_time_function func = jk_intf_instance()->reply_sync_time;
    if(NULL!=func)
    {
        func(client, &time);
    }
}


void jk_client_upload_yk_device_state(stationclient_t *client, jk_command_t *command)
{
    if(!jk_command_check_cmd(command, PRL_CMD_REQUEST_DATA))
    {
        return;
    }
    if(!jk_command_check_fnn(command, PRL_FNN_UPLOAD_YK_DEVICE_STATE))
    {
        return;
    }
    if(!jk_command_check_data_struct(command))
    {
        return;
    }
    int back = jk_command_check_back(command, PRL_CMD_REQUEST_DATA);
    if(!back)
    {
        device_function func =  jk_intf_instance()->server_yk_device_state;
        if(NULL==func)
        {
            return;
        }

        USHORT count = jk_get_ushort((char *)(command->data+3));
        int size = count*sizeof(jk_device_t);
        jk_device_t *device = (jk_device_t *)malloc(size);
        if(NULL==device)
        {
            return;
        }
        memset(device, 0, size);
        jk_device_list_t list;
        list.count = count;
        list.device = device;

        char *p = command->data+5;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            UCHAR len = (UCHAR)(*p++);
            memcpy(d->device, p, len);
            p += len;
            d->state_1 = (UCHAR)(*p++);
            d->state_2 = (UCHAR)(*p++);
        }

        //回ack
        UCHAR error_no = PRL_ACK_ERROR_NONE;
        jk_command_t *ack = jk_command_ack(command, error_no);
        jk_command_send(client, ack);
        jk_command_free(&ack);

        int station = command->source_address;
        func(station, list);
        free(list.device);
        return;
    }
    //接收到ACK,发下一组遥控闭锁状态
    if(NULL!=jk_intf_instance()->client_yk_device_state)
    {
        jk_intf_instance()->client_yk_device_state(client);
    }

}
