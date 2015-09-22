#include "testclient.h"

#include "heartbeatprotocol.h"
#include "heartbeat.h"

#include "log.h"
#if QT
#define DEBUG_OUTPUT APP_LOG
#else
#define DEBUG_OUTPUT printf
#endif

#include "utils.h"

THREAD_API remote_heartbeat_thread(void *param)
{
    TestClient *remote = (TestClient *)param;

    UINT32 seed = 0;
    int idle = 100;
    int sendTime;
    GET_TIME(sendTime);
    while(true)
    {
        Sleep(10);
        THREAD_WAITEXIT();

        if(remote->isExiting())
        {
            break;
        }        


        //定时间隔1秒发送
        int timeout = sendTime + 1;
        int now;
        GET_TIME(now);
        if(now>=timeout)
        {
            seed++;
            DEBUG_OUTPUT("[TestClient]seed=%d\n", seed);
            GET_TIME(sendTime);
            remote->heartbeat(seed);
        }

        remote->testTcpBroken();

        Sleep(idle);
    }//while
    DEBUG_OUTPUT("[TestClient]heartbeat thread exit\n");
    return NULL;
}


TestClient::TestClient()
    : Tester()
    , ITcpClient()
{
    mExiting = false;
    mClient = new TcpClient();
    mClient->setEnableReconnect(true);
    mClient->setHandler(this);

    int ret;
    THREAD_CREATE(&mHeartbeatThread, remote_heartbeat_thread, this, ret);
    if(ret)
    {
        THREAD_RUN(mHeartbeatThread, false);
    }
}

TestClient::~TestClient()
{
    enter();
    if(!mExiting)
    {
        mExiting = true;
        THREAD_CLOSE(mHeartbeatThread);
        mClient->close();
    }
    leave();

    Sleep(200);

    if(NULL!=mClient)
    {
        delete mClient;
        mClient = NULL;
    }
}

void TestClient::setIp(char *ip)
{
    enter();
    mClient->setIp(ip);
    leave();
}

void TestClient::setPort(int port)
{
    enter();
    mClient->setPort(port);
    leave();
}

int TestClient::start()
{
    enter();
    mClient->start(true);
    leave();

    return 0;
}

bool TestClient::isExiting()
{
    bool result = false;
    enter();
    result = mExiting;
    leave();
    return result;
}

bool TestClient::isConnected()
{
    bool result = false;
    enter();
    result = mClient->isConnected();
    leave();
    return result;
}

void TestClient::heartbeat(UINT32 timePoint)
{
    char *p = NULL;
    int size = 0;
    HeartbeatProtocol protocol;
    Heartbeat *hb = protocol.make(timePoint);
    if(NULL==hb)
    {
        return;
    }
    if(!hb->makeBuffer(&p, size))
    {
        delete hb;
        return;
    }
    bool connected = isConnected();
    if(!connected)
    {
        delete hb;
        delete p;
        return;  //外部断开连接
    }
    enter();
    mClient->send(p, size);
    mSendCount++;
    leave();

    hb->print();

    delete hb;
    delete p;
}

void TestClient::testTcpBroken()
{
    enter();
    if(4<=mSendCount)
    {
        mClient->tryBreakConnection();
    }
    leave();
}

void TestClient::tcpClientReceiveData(void *tcp, char *buffer, int size)
{
    UN_USE(tcp);
    bool found = false;
    DEBUG_OUTPUT("[TestClient]receive:\t%s\n", buffer_format(buffer, size));
    HeartbeatProtocol protocol;
    Heartbeat *hb = protocol.find(buffer, size);
    if(hb!=NULL)
    {
        found = true;
        delete hb;
    }
    if(found)
    {
        enter();
        mSendCount--;
        leave();
    }
}

void TestClient::tcpClientConnected(void *tcp)
{
    UN_USE(tcp);
    enter();
    mSendCount = 0;
    leave();
}

void TestClient::tcpClientDisconnected(void *tcp)
{
    UN_USE(tcp);
}

void TestClient::tcpClientError(void *tcp)
{
    UN_USE(tcp);
}

