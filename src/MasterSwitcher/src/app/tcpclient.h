#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "baseobject.h"
#include "sys/platform.h"
#include "conn/tcp.h"
#include "itcpclient.h"

//1. TCP连接不成功,耗时22秒左右

class TcpClient : public BaseObject
{
public:
    TcpClient();
    ~TcpClient();

public:
    int getFd();
    void setFd(SOCKET_HANDLE fd);

    bool getEnableReconnect();
    void setEnableReconnect(bool enable);

    void setIp(char *ip);
    void setPort(int port);
    void setReconnectInterval(int interval);

    void setHandler(ITcpClient *handler);

    void start(bool needConnect);
    void close();

    bool isExiting();

public:
    bool isStarted();
    bool isConnected();
    void receive();
    int send(char *buffer, int size);
    void tryBreakConnection();
    int connect();
    void handleData(char *buffer, int size);

    int getBrokenTime();
    int getReconnectInterval();

protected:
    void updateBrokenTime();

private:
    tcp_t mTcp;
    pthread_t receive_thread;
    pthread_t guard_thread;

    bool mEnableReconnect;
    int mReconnectInterval;
    int mBrokenTime;

    ITcpClient *mHandler;
    bool mStarted;
    bool mExiting;
};

#endif // TCPCLIENT_H
