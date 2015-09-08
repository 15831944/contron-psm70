#include "remotepc.h"

#include "utils.h"
#include "heartbeatprotocol.h"
#include "heartbeat.h"

#if QT
#include "log.h"
#define DEBUG_OUTPUT APP_LOG
#else
#define DEBUG_OUTPUT printf
#endif


#define MAX_MISSED_HEARTBEAT 3


THREAD_API remote_heartbeat_thread(void *param)
{
    RemotePC *remote = (RemotePC *)param;

    int idle = 300;
    int count = 1;
    while(true)
    {
        Sleep(10);
        THREAD_WAITEXIT();

        if(remote->isExiting())
        {
            break;
        }

        bool connected = remote->isConnected();
        if(!connected)
        {
            break;  //外部断开连接
        }

        //1.快速发送心跳count次，确认连接，间隔1秒
        if(count)
        {
            count--;

            DEBUG_OUTPUT("[HEARTBEAT]fast heartbeat: %d\n", count);
            remote->heartbeat();

            if(!count)
            {
                remote->updateSendTime();
            }

            int fast_interval = 1000;
            Sleep(fast_interval);
            continue;
        }
        //4.检测心跳发送三次无回应
        {
            bool send_error = remote->isSendFail();
            if(send_error)
            {
                remote->tryBreakConnection();
                break;
            }
        }
        //3.定时发送心跳
        {
            bool outOfTime = remote->isSendTime();
            if(outOfTime)
            {
                remote->heartbeat();
                remote->updateSendTime();
            }
        }
        Sleep(idle);
    }//while
    DEBUG_OUTPUT("[RemotePC]heartbeat thread exit\n");
    return NULL;
}

RemotePC::RemotePC()
    : BaseObject()
    , ITcpClient()
{
    mSendTime = 0;
    mHeartbeatCount = 0;
    mSendInterval = 3;
    mConnectCount = 0;
    mExiting = false;

    mClient = new TcpClient();
    mClient->setHandler(this);
}

RemotePC::~RemotePC()
{
    enter();
    mExiting = true;
    mClient->close();
    leave();

    Sleep(200);

    if(NULL!=mClient)
    {
        delete mClient;
        mClient = NULL;
    }
}

void RemotePC::start()
{
    enter();
    mClient->start(true);
    leave();
}

void RemotePC::setIp(char *ip)
{
    enter();
    mClient->setIp(ip);
    leave();
}

void RemotePC::setPort(int port)
{
    enter();
    mClient->setPort(port);
    leave();
}

void RemotePC::setEnableReconnect(bool enable)
{
    enter();
    mClient->setEnableReconnect(enable);
    leave();
}

void RemotePC::tcpClientReceiveData(void *tcp, char *buffer, int size)
{

    bool found = false;
    enter();

    DEBUG_OUTPUT("[RemotePC]receive:\t%s\n", buffer_format(buffer, size));
    HeartbeatProtocol protocol;
    Heartbeat *hb = protocol.find(buffer, size);
    if(hb!=NULL)
    {
        delete hb;
        if(tcp==mClient)
        {
            found = true;
        }
    }
    leave();
    if(found)
    {
        DEBUG_OUTPUT("[RemotePC]clear heartbeat\n");
        clearHeartbeatCount();
    }
}

void RemotePC::tcpClientConnected(void *tcp)
{
    UN_USE(tcp);

    enter();

    clearConnectCount();
    enableHeartbeat();
    leave();
}

void RemotePC::tcpClientDisconnected(void *tcp)
{
    UN_USE(tcp);

    enter();
    disableHeartbeat();
    leave();

}

void RemotePC::tcpClientError(void *tcp)
{    
    UN_USE(tcp);

    enter();
    handleConnectCount();
    leave();
}

bool RemotePC::isConnected()
{
    bool result = false;
    enter();
    if(NULL!=mClient)
    {
        result = mClient->isConnected();
    }
    leave();
    return result;
}

void RemotePC::heartbeat()
{
    enter();

    mHeartbeatCount++;
    char *p = NULL;
    int size = 0;
    HeartbeatProtocol protocol;
    bool isSlave = false;
    double timePoint;
    GET_TIME(timePoint);
    Heartbeat *hb = protocol.makeHeartbeat(isSlave, timePoint);
    if(NULL!=hb)
    {
        if(hb->makeBuffer(&p, size))
        {
            mClient->send(p, size);
            delete p;
        }
    }

    leave();
}

void RemotePC::updateSendTime()
{
    enter();
    GET_TIME(mSendTime);
    DEBUG_OUTPUT("[HEARTBEAT]update send time:%d\n", mSendTime);
    leave();
}

bool RemotePC::isSendFail()
{
    bool result = true;

    enter();
    result = (MAX_MISSED_HEARTBEAT<=mHeartbeatCount);
    if(result)
    {
        DEBUG_OUTPUT("[HEARTBEAT]is send fail:%d\n", mHeartbeatCount);
    }
    leave();

    return result;
}

void RemotePC::tryBreakConnection()
{
    enter();
    mClient->tryBreakConnection();
    leave();
}

bool RemotePC::isSendTime()
{
    bool result = false;

    enter();
    if(0!=mSendTime)
    {
        int now;
        GET_TIME(now);

        result = (now>=(mSendTime+mSendInterval));
        if(result)
        {
            DEBUG_OUTPUT("[HEARTBEAT]is send time:mSendTime=%d mSendInterval=%d now=%d\n", mSendTime, mSendInterval, now);
        }
    }
    leave();

    return result;
}

void RemotePC::clearConnectCount()
{
    enter();
    mConnectCount = 0;
    leave();
}

void RemotePC::clearHeartbeatCount()
{
    enter();
    mHeartbeatCount = 0;
    leave();
}

void RemotePC::enableHeartbeat()
{
    enter();
    mHeartbeatCount = 0;
    mSendTime = 0;
    int ret;
    THREAD_CREATE(&mHeartbeatThread, remote_heartbeat_thread, this, ret);
    if(ret)
    {
        THREAD_RUN(mHeartbeatThread, false);
    }
    leave();
}

void RemotePC::disableHeartbeat()
{
    enter();
    THREAD_CLOSE(mHeartbeatThread);
    leave();
}

bool RemotePC::isExiting()
{
    bool result = false;
    enter();
    result = mExiting;
    leave();
    return result;
}

void RemotePC::handleConnectCount()
{
    bool connectError = false;
    enter();
    int maxConnect = 3;
    mConnectCount++;
    if(maxConnect<=mConnectCount)
    {
        mConnectCount = 0;
        connectError = true;
    }
    leave();
    if(connectError)
    {
        DEBUG_OUTPUT("[RemotePC]connect fail\n");
//        if(NULL!=mHandler)
//        {
//            mHandler->canBeMaster();
//        }
    }
}
