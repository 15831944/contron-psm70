#ifndef LOCALPC_H
#define LOCALPC_H

#include "baseobject.h"
#include "itcpserver.h"
#include "tcpserver.h"
#include "tcpclient.h"

/**
 * @brief The LocalPC class
 * @note
 * 本机实现如下功能:
 * 监听心跳端口,接收对方机心跳,并向对方机回应本机状态;
 * 添加浮动IP;
 * 删除浮动IP;
 * 检测浮动IP是否在线,不在线时本机下线并删除浮动IP
 */

class LocalPC : public BaseObject, ITcpServer
{
public:
    LocalPC();
    ~LocalPC();

public:
    void setPort(int port);
    int start();

public:
    void tcpServerReceiveData(void *tcp, char *buffer, int size);

private:
    TcpServer *mTcpServer;

};

#endif // LOCALPC_H
