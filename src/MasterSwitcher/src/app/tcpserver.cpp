#include "tcpserver.h"

THREAD_API tcpserver_receive_thread(void *param)
{
    TcpServer *server = (TcpServer *)param;
    while(true)
    {
        bool isvalid = server->checkStarted();
        if(!isvalid)
        {
            Sleep(10);
            continue;
        }

        server->waitForNewConnection();

    }//true

    return NULL;
}
TcpServer::TcpServer()
    :BaseObject()
{
    memset(&mTcp, 0, sizeof(tcp_t));
    sprintf(mTcp.hostname, "0.0.0.0");
    mTcp.fd = -1;
    mTcp.local_if = 1;

    mStarted = false;

    int ret;
    THREAD_CREATE(&receive_thread, tcpserver_receive_thread, this, ret);
    if(ret)
    {
        THREAD_RUN(receive_thread, false);
    }
}

TcpServer::~TcpServer()
{
    THREAD_CLOSE(receive_thread);
}

void TcpServer::setPort(int port)
{
    enter();
    mPort = port;
    leave();
}

void TcpServer::setHandler(ITcpServer *handler)
{
    enter();
    mHandler = handler;
    leave();
}

int TcpServer::start()
{
    int ret = 0;

    enter();
    mTcp.port = mPort;
    tcp_connect(&mTcp);
    ret = tcp_isvalid(&mTcp);
    printf("listen %d %s \n",  mTcp.port, ret?"success":"fail");
    if(ret)
    {
        mStarted = true;
    }
    leave();

    return ret;
}

bool TcpServer::checkStarted()
{
    bool result = false;

    enter();
    if(mStarted && tcp_isvalid(&mTcp))
    {
        result = true;
    }
    leave();

    return result;
}

void TcpServer::waitForNewConnection()
{
    fd_set fds;
    FD_ZERO(&fds);

    enter();
    {
        FD_SET(mTcp.fd, &fds);

        //设置等待时间100ms
        struct timeval tv;
        tv.tv_sec =0;
        tv.tv_usec = 100000;

        int ret = select(mTcp.fd+1, &fds, NULL, NULL, &tv);
        if(ret > 0)
        {
            bool received = FD_ISSET(mTcp.fd, &fds);

            if(received)
            {
                SOCKET_HANDLE in_fd;

                struct sockaddr_in in;
                memset(&in, 0, sizeof(struct sockaddr_in));
                socklen_t in_len = sizeof(struct sockaddr_in);
                in_fd=accept(mTcp.fd, (struct sockaddr *)(&in), &in_len);
                if(0<(int)(in_fd))
                {
                    char *ip = inet_ntoa(in.sin_addr);
                    USHORT port = ntohs(in.sin_port);
                    printf("connect from %s:%u ...\n", ip, port);
                    addNewConnection(ip, port, in_fd);
                }
            }//receive
        }//ret
    }
    leave();

}

void TcpServer::addNewConnection(char *ip, int port, SOCKET_HANDLE fd)
{
    TcpClient *client = new TcpClient();
    client->setIp(ip);
    client->setPort(port);

    if(NULL!=mHandler)
    {
        mHandler->addNewClient(client);
    }

    client->setFd(fd);
    client->start(false);
}

