#include "localpc.h"

#include "ping.h"
#include "ipconfig.h"

#include "sys/platform.h"
#include "log.h"

#include "heartbeatprotocol.h"
#include "heartbeat.h"
#include "utils.h"

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
    :BaseObject()
    , ITcpServer()
    , ITcpClient()
{
    mState = LOCAL_SLAVE;
    mEthernet = "bond0";
    mFloatIP = "10.7.5.27";
    mFloatGateway = "10.7.5.254";
    mFloatNetmask = "255.255.255.0";

    GET_TIME(mSetupTime);

    mTcpServer = new TcpServer();
    mTcpServer->setHandler(this);
}

LocalPC::~LocalPC()
{
    delete mTcpServer;
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

void LocalPC::setPort(int port)
{
    enter();
    mPort = port;
    printf("[LOCAL]heartbeat port:%d\n", mPort);
    leave();
}

void LocalPC::start()
{
    enter();

    if(hasFloatIP()&&floatGatewayOnline())
    {
        mState = LOCAL_MASTER;
    }

    mTcpServer->setPort(mPort);
    mTcpServer->start();
    leave();
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
    enter();
    if(state != mState)
    {
        mState = state;
        if(LOCAL_SLAVE == mState)
        {
            GET_TIME(mSetupTime);
        }
    }
    leave();
}

double LocalPC::getSetupTime()
{
    double result;

    enter();
    result = mSetupTime;
    leave();

    return result;
}

void LocalPC::addNewClient(void *tcp)
{
    TcpClient *client = (TcpClient *)tcp;
    client->setHandler(this);
}

void LocalPC::tcpClientReceiveData(void *tcp, char *buffer, int size)
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
        bool isSlave = (getState()==LOCAL_SLAVE);
        double timePoint = getSetupTime();
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

void LocalPC::tcpClientConnected(void *tcp)
{
    UN_USE(tcp);
}

void LocalPC::tcpClientDisconnected(void *tcp)
{
    TcpClient *client = (TcpClient *)tcp;
    delete client;
}

bool LocalPC::floatIPOnline()
{
    Ping ping;
    return ping.exec(mFloatIP, 3);
}

bool LocalPC::floatGatewayOnline()
{
    Ping ping;
    return ping.exec(mFloatIP, 3);
}

bool LocalPC::hasFloatIP()
{
    Ipconfig ipconfig;
    return ipconfig.hasIP(mFloatIP);
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

