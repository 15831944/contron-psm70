#ifndef LOCALPC_H
#define LOCALPC_H

#include "baseobject.h"
#include "itcpserver.h"
#include "tcpserver.h"
#include "tcpclient.h"
#include "localstate.h"
#include "ilocalpc.h"

/**
 * @brief The LocalPC class
 * @note
 * 本机实现如下功能:
 * 监听心跳端口,接收对方机心跳,并向对方机回应本机状态;
 * 设置为主机;
 * 设置为备机;
 */

class LocalPC : public BaseObject, ITcpServer
{
public:
    LocalPC();
    ~LocalPC();

public:
    void setPort(int port);
    LocalState getState();
    void setState(LocalState state);
    double getSetupTime();
    void setSetupTime(double time);

    void setFloatIP(char *floatIP);
    void setFloatGateway(char *gateway);
    void setFloatNetmask(char *netmask);
    void setEthernet(char *ethernet);

    void setHandler(ILocalPC *handler);

public:
    int start();
    void makeMaster();
    void makeSlave();
    bool isSlave();

public:
    void tcpServerReceiveData(void *tcp, char *buffer, int size);
    void closeTcpClient();

protected:
    bool floatIPOnline();
    void stateChanged();
    void updateSetupTime();
    void emitOnSlave();
    void emitOnMaster();

private:
    TcpServer *mTcpServer;

    LocalState mState;
    double mSetupTime;

    char *mFloatIP;
    char *mFloatGateway;
    char *mFloatNetmask;
    char *mEthernet;

    ILocalPC *mHandler;

};

#endif // LOCALPC_H
