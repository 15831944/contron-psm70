#ifndef STATION_DEF_H
#define STATION_DEF_H

#include "sys/platform.h"
#include "conn/tcp.h"
#include "jk_cmd_buffer.h"

/*!
 * \brief 通讯
 * 目前子站通过TCP连接与主站通讯
 */
typedef struct
{
    tcp_t tcp[1];
} conn_t;

/*!
 * \brief 子站
 */
typedef struct {
    void *next;
    int server_no;                      //主站号为0
    int station_no;                     //站号
    int reconnect_interval;             //重连间隔，默认为15秒
    int disconnect_time;                //连接断开时间，连接后置0不进行重连
    int heartbeat_time;                 //接收到主站心跳的时间
    int heartbeat_interval;             //向主站发送心跳间隔，默认4秒
    int broken_interval;                /*主站定时向子站发送心跳，心跳发送间隔默认5秒，
                                         *子站在15秒内（发送三次的时间）没有收到主站的心跳监测报文，
                                         *则判断与主站通讯中断*/
    CRITICAL_SECTION guard_critical;    //临界区
    pthread_t guard_thread[1];          //监护线程，实现断线重连
    pthread_t heartbeat_thread[1];      //心跳线程，检测主站断开
    pthread_t receive_thread[1];        //数据接收线程
    conn_t conn[1];                     //通讯通道

    void *send_function;                //发送数据接口
    void *receive_function;             //处理数据接口

    void *server;                       /*主站对象，主站用该结构缓存已连接的子站，当server!=NULL，表示子站已连接到主站，
                                         *此时server_no为子站号，station_no为主站号，
                                         *接收到子站心跳时更新server_no，其值不为零表示子站在线*/

    int heartbeat_count;                //心跳发送次数，收到心跳回应时清零，其值大于等于4时断开连接
    jk_cmd_buffer_list_t cmd_buffer;    //待处理命令列表
    int online;                         //在线标识,0-离线,1-在线,收到对方心跳后
    int yk_send_index;                  //上报遥控闭锁状态的索引,>0表示正在发送

} stationclient_t;


/*!
 * \brief 主站
 */
typedef struct {
    int station_no;                     //站号
    int reconnect_interval;             //重连间隔，默认为15秒
    int disconnect_time;                //连接断开时间，连接后置0不进行重连
    int heartbeat_time;                 //接收到主站心跳的时间
    int broken_interval;                /*主站定时向子站发送心跳，心跳发送间隔默认5秒，
                                         *子站在15秒内（发送三次的时间）没有收到主站的心跳监测报文，
                                         *则判断与主站通讯中断*/
    CRITICAL_SECTION guard_critical;    //临界区
    pthread_t guard_thread[1];          //监护线程，实现断线重连
    pthread_t heartbeat_thread[1];      //心跳线程，检测主站断开
    pthread_t receive_thread[1];        //数据接收线程
    conn_t conn[1];                     //通讯通道
    stationclient_t *client;
} stationserver_t;

typedef void (*client_bind)(stationclient_t *);
typedef void (*client_send)(stationclient_t *, char *, int);
typedef void (*client_receive)(stationclient_t *, char *, int);

#endif // STATION_DEF_H

