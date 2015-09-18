#include "switchserver.h"

#include "log.h"

#if WIN32
#define MASTER_SCRIPT "../scripts/onmaster.bat"
#define MASTER_COMMAND \
    "call "MASTER_SCRIPT
#else
#define MASTER_SCRIPT "../scripts/onmaster.sh"
#define MASTER_COMMAND \
    ". "MASTER_SCRIPT
#endif

#if WIN32
#define SLAVE_SCRIPT "../scripts/onslave.bat"
#define SLAVE_COMMAND \
    "call "SLAVE_SCRIPT
#else
#define SLAVE_SCRIPT "../scripts/onslave.sh"
#define SLAVE_COMMAND \
    ". "SLAVE_SCRIPT
#endif

SwitchServer::SwitchServer(Gateway *gateway, LocalPC *local, RemotePC *remote)
    : BaseObject()
    , IGateway()
    , IRemotePC()
    , mLocal(local), mRemote(remote), mGateway(gateway)
{
}

SwitchServer::~SwitchServer()
{

}

int SwitchServer::start()
{
    mGateway->setStateChange(this);
    mRemote->setHandler(this);
    mLocal->setHandler(this);

    mLocal->start();
    mGateway->start();
    mRemote->start();

    while(true)
    {
        Sleep(100);
    }

    return 0;
}

void SwitchServer::gatewayStateChanged()
{
    enter();
    if(GATEWAY_OFFLINE==mGateway->getState())
    {
        mRemote->disableSync();
        mLocal->makeSlave();
    }
    leave();
}

void SwitchServer::canBeMaster()
{
//    enter();
    APP_LOG("[SwitchServer]can be Master \n");
    mGateway->checkOnline();
    if(GATEWAY_ONLINE==mGateway->getState())
    {
        APP_LOG("[SwitchServer]remote is slave \n");
        mLocal->makeMaster();
    }
//    leave();
}

void SwitchServer::onLocalIsMaster()
{
    APP_LOG("[SwitchServer]local is master \n");
    char buffer[128];
    char command[128];
    memset(command, 0, sizeof(command));
    sprintf(command, MASTER_COMMAND);
    FILE *f;
    if((f = popen(command, "r")) == NULL)
        return;
    while(fgets(buffer, 128, f))
    {
        printf(buffer);
    }

    pclose(f);
    int ret = strcasecmp(buffer, "success");
    if(1==ret)
    {
        return;
    }
}

void SwitchServer::onLocalIsSlave()
{
    APP_LOG("[SwitchServer]local is slave \n");
    char buffer[128];
    char command[128];
    memset(command, 0, sizeof(command));
    sprintf(command, SLAVE_COMMAND);
    FILE *f;
    if((f = popen(command, "r")) == NULL)
        return;
    while(fgets(buffer, 128, f))
    {
        printf(buffer);
    }

    pclose(f);
    int ret = strcasecmp(buffer, "success");
    if(1==ret)
    {
        return;
    }
}

void SwitchServer::switchMaster()
{
    printGatewayState();
    LocalState state = adjudge(mGateway, mRemote);
    APP_LOG("[SERVER]make local %s\n", LOCAL_MASTER==state?"MASTER":"SLAVE");
    printf("[SERVER]make local %s\n", LOCAL_MASTER==state?"MASTER":"SLAVE");
    switch (state) {
    case LOCAL_MASTER:
        mLocal->makeMaster();
        break;
    default:
        mLocal->makeSlave();
        break;
    }
}

void SwitchServer::printGatewayState()
{    
    APP_LOG("[GATEWAY]%s\n", GATEWAY_ONLINE==mGateway->getState()?"online":"offline");
    printf("[GATEWAY]%s\n", GATEWAY_ONLINE==mGateway->getState()?"online":"offline");
}

LocalState SwitchServer::adjudge(Gateway *gateway, RemotePC *remote)
{
    switch (gateway->getState()) {
    case GATEWAY_ONLINE:
    {
        if(!remote->isConnected())
        {
//            if(!remote->execPing())
//            {
//                return LOCAL_MASTER;
//            }
        }
        else
        {

        }
    }
        break;
    default:
        break;
    }
    return LOCAL_SLAVE;
}

