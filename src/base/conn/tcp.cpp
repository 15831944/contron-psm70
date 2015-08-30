#include "tcp.h"


CRITICAL_SECTION tcp_critical;

void tcp_init()
{
    MUTEX_INIT(&tcp_critical);
#if WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}


void tcp_free()
{
#if WIN32
    WSACleanup();
#endif
    MUTEX_FREE(&tcp_critical);
}


void tcp_enter()
{
    MUTEX_LOCK(&tcp_critical);
}


void tcp_leave()
{
    MUTEX_UNLOCK(&tcp_critical);
}

int tcp_setoption_client(SOCKET_HANDLE socket_fd)
{
    int keepAlive = 1;				// 设定KeepAlive
    int keepIdle = 10;              // 开始首次KeepAlive探测前的TCP空闭时间
    int keepInterval = 1;			// 两次KeepAlive探测间的时间间隔
    int keepCount = 3;				// 断开前的KeepAlive探测次数
    // 设置心跳探测功能
    if(setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, (const char *)&keepAlive, sizeof(keepAlive)) == -1)
    {
        return -1;
    }

    // 设置首次KeepAlive探测前的TCP空闭时间，缺省为10秒
    if(setsockopt(socket_fd, SOL_TCP, TCP_KEEPIDLE, (const char *)&keepIdle, sizeof(keepIdle)) == -1)
    {
        return -2;
    }

    // 设置两次KeepAlive探测间的时间间隔（1秒）
    if(setsockopt(socket_fd, SOL_TCP, TCP_KEEPINTVL, (const char *)&keepInterval, sizeof(keepInterval)) == -1)
    {
        return -3;
    }

    // 设置断开前的KeepAlive探测次数（3次）
    if(setsockopt(socket_fd, SOL_TCP, TCP_KEEPCNT, (const char *)&keepCount, sizeof(keepCount)) == -1)
    {
        return -4;
    }

    struct linger lg;
    lg.l_onoff = 1;		// 此项为1，则SO_LINGER功能有效
    lg.l_linger = 0;	// =0表示套接口关闭时TCP夭折连接，TCP将丢弃保留在套接口发送缓冲区中的任何数据并发送
    // 一个RST给对方，而不是通常的四分组终止序列，这避免了TIME_WAIT状态；
    /*
    lg.l_linger = 30;
    当此项非0时，当套接口关闭时内核将拖延一段时间。如果套接口缓冲区中仍残留数据，进程将处于睡眠状态，直到：
    a. 所有数据发送完且被对方确认，之后进行正常的终止序列（描述字访问计数为0）或
    b. 延迟时间到(4.4BSD:0.3秒；Posix.1g: 30秒)。
    注释：l_linger的单位依赖于实现: 4.4BSD假设其单位是时钟滴答（百分之一秒），但Posix.1g规定单位为秒
    */
    // 设置SOCKET的延时关闭功能
    if(setsockopt(socket_fd, SOL_SOCKET, SO_LINGER, (const char *)&lg, sizeof(struct linger)) == -1)
    {
        return -5;
    }

    struct timeval timeo = {3, 0};
    if(setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeo, sizeof(struct timeval)) == -1)
    {
        return -6;
    }

#ifndef WIN32

    int flag = 0;


    // set socket fd noblock
    flag = fcntl(socket_fd, F_GETFL, 0);
    if(flag < 0)
    {
        return -11;
    }

    if(fcntl(socket_fd, F_SETFL, flag | O_NONBLOCK) < 0)
    {
        return -12;
    }
#endif

    return 0;
}

void tcp_connect_client(SOCKET_HANDLE &fd, char *hostname, int port)
{
    struct hostent *host = NULL;
    struct sockaddr_in addr;

#if WIN32
    if (NULL == (host = gethostbyname(hostname)))
    {
        fd = INVALID_SOCKET;
        return;
    }
#else
    /* Why this loop? Because the call might return an empty record.
       At least it very rarely does, on my system... */
    for (fd = 0; fd < 5; fd++)
    {
        if (NULL == (host = gethostbyname(hostname)))
        {
            fd = -1;
            return;
        }
        if (*host->h_name)
        {
            break;
        }
    }
    if (!host || !host->h_name || !*host->h_name)
    {
        fd = -1;
        return;
    }
#endif

#if WIN32
    if (INVALID_SOCKET == (fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
    {
        fd = INVALID_SOCKET;
        return;
    }
#else
    if (-1 == (fd = socket(AF_INET, SOCK_STREAM, 0)))
    {
        fd = -1;
        return;
    }
#endif

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *((struct in_addr *)host->h_addr);

#if WIN32
    if (SOCKET_ERROR == connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)))
    {
        closesocket(fd);
        fd = INVALID_SOCKET;
        WSACleanup();
        return;
    }
#else
    if (-1 == connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)))
    {
        close(fd);
        fd = -1;
        return;
    }
#endif

    tcp_setoption_client(fd);
}

void tcp_connect_server(SOCKET_HANDLE &fd, char *hostname, int port)
{
    struct hostent *host = NULL;
    struct sockaddr_in local;

#if WIN32
    if (NULL == (host = gethostbyname(hostname)))
    {
        fd = INVALID_SOCKET;
        return;
    }
#else
    /* Why this loop? Because the call might return an empty record.
       At least it very rarely does, on my system... */
    for (fd = 0; fd < 5; fd++)
    {
        if (NULL == (host = gethostbyname(hostname)))
        {
            fd = -1;
            return;
        }
        if (*host->h_name)
        {
            break;
        }
    }
    if (!host || !host->h_name || !*host->h_name)
    {
        fd = -1;
        return;
    }
#endif

#if WIN32
    if (INVALID_SOCKET == (fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
    {
        fd = INVALID_SOCKET;
        return;
    }
#else
    if (-1 == (fd = socket(AF_INET, SOCK_STREAM, 0)))
    {
        fd = -1;
        return;
    }
#endif

    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = inet_addr(hostname);
    if (SOCKET_ERROR == bind(fd, (struct sockaddr *)&local, sizeof(struct sockaddr_in)))
    {
        closesocket(fd);
        fd = INVALID_SOCKET;
#if WIN32
        WSACleanup();
#endif
        return;
    }
    if(SOCKET_ERROR==listen(fd, 10))
    {
        closesocket(fd);
        fd = INVALID_SOCKET;
#if WIN32
        WSACleanup();
#endif
        return;
    }
}

void tcp_connect(tcp_t *tcp)
{
    tcp_enter();

    char *hostname = tcp->hostname;
    int port = tcp->port;

    SOCKET_HANDLE fd;

    if(tcp->local_if)
    {
        tcp_connect_server(fd, hostname, port);
    }
    else
    {
        tcp_connect_client(fd, hostname, port);
    }

    tcp->fd = fd;

    tcp_leave();
    return;
}


int tcp_isvalid(tcp_t *tcp)
{
#ifdef WIN32
    return tcp->fd == INVALID_SOCKET?0:1;
#else
    return tcp->fd == -1?0:1;
#endif

}


int tcp_send(tcp_t *tcp, char *data, int size)
{
    int ret = 0;
#if WIN32
    ret = send(tcp->fd, data, size, 0);
#else
    ret = write( tcp->fd, data, size);
#endif
    if(size == ret)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


void tcp_config(tcp_t *tcp, char *hostname, int port)
{
    memcpy(tcp->hostname, hostname, sizeof(tcp->hostname));
    tcp->port = port;
}


void tcp_close(tcp_t *tcp)
{
    if(tcp_isvalid(tcp))
    {
#if WIN32
        closesocket(tcp->fd);
        tcp->fd = INVALID_SOCKET; return;
#else
        close(tcp->fd);
        tcp->fd = -1; return;
#endif
    }
}
