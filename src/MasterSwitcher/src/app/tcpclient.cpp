#include "tcpclient.h"

#include "utils.h"

#include "log.h"
#if QT
#define DEBUG_OUTPUT APP_LOG
#else
#define DEBUG_OUTPUT printf
#endif

THREAD_API tcpclient_guard_thread(void *param)
{
    TcpClient *tcp = (TcpClient *)param;
    int test_count = 0;
//    int idle = 50;
    while(true)
    {
        Sleep(10);
        THREAD_WAITEXIT();

        if(tcp->isExiting())
        {
            DEBUG_OUTPUT("[TCP CLIENT]tcp exiting in guard thread exit\n");
            break;
        }

        bool started = tcp->isStarted();
        bool connected = tcp->isConnected();
        bool enableReconnect = tcp->getEnableReconnect();
        test_count++;
        if(0==test_count%100)
        {
//            DEBUG_OUTPUT("[TCP CLIENT]condition:started=%d\tconnected=%d\tenable=%d\n", started, connected, enableReconnect);
        }
        if(started&&(!connected)&&enableReconnect)
        {
            int current;
            GET_TIME(current);
            int brokenTime = tcp->getBrokenTime();
            int interval = tcp->getReconnectInterval();

            int timeout = brokenTime + interval;
            bool outOfTime = (current >= (timeout));
//            DEBUG_OUTPUT("[TCP CLIENT]check:brokenTime=%d\tinterval=%d\tnow=%d\n", brokenTime, interval, current);
            if(brokenTime && outOfTime)
            {
                DEBUG_OUTPUT("[TCP CLIENT]retry:brokenTime=%d\tinterval=%d\tnow=%d\n", brokenTime, interval, current);
                tcp->connect();
                DEBUG_OUTPUT("[TCP CLIENT]after try connect\n");
            }
        }
//        Sleep(idle);
        Sleep(10);
        THREAD_WAITEXIT();
    }
    DEBUG_OUTPUT("[TCP CLIENT]guard thread exit\n");
    return NULL;
}


THREAD_API tcpclient_receive_thread(void *param)
{
    TcpClient *tcp = (TcpClient *)param;
    while(true)
    {
        Sleep(10);
        THREAD_WAITEXIT();

        if(tcp->isExiting())
        {
            DEBUG_OUTPUT("[TCP CLIENT]tcp exiting in receive thread exit\n");
            break;
        }

        bool started = tcp->isStarted();
        bool connected = tcp->isConnected();
        if((!started)||(!connected))
        {
            continue;
        }
        tcp->receive();
        Sleep(10);
        THREAD_WAITEXIT();
    }//true
    DEBUG_OUTPUT("[TCP CLIENT]receive thread exit\n");


    return NULL;
}

TcpClient::TcpClient()
    :BaseObject()
{
    tcp_zero(&mTcp);

    mStarted = false;
    mExiting = false;
    mReconnectInterval = 5;
    mHandler = NULL;

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
}

bool TcpClient::isConnected()
{
    bool result = false;

//    enter();
    result = tcp_isvalid(&mTcp);
//    leave();

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

void TcpClient::receive()
{
    fd_set fds;
    FD_ZERO(&fds);

#define MAXTCPSIZE 1024
    char buffer[MAXTCPSIZE] = {0};
    int len = 0;
    bool received = false;

//    enter();

    SOCKET_HANDLE fd = mTcp.fd;
    FD_SET(fd, &fds);

    //设置等待时间100ms
    struct timeval tv;
    tv.tv_sec =0;
    tv.tv_usec = 100000;

    int ret = select(fd+1, &fds, NULL, NULL, &tv);
    if(0<ret)
    {
        received = FD_ISSET(fd, &fds);
        if(received)
        {
            len = recv(fd, buffer, sizeof(buffer), 0);
        }//receive
    }

//    leave();

    if(received)
    {
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
    }
#undef MAXTCPSIZE
}

int TcpClient::send(char *buffer, int size)
{
    if(NULL==buffer||0==size)
    {
        return 0;
    }
    bool sended = false;

    enter();    
    DEBUG_OUTPUT("[TCP CLIENT]send:\n%s\n", buffer_format(buffer, size));
    int ret = tcp_send(&mTcp, buffer, size);
    sended = (size == ret);
    leave();

    return sended;
}

int TcpClient::connect()
{
    int ret = 0;

        DEBUG_OUTPUT("[TcpClient]try connect\n");
    enter();
    tcp_connect(&mTcp);
//    DEBUG_OUTPUT("[TcpClient]connect 2\n");
    ret = tcp_isvalid(&mTcp);
    DEBUG_OUTPUT("connect %s:%d %s \n", mTcp.hostname, mTcp.port, ret?"success":"fail");
//    leave();
    if(!ret)
    {
        updateBrokenTime();
    }
    bool hasHandler = false;
//    enter();
    hasHandler = (NULL!=mHandler);
//    leave();
    if(hasHandler)
    {
        DEBUG_OUTPUT("[TcpClient]handle connect\n");
        userLock();
        if(ret)
        {
            mHandler->tcpClientConnected(this);
        }
        else
        {
            mHandler->tcpClientError(this);
        }
        userLeave();
    }
    leave();

    return ret;
}

void TcpClient::tryBreakConnection()
{
    DEBUG_OUTPUT("[TCP CLIENT]tryBreakConnection\n");
    enter();
    tcp_close(&mTcp);
    leave();
    updateBrokenTime();
    userLock();
    if(NULL!=mHandler)
    {
        mHandler->tcpClientDisconnected(this);
    }
    userLeave();
}

void TcpClient::handleData(char *buffer, int size)
{
    DEBUG_OUTPUT("[TCP CLIENT]receive:\n%s\n", buffer_format(buffer, size));
    userLock();
    if(NULL!=mHandler)
    {
        mHandler->tcpClientReceiveData(this, buffer, size);
    }
    userLeave();
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

void TcpClient::updateBrokenTime()
{
    enter();
    GET_TIME(mBrokenTime);
    DEBUG_OUTPUT("[TCP CLIENT]updateBrokenTime:broken time=%d\n", mBrokenTime);
    leave();
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
    memcpy(&mTcp.hostname, ip, strlen(ip));
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
//    enter();
    mHandler = handler;
//    leave();
}

void TcpClient::start(bool needConnect)
{
//    enter();
    mStarted = true;
    if(needConnect)
    {
        connect();
    }
//    leave();
}

void TcpClient::close()
{
//    enter();
    if(!mExiting)
    {
        mExiting = true;
        tryBreakConnection();
        Sleep(50);
//        tcp_close(&mTcp);
//        updateBrokenTime();
        //    leave();

//        Sleep(50);

        //    enter();
        THREAD_CLOSE(guard_thread);
        THREAD_CLOSE(receive_thread);
    }
//    leave();

    Sleep(50);
}

bool TcpClient::isExiting()
{
    bool result = false;
//    enter();
    result = mExiting;
//    leave();
    return result;
}

bool TcpClient::isStarted()
{
    bool result = false;

//    enter();
    result = mStarted;
//    leave();

    return result;
}

