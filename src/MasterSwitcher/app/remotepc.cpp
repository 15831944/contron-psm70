#include "remotepc.h"

#include "ping.h"
#include "heartbeatprotocol.h"

#define MAX_MISSED_HEARTBEAT 3


THREAD_API remote_heartbeat_thread(void *param)
{
    RemotePC *remote = (RemotePC *)param;

    int idle = 300;
    int count = 1;
    while(true)
    {
        bool connected = remote->isConnected();
        if(!connected)
        {
            break;  //外部断开连接
        }

        //1.快速发送心跳count次，确认连接，间隔1秒
        if(count)
        {
            count--;

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
            }
        }
        Sleep(idle);
    }//while
    return NULL;
}

RemotePC::RemotePC()
    :BaseObject()
    , IOnlineChecker()
    , ITcpClient()
{
    mIp = "10.7.5.33";
    mOnline = false;
    mSendTime = 0;
    mSendInterval = 3;
    mIsSlave = true;
    mTimePoint = 0.0;
    mHandler = NULL;

    mOnlineChecker = new OnlineChecker();
    mOnlineChecker->setOnlineChecker(this);
    mOnlineChecker->setCheckInterval(1);

    tcp = new TcpClient();
    tcp->setHandler(this);
}

RemotePC::~RemotePC()
{
    delete tcp;
    delete mOnlineChecker;
}

void RemotePC::setLocalPC(LocalPC *local)
{
    enter();
    mLocal = local;
    leave();
}

LocalPC *RemotePC::getLocalPC()
{
    LocalPC *result = NULL;

    enter();
    result = mLocal;
    leave();

    return result;
}

void RemotePC::setHandler(IRemotePC *handler)
{
    enter();
    mHandler = handler;
    leave();
}

void RemotePC::setIp(const char *ip)
{
    enter();
    mIp = (char *)ip;
    leave();
}

void RemotePC::setPort(int port)
{
    enter();
    mPort = port;
    leave();
}

void RemotePC::setReconnectInterval(int interval)
{
    enter();
    mReconnectInterval = interval;
    leave();
}

double RemotePC::getTimePoint()
{
    double result = 0.0;

    enter();
    result = mTimePoint;
    leave();

    return mTimePoint;

}

void RemotePC::setTimePoint(double timePoint)
{
    bool changed = false;
    enter();
    DOUBLE_CONVERTER source, target;
    source.d = mTimePoint;
    target.d = timePoint;
    if(memcmp(source.bytes, target.bytes, sizeof(double))!=0)
    {
        mTimePoint = timePoint;
        changed = true;
    }
    leave();
    if(changed)
    {
        timePointChanged();
    }
}

bool RemotePC::getIsSlave()
{
    bool result = false;

    enter();
    result = mIsSlave;
    leave();

    return result;
}

void RemotePC::setIsSlave(bool isSlave)
{
    enter();
    if(mIsSlave != isSlave)
    {
        mIsSlave = isSlave;
    }
    leave();
}

void RemotePC::start()
{
    mOnlineChecker->exec();

    tcp->setIp(mIp);
    tcp->setPort(mPort);
    tcp->setReconnectInterval(mReconnectInterval);
    tcp->setEnableReconnect(true);

    tcp->connect();
}

bool RemotePC::isConnected()
{
    return tcp->isConnected();
}

void RemotePC::enableHeartbeat()
{
    enter();
    mHeartbeatCount = 0;
    mSendTime = 0;
    int ret;
    THREAD_CREATE(&mHeartbeatThread[0], remote_heartbeat_thread, this, ret);
    if(ret)
    {
        THREAD_RUN(mHeartbeatThread[0], false);
    }
    leave();
}

void RemotePC::disenableHeartbeat()
{
    enter();
    THREAD_CLOSE(mHeartbeatThread[0]);
    leave();
}

void RemotePC::timePointChanged()
{
    bool isSlave = getIsSlave();
    if(!isSlave)
    {
        return;
    }
    double localTimePoint = getLocalPC()->getSetupTime();
    double remoteTimePoint = getTimePoint();
    if(remoteTimePoint-localTimePoint>0.0)
    {
        if(NULL!=mHandler)
        {
            mHandler->canBeMaster();
        }
    }
}

void RemotePC::tryBreakConnection()
{
    tcp->tryBreakConnection();
}

void RemotePC::heartbeat()
{
    enter();

    mHeartbeatCount++;
    char *p = NULL;
    int size = 0;
    HeartbeatProtocol protocol;
    bool isSlave = (mLocal->getState()==LOCAL_SLAVE);
    double timePoint = mLocal->getSetupTime();
    Heartbeat *hb = protocol.makeHeartbeat(isSlave, timePoint);
    if(NULL!=hb)
    {
        hb->makeBuffer(&p, size);
        tcp->send(p, size);
        delete p;
    }

    leave();
}

bool RemotePC::isSendFail()
{
    bool result = true;

    enter();
    result = (MAX_MISSED_HEARTBEAT<=mHeartbeatCount);
    leave();

    return result;

}

bool RemotePC::isSendTime()
{
    if(0==mSendTime)
    {
        return false;
    }
    bool result = false;
    int now;
    GET_TIME(now);

    enter();
    result = (now>=(mSendTime+mSendInterval));
    leave();

    return result;
}

void RemotePC::updateSendTime()
{
    enter();
    GET_TIME(mSendTime);
    leave();
}

void RemotePC::clearHeartbeatCount()
{
    enter();
    mHeartbeatCount = 0;
    leave();
}

void RemotePC::tcpClientConnected(void *tcp)
{
    UN_USE(tcp);
    enableHeartbeat();
}

void RemotePC::tcpClientDisconnected(void *tcp)
{
    UN_USE(tcp);
    disenableHeartbeat();
}

void RemotePC::tcpClientReceiveData(void *tcp, char *buffer, int size)
{
    UN_USE(tcp);
    HeartbeatProtocol protocol;
    Heartbeat *p = protocol.find(buffer, size);
    if(p!=NULL)
    {
        bool isSlave = p->getIsSlave();
        double timePoint = p->getTimePoint();
        delete p;

        clearHeartbeatCount();

        //注意调用顺序
        setIsSlave(isSlave);
        setTimePoint(timePoint);
    }
}


bool RemotePC::execPing()
{
    Ping ping;
    return ping.exec(mIp, 3);
}

void RemotePC::online()
{
    bool changed = false;
    enter();
    if(!mOnline)
    {
        mOnline = true;
        changed = true;
    }
    leave();
    if(changed)
    {
        stateChanged();
    }

}

void RemotePC::offline()
{
    bool changed = false;
    enter();
    if(mOnline)
    {
        mOnline = false;
        changed = true;
    }
    leave();
    if(changed)
    {
        stateChanged();
    }
}

void RemotePC::checkOnline()
{
    bool success = execPing();
    if(success)
    {
        online();
    }
    else
    {
        offline();
    }
}

void RemotePC::stateChanged()
{
    if(!mOnline)
    {
        tryBreakConnection();
    }
}
