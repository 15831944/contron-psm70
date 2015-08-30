#ifndef CONTROLLER
#define CONTROLLER

#include "global.h"
#include "conn/tcp.h"
#include "intf.h"

#define STATE_UNKNOWN (char)(-1)

typedef enum
{

} device_enable_e;
/*
 * 闭锁桩
 */
typedef struct
{
    char address[1]; //通讯地址
    char caption[12];//显示编号
    char state[1];   //状态，255-未知
    char id[5];      //插头id
    int enable;      //启用
} device_t;

typedef struct
{
    tcp_t tcp[1];
} conn_t;

typedef enum
{
    controller_unknown,
    controller_loaded,
    controller_started,
    controller_inited
} controller_state;
/*
 * 地线控制器
 */
typedef struct
{
    int station_no; //站号
    int contronller_no; //控制器号
    int device_count;
    device_t device[MAX_DEVICE];
    CRITICAL_SECTION guard_critical;
    pthread_t guard_thread[1];  //监护线程，实现定时重连
    int reconnect_interval;  //重连间隔
    int disconnect_time;  //连接断开时间，当为0时不进行重连
    conn_t conn[1];    
    pthread_t receive_thread[1];  //数据接收线程
    pthread_t heartbeat_thread[1];  //心跳数据线程
    pthread_t state_thread[1];  //状态查询线程
} controller_t;

void ground_controller_start(controller_t *controller);
void ground_controller_close(controller_t *controller);
void ground_controller_enter(controller_t *controller);
void ground_controller_leave(controller_t *controller);
int ground_controller_unlock(controller_t *controller, int id);
int ground_controller_lock(controller_t *controller, int id);
int ground_controller_query(controller_t *controller, int id);
void ground_controller_state_interval(controller_t *controller);
THREAD_API ground_controller_heartbeat_thread(void *param);
THREAD_API ground_controller_state_thread(void *param);


void ground_device_init(device_t *device,
                        int address, char *caption, int state);
char *ground_device_idstr(device_t *device);
char *ground_buffer_format(char *buffer, int size);

#endif // CONTROLLER

