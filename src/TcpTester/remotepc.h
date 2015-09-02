#ifndef REMOTEPC_H
#define REMOTEPC_H

#include "baseobject.h"
#include "itcpclient.h"
#include "tcpclient.h"

class RemotePC: public BaseObject, public ITcpClient
{
public:
    RemotePC();
    ~RemotePC();

public:
    void start();
    void setIp(char *ip);
    void setPort(int port);
    void setEnableReconnect(bool enable);

public:
    void tcpClientReceiveData(void *tcp, char *buffer, int size);
    void tcpClientConnected(void *tcp);
    void tcpClientDisconnected(void *tcp);
    void tcpClientError(void *tcp);

public:
    bool isConnected();
    void heartbeat();
    void updateSendTime();
    bool isSendFail();
    void tryBreakConnection();
    bool isSendTime();
    void clearConnectCount();
    void clearHeartbeatCount();
    void enableHeartbeat();
    void disableHeartbeat();
    bool isExiting();
    void handleConnectCount();

private:
    TcpClient *mClient;

    //for heartbeat
    int mSendTime;
    int mHeartbeatCount;
    int mSendInterval;
    int mConnectCount;
    pthread_t mHeartbeatThread;
    bool mExiting;
};

#endif // REMOTEPC_H
