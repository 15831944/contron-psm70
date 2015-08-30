#include "station_def.h"
#include "jk_def.h"
#include "jk_worker_common.h"
#include "jk_function.h"
#include "jk_intf.h"

void jk_server_request_data(stationclient_t *client, UCHAR frame_index)
{
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    static UCHAR data[] = {PRL_CMD_REQUEST_DATA, PRL_FNN_REQUEST_DATA, 1, frame_index};
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
/*!
 * \brief jk_get_device
 * \param buffer
 * \param mode 取值模式：0-全遥信，1-遥信，2-闭锁
 * \return
 */
jk_device_t *jk_get_device(char **buffer, int mode=0)
{
    jk_device_t *result = NULL;
    char *p = *buffer;

    UCHAR len = (UCHAR)(*(p++));
    if(len>0)
    {
        char *device = (char *)malloc(len);
        if(NULL!=device)
        {
            memset(device, 0, len);
            memcpy(device, p, len);
            p += len;

            UCHAR state = (0==mode||1==mode)?(UCHAR)(*(p++)):0;
            UCHAR lock_state = (0==mode||2==mode)?(UCHAR)(*(p++)):0;

            result = (jk_device_t *)malloc(sizeof(jk_device_t));
            if(NULL!=result)
            {
                memset(result, 0, sizeof(jk_device_t));
                result->device_len = len;
                int size = MIN(sizeof(result->device), len);
                memcpy(result->device, device, size);
                result->state_1 = state;
                result->state_2 = lock_state;
            }
            free(device);
        }
    }
    if(NULL!=result)
    {
        *buffer = p;
    }
    return result;
}

void jk_server_data_update(stationclient_t *client, jk_command_t *command, int mode=0)
{
    if(command->data_size < 5)
    {
        UCHAR error_no = PRL_ACK_ERROR_DATA;
        jk_command_t *ack = jk_command_ack(command, error_no);
        jk_command_send(client, ack);
        jk_command_free(&ack);
        return;
    }
    int index = 3;
    if(0==mode)
    {
        index++;
    }
    USHORT count =  jk_get_ushort(command->data+index);
    jk_device_list_t list;
    memset(&list, 0, sizeof(jk_device_list_t));
    list.count = count;
    if(0<count)
    {
        char *p = command->data+index+2;
        int size = count*sizeof(jk_device_t);
        jk_device_t *device = (jk_device_t *)malloc(size);
        if(NULL==device)
        {
            return;
        }
        memset(device, 0, size);
        list.device = device;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(device+i);
            UCHAR len = (UCHAR)(*(p++));
            if(0<len)
            {
                int min_size = MIN(sizeof(d->device), len);
                memcpy(d->device, p, min_size);
                p+=len;
            }
            UCHAR state = (0==mode||1==mode)?(UCHAR)(*(p++)):0;
            UCHAR lock_state = (0==mode||2==mode)?(UCHAR)(*(p++)):0;

            d->device_len = len;
            d->state_1 = state;
            d->state_2 = lock_state;

        }//for
    }//if

    client_update_function func = NULL;
    switch (mode) {
    case 0:
        func = jk_intf_instance()->device_update;
        break;
    case 1:
        func = jk_intf_instance()->device_update_state;
        break;
    case 2:
        func = jk_intf_instance()->device_update_lock_state;
        break;
    default:
        break;
    }
    if(NULL!=func)
    {
        func(client, list);
    }
    if(0<list.count)
    {
        free(list.device);
    }

}
