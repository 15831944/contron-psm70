#include "controller.h"


void ground_controller_connect(controller_t *controller);
void ground_controller_statereport(int station, int controller,
                                   int id, int state);
void ground_controller_disconnect(controller_t *controller, bool connect_immediate=false);
int ground_controller_disconnect_time_get(controller_t *controller);
void ground_controller_disconnect_time_set(controller_t *controller,
                                           int time);
THREAD_API ground_controller_guard_thread(void *param);
THREAD_API ground_controller_receive_thread(void *param);


void ground_controller_conn_find(controller_t *controller,
                                      char *data, int size);
void ground_controller_conn_handle(controller_t *controller,
                                  char *data, int size);
int ground_controller_conn_send(controller_t *controller,
                                   char *data, int size);

/*!
 * \brief ground_controller_start 启动地线控制器
 * \param controller 地线控制器
 */
void ground_controller_start(controller_t *controller)
{    
    MUTEX_INIT(&controller->guard_critical);
    controller->device_count = MAX_DEVICE;
    ground_controller_disconnect(controller, true);
    int ret;
    THREAD_CREATE(&controller->guard_thread[0], ground_controller_guard_thread, controller, ret);

    if(ret)
    {
        THREAD_RUN(controller->guard_thread[0], false);
	}
	THREAD_CREATE(&controller->receive_thread[0], ground_controller_receive_thread, controller, ret);
	if(ret)
	{
		THREAD_RUN(controller->receive_thread[0], false);
	}
}

/*!
 * \brief ground_device_init 闭锁桩初始化
 * \param device 闭锁桩
 * \param address 通讯地址
 * \param caption 显示编号
 * \param state 状态
 */
void ground_device_init(device_t *device, int address, char *caption, int state)
{
    device->address[0] = address;
    char d[MAX_STRING];
    sprintf(d, "%s", caption);
    memcpy(device->caption, d, sizeof(device->caption));
    device->state[0] = state;
}

/*!
 * \brief ground_controller_close 地线控制器关闭
 * \param controller 地线控制器
 */
void ground_controller_close(controller_t *controller)
{
    THREAD_CLOSE(controller->state_thread[0]);
    THREAD_CLOSE(controller->heartbeat_thread[0]);
    THREAD_CLOSE(controller->guard_thread[0]);
    THREAD_CLOSE(controller->receive_thread[0]);
    tcp_close(&controller->conn[0].tcp[0]);
    MUTEX_FREE(&controller->guard_critical);
}

/*!
 * \brief ground_controller_guard_thread 地线控制器临控线程
 * \param param 地线控制器
 * \return 返回NULL
 * 地线控制器连接断开指定的间隔后，重新进行连接;地线控制器disconnect_time为0时，不进行重连检测
 */
THREAD_API ground_controller_guard_thread(void *param)
{
    controller_t *controller = (controller_t *)param;
    int interval = controller->reconnect_interval;

    while(true)
    {
//        ground_log("i'm %d in ground_controller_guard_thread...\n", controller->device_no);
        Sleep(1000);

        int disconnect_time = ground_controller_disconnect_time_get(controller);
        if(disconnect_time)
        {
            int now;
            GET_TIME(now);
            int timeout = disconnect_time + interval;
            bool outOfTime = (now >= (timeout));
            if(outOfTime)
            {
                ground_log("ground controller reconnect(%d, %d, %d)...\n", disconnect_time, now, interval);
                ground_controller_connect(controller);
            }
        }
    }
    ground_log("leave controller guard thread...\n");
    return NULL;
}

/*!
 * \brief ground_controller_heartbeat_thread 地线控制器心跳线程
 * \param param 地线控制器
 * \return 返回值NULL，不使用
 * 地线控制器判定6秒链路无数据将断开链路，默认间隔3秒发送心跳指令
 */
THREAD_API ground_controller_heartbeat_thread(void *param)
{
    controller_t *controller = (controller_t *)param;
    int last_heatbeat;
    GET_TIME(last_heatbeat);
    int interval = 3;
    last_heatbeat -= interval;

    while(true)
    {
        ground_controller_enter(controller);
        bool connected = tcp_isvalid(&controller->conn[0].tcp[0]);
        ground_controller_leave(controller);
        if(!connected)
        {
            break;
        }
        int now;
        GET_TIME(now);
        int timeout = last_heatbeat + interval;
        bool outOfTime = (now >= (timeout));
        if(outOfTime)
        {
            last_heatbeat = now; //忽略指令发送时间
            UCHAR d[] = {0x55,0xaa,0x03,0x01,0xc7};
            bool sended = false;
            ground_controller_enter(controller);
            int ret = ground_controller_conn_send(controller, (char *)d, sizeof(d));
            sended = (1==ret);

            ground_log("ground controller heartbeat[%s:%d][ret=%d]...\n",
                       controller->conn[0].tcp[0].hostname,
                       controller->conn[0].tcp[0].port,
                       ret);
            ground_controller_leave(controller);
        }
        Sleep(1000);
    }
    ground_log("leave controller heartbeat thread...\n");
    return NULL;
}

/*!
 * \brief ground_controller_state_thread 地线控制器状态查询线程
 * \param param 地线控制器
 * \return 返回值NULL，不使用
 * 查询地线控制器当前地线桩的状态
 */
THREAD_API ground_controller_state_thread(void *param)
{
    controller_t *controller = (controller_t *)param;
    int last_state;
    GET_TIME(last_state);
    int interval = 15;
    last_state -= interval;

    while(true)
    {
        ground_controller_enter(controller);
        bool connected = tcp_isvalid(&controller->conn[0].tcp[0]);
        ground_controller_leave(controller);
        if(!connected)
        {
            break;
        }
        int now;
        GET_TIME(now);
        int timeout = last_state + interval;
        bool outOfTime = (now >= (timeout));
        if(outOfTime)
        {
            last_state = now; //忽略指令发送时间
            UCHAR d[] = {0x55,0xaa,0x05,0x02,0x00,0x00,0x67};
            bool sended = false;
            ground_controller_enter(controller);
            int ret = ground_controller_conn_send(controller, (char *)d, sizeof(d));
            sended = (1==ret);

            ground_log("ground controller state[%s:%d][ret=%d]...\n",
                       controller->conn[0].tcp[0].hostname,
                       controller->conn[0].tcp[0].port,
                       ret);
            ground_controller_leave(controller);
            if(!sended)
            {
                break;
            }
        }
        Sleep(1000);
    }
    ground_log("leave controller state thread...\n");
    return NULL;
}

/*!
 * \brief ground_controller_disconnect_time_get 取地线控制器连接断开时间
 * \param controller 地线控制器
 * \return 连接断开时间
 * 线程安全
 */
int ground_controller_disconnect_time_get(controller_t *controller)
{
    int ret = 0;
//    MUTEX_LOCK(&controller->guard_critical);
    ret = controller->disconnect_time;
//    MUTEX_UNLOCK(&controller->guard_critical);
    return ret;

}

/*!
 * \brief ground_controller_disconnect_time_set 设置地线控制器连接断开时间
 * \param controller 地线控制器
 * \param time 连接断开时间
 */
void ground_controller_disconnect_time_set(controller_t *controller, int time)
{
	ground_log("disconnect time:%d\n", time);
    controller->disconnect_time = time;
}

/*!
 * \brief ground_controller_connect 地线控制器通讯连接
 * \param controller 地线控制器
 * 连接成功则启动数据接收线程，否则启动定时重连机制
 */
void ground_controller_connect(controller_t *controller)
{
    bool success = false;
    ground_controller_enter(controller);
    tcp_connect(&controller->conn[0].tcp[0]);
    bool connected = tcp_isvalid(&controller->conn[0].tcp[0]);
    if(connected)
    {
        ground_controller_disconnect_time_set(controller, 0);
		success = true;
    }
    if(!success)
    {
        ground_controller_disconnect(controller);
    }
    else
    {
        int ret;
        THREAD_CREATE(&controller->heartbeat_thread[0], ground_controller_heartbeat_thread, controller, ret);
        if(ret)
        {
            THREAD_RUN(controller->heartbeat_thread[0], false);
        }
        THREAD_CREATE(&controller->state_thread[0], ground_controller_state_thread, controller, ret);
        if(ret)
        {
            THREAD_RUN(controller->state_thread[0], false);
        }
//        UCHAR d[] = {0x55,0xaa,0x05,0x02,0x00,0x00,0x67};
//        ground_controller_conn_send(controller, (char *)d, sizeof(d));
    }
    ground_log("ground controller connect[%s:%d][success=%d]...\n",
               controller->conn[0].tcp[0].hostname,
               controller->conn[0].tcp[0].port,
               success);
    ground_controller_leave(controller);

}

/*!
 * \brief ground_controller_disconnect 地线控制器通讯断开
 * \param controller 地线控制器
 * 断开后，初始化地线桩状态为未知，并启用重连机制
 */
void ground_controller_disconnect(controller_t *controller, bool connect_immediate)
{
    int size = MAX_DEVICE;
    for(int i=0;i<size;i++)
    {
        controller->device[i].state[0] = STATE_UNKNOWN;
    }
    int now;
    GET_TIME(now);
    if(connect_immediate)
    {
        now -= controller->reconnect_interval;
    }
    ground_controller_disconnect_time_set(controller, now);
}


const UCHAR ccitt_table8[] = {
    // 用 X8+X2+X+1计算得到的CRC表
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
    0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
    0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
    0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
    0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
    0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
    0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
    0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
    0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
    0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
    0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
    0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
    0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
    0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
    0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
    0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
    0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

UCHAR ground_controller_conn_crc(const char *buffer, USHORT len)
{
    UCHAR crc_reg = 0;
    UCHAR *p = (UCHAR *)buffer;
    while (len-- > 0)
        crc_reg = ccitt_table8[crc_reg ^ *p++];

    return ~crc_reg;
}

/*!
 * \brief ground_controller_receive_thread 地线控制器接收线程
 * \param param 地线控制器
 * \return 返回NULL，不使用
 */
THREAD_API ground_controller_receive_thread(void *param)
{
    controller_t *controller = (controller_t *)param;
    tcp_t *tcp = &controller->conn[0].tcp[0];
    fd_set fds;
    while(true)
    {
        ground_controller_enter(controller);
        int isvalid = tcp_isvalid(tcp);
        ground_controller_leave(controller);
        if(!isvalid)
        {
            Sleep(10);
            continue;
        }
        int hifd = 0;
        FD_ZERO(&fds);
        ground_controller_enter(controller);
        FD_SET(tcp->fd, &fds);
        hifd = MAX(hifd, (int)tcp->fd);
        ground_controller_leave(controller);
        if(0==hifd)
        {
            Sleep(10);
            continue;
        }

        //设置等待时间100ms
        struct timeval tv;
        tv.tv_sec =0;
        tv.tv_usec = 100000;

        int ret = select(hifd+1, &fds, NULL, NULL, &tv);
//        ground_log("select=%d\n", ret);
        if((-1==ret)||(0>=ret))
        {
            continue;
        }
//        if(0<ret)
        {
            bool received = false;
            ground_controller_enter(controller);
            received = FD_ISSET(tcp->fd, &fds);

            if(received)
            {
                char buffer[MAXTCPSIZE] = {0};
                int len = 0;
                len = recv(tcp->fd, buffer, sizeof(buffer), 0);
                if(0<len)
                {
                    char str[MAX_STRING];
                    sprintf(str, "ground controller receive[%s:%d]:%s\n",
                            tcp->hostname, tcp->port,
                            ground_buffer_format(buffer, len));
                    ground_log(str);
                    ground_controller_conn_find(controller, buffer, len);
//                    int strlen = len*3+1;
//                    char *str = (char *)malloc(strlen);
//                    memset(str, 0, strlen);
//                    char *p = str;
//                    for(int i=0;i<len;i++)
//                    {
//                        char c[4] = {0};
//                        sprintf(c, "%02X ", (UCHAR)buffer[i]);
//                        memcpy(p, c, 3);
//                        p = p + 3;
//                    }
//                    *(p-1) = (char)('\n');
//                    ground_log(str);
//                    free(str);
                }
                else
                {
                    ground_log("ground controller disconnect[%s:%d]...\n",
						tcp->hostname, tcp->port);
                    THREAD_CLOSE(controller->heartbeat_thread[0]);
					THREAD_CLOSE(controller->state_thread[0]);
					tcp_close(tcp);
					ground_controller_disconnect(controller);
					//THREAD_CLOSE(controller->receive_thread[0]);
                }
            }//receive
            ground_controller_leave(controller);
        }

    }

    return NULL;
}

/*!
 * \brief ground_controller_conn_find 查找地线控制器通讯数据数据帧
 * \param controller 地线控制器
 * \param data 接收的数据
 * \param size 接收的数据长度
 */
void ground_controller_conn_find(controller_t *controller, char *data, int size)
{
    UCHAR header[] = {0x55, 0xaa};
    int headerSize = sizeof(header);
    int pos = 0;
    int newsize = size;
    UCHAR *buffer = (UCHAR *)malloc(size);
    memset(buffer, 0, size);
    memcpy(buffer, data, size);
    ground_log("ground controller conn find[data]:%s\n",
               ground_buffer_format(data, size));
    tcp_t *tcp = &controller->conn[0].tcp[0];
    if(0<tcp->buffer_size)
    {
        newsize = tcp->buffer_size+size;
        buffer = (UCHAR *)realloc(buffer, newsize);
        memset(buffer, 0, newsize);
        memcpy(buffer, tcp->buffer, tcp->buffer_size);
        memcpy(buffer+tcp->buffer_size, data, size);
        ground_log("ground controller conn find[tcp]:%s\n",
                   ground_buffer_format(tcp->buffer, tcp->buffer_size));
    }
    ground_log("ground controller conn find[buffer]:%s\n",
               ground_buffer_format((char*)buffer, newsize));
	UCHAR *p = buffer;
    while(pos<newsize)
    {
        bool hasmore = (newsize-pos)>=(headerSize+1);  //+1为报文长度
        if(hasmore)
        {
            bool foundheader = (((*(p))==header[0])&&((*(p+1))==header[1]));
            if(foundheader)
            {
                UCHAR len = *(p+headerSize);
                int frameLen = len + headerSize;
                bool found = (frameLen <= (newsize-pos));
                if(found)
                {
                    UCHAR crc = *(p+frameLen-1);
                    UCHAR calcCrc = ground_controller_conn_crc((char *)(p+2), len-1);
                    found = (crc==calcCrc);
                    if(found)
                    {
                        UCHAR *frame = (UCHAR *)malloc(frameLen);
                        memset(frame, 0, frameLen);
                        memcpy(frame, p, frameLen);
                        ground_controller_conn_handle(controller, (char *)frame, frameLen);
                        free(frame);
                        pos += frameLen;
                        p += frameLen;
                    }
                    else
                    {
                        //报文检验不正确
                        pos++;
                        p++;
                    }
                }
                else
                {
                    //报文长度不够
                    break;
                }
            }
            else
            {
                //未找到报文头
                pos++;
                p++;
            }//foundheader
        }
        else
        {
            //未找到报文头
            break;
        }//hasmore
    }//while

    newsize -= pos;
    if(0<newsize)
    {
        if(0<tcp->buffer_size)
        {
            free(tcp->buffer);
        }
        tcp->buffer_size = newsize;
        tcp->buffer = (char *)malloc(newsize);
        memset(tcp->buffer, 0, newsize);
        memcpy(tcp->buffer, (char *)p, newsize);
        ground_log("ground controller conn find[tcp]:%s\n",
                   ground_buffer_format(tcp->buffer, tcp->buffer_size));
    }
    free(buffer);
}

/*!
 * \brief ground_controller_conn_handle 处理地线控制器数据帧
 * \param controller 地线控制器
 * \param data 数据帧
 * \param size 数据帧长度
 */
void ground_controller_conn_handle(controller_t *controller, char *data, int size)
{
    UCHAR cmd = *(data+3);
    if(5==size)
    {
        //回应帧，暂不处理
    }
    else
    {
        //发回应帧
        UCHAR ack[5] = {0x55, 0xAA, 0x03, 0x0, 0x0};
        ack[3] = cmd;
        UCHAR crc = ground_controller_conn_crc((char *)(ack+2), 2);
        ack[4] = crc;
        ground_controller_conn_send(controller, (char *)ack, sizeof(ack));

        switch (cmd) {
        case 0x03:
        {
            int station = controller->station_no;
            int no = controller->contronller_no;
            int deviceNo = *(data+5);
            int state = *(data+6);
            if(deviceNo < (int)sizeof(controller->device))
            {
                device_t *device = &controller->device[deviceNo-1];
                device->state[0] = state;
                memcpy(device->id, data+7, sizeof(device->id));
                char str[MAX_STRING] = {0};
                sprintf(str, "ground controller state:station=%d,controller=%d,id=%d,state=%d,rfid=%s",
                        station, no, deviceNo, state, ground_device_idstr(device));
                ground_controller_statereport(station, no, deviceNo, state);
            }
        }
            break;
        default:
            break;
        }
    }
}

/*!
 * \brief ground_controller_conn_send 发送地线控制器数据帧
 * \param controller 地线控制器
 * \param data 数据帧
 * \param size 数据帧长度
 * \return 发送成功返回1，否则返回0
 */
int ground_controller_conn_send(controller_t *controller, char *data, int size)
{
    tcp_t *tcp = &controller->conn[0].tcp[0];
    int ret = tcp_send(tcp, data, size);
    char str[MAX_STRING];
    sprintf(str, "ground controller send[%s:%d][ret=%d]:%s\n",
            tcp->hostname, tcp->port,
            ret, ground_buffer_format(data, size));
    ground_log(str);
    return ret;
}

/*!
 * \brief ground_controller_enter 进入地线控制器临界区
 * \param controller 地线控制器
 * 用于线程安全
 */
void ground_controller_enter(controller_t *controller)
{
    MUTEX_LOCK(&controller->guard_critical);
}

/*!
 * \brief ground_controller_leave 退出地线控制器临界区
 * \param controller 地线控制器
 * 用于线程安全
 */
void ground_controller_leave(controller_t *controller)
{
    MUTEX_UNLOCK(&controller->guard_critical);
}

/*!
 * \brief ground_controller_unlock 发送解锁地线桩指令
 * \param controller 地线控制器
 * \param id 地线桩号
 * \return 成功返回1，失败返回0
 */
int ground_controller_unlock(controller_t *controller, int id)
{
    //解锁指令
    UCHAR cmd[7] = {0x55, 0xAA, 0x05, 0x04, 0x00, 0x00, 0x00};
    cmd[5] = (UCHAR)id;
    UCHAR crc = ground_controller_conn_crc((char *)(&cmd[2]), 4);
    cmd[6] = crc;
    ground_controller_enter(controller);
    bool connected = tcp_isvalid(&controller->conn[0].tcp[0]);
    int ret = 0;
    int station = controller->station_no;
    int no = controller->contronller_no;
    if(connected)
    {
        ret = ground_controller_conn_send(controller, (char *)cmd, sizeof(cmd));
    }
    ground_controller_leave(controller);
    ground_log("ground controller unlock:station=%d,controller=%d,id=%d,ret=%d\n", station, no, id, ret);
    return ret;
}

/*!
 * \brief ground_device_idstr 生成用于显示的ID字符串
 * \param device 地线桩
 * \return ID字符串
 */
char *ground_device_idstr(device_t *device)
{
    static char str[MAX_STRING] = {0};
    char *p = str;
    int size = sizeof(device->id);
    for(int i=0;i<size;i++)
    {
        sprintf(p, "%02X", (UCHAR)device->id[i]);
        p+=2;
    }
    return str;
}

/*!
 * \brief ground_buffer_format 格式化内存数据
 * \param buffer 内存区域
 * \param size 内存区域大小
 * \return 格式化的字符串，用十六进制显示字节，第个字节用空格格开
 */
char *ground_buffer_format(char *buffer, int size)
{
    static char str[MAX_STRING] = {0};
    char *p = str;
    for(int i=0;i<size;i++)
    {
        sprintf(p, "%02X ", (UCHAR)(*(buffer+i)));
        p+=3;
    }
    *(p-1) = 0;
    return str;
}

/*!
 * \brief ground_controller_statereport 汇报地线桩状态
 * \param station 站号
 * \param controller 地线控制器号
 * \param id 桩号
 * \param state 桩状态
 */
void ground_controller_statereport(int station, int controller, int id, int state)
{
    if(ground_intf()->statereport_immediate)
    {
        ground_intf()->statereport_immediate(station, controller, id, state);
    }
}

/*!
 * \brief ground_controller_lock 发送闭锁地线桩指令
 * \param controller 地线控制器
 * \param id 地线桩号
 * \return 成功返回1，失败返回0
 */
int ground_controller_lock(controller_t *controller, int id)
{
    //闭锁指令
    UCHAR cmd[7] = {0x55, 0xAA, 0x05, 0x09, 0x00, 0x00, 0x00};
    cmd[5] = (UCHAR)id;
    UCHAR crc = ground_controller_conn_crc((char *)(&cmd[2]), 4);
    cmd[6] = crc;
    ground_controller_enter(controller);
    bool connected = tcp_isvalid(&controller->conn[0].tcp[0]);
    int ret = 0;
    int station = controller->station_no;
    int no = controller->contronller_no;
    if(connected)
    {
        ret = ground_controller_conn_send(controller, (char *)cmd, sizeof(cmd));
    }
    ground_controller_leave(controller);
    ground_log("ground controller lock:station=%d,controller=%d,id=%d,ret=%d\n", station, no, id, ret);
    return ret;
}


int ground_controller_query(controller_t *controller, int id)
{
    //查询状态指令
    UCHAR cmd[7] = {0x55, 0xAA, 0x05, 0x02, 0x00, 0x00, 0x00};
    cmd[5] = (UCHAR)id;
    UCHAR crc = ground_controller_conn_crc((char *)(&cmd[2]), 4);
    cmd[6] = crc;
    ground_controller_enter(controller);
    bool connected = tcp_isvalid(&controller->conn[0].tcp[0]);
    int ret = 0;
    int station = controller->station_no;
    int no = controller->contronller_no;
    if(connected)
    {
        ret = ground_controller_conn_send(controller, (char *)cmd, sizeof(cmd));
    }
    ground_controller_leave(controller);
    ground_log("ground controller query:station=%d,controller=%d,id=%d,ret=%d\n", station, no, id, ret);
    return ret;

}


void ground_controller_state_interval(controller_t *controller)
{
    if(!ground_intf()->statereport_interval)
    {
        return;
    }
    controller_t *c = (controller_t *)malloc(sizeof(controller_t));
    memset(c, 0, sizeof(controller_t));
//    ground_controller_enter(controller);
    memcpy(c, controller, sizeof(controller_t));
//    ground_controller_leave(controller);
    int station = c->station_no;
    int no = c->contronller_no;
    int size = c->device_count;
    for(int i=0;i<size;i++)
    {
        int index = i+1;
        int state = (int)c->device[i].state[0];
        ground_intf()->statereport_interval(station, no, index, state);
    }
}
