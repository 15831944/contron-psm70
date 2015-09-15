#include "localpc.h"

#include "heartbeatprotocol.h"
#include "heartbeat.h"

#include "sys/platform.h"
#include "utils.h"
#include "ipconfig.h"
#include "ping.h"

#include "log.h"
#if QT
#define DEBUG_OUTPUT APP_LOG
#else
#define DEBUG_OUTPUT printf
#endif

LocalPC::LocalPC()
    : BaseObject()
    , ITcpServer()
{
    mState = LOCAL_SLAVE;

    mTcpServer = new TcpServer();
    mTcpServer->setHandler(this);
}
LocalPC::~LocalPC()
{
    enter();
    mTcpServer->close();
    leave();

    delete mTcpServer;
}

void LocalPC::setPort(int port)
{
    enter();
    mTcpServer->setPort(port);
    leave();
}

LocalState LocalPC::getState()
{
    LocalState result = LOCAL_SLAVE;

    enter();
    result = mState;
    leave();

    return result;
}

void LocalPC::setState(LocalState state)
{
    bool changed = false;
    enter();
    if(state != mState)
    {
        mState = state;
        changed = true;
        if(LOCAL_SLAVE == mState)
        {
            GET_TIME(mSetupTime);
        }
    }
    leave();
    if(changed)
    {
        stateChanged();
    }
}

double LocalPC::getSetupTime()
{
    double result;

    enter();
    result = mSetupTime;
    leave();

    return result;
}

void LocalPC::setSetupTime(double time)
{
    enter();
    mSetupTime = time;
    leave();
}

void LocalPC::setFloatIP(char *floatIP)
{
    enter();
    mFloatIP = floatIP;
    DEBUG_OUTPUT("[LocalPC]float ip:%s\n", mFloatIP);
    leave();
}

void LocalPC::setFloatGateway(char *gateway)
{
    enter();
    mFloatGateway = gateway;
    DEBUG_OUTPUT("[LocalPC]float gateway:%s\n", mFloatGateway);
    leave();
}

void LocalPC::setFloatNetmask(char *netmask)
{
    enter();
    mFloatNetmask = netmask;
    DEBUG_OUTPUT("[LocalPC]float netmask:%s\n", mFloatNetmask);
    leave();
}

void LocalPC::setEthernet(char *ethernet)
{
    enter();
    mEthernet = ethernet;
    DEBUG_OUTPUT("[LocalPC]ethernet:%s\n", mEthernet);
    leave();
}

void LocalPC::setHandler(ILocalPC *handler)
{
    enter();
    mHandler = handler;
    leave();
}

int LocalPC::start()
{
    int result = NUMBER_FALSE;

    enter();
    result = mTcpServer->start();
    leave();

    return result;
}

void LocalPC::makeMaster()
{
    bool online = false;
    DEBUG_OUTPUT("[LocalPC]try make MASTER\n");
    enter();
    online = floatIPOnline();
    leave();
    if(online)
    {
        DEBUG_OUTPUT("[LocalPC]ip(%s) online\n", mFloatIP);
        return;
    }

    DEBUG_OUTPUT("[LocalPC]I'm %s\n", LOCAL_MASTER==getState()?"MASTER":"SLAVE");
    bool master = !isSlave();
    if(master)
    {
        makeSlave();
        return;
    }

    enter();
    DEBUG_OUTPUT("[LocalPC]add ip(%s) to ethernet(%s) ... \n", mFloatIP, mEthernet);
    Ipconfig ipconfig;
    ipconfig.addIP(mEthernet, mFloatIP, mFloatNetmask, mFloatGateway);
    online = floatIPOnline();
    leave();
    if(online)
    {
        DEBUG_OUTPUT("[LocalPC]add ip ok\n");
        setState(LOCAL_MASTER);
    }
}

void LocalPC::makeSlave()
{
    bool found = false;
    enter();
    DEBUG_OUTPUT("[LocalPC]delete ip(%s) from ethernet(%s) ... \n", mFloatIP, mEthernet);
    Ipconfig ipconfig;
    ipconfig.delIP(mEthernet, mFloatIP);
    found = (!ipconfig.hasIP(mFloatIP));
    leave();
    if(found)
    {
        DEBUG_OUTPUT("[LocalPC]delete ip ok\n");
        setState(LOCAL_SLAVE);
    }
}

bool LocalPC::isSlave()
{
    bool result = false;
    enter();
    result = (LOCAL_SLAVE==mState);
    leave();
    return result;
}

void LocalPC::tcpServerReceiveData(void *tcp, char *buffer, int size)
{
    enter();

    DEBUG_OUTPUT("[LocalPC]receive:\n%s\n", buffer_format(buffer, size));
    HeartbeatProtocol protocol;
    Heartbeat *hb = protocol.find(buffer, size);
    if(hb!=NULL)
    {
        delete hb;

        char *p = NULL;
        int size = 0;
        bool isSlave = false;
        double timePoint;
        GET_TIME(timePoint);
        Heartbeat *t = protocol.makeHeartbeat(isSlave, timePoint);
        if(NULL!=t)
        {
            if(t->makeBuffer(&p, size))
            {
                TcpClient *client = (TcpClient *)tcp;
                client->send(p, size);
                delete p;
            }
        }

    }
    leave();
}

bool LocalPC::floatIPOnline()
{
    Ping ping;
    return ping.exec(mFloatIP, 3);
}


void LocalPC::stateChanged()
{
    bool slave = isSlave();

    if(slave)
    {
        updateSetupTime();
        emitOnSlave();
    }
    else
    {
        emitOnMaster();
    }
}

void LocalPC::updateSetupTime()
{
    enter();
    GET_TIME(mSetupTime);
    leave();
}

void LocalPC::emitOnSlave()
{
    DEBUG_OUTPUT("[LocalPC]emit OnSlave\n");
    bool hasHandler = false;
    enter();
    hasHandler = (NULL!=mHandler);
    leave();

    if(!hasHandler)
    {
        return;
    }
    enter();
    mHandler->onLocalIsSlave();
    leave();
}

void LocalPC::emitOnMaster()
{
    DEBUG_OUTPUT("[LocalPC]emit OnMaster\n");
    bool hasHandler = false;
    enter();
    hasHandler = (NULL!=mHandler);
    leave();

    if(!hasHandler)
    {
        return;
    }

    enter();
    mHandler->onLocalIsMaster();
    leave();
}

