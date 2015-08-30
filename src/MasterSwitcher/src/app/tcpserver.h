#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "baseobject.h"
#include "tcpclient.h"

class TcpServer : public BaseObject
{
public:
    TcpServer();
    ~TcpServer();

    void setPort(int port);

    void start();
    bool checkStarted();
    void waitForNewConnection();

protected:
    void addNewConnection(char *ip, int port, SOCKET_HANDLE fd);

private:
    int mPort;
    tcp_t mTcp;
    pthread_t receive_thread;

};

#endif // TCPSERVER_H
