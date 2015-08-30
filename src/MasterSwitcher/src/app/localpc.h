#ifndef LOCALPC_H
#define LOCALPC_H

#include "baseobject.h"
#include "localstate.h"

class LocalPC : public BaseObject
{
public:
    LocalPC();
    ~LocalPC();

    void setFloatIP(char *floatIP);
    void setFloatGateway(char *gateway);
    void setEthernet(char *ethernet);
    void setPort(int port);

    void makeMaster();
    void makeSlave();

    LocalState getState();
    void setState(LocalState state);

    double getSetupTime();
protected:
    bool floatIPOnline();
    bool floatGatewayOnline();
    bool hasFloatIP();
    bool execScript();

private:
    LocalState mState;

    char *mFloatIP;
    char *mFloatGateway;
    char *mEthernet;
    double mSetupTime;
    
    int mPort;
};

#endif // LOCALPC_H
