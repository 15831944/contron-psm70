#include "station_def.h"
#include "jk_def.h"

#include "psmlib/psmdll/psm_sys.h"
#include "jk_function.h"
#include "utils/utils.h"
#include "station_client.h"
#include "jk_worker_common.h"
#include "jk_intf.h"


UCHAR jk_client_data_count_calc(int station_no)
{
    UCHAR device_count = 0;
    if(NULL!=jk_intf_instance()->client_device_get)
    {
        jk_device_list_t *list = jk_intf_instance()->client_device_get(station_no);
        if(NULL!=list)
        {
            device_count = list->count;
            if(0<device_count)
            {
                free(list->device);
            }
            free(list);
        }
    }
    UCHAR result = (device_count+DEVICE_PER_FRAME-1)/DEVICE_PER_FRAME;
    return result;
}

void jk_client_data_count_send(stationclient_t *client)
{
    UCHAR count = jk_client_data_count_calc(client->station_no);

    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    static UCHAR data[] = {PRL_CMD_REQUEST_DATA|PRL_CT_BACK, PRL_FNN_REQUEST_DATA_COUNT, 1, count};
    cmd->data_size = sizeof(data);
    cmd->data = (char *)malloc(cmd->data_size);
    if(NULL!=cmd->data)
    {
        memset(cmd->data, 0, cmd->data_size);
        memcpy(cmd->data, data, cmd->data_size);

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);
}

void jk_client_data(int station_no, int frame_index, char **buffer, int &buffer_count, USHORT &frame_count)
{
    if(NULL==jk_intf_instance()->client_device_get)
    {
        return;
    }
    USHORT count = 0;
    int data_size = sizeof(USHORT);
    char *data = (char *)malloc(data_size);
    if(NULL==data)
    {
        return;
    }
    memset(data, 0, data_size);
    jk_device_list_t *list = jk_intf_instance()->client_device_get(station_no);
    int device_count = list->count;
    count = DEVICE_PER_FRAME;
    if(frame_index*DEVICE_PER_FRAME>device_count)
    {
        count = device_count%DEVICE_PER_FRAME;
    }
    char *p = data;
    jk_set_ushort(&p, count);

    for(int i=0;i<count;i++)
    {
        jk_device_t *device = (jk_device_t *)(list->device+(frame_index-1)*DEVICE_PER_FRAME+i);
        int size = 4+device->device_len; //4=1(设备主编号长度)+1(设备状态)+1(设备闭锁状态)+1(确保字符串以零结尾)
        int new_size = data_size+size;

        data = (char *)realloc(data, new_size);
        if(NULL==data)
        {
            data_size = 0;
            count = 0;
            break;
        }

        char *p = (char *)(data+data_size);
        memset(p, 0, size);
        jk_set_uchar(&p, device->device_len+1);
        jk_set_buffer(&p, device->device, device->device_len);
        p++;  //字符串以零结尾
        jk_set_uchar(&p, device->state_1);
        jk_set_uchar(&p, device->state_2);

        data_size = new_size;
    }

    *buffer = data;
    buffer_count = data_size;
    frame_count = count;
}

void jk_client_data_send(stationclient_t *client, jk_command_t *command, UCHAR frame_index)
{
    UCHAR count = jk_client_data_count_calc(client->station_no);
    if(count<frame_index || 1>frame_index)
    {
        jk_command_t *ack = jk_command_ack(command, PRL_ACK_ERROR_DATA);
        jk_command_send(client, ack);
        jk_command_free(&ack);
        return;
    }
    char *buffer = NULL;
    int buffer_size = 0;
    USHORT device_count = 0;
    jk_client_data(client->station_no, frame_index, &buffer, buffer_size, device_count);
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_REQUEST_DATA|PRL_CT_BACK, PRL_FNN_REQUEST_DATA, PRL_STRUCT_NAME};
    int data_size = sizeof(data);
    int size = data_size;
    size += (1+buffer_size);  //1(帧号)
    cmd->data = (char *)malloc(size);
    char *p = cmd->data;
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;

        p += data_size;
        jk_set_uchar(&p, frame_index);
//        jk_set_ushort(&p, device_count);
        if(0<buffer_size)
        {
            jk_set_buffer(&p, buffer, buffer_size);
            free(buffer);
        }

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);

}
