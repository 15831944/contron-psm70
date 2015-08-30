#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "baseobject.h"
#include "sys/platform.h"
#include "conn/tcp.h"
#include "itcpclient.h"

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

public:
    bool isConnected();
    void receive(fd_set &fds);
    int send(char *buffer, int size);
    int connect();
    void tryBreakConnection();
    void handleData(char *buffer, int size);

    int getBrokenTime();
    int getReconnectInterval();
private:
    tcp_t mTcp;
    pthread_t receive_thread;
    pthread_t guard_thread;

    bool mEnableReconnect;
    int mReconnectInterval;
    int mBrokenTime;

    ITcpClient *mHandler;
};

#endif // TCPCLIENT_H
