#include "testserver.h"

#include "heartbeatprotocol.h"
#include "heartbeat.h"

#include "utils.h"

#include "log.h"
#if QT
#define DEBUG_OUTPUT APP_LOG
#else
#define DEBUG_OUTPUT printf
#endif

TestServer::TestServer()
    : Tester()
{
    mServer = new TcpServer();
    mServer->setHandler(this);
}

TestServer::~TestServer()
{
    mServer->close();
    delete mServer;
}

void TestServer::setPort(int port)
{
    enter();
    mServer->setPort(port);
    leave();
}

int TestServer::start()
{
    int result = 0;
    enter();
    result = mServer->start();
    leave();

    return result;
}

void TestServer::tcpServerReceiveData(void *tcp, char *buffer, int size)
{
    UN_USE(tcp);

    TcpClient *client = (TcpClient *)tcp;
    client->send(buffer, size);

    DEBUG_OUTPUT("[TestServer]receive:\t%s\n", buffer_format(buffer, size));
    HeartbeatProtocol protocol;
    Heartbeat *hb = protocol.find(buffer, size);
    if(hb==NULL)
    {
        return;
    }
    hb->print();
    delete hb;
}

void TestServer::printData(double time, UINT32 timePoint)
{
    char strTime[128];
    memset(strTime, 0, 128);

    struct timeval tv;
    TIME_TO_STRUCT(time, tv);

   struct tm *ptm = localtime(&tv.tv_sec);  //将秒转换成struct tm的形式
   strftime(strTime, 128, "%Y-%m-%d %H:%M:%S", ptm);
   printf("now=%s.%ld\n", strTime, tv.tv_usec); //输出精确到微秒


    APP_LOG("%s.%ld\t%d\n", strTime, tv.tv_usec, timePoint);
}

