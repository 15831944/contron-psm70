#include "station_client.h"

#include "utils/utils.h"
#include "protocol/jk_function.h"
#include "app_intf.h"

/*!
 * \brief stationclient_new 创建子站结构并初始化
 * \return 创建成功返回子站结构，否则返回NULL
 */
stationclient_t *stationclient_new()
{
    stationclient_t *client;
    client = (stationclient_t *)malloc(sizeof(stationclient_t));
    if(NULL!=client)
    {
        memset(client, 0, sizeof(stationclient_t));

        client->server_no = 0;
        client->reconnect_interval = 15;
        client->broken_interval = 15;
        client->heartbeat_interval = 4;

        client->send_function = (void *)protocol_client_send;
        client->receive_function = (void *)jk_client_receive;
    }
    return client;
}

/*!
 * \brief stationclient_guard_thread 子站监护线程
 * \param param 子站
 * \return 返回NULL
 * 子站断开后重连
 */
THREAD_API stationclient_guard_thread(void *param)
{
    stationclient_t *client = (stationclient_t *)param;
    int now;
    GET_TIME(now);
    now -= client->reconnect_interval;
    client->disconnect_time = now;
    int interval = client->reconnect_interval;
    int stationno = client->station_no;
    int idle = 500;
    while(true)
    {
        stationclient_enter(client);
        int disconnect_time = client->disconnect_time;
        stationclient_leave(client);
        if(disconnect_time)
        {
            int current;
            GET_TIME(current);
            int timeout = disconnect_time + interval;
            bool outOfTime = (current >= (timeout));
            if(outOfTime)
            {
                jk_log("station client reconnect(station=%d, current=%d, disconnect=%d, interval=%d)...\n",
                    stationno, current, disconnect_time, interval);
                stationclient_enter(client);
                stationclient_connect(client);
                stationclient_leave(client);
            }
        }
        Sleep(idle);
    }
    return NULL;
}

/*!
 * \brief stationclient_heartbeat_thread 主站心跳检测线程
 * \param param 子站
 * \return 返回NULL
 * 检测主站上次发送心跳时间，如主站超时未发送心跳，则断开连接
 */
THREAD_API stationclient_heartbeat_thread(void *param)
{
    stationclient_t *client = (stationclient_t *)param;

    int heartbeat_time;
    int send_interval = 4;
    int idle = 500;
    int in_server = 0;
    int station_no = 0;
    char *ip = NULL;
    int port = 0;

    int broken_interval = 15;
    stationclient_enter(client);
    broken_interval = client->broken_interval;
    send_interval = client->heartbeat_interval;
    in_server = (NULL!=client->server);
    ip = client->conn[0].tcp[0].hostname;
    port = client->conn[0].tcp[0].port;
    stationclient_leave(client);


    int count = 1;
    while(true)
    {
        stationclient_enter(client);
        bool connected = tcp_isvalid(&client->conn[0].tcp[0]);
        stationclient_leave(client);
        if(!connected)
        {
            break;  //外部断开连接
        }
        stationclient_enter(client);
        bool online = (!client->server) || (client->server && (0!=client->server_no));
        stationclient_leave(client);
        if(!online)
        {
            Sleep(idle);
            continue;  //子站没有向主站发送心跳，主站不向子站发心跳
        }
        if(in_server)
        {
            jk_log("client online...\n");
        }
        //1.快速发送三次心跳，确认连接，间隔1秒
        if(count)
        {
            count--;

            stationclient_enter(client);
            jk_command_heartbeat(client);
            client->heartbeat_count++;
            stationclient_leave(client);

            int current;
            GET_TIME(current);
            heartbeat_time = current;

            int fast_interval = 1000;
            Sleep(fast_interval);
            continue;
        }
        //4.检测心跳发送三次无回应
        {
            stationclient_enter(client);
            int send_error = (4<=client->heartbeat_count);
            stationclient_leave(client);
            if(send_error)
            {
                jk_log("(station=%d)(%s:%d)heartbeat no response..\n", station_no, ip, port);
                stationclient_enter(client);
                stationclient_break(client);
                stationclient_leave(client);
                break;
            }
        }
        //3.定时发送心跳
        {
            int current;
            GET_TIME(current);

            int timeout = heartbeat_time + send_interval;
            bool outOfTime = (current >= (timeout));
            if(outOfTime)
            {
                jk_log("(type=%d)(station=%d)heartbeat(last=%d, current=%d)...\n",
                    in_server, station_no, heartbeat_time, current);
                stationclient_enter(client);
                jk_command_heartbeat(client);
                client->heartbeat_count++;
                stationclient_leave(client);

                GET_TIME(current);
                heartbeat_time = current;
            }
        }
        //2.检测主站心跳超时
//        if(!in_server)
        {
            stationclient_enter(client);
            int last_heartbeat_time = client->heartbeat_time;
            stationclient_leave(client);
            if(last_heartbeat_time)
            {
                int current;
                GET_TIME(current);
                int timeout = last_heartbeat_time + broken_interval;
                bool outOfTime = (current >= (timeout));
                if(outOfTime)
                {
                    jk_log("(station=%d)(%s:%d)heartbeat timeout, break connection...\n",
                        station_no, ip, port);
                    stationclient_enter(client);
                    stationclient_break(client);
                    stationclient_leave(client);
                    continue;
                }
            }
        }//in_server
        Sleep(idle);
    }//while
    return NULL;
}


THREAD_API stationclient_receive_thread(void *param)
{
    stationclient_t *client = (stationclient_t *)param;
    tcp_t *tcp = &client->conn[0].tcp[0];
    fd_set fds;
    while(true)
    {
        stationclient_enter(client);
        int isvalid = tcp_isvalid(tcp);
        stationclient_leave(client);
        if(!isvalid)
        {
            Sleep(10);
            continue;
        }
        int hifd = 0;
        FD_ZERO(&fds);
        stationclient_enter(client);
        FD_SET(tcp->fd, &fds);
        hifd = MAX(hifd, (int)tcp->fd);
        stationclient_leave(client);
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
        if((-1==ret)||(0>=ret))
        {
            continue;
        }
        {
            bool received = false;
            stationclient_enter(client);
            received = FD_ISSET(tcp->fd, &fds);

            if(received)
            {
                static int MAXTCPSIZE = 1024;
                char buffer[MAXTCPSIZE] = {0};
                int len = 0;
                len = recv(tcp->fd, buffer, sizeof(buffer), 0);
                if(0<len)
                {
                    client_receive revc = (client_receive)client->receive_function;
                    if(NULL!=revc)
                    {
                        revc(client, buffer, len);
                    }
                }
                else
                {
                    jk_log("station client disconnect[%s:%d]...\n",
                               tcp->hostname, tcp->port);
                    stationclient_break(client);
                }
            }//receive
            stationclient_leave(client);
        }

    }//true

    return NULL;
}


/*!
 * \brief stationclient_start 启动子站
 * \param client 子站
 * 初始化临界区，启动监护线程实现通讯断开后自动重连
 */
void stationclient_start(stationclient_t *client)
{
    char *ip = client->conn[0].tcp[0].hostname;
    int port = client->conn[0].tcp[0].port;
    jk_log("(station=%d)(%s:%d)client start...\n",
        client->station_no, ip, port);
    MUTEX_INIT(&client->guard_critical);
    stationclient_disconnect(client, true);
    int ret,success;
    THREAD_CREATE(&client->guard_thread[0], stationclient_guard_thread, client, ret);
    success = ret;
    if(success)
    {
        THREAD_RUN(client->guard_thread[0], false);
    }
    THREAD_CREATE(&client->receive_thread[0], stationclient_receive_thread, client, ret);
    success = success && ret;
    if(success)
    {
        THREAD_RUN(client->receive_thread[0], false);
    }
}

/*!
 * \brief stationclient_close 关闭子站
 * \param client 子站
 * 关闭与主站通讯，释放临界区
 */
void stationclient_close(stationclient_t *client)
{
    THREAD_CLOSE(client->guard_thread[0]);
    tcp_close(&client->conn[0].tcp[0]);
    MUTEX_FREE(&client->guard_critical);
    free(client);
}

/*!
 * \brief stationclient_disconnect 子站断开
 * \param client 子站
 * \param reconnect_immediate 立即重连标志
 * 当需要立即重连时，将断开时间设置为已需要重连的时间点，由监护线程进行重连
 */
void stationclient_disconnect(stationclient_t *client, bool reconnect_immediate)
{
    int now;
    GET_TIME(now);
    if(reconnect_immediate)
    {
        now -= client->reconnect_interval;
    }
    client->disconnect_time = now;
    client->heartbeat_count = 0;
    client->heartbeat_time = 0;
}

/*!
 * \brief stationclient_connect 子站连接
 * \param client 子站
 */
void stationclient_connect(stationclient_t *client)
{
    bool success = true;
    stationclient_enter(client);
    client->disconnect_time = 0;
    tcp_connect(&client->conn[0].tcp[0]);
    bool connected = tcp_isvalid(&client->conn[0].tcp[0]);
    success = connected;
    if(connected)
    {
        client->disconnect_time = 0;
        int ret;
        THREAD_CREATE(&client->heartbeat_thread[0], stationclient_heartbeat_thread, client, ret);
        success = success && ret;
        if(ret)
        {
            THREAD_RUN(client->heartbeat_thread[0], false);
        }
    }
    if(!success)
    {
        stationclient_disconnect(client, false);
    }
    jk_log("station client connect[%s:%d][success=%d]...\n",
               client->conn[0].tcp[0].hostname,
               client->conn[0].tcp[0].port,
               success);
    stationclient_leave(client);
}

/*!
 * \brief stationclient_enter 进入子站临界区
 * \param client 子站
 * 用于线程安全
 */
void stationclient_enter(stationclient_t *client)
{
    MUTEX_LOCK(&client->guard_critical);
}

/*!
 * \brief stationclient_leave 离开子站临界区
 * \param client 子站
 * 用于线程安全
 */
void stationclient_leave(stationclient_t *client)
{
    MUTEX_UNLOCK(&client->guard_critical);
}


void stationclient_break(stationclient_t *client)
{
    THREAD_CLOSE(client->heartbeat_thread[0]);
//    THREAD_CLOSE(client->receive_thread[0]);
    tcp_close(&client->conn[0].tcp[0]);
    stationclient_disconnect(client, false);
}

void protocol_client_send(stationclient_t *client, char *buffer, int len)
{
    stationclient_enter(client);
    tcp_t *tcp = &client->conn[0].tcp[0];
    int connected = tcp_isvalid(tcp);
    stationclient_leave(client);
    if(!connected)
    {
        return;
    }
    stationclient_enter(client);
    int ret = tcp_send(tcp, buffer, len);
    jk_log("(type=%d)station client send[%s:%d][success=%d]%s\n",
        client->server,
        tcp->hostname,
        tcp->port,
        ret,
        buffer_format(buffer, len));
    stationclient_leave(client);
}


void stationclient_server(stationclient_t *client)
{
    char *ip = client->conn[0].tcp[0].hostname;
    int port = client->conn[0].tcp[0].port;
    jk_log("(station=%d)(%s:%d)client server...\n",
        client->station_no, ip, port);
    MUTEX_INIT(&client->guard_critical);
    bool success = true;
    stationclient_enter(client);
    bool connected = tcp_isvalid(&client->conn[0].tcp[0]);
    if(connected)
    {
        client->disconnect_time = 0;
        int ret;
        THREAD_CREATE(&client->receive_thread[0], stationclient_receive_thread, client, ret);
        success = success && ret;
        if(ret)
        {
            THREAD_RUN(client->receive_thread[0], false);
        }
        THREAD_CREATE(&client->heartbeat_thread[0], stationclient_heartbeat_thread, client, ret);
        success = success && ret;
        if(ret)
        {
            THREAD_RUN(client->heartbeat_thread[0], false);
        }
    }
    if(!success)
    {
        stationclient_disconnect(client, false);
    }
    jk_log("station client server[%s:%d][success=%d]...\n",
               client->conn[0].tcp[0].hostname,
               client->conn[0].tcp[0].port,
               success);
    stationclient_leave(client);
}

/*!
 * \brief stationclient_heartbeat_update
 * \param client
 * \param update_mode                       心跳类型,0-收到心跳,1-收到心跳回应
 * \param station_no
 */
void stationclient_heartbeat_update(stationclient_t *client, int update_mode, USHORT station_no)
{
    switch (update_mode) {
    case 0:
    {
        int current;
        GET_TIME(current);
        stationclient_enter(client);
        client->heartbeat_time = current;  //收到心跳，更新心跳时间
        if(stationclient_in_server(client))
        {
            jk_log("(type=%d)server client update...\n", client->server);
//            bool online = (0==client->server_no);
            client->server_no = station_no;
//            if(online)
//            {
//                jk_server_request_data_count(client);
//            }
        }
        if(0==client->online)
        {
            client->online = 1;
            if(NULL!=app_intf_instance()->client_online)
            {
                app_intf_instance()->client_online(client);
            }
        }
        stationclient_leave(client);
    }
        break;
    case 1:
    {
        stationclient_enter(client);
        jk_log("(type=%d)clear heartbeat count:%d->0\n", client->server, client->heartbeat_count);
        client->heartbeat_count = 0;  //收到心跳回应，发送次数清零
        stationclient_leave(client);
    }
        break;
    default:
        break;
    }
}


stationclient_t *stationclient_find(stationclient_t *client, int station_no)
{
    stationclient_t *result = NULL;
    stationclient_t *n = client;
    while(NULL!=n)
    {
        int id = stationclient_id(n);
        bool same = (id==station_no);
        if(same)
        {
            result = n;
            break;
        }
        n = (stationclient_t *)n->next;
    }
    return result;
}


bool stationclient_in_server(stationclient_t *client)
{
    bool result = false;
    stationclient_enter(client);
    result = (NULL!=client->server);
    stationclient_leave(client);

    return result;
}


int stationclient_id(stationclient_t *client)
{
    int result = 0;
    stationclient_enter(client);
    result = (NULL!=client->server)?client->server_no:client->station_no;
    stationclient_leave(client);

    return result;

}


void stationclient_todo_cmd(stationclient_t *client, int cmd)
{
    stationclient_enter(client);
    jk_cmd_buffer_append(&client->cmd_buffer, cmd);
    stationclient_leave(client);
}
