#include "tcpclient.h"

THREAD_API tcpclient_guard_thread(void *param)
{
    TcpClient *tcp = (TcpClient *)param;
    int idle = 500;
    while(true)
    {
        bool connected = tcp->isConnected();
        bool enableReconnect = tcp->getEnableReconnect();
        if((!connected)&&enableReconnect)
        {
            int current;
            GET_TIME(current);
            int brokenTime = tcp->getBrokenTime();
            int interval = tcp->getReconnectInterval();

            int timeout = brokenTime + interval;
            bool outOfTime = (current >= (timeout));
            if(outOfTime)
            {
                tcp->connect();
            }
        }
        Sleep(idle);
    }
    return NULL;
}


THREAD_API tcpclient_receive_thread(void *param)
{
    TcpClient *tcp = (TcpClient *)param;
    fd_set fds;
    while(true)
    {
        bool connected = tcp->isConnected();
        if(!connected)
        {
            Sleep(10);
            continue;
        }
        int hifd = 0;
        FD_ZERO(&fds);
        int fd = tcp->getFd();
        FD_SET(fd, &fds);
        hifd = MAX(hifd, fd);
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
        tcp->receive(fds);

    }//true

    return NULL;
}

TcpClient::TcpClient()
    :BaseObject()
{
    memset(&mTcp, 0, sizeof(tcp_t));

    mReconnectInterval = 5;

    int ret;
    THREAD_CREATE(&receive_thread, tcpclient_receive_thread, this, ret);
    if(ret)
    {
        THREAD_RUN(receive_thread, false);
    }
    THREAD_CREATE(&guard_thread, tcpclient_guard_thread, this, ret);
    if(ret)
    {
        THREAD_RUN(guard_thread, false);
    }
}

TcpClient::~TcpClient()
{
    THREAD_CLOSE(guard_thread);
    THREAD_CLOSE(receive_thread);
}

bool TcpClient::isConnected()
{
    bool result = false;

    enter();
    result = tcp_isvalid(&mTcp);
    leave();

    return result;
}

int TcpClient::getFd()
{
    int fd = 0;

    enter();
    fd = mTcp.fd;
    leave();

    return fd;
}

void TcpClient::setFd(SOCKET_HANDLE fd)
{
    enter();
    mTcp.fd = fd;
    leave();
}

void TcpClient::receive(fd_set &fds)
{
    int fd = getFd();
    enter();
    bool received = FD_ISSET(fd, &fds);

    if(received)
    {
#define MAXTCPSIZE 1024
        char buffer[MAXTCPSIZE] = {0};
        int len = 0;
        len = recv(fd, buffer, sizeof(buffer), 0);
        if(0<len)
        {
            // handle data here.
            handleData(buffer, len);
        }
        else
        {
            // connection broken.
            tryBreakConnection();
        }
#undef MAXTCPSIZE
    }//receive
    leave();
}

int TcpClient::send(char *buffer, int size)
{
    if(NULL==buffer||0==size)
    {
        return 0;
    }
    bool sended = false;

    enter();
    int ret = tcp_send(&mTcp, buffer, size);
    sended = (size == ret);
    leave();

    return sended;
}

int TcpClient::connect()
{
    int ret = 0;

    enter();
    tcp_connect(&mTcp);
    ret = tcp_isvalid(&mTcp);
    leave();

    if(ret&&NULL!=mHandler)
    {
        mHandler->tcpClientConnected(this);
    }

    return ret;
}

void TcpClient::tryBreakConnection()
{
    enter();
    tcp_close(&mTcp);
    GET_TIME(mBrokenTime);
    leave();
    if(NULL!=mHandler)
    {
        mHandler->tcpClientDisconnected(this);
    }
}

void TcpClient::handleData(char *buffer, int size)
{
    if(NULL!=mHandler)
    {
        mHandler->tcpClientReceiveData(this, buffer, size);
    }

}

int TcpClient::getBrokenTime()
{
    int result = 0;

    enter();
    result = mBrokenTime;
    leave();

    return result;
}

int TcpClient::getReconnectInterval()
{
    int interval = 0;

    enter();
    interval = mReconnectInterval;
    leave();

    return interval;
}

bool TcpClient::getEnableReconnect()
{
    bool result = false;

    enter();
    result = mEnableReconnect;
    leave();

    return result;

}

void TcpClient::setEnableReconnect(bool enable)
{
    enter();
    mEnableReconnect = enable;
    leave();

}

void TcpClient::setIp(char *ip)
{
    enter();
    memset(&mTcp.hostname, 0, sizeof(mTcp.hostname));
    memcpy(ip, &mTcp.hostname, strlen(ip));
    leave();
}

void TcpClient::setPort(int port)
{
    enter();
    mTcp.port = port;
    leave();
}

void TcpClient::setReconnectInterval(int interval)
{
    enter();
    mReconnectInterval = interval;
    leave();
}

void TcpClient::setHandler(ITcpClient *handler)
{
    enter();
    mHandler = handler;
    leave();
}

