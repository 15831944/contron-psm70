#include "station_server.h"

#include "utils/utils.h"
#include "station_client.h"

stationserver_t *stationserver_new()
{
    stationserver_t *server;
    server = (stationserver_t *)malloc(sizeof(stationserver_t));
    if(NULL!=server)
    {
        memset(server, 0, sizeof(stationserver_t));
        server->reconnect_interval = 15;
        server->broken_interval = 15;
    }
    return server;

}

void stationserver_enter(stationserver_t *server)
{
    MUTEX_LOCK(&server->guard_critical);
}

void stationserver_leave(stationserver_t *server)
{
    MUTEX_UNLOCK(&server->guard_critical);
}

void stationserver_disconnect(stationserver_t *server, bool reconnect_immediate)
{
    int now;
    GET_TIME(now);
    if(reconnect_immediate)
    {
        now -= server->reconnect_interval;
    }
    server->disconnect_time = now;
}

THREAD_API stationserver_receive_thread(void *param)
{
    stationserver_t *server = (stationserver_t *)param;
    tcp_t *tcp = &server->conn[0].tcp[0];
    fd_set fds;
    while(true)
    {
        stationserver_enter(server);
        int isvalid = tcp_isvalid(tcp);
        stationserver_leave(server);
        if(!isvalid)
        {
            Sleep(10);
            continue;
        }
        int hifd = 0;
        FD_ZERO(&fds);
        stationserver_enter(server);
        FD_SET(tcp->fd, &fds);
        hifd = MAX(hifd, (int)tcp->fd);
        stationserver_leave(server);
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
            stationserver_enter(server);
            received = FD_ISSET(tcp->fd, &fds);

            if(received)
            {
                SOCKET_HANDLE in_fd;

                struct sockaddr_in in;
                memset(&in, 0, sizeof(struct sockaddr_in));
                int in_len = sizeof(struct sockaddr_in);
                if(-1!=(int)(in_fd=accept(tcp->fd, (struct sockaddr *)(&in), &in_len)))
                {
                    char *ip = inet_ntoa(in.sin_addr);
                    USHORT port = ntohs(in.sin_port);
                    log("station client connect from %s:%u ...\n", ip, port);
                    stationclient_t *client = stationclient_new();
                    client->server = server;
                    client->station_no = server->station_no;
                    client->conn[0].tcp[0].fd = in_fd;
                    client->conn[0].tcp[0].port = port;
                    strcpy(client->conn[0].tcp[0].hostname, ip);
                    stationserver_append(&server->client, client);
                    stationclient_server(client);
                }
            }//receive
            stationserver_leave(server);
        }

    }//true

    return NULL;
}


void stationserver_connect(stationserver_t *server)
{
    bool success = true;
    stationserver_enter(server);
    tcp_connect(&server->conn[0].tcp[0]);
    bool connected = tcp_isvalid(&server->conn[0].tcp[0]);
    if(connected)
    {
        server->disconnect_time = 0;
        int ret;
        THREAD_CREATE(&server->receive_thread[0], stationserver_receive_thread, server, ret);
        success = success && ret;
        if(ret)
        {
            THREAD_RUN(server->receive_thread[0], false);
        }
    }
    if(!success)
    {
        stationserver_disconnect(server, false);
    }
    log("station server connect[%s:%d][success=%d]...\n",
               server->conn[0].tcp[0].hostname,
               server->conn[0].tcp[0].port,
               success);
    stationserver_leave(server);
}




THREAD_API stationserver_guard_thread(void *param)
{
    stationserver_t *server = (stationserver_t *)param;
    int now;
    GET_TIME(now);
    now -= server->reconnect_interval;
    server->disconnect_time = now;
    int interval = server->reconnect_interval;
    int stationno = server->station_no;
    int idle = 500;
    while(true)
    {
        stationserver_enter(server);
        int disconnect_time = server->disconnect_time;
        stationserver_leave(server);
        if(disconnect_time)
        {
            int current;
            GET_TIME(current);
            int timeout = disconnect_time + interval;
            bool outOfTime = (current >= (timeout));
            if(outOfTime)
            {
                log("(station=%d)server reconnect(current=%d, disconnect=%d, interval=%d)...\n",
                    stationno, current, disconnect_time, interval);
                stationserver_enter(server);
                stationserver_connect(server);
                stationserver_leave(server);
            }
        }
        Sleep(idle);
    }
    return NULL;
}

void stationserver_start(stationserver_t *server)
{
    char *ip = server->conn[0].tcp[0].hostname;
    int port = server->conn[0].tcp[0].port;
    log("(station=%d)(%s:%d)server start...\n",
        server->station_no, ip, port);
    MUTEX_INIT(&server->guard_critical);
    stationserver_disconnect(server, true);
    int ret,success;
    THREAD_CREATE(&server->guard_thread[0], stationserver_guard_thread, server, ret);
    success = ret;
    if(success)
    {
        THREAD_RUN(server->guard_thread[0], false);
    }
}


void stationserver_append(stationclient_t **list, stationclient_t *client)
{
    stationclient_t *n = *list;
    while((NULL!=n)&&(NULL!=n->next))
    {
        n = (stationclient_t *)n->next;
    }
    if(NULL==n)
    {
        *list = client;
        return;
    }
    n->next = client;
}


stationclient_t *stationserver_find(stationserver_t *server, int station_no, stationclient_t *client)
{
    stationclient_t *result = NULL;
    stationserver_enter(server);
    stationclient_t *n = (NULL==client?server->client:(stationclient_t *)client->next);
    while(NULL!=n)
    {        
        stationclient_t *c = n;
        int id = stationclient_id(c);
        stationclient_enter(c);
        n = (stationclient_t *)c->next;
        stationclient_leave(c);
        if(station_no==id)
        {
            result = c;
            break;
        }
    }
    stationserver_leave(server);
    return result;
}


stationclient_t *stationserver_find_todo(stationserver_t *server, int station_no, int cmd, stationclient_t *client)
{
    stationclient_t *result = NULL;
    stationserver_enter(server);
    stationclient_t *n = (NULL==client?server->client:(stationclient_t *)client->next);
    while(NULL!=n)
    {
        stationclient_t *c = n;
        int found = NUMBER_FALSE;
        int id = stationclient_id(c);
        stationclient_enter(c);
        found = jk_cmd_buffer_exist(&c->cmd_buffer, cmd);
        n = (stationclient_t *)c->next;
        stationclient_leave(c);
        if(found && (station_no==id))
        {
            result = c;
            break;
        }
    }
    stationserver_leave(server);
    return result;
}


void stationserver_clear_todo(stationserver_t *server, stationclient_t *client, int cmd)
{
    if(NULL==client)
    {
        return;
    }
    stationserver_enter(server);
    stationclient_enter(client);
    jk_cmd_buffer_clear(&client->cmd_buffer, cmd);
    stationclient_leave(client);
    stationserver_leave(server);
}
