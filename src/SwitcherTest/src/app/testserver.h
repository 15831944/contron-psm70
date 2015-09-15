#ifndef TESTSERVER_H
#define TESTSERVER_H

#include "tester.h"
#include "tcpclient.h"
#include "tcpserver.h"
#include "itcpclient.h"
#include "itcpserver.h"

class TestServer : public Tester, public ITcpServer
{
public:
    TestServer();
    ~TestServer();

    void setPort(int port);
    int start();

public:
    void tcpServerReceiveData(void *tcp, char *buffer, int size);

protected:
    void printData(double time, UINT32 timePoint);

private:
    TcpServer *mServer;
};

#endif // TESTSERVER_H
