#ifndef TESTCLIENT_H
#define TESTCLIENT_H

#include "tester.h"
#include "itcpclient.h"
#include "tcpclient.h"
#include "sys/platform.h"

class TestClient : public Tester, public ITcpClient
{
public:
    TestClient();
    ~TestClient();

    void setIp(char *ip);
    void setPort(int port);

    int start();
public:
    bool isExiting();
    bool isConnected();
    void heartbeat(UINT32 timePoint);

public:
    void tcpClientReceiveData(void *tcp, char *buffer, int size);
    void tcpClientConnected(void *tcp);
    void tcpClientDisconnected(void *tcp);
    void tcpClientError(void *tcp);
private:
    TcpClient *mClient;
    pthread_t mHeartbeatThread;
    bool mExiting;
};

#endif // TESTCLIENT_H
