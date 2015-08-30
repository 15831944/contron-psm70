#ifndef GATEWATY_H
#define GATEWATY_H

#include "baseobject.h"
#include "igateway.h"
#include "ionlinechecker.h"
#include "onlinechecker.h"

enum GatewayState {
    GATEWAY_ONLINE,
    GATEWAY_OFFLINE
};

class Gateway : public BaseObject, public IOnlineChecker
{
public:
    Gateway();
    ~Gateway();

    void start();

    IGateway *getStateChange() { return mStateChange; }
    void setStateChange(IGateway *stateChange);
    void setIp(char *ip);

    GatewayState getState() { return mState; }

public:
    bool execPing();
    void online();
    void offline();
    void checkOnline();

protected:
    void stateChanged();

private:
    IGateway *mStateChange;
    GatewayState mState;

    char *mGatewayIP;
    OnlineChecker *mOnlineChecker;

};

#endif // GATEWATY_H
