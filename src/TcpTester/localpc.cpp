#include "localpc.h"

#include "heartbeatprotocol.h"
#include "heartbeat.h"

#include "sys/platform.h"
#include "utils.h"
#include "ipconfig.h"
#include "ping.h"

#if QT
#include "log.h"
#define DEBUG_OUTPUT APP_LOG
#else
#define DEBUG_OUTPUT printf
#endif

#if WIN32
#define SWITCH_SCRIPT "../scripts/onmaster.bat"
#define SWITCH_COMMAND \
    "call "SWITCH_SCRIPT
#else
#define SWITCH_SCRIPT "../scripts/onmaster.sh"
#define SWITCH_COMMAND \
    ". "SWITCH_SCRIPT
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
    printf("[LOCAL]float ip:%s\n", mFloatIP);
    leave();
}

void LocalPC::setFloatGateway(char *gateway)
{
    enter();
    mFloatGateway = gateway;
    printf("[LOCAL]float gateway:%s\n", mFloatGateway);
    leave();
}

void LocalPC::setFloatNetmask(char *netmask)
{
    enter();
    mFloatNetmask = netmask;
    printf("[LOCAL]float netmask:%s\n", mFloatNetmask);
    leave();
}

void LocalPC::setEthernet(char *ethernet)
{
    enter();
    mEthernet = ethernet;
    printf("[LOCAL]ethernet:%s\n", mEthernet);
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
    enter();

    APP_LOG("[LOCAL]try make MASTER\n");
    printf("[LOCAL]try make MASTER\n");
    if(floatIPOnline())
    {
        APP_LOG("[LOCAL]ip(%s) online\n", mFloatIP);
        printf("[LOCAL]ip(%s) online\n", mFloatIP);
        return;
    }

    APP_LOG("[LOCAL]I'm %s\n", LOCAL_MASTER==getState()?"MASTER":"SLAVE");
    printf("[LOCAL]I'm %s\n", LOCAL_MASTER==getState()?"MASTER":"SLAVE");
    if(LOCAL_MASTER==getState())
    {
        if(!floatIPOnline())
        {
            makeSlave();
        }
        return;
    }

    APP_LOG("[LOCAL]add ip(%s) to ethernet(%s) ... \n", mFloatIP, mEthernet);
    printf("[LOCAL]add ip(%s) to ethernet(%s) ... \n", mFloatIP, mEthernet);
    Ipconfig ipconfig;
    ipconfig.addIP(mEthernet, mFloatIP, mFloatNetmask, mFloatGateway);
    if(floatIPOnline())
    {
        APP_LOG("[LOCAL]add ip ok\n");
        printf("[LOCAL]add ip ok\n");
        setState(LOCAL_MASTER);


        APP_LOG("[LOCAL]exec script\n");
        printf("[LOCAL]exec script\n");
        execScript();
    }
    leave();

}

void LocalPC::makeSlave()
{
    APP_LOG("[LOCAL]delete ip(%s) from ethernet(%s) ... \n", mFloatIP, mEthernet);
    Ipconfig ipconfig;
    ipconfig.delIP(mEthernet, mFloatIP);
    if(!ipconfig.hasIP(mFloatIP))
    {
        APP_LOG("[LOCAL]delete ip ok\n");
        printf("[LOCAL]delete ip ok\n");
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

    printf("[LocalPC]receive:\n%s\n", buffer_format(buffer, size));
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

bool LocalPC::execScript()
{
    char buffer[128];
    char command[128];
    memset(command, 0, sizeof(command));
    sprintf(command, SWITCH_COMMAND);
    FILE *f;
    if((f = popen(command, "r")) == NULL)
        return false;
    while(fgets(buffer, 128, f))
    {
        printf(buffer);
    }

    pclose(f);
    int ret = strcasecmp(buffer, "success");
    if(1==ret)
    {
        return true;
    }
    return false;
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

