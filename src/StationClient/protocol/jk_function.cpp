#include "jk_function.h"

#include "jk_crc.h"
#include "jk_worker.h"


static UCHAR header[HEADER_SIZE] = {0xa5, 0x5a, 0xa5, 0x5a};

queue_t *queue_new()
{
    queue_t *q = (queue_t *) malloc(sizeof(queue_t));
    if(NULL != q)
    {
        q->next = q;
    }
    return q;
}

queue_t *queue_find(queue_t *queue, int station_no)
{
    queue_t *next = (queue_t *)queue->next;
    while(next != next->next)
    {
        if(station_no == next->station_no)
        {
            break;
        }
        next = (queue_t *)next->next;
    }
    if(station_no != next->station_no)
    {
        //没找到，创建并加到队列中
        queue_t *q = queue_new();
        next->next = q;
        next = q;
    }
    return next;
}

//void get_data(const buffer_t *buf, char *out, int &s)
//{
//    char *result = NULL;
//    buffer_t *t = (buffer_t *)buf->next;
//    int newsize = 0;
//    int pos = newsize;
//    while(t != buf->next)
//    {
//        newsize += t->size;
//        if(NULL==result)
//        {
//            result = (char *)malloc(newsize);
//            if(NULL==result)
//            {
//                break;
//            }
//            memcpy(result, t->data, newsize);
//        }
//        else
//        {
//            result = (char *)realloc(result, newsize);
//            if(NULL==result)
//            {
//                break;
//            }
//            memcpy(result+pos, t->data, t->size);
//        }
//        pos = newsize;
//        t = (buffer_t *)t->next;
//    }
//    out = result;
//    s = pos;
//}


//void jk_find_frame(const buffer_t *buf, frame_t *f, buffer_t *remain)
//{
//    int minsize = 16; //不包含正文数据和校验码的长度
//    int headersize = sizeof(header);
//    int pos = 0;
//    int newsize = buf->size;
//    UCHAR *buffer = (UCHAR *)buf->data;
//    UCHAR *p = buffer;
//    while(pos<newsize)
//    {
//        bool hasmore = (newsize-pos)>=minsize;
//        if(hasmore)
//        {
//            bool foundheader = (((*(p))==header[0])
//                                &&((*(p+1))==header[1])
//                                &&((*(p+2))==header[2])
//                                &&((*(p+3))==header[3])
//                                );
//            if(foundheader)
//            {
//                USHORT len = jk_get_ushort((const char *)(p+minsize));
//                int crcsize = 2;
//                int frameLen = len + minsize + crcsize;
//                bool found = (frameLen <= (newsize-pos));
//                if(found)
//                {
//                    USHORT crc = jk_get_ushort((const char *)(p+frameLen-crcsize));
//                    UCHAR calcCrc = jk_crc_32((char *)(p+headersize), frameLen-headersize-crcsize);
//                    found = (crc==calcCrc);
//                    if(found)
//                    {
//                        UCHAR *frame = (UCHAR *)malloc(frameLen);
//                        memset(frame, 0, frameLen);
//                        memcpy(frame, p, frameLen);
//                        //FIXME:handle frame
////                        ground_controller_conn_handle(controller, (char *)frame, frameLen);
//                        free(frame);
//                        pos += frameLen;
//                        p += frameLen;
//                    }
//                    else
//                    {
//                        //报文检验不正确
//                        pos++;
//                        p++;
//                    }
//                }
//                else
//                {
//                    //报文长度不够
//                    break;
//                }
//            }
//            else
//            {
//                //未找到报文头
//                pos++;
//                p++;
//            }//foundheader
//        }
//        else
//        {
//            //未找到报文头
//            break;
//        }//hasmore
//    }//while

//    newsize -= pos;
//    if(0<newsize)
//    {
////        if(0<tcp->buffer_size)
////        {
////            free(tcp->buffer);
////        }
////        tcp->buffer_size = newsize;
////        tcp->buffer = (char *)malloc(newsize);
////        memset(tcp->buffer, 0, newsize);
////        memcpy(tcp->buffer, (char *)p, newsize);
////        ground_log("ground controller conn find[tcp]:%s\n",
////                   ground_buffer_format(tcp->buffer, tcp->buffer_size));
//    }
////    free(buffer);

//}



command_t *jk_command_build(char *frame)
{
    command_t *result = (command_t *)malloc(sizeof(command_t));
    if(NULL!=result)
    {
        memset(result, 0, sizeof(command_t));
        memcpy(result->header, header, HEADER_SIZE);
        int len_pos = HEADER_SIZE;
        int pt_pos = len_pos + 2;
        int ct_pos = pt_pos + 1;
        int target_address_pos = ct_pos + 1;
        int source_address_pos = target_address_pos + 2;
        int target_type_pos = source_address_pos + 2;
        int source_type_pos = target_type_pos + 1;
        int data_pos = source_type_pos + 1;
        result->data_size = jk_get_ushort((const char *)(frame + len_pos));
        result->protocol_type = *(frame + pt_pos);
        result->control_type = *(frame + ct_pos);
        result->source_type = *(frame + source_type_pos);
        result->target_type = *(frame + target_type_pos);
        //站号只取高8位?
        result->source_address = jk_get_ushort((const char *)(frame + source_address_pos))>>8;
        result->target_address = jk_get_ushort((const char *)(frame + target_address_pos))>>8;
        int crc_pos = data_pos + result->data_size;
        int len = result->data_size;
        if(0<len)
        {
            result->data = (char *)malloc(len);
            if(NULL==result->data)
            {
                free(result);
                result = NULL;
            }
            else
            {
                memset(result->data, 0, len);
                memcpy(result->data, frame+data_pos, len);
                result->crc = jk_get_ushort((const char *)(frame + crc_pos));
            }
        }
    }
    return result;
}


command_t *jk_command_ack(const command_t *cmd, UCHAR error_no)
{
    command_t *result = (command_t *)malloc(sizeof(command_t));
    if(NULL!=result)
    {
        memset(result, 0, sizeof(command_t));
        memcpy(result, cmd, sizeof(command_t));
        result->source_address = cmd->target_address;
        result->target_address = cmd->source_address;
        result->source_type = cmd->target_type;
        result->target_type = cmd->source_type;

        result->control_type = PRL_CT_ACK;
        UCHAR data[] = {0, 0, 0, 0};
        result->data_size = sizeof(data);
        result->data = (char *)malloc(result->data_size);
        if(NULL==result->data)
        {
            free(result);
            result = NULL;
        }
        else
        {
            memset(result->data, 0, result->data_size);
            data[0] = *(cmd->data) | 0x80;
            data[1] = *(cmd->data + 1);
            data[2] = (error_no==PRL_ACK_ERROR_NONE)?0x1:0x2;
            data[3] = error_no | 0x80;
            memcpy(result->data, data, result->data_size);
        }
    }
    return result;

}

void jk_client_frame_handle(stationclient_t *client, jk_command_t *command)
{
    jk_worker_t *worker_list = jk_worker_client_create(client);
    jk_worker_t *next = worker_list;
    while(next != NULL)
    {
        jk_worker_client_handle(next, command);
        next = (jk_worker_t *)next->next;
    }
    jk_worker_free(worker_list);
}

USHORT jk_frame_crc_calc(char *frame, int len)
{
    int header_size = sizeof(header);
    int crc_size = 2;
    char *p = frame;
    int frame_len = len;
    USHORT calc_crc = jk_crc_32((char *)(p+header_size), frame_len-header_size-crc_size);  //crc校验不包括同步头和校验码
    return calc_crc;
}

int jk_frame_crc_check(char *frame, int len)
{
    int crc_size = 2;
    char *p = frame;
    int frame_len = len;
    USHORT crc = jk_get_ushort((char *)(p+frame_len-crc_size));
    USHORT calc_crc = jk_frame_crc_calc(frame, len);
    int found = (crc==calc_crc);
    return found;
}

data_t *jk_command_buffer(jk_command_t *cmd)
{
    data_t *result = NULL;
    int frame_len = cmd->data_size;
    int frame_min_len = 16;
    frame_len += frame_min_len;
    char *frame = (char *)malloc(frame_len);
    if(NULL!=frame)
    {
        memset(frame, 0, frame_len);
        char *p = frame;
        jk_set_buffer(&p, (char *)cmd->header, HEADER_SIZE);
        jk_set_ushort(&p, cmd->data_size);
        jk_set_uchar(&p, cmd->protocol_type);
        jk_set_uchar(&p, cmd->control_type);
        jk_set_uchar(&p, (UCHAR)(cmd->target_address));
        jk_set_uchar(&p, (UCHAR)(0x0));
        jk_set_uchar(&p, (UCHAR)(cmd->source_address));
        jk_set_uchar(&p, (UCHAR)(0x0));
        jk_set_uchar(&p, cmd->target_type);
        jk_set_uchar(&p, cmd->source_type);
        jk_set_buffer(&p, cmd->data, cmd->data_size);
        //CRC重新计算，不使用原值
        USHORT crc = jk_frame_crc_calc(frame, frame_len);
        jk_set_ushort(&p, crc);

        int ret = data_append(&result, frame, frame_len);
        if(ret)
        {
            return result;
        }

    }//if
    return result;
}

void jk_command_free(jk_command_t **cmd)
{
    jk_command_t *n = *cmd;
    if(0<n->data_size)
    {
        free(n->data);
    }
    free(n);
    *cmd = NULL;
}

void jk_client_frame_find(data_t **frame_list, char *buffer, int len)
{
    data_t *list = *frame_list;

    //查找数据帧
    {
        int pos = 0;
        int frame_pos, frame_len;
        while(1==jk_find_frame(buffer, len, pos, frame_pos, frame_len))
        {
            char *frame = (char *)malloc(frame_len);
            if(NULL!=frame)
            {
                memset(frame, 0, frame_len);
                memcpy(frame, buffer+frame_pos, frame_len);
                int ret = data_append(&list, frame, frame_len);
                free(frame);
                if(!ret)
                {
                    break;
                }
            }//if
        }//while
    }

    *frame_list = list;
}

void jk_client_frame_check(stationclient_t *client, data_t **frame_list)
{
    data_t *list = *frame_list;

    //检验数据帧
    {
        data_t *n = list;
        while(NULL!=n)
        {
            data_t *t = n;
            n = (data_t *)t->next;
            jk_command_t *cmd = jk_command_build(t->data);
            //数据错误检测
            {
                int prl_error = (PRL_PT_JK != cmd->protocol_type);              //协议类型校验
                int crc_error = (jk_frame_crc_check(t->data, t->size)==0);      //CRC校验
                int no_error = (NULL!=client->server)
                                &&(client->station_no != cmd->target_address);     //站号匹配
                if((prl_error) || (crc_error) || (no_error))
                {
                    //CRC校验或站号匹配错误需要向对方发送数据错误ACK
                    jk_command_t *ack = jk_command_ack(cmd, PRL_ACK_ERROR_DATA);
                    jk_command_send(client, ack);
                    jk_command_free(&ack);
                    data_remove(&list, t);
                }//if
            }
            jk_command_free(&cmd);
        }//while
    }

    *frame_list = list;
}

void jk_client_receive(stationclient_t *client, char *buffer, int len)
{
    tcp_t *tcp = &client->conn[0].tcp[0];
    char str[MAX_STRING];
    sprintf(str, "station client receive[%s:%d]:%s\n",
            tcp->hostname, tcp->port,
            buffer_format(buffer, len));
    jk_log(str);
    data_t *frame_list = NULL;
    jk_client_frame_find(&frame_list, buffer, len);
    jk_client_frame_check(client, &frame_list);
    //数据帧处理
    {
        data_t *n = frame_list;
        while(NULL!=n)
        {
            data_t *t = n;
            n = (data_t *)t->next;
            jk_command_t *cmd = jk_command_build(t->data);
            jk_client_frame_handle(client, cmd);
            jk_command_free(&cmd);
        }//while
    }
    data_free(&frame_list);
}


void jk_command_heartbeat(stationclient_t *client)
{
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    static UCHAR data[] = {PRL_CMD_HEARTBEAT, PRL_FNN_HEARTBEAT};
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


command_t *jk_command_new()
{
    command_t *result = (command_t *)malloc(sizeof(command_t));
    if(NULL!=result)
    {
        memset(result, 0, sizeof(command_t));
        memcpy(result->header, header, HEADER_SIZE);
        result->protocol_type = PRL_PT_JK;
        result->control_type = PRL_CT_SEND;
        result->source_type = PRL_ST_CONTRON;
        result->target_type = PRL_ST_CONTRON;
    }
    return result;
}


void jk_command_send(stationclient_t *client, jk_command_t *command)
{
    command->source_address = client->station_no;
    command->source_type = PRL_ST_CONTRON;
    data_t *data = jk_command_buffer(command);
    if(NULL!=data)
    {
        client_send func = (client_send)client->send_function;
        func(client, data->data, data->size);
        data_free(&data);
    }
}


void jk_server_request_data_count(stationclient_t *client)
{
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    static UCHAR data[] = {PRL_CMD_REQUEST_DATA, PRL_FNN_REQUEST_DATA_COUNT, 1};
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

void jk_send_device_unlock_result(stationclient_t *client, jk_device_list_t *devices)
{
    if(NULL==client)
    {
        return;
    }

    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    cmd->control_type = PRL_CT_BACK;
    UCHAR data[] = {PRL_CMD_REQUEST_DEVICE|PRL_CT_BACK, PRL_FNN_REQUEST_DEVICE_UNLOCK, PRL_STRUCT_NAME};
    char *buffer = NULL;
    int buffer_size = 0;
    {
        USHORT count = 0;
        int data_size = sizeof(USHORT);
        char *data = (char *)malloc(data_size);
        if(NULL==data)
        {
            return;
        }
        memset(data, 0, data_size);
        count = devices->count;
        char *p = data;
        jk_set_ushort(&p, count);

        for(int i=0;i<count;i++)
        {
            jk_device_t *device = (jk_device_t *)(devices->device+i);
            int size = 3+device->device_len; //3=1(设备主编号长度)+1(状态)+1(确保字符串以零结尾)
            int new_size = data_size+size;

            data = (char *)realloc(data, new_size);
            if(NULL==data)
            {
                data_size = 0;
                break;
            }

            char *p = (char *)(data+data_size);
            memset(p, 0, size);
            jk_set_uchar(&p, device->device_len+1);
            jk_set_buffer(&p, device->device, device->device_len);
            p++;  //字符串以零结尾
            jk_set_uchar(&p, device->state_1);

            data_size = new_size;
        }
        buffer = data;
        buffer_size = data_size;
    }
    int size = sizeof(data);
    size += buffer_size;
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        if(0<buffer_size)
        {
            jk_set_buffer(&p, buffer, buffer_size);
            free(buffer);
        }

        jk_command_send(client, cmd);
    }


    jk_command_free(&cmd);
}

void jk_server_device_unlock(stationclient_t *client, int device_len, char *device, int operate)
{
    if(NULL==client)
    {
        return;
    }
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_REQUEST_DEVICE, PRL_FNN_REQUEST_DEVICE_UNLOCK, PRL_STRUCT_NAME};
    int size = sizeof(data);
    size += (5+device_len);
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        jk_set_ushort(&p, 1);
        jk_set_uchar(&p, device_len);
        jk_set_buffer(&p, device, device_len);
        jk_set_uchar(&p, operate);

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);
}


void jk_request_data_count(stationclient_t *client)
{
    jk_server_request_data_count(client);
}


void jk_client_device_state_upload(stationclient_t *client, int device_len, char *device, int state)
{
    if(NULL==client)
    {
        return;
    }
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_REQUEST_DATA, PRL_FNN_UPLOAD_DEVICE_STATE, PRL_STRUCT_NAME};
    int size = sizeof(data);
    size += (2+2+device_len+1);
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        jk_set_ushort(&p, 1);
        jk_set_uchar(&p, device_len+1);
        jk_set_buffer(&p, device, device_len);
        jk_set_uchar(&p, 0);
        jk_set_uchar(&p, state);

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);
}


void jk_client_device_lock_state_upload(stationclient_t *client, int device_len, char *device, int state)
{
    if(NULL==client)
    {
        return;
    }
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_REQUEST_DATA, PRL_FNN_UPLOAD_DEVICE_LOCK_STATE, PRL_STRUCT_NAME};
    int size = sizeof(data);
    size += (2+2+device_len+1);
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        jk_set_ushort(&p, 1);
        jk_set_uchar(&p, device_len+1);
        jk_set_buffer(&p, device, device_len);
        jk_set_uchar(&p, 0);  //字符串以0结尾
        jk_set_uchar(&p, state);

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);
}


void jk_request_sync_time(stationclient_t *client)
{
    if(NULL==client)
    {
        return;
    }
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_TIME, PRL_FNN_SYNC_TIME};
    int size = sizeof(data);
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);
}


void jk_reply_sync_time(stationclient_t *client, jk_time_t *time)
{
    if(NULL==client)
    {
        return;
    }
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_TIME|PRL_CT_BACK, PRL_FNN_SYNC_TIME};
    int size = sizeof(data)+sizeof(jk_time_t);
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        jk_set_ushort(&p, time->year);
        jk_set_uchar(&p, time->month);
        jk_set_uchar(&p, time->day);
        jk_set_uchar(&p, time->hour);
        jk_set_uchar(&p, time->minute);
        jk_set_uchar(&p, time->second);

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);
}


void jk_send_ticket(stationclient_t *client, jk_device_list_t *devices)
{
    if(NULL==client)
    {
        return;
    }
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_TICKET, PRL_FNN_SEND_TICKET, PRL_STRUCT_NAME};
    char *buffer = NULL;
    int buffer_size = 0;
    {
        USHORT count = 0;
        int data_size = sizeof(USHORT);
        char *data = (char *)malloc(data_size);
        if(NULL==data)
        {
            return;
        }
        memset(data, 0, data_size);
        count = devices->count;
        char *p = data;
        jk_set_ushort(&p, count);

        for(int i=0;i<count;i++)
        {
            jk_device_t *device = (jk_device_t *)(devices->device+i);
            int size = 4+device->device_len; //4=1(设备主编号长度)+1(操作前状态)+1(操作后状态)+1(确保字符串以零结尾)
            int new_size = data_size+size;

            data = (char *)realloc(data, new_size);
            if(NULL==data)
            {
                data_size = 0;
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
        buffer = data;
        buffer_size = data_size;
    }
    int size = sizeof(data);
    size += buffer_size;
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        if(0<buffer_size)
        {
            jk_set_buffer(&p, buffer, buffer_size);
            free(buffer);
        }

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);
}


void jk_send_yk_device_state(stationclient_t *client, jk_device_list_t *devices)
{
    if(NULL==client)
    {
        return;
    }
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_REQUEST_DATA, PRL_FNN_UPLOAD_YK_DEVICE_STATE, PRL_STRUCT_NAME};
    char *buffer = NULL;
    int buffer_size = 0;
    {
        USHORT count = 0;
        int data_size = sizeof(USHORT);
        char *data = (char *)malloc(data_size);
        if(NULL==data)
        {
            return;
        }
        memset(data, 0, data_size);
        count = devices->count;
        char *p = data;
        jk_set_ushort(&p, count);

        for(int i=0;i<count;i++)
        {
            jk_device_t *device = (jk_device_t *)(devices->device+i);
            int size = 4+device->device_len; //4=1(设备主编号长度)+1(类型)+1(状态)+1(确保字符串以零结尾)
            int new_size = data_size+size;

            data = (char *)realloc(data, new_size);
            if(NULL==data)
            {
                data_size = 0;
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
        buffer = data;
        buffer_size = data_size;
    }
    int size = sizeof(data);
    size += buffer_size;
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        if(0<buffer_size)
        {
            jk_set_buffer(&p, buffer, buffer_size);
            free(buffer);
        }

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);
}


void jk_send_lock_device(stationclient_t *client, jk_device_list_t *devices)
{
    if(NULL==client)
    {
        return;
    }
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_REQUEST_DEVICE, PRL_FNN_REQUEST_DEVICE_LOCK, PRL_STRUCT_NAME};
    char *buffer = NULL;
    int buffer_size = 0;
    {
        USHORT count = 0;
        int data_size = sizeof(USHORT);
        char *data = (char *)malloc(data_size);
        if(NULL==data)
        {
            return;
        }
        memset(data, 0, data_size);
        count = devices->count;
        char *p = data;
        jk_set_ushort(&p, count);

        for(int i=0;i<count;i++)
        {
            jk_device_t *device = (jk_device_t *)(devices->device+i);
            int size = 3+device->device_len; //3=1(设备主编号长度)+1(状态)+1(确保字符串以零结尾)
            int new_size = data_size+size;

            data = (char *)realloc(data, new_size);
            if(NULL==data)
            {
                data_size = 0;
                break;
            }

            char *p = (char *)(data+data_size);
            memset(p, 0, size);
            jk_set_uchar(&p, device->device_len+1);
            jk_set_buffer(&p, device->device, device->device_len);
            p++;  //字符串以零结尾
            jk_set_uchar(&p, device->state_1);

            data_size = new_size;
        }
        buffer = data;
        buffer_size = data_size;
    }
    int size = sizeof(data);
    size += buffer_size;
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        if(0<buffer_size)
        {
            jk_set_buffer(&p, buffer, buffer_size);
            free(buffer);
        }

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);

}


void jk_send_unlock_device(stationclient_t *client, jk_device_list_t *devices)
{
    if(NULL==client)
    {
        return;
    }
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_REQUEST_DEVICE, PRL_FNN_REQUEST_DEVICE_UNLOCK, PRL_STRUCT_NAME};
    char *buffer = NULL;
    int buffer_size = 0;
    {
        USHORT count = 0;
        int data_size = sizeof(USHORT);
        char *data = (char *)malloc(data_size);
        if(NULL==data)
        {
            return;
        }
        memset(data, 0, data_size);
        count = devices->count;
        char *p = data;
        jk_set_ushort(&p, count);

        for(int i=0;i<count;i++)
        {
            jk_device_t *device = (jk_device_t *)(devices->device+i);
            int size = 3+device->device_len; //3=1(设备主编号长度)+1(状态)+1(确保字符串以零结尾)
            int new_size = data_size+size;

            data = (char *)realloc(data, new_size);
            if(NULL==data)
            {
                data_size = 0;
                break;
            }

            char *p = (char *)(data+data_size);
            memset(p, 0, size);
            jk_set_uchar(&p, device->device_len+1);
            jk_set_buffer(&p, device->device, device->device_len);
            p++;  //字符串以零结尾
            jk_set_uchar(&p, device->state_1);

            data_size = new_size;
        }
        buffer = data;
        buffer_size = data_size;
    }
    int size = sizeof(data);
    size += buffer_size;
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        if(0<buffer_size)
        {
            jk_set_buffer(&p, buffer, buffer_size);
            free(buffer);
        }

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);
}


void jk_send_device_lock_result(stationclient_t *client, jk_device_list_t *devices)
{
    if(NULL==client)
    {
        return;
    }
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_REQUEST_DEVICE, PRL_FNN_REQUEST_DEVICE_LOCK, PRL_STRUCT_NAME};
    char *buffer = NULL;
    int buffer_size = 0;
    {
        USHORT count = 0;
        int data_size = sizeof(USHORT);
        char *data = (char *)malloc(data_size);
        if(NULL==data)
        {
            return;
        }
        memset(data, 0, data_size);
        count = devices->count;
        char *p = data;
        jk_set_ushort(&p, count);

        for(int i=0;i<count;i++)
        {
            jk_device_t *device = (jk_device_t *)(devices->device+i);
            int size = 3+device->device_len; //3=1(设备主编号长度)+1(状态)+1(确保字符串以零结尾)
            int new_size = data_size+size;

            data = (char *)realloc(data, new_size);
            if(NULL==data)
            {
                data_size = 0;
                break;
            }

            char *p = (char *)(data+data_size);
            memset(p, 0, size);
            jk_set_uchar(&p, device->device_len+1);
            jk_set_buffer(&p, device->device, device->device_len);
            p++;  //字符串以零结尾
            jk_set_uchar(&p, device->state_1);

            data_size = new_size;
        }
        buffer = data;
        buffer_size = data_size;
    }
    int size = sizeof(data);
    size += buffer_size;
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        if(0<buffer_size)
        {
            jk_set_buffer(&p, buffer, buffer_size);
            free(buffer);
        }

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);
}


void jk_send_lock_all_device(stationclient_t *client)
{
    if(NULL==client)
    {
        return;
    }
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_REQUEST_DEVICE, PRL_FNN_REQUEST_DEVICE_LOCK_ALL};
    char *buffer = NULL;
    int buffer_size = 0;
    {
    }
    int size = sizeof(data);
    size += buffer_size;
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        if(0<buffer_size)
        {
            jk_set_buffer(&p, buffer, buffer_size);
            free(buffer);
        }

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);
}


void jk_send_unlock_all_device(stationclient_t *client)
{
    if(NULL==client)
    {
        return;
    }

    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_REQUEST_DEVICE, PRL_FNN_REQUEST_DEVICE_UNLOCK_ALL};
    char *buffer = NULL;
    int buffer_size = 0;
    {
    }
    int size = sizeof(data);
    size += buffer_size;
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        if(0<buffer_size)
        {
            jk_set_buffer(&p, buffer, buffer_size);
            free(buffer);
        }

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);
}


void jk_send_lock_all_device_result(stationclient_t *client, int result)
{
    if(NULL==client)
    {
        return;
    }
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_REQUEST_DEVICE|PRL_CT_BACK, PRL_FNN_REQUEST_DEVICE_LOCK_ALL, result};
    char *buffer = NULL;
    int buffer_size = 0;
    {
    }
    int size = sizeof(data);
    size += buffer_size;
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        if(0<buffer_size)
        {
            jk_set_buffer(&p, buffer, buffer_size);
            free(buffer);
        }

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);
}


void jk_send_unlock_all_device_result(stationclient_t *client, int result)
{
    if(NULL==client)
    {
        return;
    }
    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_REQUEST_DEVICE|PRL_CT_BACK, PRL_FNN_REQUEST_DEVICE_UNLOCK_ALL, result};
    char *buffer = NULL;
    int buffer_size = 0;
    {
    }
    int size = sizeof(data);
    size += buffer_size;
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        if(0<buffer_size)
        {
            jk_set_buffer(&p, buffer, buffer_size);
            free(buffer);
        }

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);

}


void jk_send_kbq(stationclient_t *client, jk_device_list_t *devices)
{
    if(NULL==client)
    {
        return;
    }

    int dev_count = devices->count;
    int index = client->yk_send_index;
    bool can_send = (index>0)
            &&((index-1)*DEVICE_PER_FRAME<dev_count)
            &&(index*DEVICE_PER_FRAME<=dev_count);
    if(!can_send)
    {
        client->yk_send_index = 0;
        return;
    }

    command_t *cmd = jk_command_new();
    if(NULL==cmd)
    {
        return;
    }
    cmd->source_address = client->station_no;
    cmd->target_address = client->server_no;
    UCHAR data[] = {PRL_CMD_REQUEST_DATA, PRL_FNN_UPLOAD_YK_DEVICE_STATE, PRL_STRUCT_NAME};
    char *buffer = NULL;
    int buffer_size = 0;
    {
        USHORT count = 0;
        int data_size = sizeof(USHORT);
        char *data = (char *)malloc(data_size);
        if(NULL==data)
        {
            return;
        }
        memset(data, 0, data_size);
        count = devices->count;
        char *p = data;
        jk_set_ushort(&p, count);

        for(int i=0;i<count;i++)
        {
            jk_device_t *device = (jk_device_t *)(devices->device+(index-1)*DEVICE_PER_FRAME+i);
            int size = 4+device->device_len; //4=1(设备主编号长度)+1(类型)+1(状态)+1(确保字符串以零结尾)
            int new_size = data_size+size;

            data = (char *)realloc(data, new_size);
            if(NULL==data)
            {
                data_size = 0;
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
        buffer = data;
        buffer_size = data_size;
    }
    int size = sizeof(data);
    size += buffer_size;
    char *p = (char *)malloc(size);
    if(NULL!=p)
    {
        memset(p, 0, size);
        memcpy(p, data, sizeof(data));

        cmd->data_size =  size;
        cmd->data = p;

        p += sizeof(data);
        if(0<buffer_size)
        {
            jk_set_buffer(&p, buffer, buffer_size);
            free(buffer);
        }

        jk_command_send(client, cmd);
    }

    jk_command_free(&cmd);

}
