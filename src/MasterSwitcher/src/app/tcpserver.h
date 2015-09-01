#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "baseobject.h"
#include "tcpclient.h"
#include "itcpserver.h"

class TcpServer : public BaseObject
{
public:
    TcpServer();
    ~TcpServer();

    void setPort(int port);
    void setHandler(ITcpServer *handler);

    int start();
    void close();

    bool checkStarted();
    void waitForNewConnection();

public:
    bool isExiting();

protected:
    void addNewConnection(char *ip, int port, SOCKET_HANDLE fd);

private:
    int mPort;
    tcp_t mTcp;
    pthread_t receive_thread;
    ITcpServer *mHandler;
    bool mStarted;
    bool mExiting;

};

#endif // TCPSERVER_H
