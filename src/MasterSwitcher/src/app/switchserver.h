#ifndef SWITCHSERVER_H
#define SWITCHSERVER_H

#include "localpc.h"
#include "remotepc.h"
#include "gateway.h"
#include "igateway.h"
#include "localstate.h"
#include "iremotepc.h"

class SwitchServer :public IGateway, public IRemotePC
{
public:
    SwitchServer(Gateway *gateway, LocalPC *local, RemotePC *remote);
    virtual ~SwitchServer();

    int start();

    //IGateway
    void gatewayStateChanged();

    //IRemotePC
    void canBeMaster();

protected:
    void switchMaster();
    void printGatewayState();
    LocalState adjudge(Gateway *gateway, RemotePC *remote);

private:
    LocalPC         *mLocal;
    RemotePC        *mRemote;
    Gateway         *mGateway;

};

#endif // SWITCHSERVER_H
