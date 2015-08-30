#include "masterswitcherapp.h"

#include "log.h"
#include "config.h"


MasterSwitcherApp::MasterSwitcherApp()
{
    mLocal = new LocalPC();

    mRemote = new RemotePC();
    mRemote->setIp(config_instance()->RemoteIP);
    mRemote->setPort(config_instance()->HeartbeatPort);
    mRemote->setReconnectInterval(config_instance()->ReconnectInterval);

    mGateway = new Gateway();
    mGateway->setIp(config_instance()->FloatGateway);

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
    int ret = mServer->start();
    log_free();
    return ret;
}

