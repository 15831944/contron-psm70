#ifndef ITCPSERVER_H
#define ITCPSERVER_H

#include "sys/platform.h"

class ITcpServer
{
public:
    ITcpServer();
    ~ITcpServer();

    virtual void addNewClient(char *ip, int port, SOCKET_HANDLE fd)=0;

};

#endif // ITCPSERVER_H
