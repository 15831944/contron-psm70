#ifndef REMOTEPC_H
#define REMOTEPC_H

#include "baseobject.h"
#include "itcpclient.h"
#include "tcpclient.h"
#include "localpc.h"
#include "iremotepc.h"

/**
 * @brief The RemotePC class
 * @note
 * 对方机实现功能:
 * 主动发起心跳连接,通过心跳报文发发送本机状态;
 * 分析心跳报文,本机和对方机均为备机时,本机的启动时间小于对方机的启动时间可将本机设置为主机;
 * 重连计数,重连错误则将本机设置为主机;
 */

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
    void setLocalPC(LocalPC *local);
    void setMaxConnect(int maxConnect);
    void setReconnectInterval(int interval);
    void setHandler(IRemotePC *handler);

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

protected:
    bool getIsSlave();
    double getTimePoint();
    void checkRemote(bool isSlave, double timePoint);
    int compareTimePoint(double timePoint1, double timePoint2);
    bool hasHandler();

private:
    TcpClient *mClient;

    //for heartbeat
    int mSendTime;
    int mHeartbeatCount;
    int mSendInterval;
    int mConnectCount;
    pthread_t mHeartbeatThread;
    bool mExiting;
    double mTimePoint;
    int mMaxConnect;

    LocalPC *mLocal;
    IRemotePC *mHandler;
};

#endif // REMOTEPC_H
