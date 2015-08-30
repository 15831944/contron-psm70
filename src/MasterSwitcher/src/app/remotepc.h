#ifndef REMOTEPC_H
#define REMOTEPC_H

#include "baseobject.h"
#include "localstate.h"
#include "localpc.h"
#include "ionlinechecker.h"
#include "onlinechecker.h"
#include "tcpclient.h"
#include "itcpclient.h"
#include "iremotepc.h"

class RemotePC : public BaseObject
        , public IOnlineChecker
        , public ITcpClient
{
public:
    RemotePC();
    ~RemotePC();

    void setLocalPC(LocalPC *local);
    LocalPC *getLocalPC();

    void setHandler(IRemotePC *handler);

    void setIp(const char *ip);
    void setPort(int port);
    void setReconnectInterval(int interval);

    double getTimePoint();
    void setTimePoint(double timePoint);

    bool getIsSlave();
    void setIsSlave(bool isSlave);

    void start();

    bool isConnected();
    void tryBreakConnection();
    void heartbeat();
    bool isSendFail();
    bool isSendTime();
    void updateSendTime();
    void clearHeartbeatCount();

public:
    void tcpClientConnected(void *tcp);
    void tcpClientDisconnected(void *tcp);
    void tcpClientReceiveData(void *tcp, char *buffer, int size);
    void tcpClientError(void *tcp);

public:
    bool execPing();
    void online();
    void offline();
    void checkOnline();

protected:
    void stateChanged();
    void enableHeartbeat();
    void disenableHeartbeat();
    void timePointChanged();
    void clearConnectCount();
    void handleConnectCount();

private:
    LocalPC *mLocal;
    char *mIp;
    int mPort;
    int mReconnectInterval;
    int mConnectCount;

    pthread_t mHeartbeatThread[1];

    OnlineChecker *mOnlineChecker;

    bool mOnline;
    TcpClient *tcp;
    int mHeartbeatCount;
    int mSendTime;
    int mSendInterval;

    bool mIsSlave;
    double mTimePoint;

    IRemotePC *mHandler;
};

#endif // REMOTEPC_H
