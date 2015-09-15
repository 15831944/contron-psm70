#include "masterswitcherapp.h"

#include "log.h"
#include "config.h"
#include "tcp.h"


MasterSwitcherApp::MasterSwitcherApp()
{
    mLocal = new LocalPC();

    mRemote = new RemotePC();

    mGateway = new Gateway();

    mServer = new SwitchServer(mGateway, mLocal, mRemote);
}

MasterSwitcherApp::~MasterSwitcherApp()
{
    delete mServer;
    delete mGateway;
    delete mRemote;
    delete mLocal;
}

int MasterSwitcherApp::run()
{
    log_init();
    tcp_init();

    config_t *config = loadConfig();
    if(NULL==config)
    {
        return -1;
    }

    mRemote->setLocalPC(mLocal);
    mRemote->setIp(config->RemoteIP);
    mRemote->setPort(config->HeartbeatPort);
    mRemote->setEnableReconnect(true);
    mRemote->setReconnectInterval(config->ReconnectInterval);
    mRemote->setSyncInterval(config->SyncInterval);

    mGateway->setIp(config->FloatGateway);

    mLocal->setPort(config->HeartbeatPort);
    mLocal->setEthernet(config->LocalEthernet);
    mLocal->setFloatIP(config->FloatIP);
    mLocal->setFloatGateway(config->FloatGateway);
    mLocal->setFloatNetmask(config->FloatNetmask);

    int ret = mServer->start();

    delete config;
    tcp_free();
    log_free();
    return ret;
}

