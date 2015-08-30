#ifndef MASTERSWITCHERAPP_H
#define MASTERSWITCHERAPP_H

#include "switchserver.h"

class MasterSwitcherApp
{
public:
    MasterSwitcherApp();
    ~MasterSwitcherApp();

    int run();

private:
    LocalPC         *mLocal;
    RemotePC        *mRemote;
    Gateway         *mGateway;
    SwitchServer    *mServer;
};

#endif // MASTERSWITCHERAPP_H
