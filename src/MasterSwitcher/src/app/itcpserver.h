#ifndef ITCPSERVER_H
#define ITCPSERVER_H

#include "sys/platform.h"

class ITcpServer
{
public:
    ITcpServer();
    ~ITcpServer();

    virtual void addNewClient(void *tcp)=0;

};

#endif // ITCPSERVER_H
