#include "localpc.h"

#include "ping.h"
#include "ipconfig.h"

#include "sys/platform.h"
#include "log.h"

#if WIN32
#define SWITCH_SCRIPT "scripts/onmaster.bat"
#define SWITCH_COMMAND \
    "call "SWITCH_SCRIPT
#else
#define SWITCH_SCRIPT "scripts/onmaster.sh"
#define SWITCH_COMMAND \
    ". "SWITCH_SCRIPT
#endif

LocalPC::LocalPC()
    :BaseObject()
{
    mState = LOCAL_SLAVE;
    mEthernet = "bond0";
    mFloatIP = "10.7.5.27";
    mFloatGateway = "10.7.5.254";

    GET_TIME(mSetupTime);

    if(hasFloatIP()&&floatGatewayOnline())
    {
        mState = LOCAL_MASTER;
    }
}

LocalPC::~LocalPC()
{

}

void LocalPC::setFloatIP(char *floatIP)
{
    enter();
    mFloatIP = floatIP;
    leave();
}

void LocalPC::setFloatGateway(char *gateway)
{
    enter();
    mFloatGateway = gateway;
    leave();
}

void LocalPC::setEthernet(char *ethernet)
{
    enter();
    mEthernet = ethernet;
    leave();
}

void LocalPC::setPort(int port)
{
    enter();
    mPort = port;
    leave();
}

void LocalPC::makeMaster()
{    
    enter();

    APP_LOG("[LOCAL]check ip(%s) exists\n", mFloatIP);
    printf("[LOCAL]check ip(%s) exists\n", mFloatIP);
    if(floatIPOnline())
    {
        return;
    }

    APP_LOG("[LOCAL]I'm %s\n", LOCAL_MASTER==getState()?"MASTER":"SLAVE");
    printf("[LOCAL]I'm %s\n", LOCAL_MASTER==getState()?"MASTER":"SLAVE");
    if(LOCAL_MASTER==getState())
    {
        return;
    }

    APP_LOG("[LOCAL]add ip(%s) to ethernet(%s) ... \n", mFloatIP, mEthernet);
    printf("[LOCAL]add ip(%s) to ethernet(%s) ... \n", mFloatIP, mEthernet);
    Ipconfig ipconfig;
    if(ipconfig.addIP(mEthernet, mFloatIP))
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

    result = result;
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

