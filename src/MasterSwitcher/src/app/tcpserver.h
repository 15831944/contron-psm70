#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "baseobject.h"
#include "tcpclient.h"
#include "itcpserver.h"

#include <vector>
using namespace std;

class TcpServer : public BaseObject, public ITcpClient
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
    void deleteCloseClient();

protected:
    //ITcpServer
    void addNewConnection(void *tcp);
public:
    //ITcpClient
    void tcpClientReceiveData(void *tcp, char *buffer, int size);
    void tcpClientConnected(void *tcp);
    void tcpClientDisconnected(void *tcp);
    void tcpClientError(void *tcp);
private:
    int mPort;
    tcp_t mTcp;
    pthread_t receive_thread;
    pthread_t guard_thread;
    ITcpServer *mHandler;
    bool mStarted;
    bool mExiting;

    vector<TcpClient *> mClients;

};

#endif // TCPSERVER_H
