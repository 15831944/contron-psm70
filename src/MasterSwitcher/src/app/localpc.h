#ifndef LOCALPC_H
#define LOCALPC_H

#include "baseobject.h"
#include "localstate.h"
#include "itcpserver.h"
#include "itcpclient.h"
#include "tcpserver.h"

class LocalPC : public BaseObject, public ITcpServer, public ITcpClient
{
public:
    LocalPC();
    ~LocalPC();

    void setFloatIP(char *floatIP);
    void setFloatGateway(char *gateway);
    void setFloatNetmask(char *netmask);
    void setEthernet(char *ethernet);
    void setPort(int port);

    void start();

    void makeMaster();
    void makeSlave();

    LocalState getState();
    void setState(LocalState state);

    double getSetupTime();
public:
    void addNewClient(void *tcp);
    void tcpClientReceiveData(void *tcp, char *buffer, int size);
    void tcpClientConnected(void *tcp);
    void tcpClientDisconnected(void *tcp);

protected:
    bool floatIPOnline();
    bool floatGatewayOnline();
    bool hasFloatIP();
    bool execScript();

private:
    LocalState mState;

    char *mFloatIP;
    char *mFloatGateway;
    char *mFloatNetmask;
    char *mEthernet;
    double mSetupTime;
    
    int mPort;
    TcpServer *mTcpServer;

};

#endif // LOCALPC_H
