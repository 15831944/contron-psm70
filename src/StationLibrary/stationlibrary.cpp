#include "stationlibrary.h"

#include "sys/platform.h"

#include <QSharedMemory>
#include <QBuffer>
#include <QDataStream>

#define DATA_SIZE   1024
#define CMD_SIZE    10
#define DLL_KEY     "DLL-SHARE-MEMORY"

#define LOG _log

#define CMD_APP_REQUEST_DATA                0x01
#define CMD_APP_DATA_CHANGE                 0x02
#define CMD_APP_LOCK_DATA_CHANGE            0x03
#define CMD_RECEIVE_TICKET                  0x04
#define CMD_REQUEST_UNLOCK_DEVICE           0x05
#define CMD_REQUEST_LOCK_DEVICE             0x06
#define CMD_SEND_UNLOCK_DEVICE_RESULT       0x07
#define CMD_SEND_LOCK_DEVICE_RESULT         0x08
#define CMD_SEND_REQUEST_SYNC_TIME          0x09
#define CMD_REPLY_SYNC_TIME                 0x0A
#define CMD_REQUEST_SYNC_TIME               0x0B
#define CMD_SEND_REPLY_SYNC_TIME            0x0C
#define CMD_SEND_TICKET                     0x0D
#define CMD_YK_DEVICE_STATE                 0x0E
#define CMD_SEND_YK_DEVICE_STATE            0x0F
#define CMD_SEND_LOCK_DEVICE                0x10
#define CMD_SEND_UNLOCK_DEVICE              0x11
#define CMD_LOCK_DEVICE_RESULT              0x12
#define CMD_UNLOCK_DEVICE_RESULT            0x13
#define CMD_SEND_LOCK_DEVICE_ALL            0x14
#define CMD_SEND_UNLOCK_DEVICE_ALL          0x15
#define CMD_LOCK_DEVICE_ALL                 0x16
#define CMD_UNLOCK_DEVICE_ALL               0x17
#define CMD_SEND_LOCK_DEVICE_ALL_RESULT     0x18
#define CMD_SEND_UNLOCK_DEVICE_ALL_RESULT   0x19
#define CMD_LOCK_DEVICE_ALL_RESULT          0x1A
#define CMD_UNLOCK_DEVICE_ALL_RESULT        0x1B

typedef struct _dll_cmd
{
    int target;
    int flag;
    int source;
    int fnn;
    char data[DATA_SIZE];
} dll_cmd_t;

static int inited = NUMBER_FALSE;
static dll_t dll_fnn;
QSharedMemory s_c;
QSharedMemory c_s;
QSharedMemory share;
CRITICAL_SECTION guard_critical;    ///临界区////
pthread_t guard_thread[1];          ///监护线程，实现断线重连////
static dll_cmd_t cmd[CMD_SIZE];


void lock()
{
    MUTEX_LOCK(&guard_critical);
}

void unlock()
{
    MUTEX_UNLOCK(&guard_critical);
}

void _log(const char *fmt, ...)
{
    char logstr[10240+1];
    va_list argp;
    va_start(argp, fmt);
    vsnprintf(logstr, 10240, fmt, argp);
    va_end(argp);
    if(NULL!=dll_fnn.log_function)
    {
        dll_fnn.log_function(logstr);
    }
}

void log_cmd(dll_cmd_t *cmd)
{
    return;
    QByteArray ba(cmd->data, sizeof(cmd->data));
    LOG("set cmd:target(%d),flag(%d),cmd(%d),data(%s)", cmd->target, cmd->flag, cmd->fnn, ba.toHex().data());
}


void client_device_unlock_result(int station_no, int device_len, char *device, int operate, int operate_result)
{
    if(NULL!=dll_fnn.device_unlock_result_function)
    {
        dll_fnn.device_unlock_result_function(station_no, device_len, device, operate, operate_result);
    }
}

void client_device_unlock(int station_no, int device_len, char *device, int operate)
{
    client_device_unlock_result(station_no, device_len, device, operate, 1);
}

int buffer_size()
{
    return CMD_SIZE*sizeof(dll_cmd_t);
}

int buffer_count()
{
    return CMD_SIZE;
}

void try_create()
{
    if(share.isAttached() || (share.attach()))
    {
        return;
    }
//LOG("in try_create");
    QSharedMemory::SharedMemoryError error = share.error();
    if(QSharedMemory::NotFound==error)
    {
        int size = buffer_size();
        if(!share.create(size))
        {
            return;
        }
        share.lock();
        char *p = (char *)share.data();
        memset(p, 0, share.size());
        share.unlock();
    }
}



/*!
 * \brief handle_fnn
 * \param cmd
 * \return 1:未处理,2:成功,3:失败
 */
int handle_fnn(dll_cmd_t *cmd)
{
    if(cmd->target!=dll_fnn.mode)
    {
        return 1;
    }
    UCHAR fnn = cmd->fnn;
//    log_cmd(cmd);
    switch (fnn) {
    case CMD_APP_REQUEST_DATA:
    {
//        LOG("case");
        psm_request_data func = dll_fnn.main2jk_request_data;
        if(MODE_SUBSTATION==cmd->source)
        {
            func = dll_fnn.sub2jk_request_data;
        }
        if(NULL==func)
        {
            return 3;
        }

        USHORT station_no = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;

//        LOG("station %d", station_no);

//        LOG("fnn(%d)\n", (int)dll_fnn.server_request_data);
        func(station_no);
        return 2;
    }
        break;
    case CMD_APP_DATA_CHANGE:
    {
        psm_device_list func = dll_fnn.server_data_change;
        if(MODE_SUBSTATION==cmd->source)
        {
            func = dll_fnn.client_data_change;
        }
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        USHORT count = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> count;
//        LOG("station=%d,count=%d", station_no, count);
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        list.device = (jk_device_t *)malloc(count*sizeof(jk_device_t));
        if(NULL==list.device)
        {
            return NUMBER_FALSE;
        }
        memset(list.device, 0, count*sizeof(jk_device_t));
        list.count = count;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            in >> d->device_len;
            in.readRawData(d->device, d->device_len);
            in >> d->state_1;
        }
        func(station_no, &list);
        free(list.device);

        return 2;
    }
        break;
    case CMD_APP_LOCK_DATA_CHANGE:
    {
        psm_device_list func = dll_fnn.server_lock_data_change;
        if(MODE_SUBSTATION==cmd->source)
        {
            func = dll_fnn.client_lock_data_change;
        }
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        USHORT count = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> count;
//        LOG("station=%d,count=%d", station_no, count);
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        list.device = (jk_device_t *)malloc(count*sizeof(jk_device_t));
        if(NULL==list.device)
        {
            return 3;
        }
        memset(list.device, 0, count*sizeof(jk_device_t));
        list.count = count;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            in >> d->device_len;
            in.readRawData(d->device, d->device_len);
            in >> d->state_2;
        }
        func(station_no, &list);
        free(list.device);

        return 2;
    }
        break;
    case CMD_RECEIVE_TICKET:
    {
        psm_device_list func = dll_fnn.app_receive_ticket;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        USHORT count = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> count;
//        LOG("station=%d,count=%d", station_no, count);
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        list.device = (jk_device_t *)malloc(count*sizeof(jk_device_t));
        if(NULL==list.device)
        {
            return 3;
        }
        memset(list.device, 0, count*sizeof(jk_device_t));
        list.count = count;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            in >> d->device_len;
            in.readRawData(d->device, d->device_len);
            in >> d->state_1;
            in >> d->state_2;
        }
        func(station_no, &list);
        free(list.device);

        return 2;
    }
        break;
    case CMD_REQUEST_UNLOCK_DEVICE:
    {
        psm_device_list func = dll_fnn.app_unlock_device;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        USHORT count = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> count;
//        LOG("station=%d,count=%d", station_no, count);
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        int size = count*sizeof(jk_device_t);
        list.device = (jk_device_t *)malloc(size);
        if(NULL==list.device)
        {
            return 3;
        }
        memset(list.device, 0, size);
        list.count = count;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            in >> d->device_len;
            in.readRawData(d->device, d->device_len);
            in >> d->state_1;
        }
        func(station_no, &list);
        free(list.device);
        return 2;
    }
        break;
    case CMD_REQUEST_LOCK_DEVICE:
    {
        psm_device_list func = dll_fnn.app_lock_device;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        USHORT count = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> count;
//        LOG("station=%d,count=%d", station_no, count);
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        int size = count*sizeof(jk_device_t);
        list.device = (jk_device_t *)malloc(size);
        if(NULL==list.device)
        {
            return 3;
        }
        memset(list.device, 0, size);
        list.count = count;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            in >> d->device_len;
            in.readRawData(d->device, d->device_len);
            in >> d->state_1;
        }
        func(station_no, &list);
        free(list.device);

        return 2;
    }
        break;
    case CMD_SEND_UNLOCK_DEVICE_RESULT:
    {
        psm_device_list func = dll_fnn.client_unlock_device_result;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        USHORT count = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> count;
//        LOG("station=%d,count=%d", station_no, count);
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        int size = count*sizeof(jk_device_t);
        list.device = (jk_device_t *)malloc(size);
        if(NULL==list.device)
        {
            return 3;
        }
        memset(list.device, 0, size);
        list.count = count;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            in >> d->device_len;
            in.readRawData(d->device, d->device_len);
            in >> d->state_1;
            in >> d->state_2;
        }
        func(station_no, &list);
        free(list.device);

        return 2;
    }
        break;
    case CMD_SEND_LOCK_DEVICE_RESULT:
    {
        psm_device_list func = dll_fnn.client_lock_device_result;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        USHORT count = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> count;
//        LOG("station=%d,count=%d", station_no, count);
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        int size = count*sizeof(jk_device_t);
        list.device = (jk_device_t *)malloc(size);
        if(NULL==list.device)
        {
            return 3;
        }
        memset(list.device, 0, size);
        list.count = count;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            in >> d->device_len;
            in.readRawData(d->device, d->device_len);
            in >> d->state_1;
            in >> d->state_2;
        }
        func(station_no, &list);
        free(list.device);

        return 2;
    }
        break;
    case CMD_SEND_REQUEST_SYNC_TIME:
    {
        psm_request_data func = dll_fnn.main2jk_send_request_sync_time;
        if(MODE_SUBSTATION==cmd->source)
        {
            func = dll_fnn.sub2jk_send_request_sync_time;
        }
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        func(station_no);

        return 2;
    }
        break;
    case CMD_SEND_REPLY_SYNC_TIME:
    {
        psm_sync_time func = dll_fnn.main2jk_send_sync_time;
        if(MODE_SUBSTATION==cmd->source)
        {
            func = dll_fnn.sub2jk_send_sync_time;
        }
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;

        jk_time_t time;
        memset(&time, 0, sizeof(time));
        in >> time.year;
        in >> time.month;
        in >> time.day;
        in >> time.hour;
        in >> time.minute;
        in >> time.second;

        func(station_no, &time);

        return 2;
    }
        break;
    case CMD_REQUEST_SYNC_TIME:
    {
        psm_request_data func = dll_fnn.mainstation_request_sync_time;
        if(MODE_SUBSTATION==cmd->target)
        {
            func = dll_fnn.substation_request_sync_time;
        }
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        func(station_no);

        return 2;
    }
        break;
    case CMD_REPLY_SYNC_TIME:
    {
        psm_sync_time func = dll_fnn.jk2main_reply_sync_time;
        if(MODE_SUBSTATION==cmd->target)
        {
            func = dll_fnn.jk2sub_reply_sync_time;
        }
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;

        jk_time_t time;
        memset(&time, 0, sizeof(jk_time_t));
        in >> time.year;
        in >> time.month;
        in >> time.day;
        in >> time.hour;
        in >> time.minute;
        in >> time.second;

        func(station_no, &time);

        return 2;
    }
        break;
    case CMD_SEND_TICKET:
    {
        psm_device_list func = dll_fnn.main2jk_send_ticket;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        USHORT count = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> count;
//        LOG("station=%d,count=%d", station_no, count);
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        int size = count*sizeof(jk_device_t);
        list.device = (jk_device_t *)malloc(size);
        if(NULL==list.device)
        {
            return 3;
        }
        memset(list.device, 0, size);
        list.count = count;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            in >> d->device_len;
            in.readRawData(d->device, d->device_len);
            in >> d->state_1;
            in >> d->state_2;
        }
        func(station_no, &list);
        free(list.device);

        return 2;
    }
        break;
    case CMD_YK_DEVICE_STATE:
    {
        psm_device_list func = dll_fnn.jk2main_yk_device_state;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        USHORT count = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> count;
        //        LOG("station=%d,count=%d", station_no, count);
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        int size = count*sizeof(jk_device_t);
        list.device = (jk_device_t *)malloc(size);
        if(NULL==list.device)
        {
            return 3;
        }
        memset(list.device, 0, size);
        list.count = count;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            in >> d->device_len;
            in.readRawData(d->device, d->device_len);
            in >> d->state_1;
            in >> d->state_2;
        }
        func(station_no, &list);
        free(list.device);

        return 2;
    }
        break;
    case CMD_SEND_YK_DEVICE_STATE:
    {
        psm_device_list func = dll_fnn.sub2jk_send_yk_device_state;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        USHORT count = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> count;
        //        LOG("station=%d,count=%d", station_no, count);
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        int size = count*sizeof(jk_device_t);
        list.device = (jk_device_t *)malloc(size);
        if(NULL==list.device)
        {
            return 3;
        }
        memset(list.device, 0, size);
        list.count = count;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            in >> d->device_len;
            in.readRawData(d->device, d->device_len);
            in >> d->state_1;
            in >> d->state_2;
        }
        func(station_no, &list);
        free(list.device);

        return 2;
    }
        break;
    case CMD_SEND_LOCK_DEVICE:
    {
        psm_device_list func = dll_fnn.main2jk_send_lock_device;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        USHORT count = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> count;
        //        LOG("station=%d,count=%d", station_no, count);
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        int size = count*sizeof(jk_device_t);
        list.device = (jk_device_t *)malloc(size);
        if(NULL==list.device)
        {
            return 3;
        }
        memset(list.device, 0, size);
        list.count = count;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            in >> d->device_len;
            in.readRawData(d->device, d->device_len);
            in >> d->state_1;
            in >> d->state_2;
        }
        func(station_no, &list);
        free(list.device);

        return 2;
    }
        break;
    case CMD_SEND_UNLOCK_DEVICE:
    {
        psm_device_list func = dll_fnn.main2jk_send_unlock_device;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        USHORT count = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> count;
        //        LOG("station=%d,count=%d", station_no, count);
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        int size = count*sizeof(jk_device_t);
        list.device = (jk_device_t *)malloc(size);
        if(NULL==list.device)
        {
            return 3;
        }
        memset(list.device, 0, size);
        list.count = count;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            in >> d->device_len;
            in.readRawData(d->device, d->device_len);
            in >> d->state_1;
            in >> d->state_2;
        }
        func(station_no, &list);
        free(list.device);

        return 2;
    }
        break;
    case CMD_UNLOCK_DEVICE_RESULT:
    {
        psm_device_list func = dll_fnn.jk2main_unlock_device_result;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        USHORT count = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> count;
        //        LOG("station=%d,count=%d", station_no, count);
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        int size = count*sizeof(jk_device_t);
        list.device = (jk_device_t *)malloc(size);
        if(NULL==list.device)
        {
            return 3;
        }
        memset(list.device, 0, size);
        list.count = count;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            in >> d->device_len;
            in.readRawData(d->device, d->device_len);
            in >> d->state_1;
            in >> d->state_2;
        }
        func(station_no, &list);
        free(list.device);

        return 2;
    }
        break;
    case CMD_LOCK_DEVICE_RESULT:
    {
        psm_device_list func = dll_fnn.jk2main_lock_device_result;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        USHORT count = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> count;
        //        LOG("station=%d,count=%d", station_no, count);
        jk_device_list_t list;
        memset(&list, 0, sizeof(jk_device_list_t));
        int size = count*sizeof(jk_device_t);
        list.device = (jk_device_t *)malloc(size);
        if(NULL==list.device)
        {
            return 3;
        }
        memset(list.device, 0, size);
        list.count = count;
        for(int i=0;i<count;i++)
        {
            jk_device_t *d = (jk_device_t *)(list.device+i);
            in >> d->device_len;
            in.readRawData(d->device, d->device_len);
            in >> d->state_1;
            in >> d->state_2;
        }
        func(station_no, &list);
        free(list.device);

        return 2;
    }
        break;
    case CMD_SEND_LOCK_DEVICE_ALL:
    {
        psm_request_data func = dll_fnn.main2jk_send_lock_all_device;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        func(station_no);

        return 2;
    }
        break;
    case CMD_SEND_UNLOCK_DEVICE_ALL:
    {
        psm_request_data func = dll_fnn.main2jk_send_unlock_all_device;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        func(station_no);

        return 2;
    }
        break;
    case CMD_LOCK_DEVICE_ALL:
    {
        psm_request_data func = dll_fnn.jk2sub_lock_all_device;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        func(station_no);

        return 2;
    }
        break;
    case CMD_UNLOCK_DEVICE_ALL:
    {
        psm_request_data func = dll_fnn.jk2sub_unlock_all_device;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        func(station_no);

        return 2;
    }
        break;
    case CMD_SEND_LOCK_DEVICE_ALL_RESULT:
    {
        psm_device_all func = dll_fnn.sub2jk_send_lock_device_all_result;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        UCHAR result = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> result;

        func(station_no, result);

        return 2;
    }
        break;
    case CMD_SEND_UNLOCK_DEVICE_ALL_RESULT:
    {
        psm_device_all func = dll_fnn.sub2jk_send_unlock_device_all_result;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        UCHAR result = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> result;

        func(station_no, result);

        return 2;
    }
        break;
    case CMD_LOCK_DEVICE_ALL_RESULT:
    {
        psm_device_all func = dll_fnn.jk2main_lock_device_all_result;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        UCHAR result = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> result;

        func(station_no, result);

        return 2;
    }
        break;
    case CMD_UNLOCK_DEVICE_ALL_RESULT:
    {
        psm_device_all func = dll_fnn.jk2main_unlock_device_all_result;
        if(NULL==func)
        {
            return 3;
        }
        USHORT station_no = 0;
        UCHAR result = 0;
        QBuffer buffer;
        buffer.setData(cmd->data, sizeof(cmd->data));
        buffer.open(QBuffer::ReadOnly);
        QDataStream in(&buffer);
        in >> station_no;
        in >> result;

        func(station_no, result);

        return 2;
    }
        break;
    default:
        break;
    }
    return NUMBER_TRUE;
}

void handle_cmd(dll_cmd_t *cmd)
{
    switch (cmd->flag) {
    case 1:
        ///启用////
    {
        int flag = handle_fnn(cmd);
        cmd->flag = flag;
    }
        break;
    case 2:///处理成功////
    case 3:///处理失败////
        ///已处理////
    {
        memset(cmd, 0, sizeof(dll_cmd_t));
    }
        break;
    default:
        break;
    }

}

void get_data()
{
    int size = buffer_size();
    memset(&cmd, 0, size);

    QSharedMemory *sm = &share;
//    LOG("in get_data");
    if(sm->isAttached() || sm->attach())
    {
//        LOG("attached");
        sm->lock();
//        LOG("size=%d,sm=%d", size, sm->size());
        if(size<=sm->size())
        {
            char *p = (char *)sm->data();
            memcpy(&cmd, p, size);
//            for(int i=0;i<buffer_count();i++)
//            {
//                dll_cmd_t *c = &cmd[i];
//                LOG("cmd%d:target=%d,flag=%d", i, c->target, c->flag);
//            }
        }
        sm->unlock();
    }
    else
    {
        LOG("error %d", (int)sm->error());
    }
}

void handle_data()
{
    int mode = dll_fnn.mode;
    int count = buffer_count();
    for(int i=0;i<count;i++)
    {
        dll_cmd_t *c = (dll_cmd_t *)(cmd+i);
        if(mode==c->target)
        {
            handle_cmd(c);
        }
    }

}

int sync()
{
    QSharedMemory *sm = &share;
    if(!(sm->isAttached() || sm->attach()))
    {
        return NUMBER_FALSE;
    }
//    LOG("sync");
//    for(int i=0;i<buffer_count();i++)
//    {
//        dll_cmd_t *c = &cmd[i];
//        LOG("cmd%d:target=%d,flag=%d", i, c->target, c->flag);
//    }
    sm->lock();
    int size = buffer_size();
    if(size<=sm->size())
    {
        char *p = (char *)sm->data();
        memcpy(p, &cmd, size);
    }
    sm->unlock();
    return NUMBER_TRUE;
}

THREAD_API stationclient_guard_thread(void *)
{
    while(true)
    {
        Sleep(20);
///
        lock();
//        LOG("get_data");
        get_data();
//        LOG("handle_data");
        handle_data();

        sync();

        unlock();
        ////
    }
    return NULL;
}

void dll_uninit()
{
    if(inited)
    {
        THREAD_CLOSE(guard_thread[0]);
        MUTEX_FREE(&guard_critical);

        inited = NUMBER_FALSE;
    }
}

void dll_init(int mode)
{
    dll_uninit();
    if(!inited)
    {
        memset(&dll_fnn, 0, sizeof(dll_t));
        dll_fnn.mode = mode;

        MUTEX_INIT(&guard_critical);

        share.setKey(DLL_KEY);
//        s_c.setKey("s_c");
//        c_s.setKey("c_s");
        int ret,success;
        THREAD_CREATE(&guard_thread[0], stationclient_guard_thread, NULL, ret);
        success = ret;
        if(success)
        {
            THREAD_RUN(guard_thread[0], false);
        }
        try_create();
        inited = NUMBER_TRUE;
    }
}


void set_sub2jk_device_unlock_result(device_unlock_result fnn)
{
    lock();
    dll_fnn.device_unlock_result_function = fnn;
    unlock();
}


void set_value(int value)
{
    int mode = 0;
    lock();
    mode = dll_fnn.mode;
    unlock();
    QSharedMemory *sm = &s_c;
    if(1==mode)
    {
        sm = &c_s;
    }
    if (sm->isAttached())
    {
        sm->detach();
    }
    {
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    QDataStream out(&buffer);
    int type = 1;
    out << type;
    out << value;
    int size = buffer.size();
    if (!sm->create(size)) {
        return;
    }

    sm->lock();
    char *to = (char*)sm->data();
    const char *from = buffer.data().data();
    memcpy(to, from, qMin(sm->size(), size));
    sm->unlock();
    }

    printf("set value=%d\n", value);
//    dll_fnn.value = value;
}


int get_value()
{
    int value;
    lock();
    value = dll_fnn.value;
    unlock();
    return value;
}


void set_main2jk_request_data(psm_request_data fnn)
{
    lock();
    dll_fnn.main2jk_request_data = fnn;
//    LOG("set server_request_data:%d", (int)fnn);
    unlock();
}

dll_cmd_t *find_cmd()
{
    dll_cmd_t *result = NULL;
    int count = buffer_count();
    for(int i=0;i<count;i++)
    {
        dll_cmd_t *t = cmd+i;
        if(0==t->flag)
        {
            result = t;
            break;
        }
    }
    return result;
}


void app_request_data(int station_no)
{
//    LOG("in dll client_request_data(%d)\n", station_no);
    try_create();
    int connected = share.isAttached() || share.attach();
//    LOG("connected %d", connected);
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
//        LOG("find cmd\n");
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SERVER;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_APP_REQUEST_DATA;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();

        }

        unlock();
    }
//    LOG("leave client_request_data(%d)\n", station_no);
}


void set_log(def_log fnn)
{
    lock();
    dll_fnn.log_function = fnn;
    unlock();
}


void app_data_change(int station_no, jk_device_list_t *devices)
{
    int count = devices->count;
//    LOG("in dll app_data_change(station=%d,count=%d)", station_no, count);
//    for(int i=0;i<count;i++)
//    {
//        jk_device_t *d = (jk_device_t *)(devices->device+i);
//        LOG("%s,%d", d->device, d->state);
//    }
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SERVER;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_APP_DATA_CHANGE;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;
            out << (USHORT)count;

            for(int i=0;i<count;i++)
            {
                jk_device_t *d = (jk_device_t *)(devices->device+i);
                out << (UCHAR)d->device_len;
                out.writeRawData(d->device, d->device_len);
                out << (UCHAR)d->state_1;
            }

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();

        }

        unlock();
    }

}


void app_lock_data_change(int station_no, jk_device_list_t *devices)
{
    int count = devices->count;
//    LOG("in dll app_lock_data_change(station=%d,count=%d)", station_no, count);
//    for(int i=0;i<count;i++)
//    {
//        jk_device_t *d = (jk_device_t *)(devices->device+i);
//        LOG("%s,%d", d->device, d->lock_state);
//    }
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = 0;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_APP_LOCK_DATA_CHANGE;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;
            out << (USHORT)count;

            for(int i=0;i<count;i++)
            {
                jk_device_t *d = (jk_device_t *)(devices->device+i);
                out << (UCHAR)d->device_len;
                out.writeRawData(d->device, d->device_len);
                out << (UCHAR)d->state_2;
            }

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();

        }

        unlock();
    }
}


void set_server_data_change(psm_device_list fnn)
{
    lock();
    dll_fnn.server_data_change = fnn;
    unlock();
}


void set_server_lock_data_change(psm_device_list fnn)
{
    lock();
    dll_fnn.server_lock_data_change = fnn;
    unlock();
}


void set_sub2jk_request_data(psm_request_data fnn)
{
    lock();
    dll_fnn.sub2jk_request_data = fnn;
    unlock();
}


void set_client_data_change(psm_device_list fnn)
{
    lock();
    dll_fnn.client_data_change = fnn;
    unlock();
}


void set_client_lock_data_change(psm_device_list fnn)
{
    lock();
    dll_fnn.client_lock_data_change = fnn;
    unlock();
}

void set_substation_receive_ticket(psm_device_list fnn)
{
    lock();
    dll_fnn.app_receive_ticket = fnn;
    unlock();

}


void set_substation_unlock_device(psm_device_list fnn)
{
    lock();
    dll_fnn.app_unlock_device = fnn;
    unlock();
}


void set_substation_lock_device(psm_device_list fnn)
{
    lock();
    dll_fnn.app_lock_device = fnn;
    unlock();
}

/*!
 * \brief jk2sub_unlock_device  子站处理主站通过集控服务发送的请求解锁设备
 * \param station_no            子站号
 * \param devices               需要解锁的设备
 * \note                        子站处理请求后，使用\sa app_send_unlock_device_result向主站汇报结果
 */
void jk2sub_unlock_device(int station_no, jk_device_list_t *devices)
{
    int count = devices->count;
//    LOG("in dll s_request_unlock_device(station=%d,count=%d)", station_no, count);
//    for(int i=0;i<count;i++)
//    {
//        jk_device_t *d = (jk_device_t *)(devices->device+i);
//        LOG("%s,%d", d->device, d->state_1);
//    }
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SUBSTATION;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_REQUEST_UNLOCK_DEVICE;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;
            out << (USHORT)count;

            for(int i=0;i<count;i++)
            {
                jk_device_t *d = (jk_device_t *)(devices->device+i);
                out << (UCHAR)d->device_len;
                out.writeRawData(d->device, d->device_len);
                out << (UCHAR)d->state_1;
            }

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();

        }

        unlock();
    }
}

/*!
 * \brief jk2sub_lock_device    子站处理主站通过集控服务发送的请求闭锁设备
 * \param station_no            子站号
 * \param devices               需要闭锁的设备
 * \note                        子站处理请求后，使用\sa app_send_lock_device_result向主站汇报结果
 */
void jk2sub_lock_device(int station_no, jk_device_list_t *devices)
{
    int count = devices->count;
//    LOG("in dll s_request_lock_device(station=%d,count=%d)", station_no, count);
//    for(int i=0;i<count;i++)
//    {
//        jk_device_t *d = (jk_device_t *)(devices->device+i);
//        LOG("%s,%d", d->device, d->state_1);
//    }
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SUBSTATION;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_REQUEST_LOCK_DEVICE;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;
            out << (USHORT)count;

            for(int i=0;i<count;i++)
            {
                jk_device_t *d = (jk_device_t *)(devices->device+i);
                out << (UCHAR)d->device_len;
                out.writeRawData(d->device, d->device_len);
                out << (UCHAR)d->state_1;
            }

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }

}

/*!
 * \brief app_send_lock_device_result   子站通过集控服务向主站发送设备闭锁结果
 * \param station_no                    子站号
 * \param devices                       设备闭锁结果
 */
void app_send_lock_device_result(int station_no, jk_device_list_t *devices)
{
    int count = devices->count;
//    LOG("in dll app_lock_device_result(station=%d,count=%d)", station_no, count);
//    for(int i=0;i<count;i++)
//    {
//        jk_device_operate_t *d = (jk_device_operate_t *)(devices->device+i);
//        LOG("%s,%d", d->device, d->state, d->result);
//    }
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SERVER;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_SEND_LOCK_DEVICE_RESULT;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;
            out << (USHORT)count;

            for(int i=0;i<count;i++)
            {
                jk_device_t *d = (jk_device_t *)(devices->device+i);
                out << (UCHAR)d->device_len;
                out.writeRawData(d->device, d->device_len);
                out << (UCHAR)d->state_1;
                out << (UCHAR)d->state_2;
            }

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}

/*!
 * \brief app_send_unlock_device_result 子站通过集控服务向主站发送设备解锁结果
 * \param station_no                    子站号
 * \param devices                       设备解锁结果
 */
void app_send_unlock_device_result(int station_no, jk_device_list_t *devices)
{
    int count = devices->count;
//    LOG("in dll app_unlock_device_result(station=%d,count=%d)", station_no, count);
//    for(int i=0;i<count;i++)
//    {
//        jk_device_operate_t *d = (jk_device_operate_t *)(devices->device+i);
//        LOG("%s,%d", d->device, d->state, d->result);
//    }
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SERVER;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_SEND_UNLOCK_DEVICE_RESULT;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;
            out << (USHORT)count;

            for(int i=0;i<count;i++)
            {
                jk_device_t *d = (jk_device_t *)(devices->device+i);
                out << (UCHAR)d->device_len;
                out.writeRawData(d->device, d->device_len);
                out << (UCHAR)d->state_1;
                out << (UCHAR)d->state_2;
            }

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}


void set_sub2jk_send_unlock_device_result(psm_device_list fnn)
{
    lock();
    dll_fnn.client_unlock_device_result = fnn;
    unlock();

}


void set_sub2jk_send_lock_device_result(psm_device_list fnn)
{
    lock();
    dll_fnn.client_lock_device_result = fnn;
    unlock();
}

/*!
 * \brief app_send_request_sync_time    子站通过集控服务向主站发送请求时钟同步
 * \param station_no                    子站号
 */
void app_send_request_sync_time(int station_no)
{
//    LOG("in dll app_sync_time(station=%d)", station_no);
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SERVER;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_SEND_REQUEST_SYNC_TIME;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}


void set_main2jk_send_request_sync_time(psm_request_data fnn)
{
    lock();
    dll_fnn.main2jk_send_request_sync_time = fnn;
    unlock();

}


void set_sub2jk_send_request_sync_time(psm_request_data fnn)
{
    lock();
    dll_fnn.sub2jk_send_request_sync_time = fnn;
    unlock();
}

void reply_sync_time(int target, int station_no, jk_time_t *time)
{
        try_create();
        int connected = share.isAttached() || share.attach();
        if(!connected)
        {
            LOG("error %d", (int)share.error());
            return;
        }
        {
            lock();
            get_data();
            dll_cmd_t *cmd = find_cmd();
            if(NULL!=cmd)
            {
                memset(cmd, 0, sizeof(dll_cmd_t));

                cmd->flag = 1;
                cmd->target = target;
                cmd->source = dll_fnn.mode;
                cmd->fnn = CMD_REPLY_SYNC_TIME;

                QBuffer buffer;
                buffer.open(QBuffer::ReadWrite);
                QDataStream out(&buffer);
                out << (USHORT)station_no;
                out << time->year;
                out << time->month;
                out << time->day;
                out << time->hour;
                out << time->minute;
                out << time->second;

                const char *p = buffer.data().data();
                memcpy(cmd->data, p, buffer.data().size());

                log_cmd(cmd);

                sync();
            }

            unlock();
        }
}

/*!
 * \brief jk2sub_reply_sync_time    子站处理主站通过集控服务发送的时钟数据(时钟同步)
 * \param station_no                子站号
 * \param time                      主站时间
 */
void jk2sub_reply_sync_time(int station_no, jk_time_t *time)
{
    reply_sync_time(MODE_SUBSTATION, station_no, time);
}


void jk2main_reply_sync_time(int station_no, jk_time_t *time)
{
    reply_sync_time(MODE_MAINSTATION, station_no, time);
}


void set_substation_sync_time(psm_sync_time fnn)
{
    lock();
    dll_fnn.jk2sub_reply_sync_time = fnn;
    unlock();

}


void set_mainstation_sync_time(psm_sync_time fnn)
{
    lock();
    dll_fnn.jk2main_reply_sync_time = fnn;
    unlock();
}

/*!
 * \brief app_send_reply_sync_time  主站通过集控服务向子站发送时钟同步信息
 * \param station_no                子站号
 * \param time                      时钟同步信息
 */
void app_send_reply_sync_time(int station_no, jk_time_t *time)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SERVER;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_SEND_REPLY_SYNC_TIME;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;
            out << time->year;
            out << time->month;
            out << time->day;
            out << time->hour;
            out << time->minute;
            out << time->second;

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }

}


void request_sync_time(int target, int station_no)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = target;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_REQUEST_SYNC_TIME;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}

/*!
 * \brief jk2sub_request_sync_time  主站处理子站通过集控服务发送的请求时钟同步
 * \param station_no                子站号
 * \note                            主站使用\sa app_send_reply_sync_time向子站发送时钟信息
 */
void jk2sub_request_sync_time(int station_no)
{
    //子站请求，主站响应
    request_sync_time(MODE_MAINSTATION, station_no);
}


void jk2main_request_sync_time(int station_no)
{
    //主站请求，子站响应
    request_sync_time(MODE_SUBSTATION, station_no);
}


void set_sub2jk_send_reply_sync_time(psm_sync_time fnn)
{
    lock();
    dll_fnn.sub2jk_send_sync_time = fnn;
    unlock();

}


void set_main2jk_send_reply_sync_time(psm_sync_time fnn)
{
    lock();
    dll_fnn.main2jk_send_sync_time = fnn;
    unlock();

}


void set_substation_request_sync_time(psm_request_data fnn)
{
    lock();
    dll_fnn.substation_request_sync_time = fnn;
    unlock();
}


void set_mainstation_request_sync_time(psm_request_data fnn)
{
    lock();
    dll_fnn.mainstation_request_sync_time = fnn;
    unlock();
}

/*!
 * \brief app_send_ticket   主站通过集控服务向子站发送操作序列
 * \param station_no        子站号
 * \param devices           操作序列
 */
void app_send_ticket(int station_no, jk_device_list_t *devices)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SERVER;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_SEND_TICKET;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            USHORT count = devices->count;
            out << count;
            for(int i=0;i<count;i++)
            {
                jk_device_t *d = (jk_device_t *)(devices->device+i);
                out << (UCHAR)d->device_len;
                out.writeRawData(d->device, d->device_len);
                out << (UCHAR)d->state_1;
                out << (UCHAR)d->state_2;
            }

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }

}


void set_main2jk_send_ticket(psm_device_list fnn)
{
    lock();
    dll_fnn.main2jk_send_ticket = fnn;
    unlock();

}

/*!
 * \brief jk2sub_send_ticket    子站处理主站通过集控服务发送的操作序列
 * \param station_no            子站号
 * \param devices               操作序列，state_1操作前状态，state_2操作后状态
 */
void jk2sub_send_ticket(int station_no, jk_device_list_t *devices)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SUBSTATION;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_RECEIVE_TICKET;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            USHORT count = devices->count;
            out << count;
            for(int i=0;i<count;i++)
            {
                jk_device_t *d = (jk_device_t *)(devices->device+i);
                out << (UCHAR)d->device_len;
                out.writeRawData(d->device, d->device_len);
                out << (UCHAR)d->state_1;
                out << (UCHAR)d->state_2;
            }

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }

}

/*!
 * \brief jk2main_yk_device_state   主站处理子站通过集控服务发送的遥控闭锁继电器状态
 * \param station_no                子站号
 * \param devices                   遥控闭锁继电器状态，state_1继电器类型，state_2继电器状态
 */
void jk2main_yk_device_state(int station_no, jk_device_list_t *devices)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_MAINSTATION;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_YK_DEVICE_STATE;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            USHORT count = devices->count;
            out << count;
            for(int i=0;i<count;i++)
            {
                jk_device_t *d = (jk_device_t *)(devices->device+i);
                out << (UCHAR)d->device_len;
                out.writeRawData(d->device, d->device_len);
                out << (UCHAR)d->state_1;
                out << (UCHAR)d->state_2;
            }

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}


void set_mainstation_yk_device_state(psm_device_list fnn)
{
    lock();
    dll_fnn.jk2main_yk_device_state = fnn;
    unlock();

}

/*!
 * \brief app_send_yk_device_state  子站通过集控服务向主站发送遥控闭锁继电器状态
 * \param station_no
 * \param devices
 */
void app_send_yk_device_state(int station_no, jk_device_list_t *devices)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SERVER;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_SEND_YK_DEVICE_STATE;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            USHORT count = devices->count;
            out << count;
            for(int i=0;i<count;i++)
            {
                jk_device_t *d = (jk_device_t *)(devices->device+i);
                out << (UCHAR)d->device_len;
                out.writeRawData(d->device, d->device_len);
                out << (UCHAR)d->state_1;
                out << (UCHAR)d->state_2;
            }

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}


void set_sub2jk_send_yk_device_state(psm_device_list fnn)
{
    lock();
    dll_fnn.sub2jk_send_yk_device_state = fnn;
    unlock();
}

/*!
 * \brief app_send_lock_device  主站通过集控服务向子站请求闭锁设备
 * \param station_no            子站号
 * \param devices               需要闭锁的设备
 */
void app_send_lock_device(int station_no, jk_device_list_t *devices)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SERVER;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_SEND_LOCK_DEVICE;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            USHORT count = devices->count;
            out << count;
            for(int i=0;i<count;i++)
            {
                jk_device_t *d = (jk_device_t *)(devices->device+i);
                out << (UCHAR)d->device_len;
                out.writeRawData(d->device, d->device_len);
                out << (UCHAR)d->state_1;
                out << (UCHAR)d->state_2;
            }

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }

}

/*!
 * \brief app_send_unlock_device    主站通过集控服务向子站请求解锁设备
 * \param station_no                子站号
 * \param devices                   需要解锁的设备
 */
void app_send_unlock_device(int station_no, jk_device_list_t *devices)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SERVER;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_SEND_UNLOCK_DEVICE;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            USHORT count = devices->count;
            out << count;
            for(int i=0;i<count;i++)
            {
                jk_device_t *d = (jk_device_t *)(devices->device+i);
                out << (UCHAR)d->device_len;
                out.writeRawData(d->device, d->device_len);
                out << (UCHAR)d->state_1;
                out << (UCHAR)d->state_2;
            }

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }

}


void set_main2jk_send_lock_device(psm_device_list fnn)
{
    lock();
    dll_fnn.main2jk_send_lock_device = fnn;
    unlock();
}


void set_main2jk_send_unlock_device(psm_device_list fnn)
{
    lock();
    dll_fnn.main2jk_send_unlock_device = fnn;
    unlock();

}

/*!
 * \brief jk2main_lock_device_result    主站处理子站通过集控服务发送的设备闭锁结果
 * \param station_no                    子站号
 * \param devices                       设备闭锁结果，state_1操作，state_2操作结果
 */
void jk2main_lock_device_result(int station_no, jk_device_list_t *devices)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_MAINSTATION;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_LOCK_DEVICE_RESULT;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            USHORT count = devices->count;
            out << count;
            for(int i=0;i<count;i++)
            {
                jk_device_t *d = (jk_device_t *)(devices->device+i);
                out << (UCHAR)d->device_len;
                out.writeRawData(d->device, d->device_len);
                out << (UCHAR)d->state_1;
                out << (UCHAR)d->state_2;
            }

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}

/*!
 * \brief jk2main_unlock_device_result  主站处理子站通过集控服务发送的设备解锁结果
 * \param station_no                    子站号
 * \param devices                       设备闭锁结果，state_1操作，state_2操作结果
 */
void jk2main_unlock_device_result(int station_no, jk_device_list_t *devices)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_MAINSTATION;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_UNLOCK_DEVICE_RESULT;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            USHORT count = devices->count;
            out << count;
            for(int i=0;i<count;i++)
            {
                jk_device_t *d = (jk_device_t *)(devices->device+i);
                out << (UCHAR)d->device_len;
                out.writeRawData(d->device, d->device_len);
                out << (UCHAR)d->state_1;
                out << (UCHAR)d->state_2;
            }

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}


void set_mainstation_lock_device_result(psm_device_list fnn)
{
    lock();
    dll_fnn.jk2main_lock_device_result = fnn;
    unlock();
}


void set_mainstation_unlock_device_result(psm_device_list fnn)
{
    lock();
    dll_fnn.jk2main_unlock_device_result = fnn;
    unlock();
}


void app_send_lock_device_all(int station_no)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SERVER;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_SEND_LOCK_DEVICE_ALL;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}

/*!
 * \brief app_send_unlock_device_all    主站通过集控服务向子站请求解锁所有设备
 * \param station_no                    子站号
 */
void app_send_unlock_device_all(int station_no)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SERVER;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_SEND_UNLOCK_DEVICE_ALL;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}


void set_main2jk_send_lock_all_device(psm_request_data fnn)
{
    lock();
    dll_fnn.main2jk_send_lock_all_device = fnn;
    unlock();
}


void set_main2jk_send_unlock_all_device(psm_request_data fnn)
{
    lock();
    dll_fnn.main2jk_send_unlock_all_device = fnn;
    unlock();
}

/*!
 * \brief jk2sub_lock_device_all    子站处理主站通过集控服务发送的请求所有设备闭锁
 * \param station_no                子站号
 */
void jk2sub_lock_all_device(int station_no)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SUBSTATION;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_LOCK_DEVICE_ALL;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}

/*!
 * \brief jk2sub_unlock_device_all  子站处理主站通过集控服务发送的请求所有设备解锁
 * \param station_no                子站号
 */
void jk2sub_unlock_all_device(int station_no)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SUBSTATION;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_UNLOCK_DEVICE_ALL;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}


void set_substation_lock_all_device(psm_request_data fnn)
{
    lock();
    dll_fnn.jk2sub_lock_all_device = fnn;
    unlock();
}


void set_substation_unlock_all_device(psm_request_data fnn)
{
    lock();
    dll_fnn.jk2sub_unlock_all_device = fnn;
    unlock();
}


void app_send_lock_device_all_result(int station_no, int result)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SERVER;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_SEND_LOCK_DEVICE_ALL_RESULT;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;
            out << (UCHAR)result;

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}


void app_send_unlock_device_all_result(int station_no, int result)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_SERVER;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_SEND_UNLOCK_DEVICE_ALL_RESULT;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;
            out << (UCHAR)result;

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}


void set_sub2jk_send_lock_all_device_result(psm_device_all fnn)
{
    lock();
    dll_fnn.sub2jk_send_lock_device_all_result = fnn;
    unlock();
}


void set_sub2jk_send_unlock_all_device_result(psm_device_all fnn)
{
    lock();
    dll_fnn.sub2jk_send_unlock_device_all_result = fnn;
    unlock();
}


void set_mainstation_lock_device_all_result(psm_device_all fnn)
{
    lock();
    dll_fnn.jk2main_lock_device_all_result = fnn;
    unlock();
}


void set_mainstation_unlock_device_all_result(psm_device_all fnn)
{
    lock();
    dll_fnn.jk2main_unlock_device_all_result = fnn;
    unlock();
}


void jk2main_lock_all_device_result(int station_no, int result)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_MAINSTATION;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_LOCK_DEVICE_ALL_RESULT;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;
            out << (UCHAR)result;

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }
}


void jk2main_unlock_all_device_result(int station_no, int result)
{
    try_create();
    int connected = share.isAttached() || share.attach();
    if(!connected)
    {
        LOG("error %d", (int)share.error());
        return;
    }
    {
        lock();
        get_data();
        dll_cmd_t *cmd = find_cmd();
        if(NULL!=cmd)
        {
            memset(cmd, 0, sizeof(dll_cmd_t));

            cmd->flag = 1;
            cmd->target = MODE_MAINSTATION;
            cmd->source = dll_fnn.mode;
            cmd->fnn = CMD_UNLOCK_DEVICE_ALL_RESULT;

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            QDataStream out(&buffer);
            out << (USHORT)station_no;
            out << (UCHAR)result;

            const char *p = buffer.data().data();
            memcpy(cmd->data, p, buffer.data().size());

            log_cmd(cmd);

            sync();
        }

        unlock();
    }

}
