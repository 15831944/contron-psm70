#ifndef ITCPCLIENT_H
#define ITCPCLIENT_H


class ITcpClient
{
public:
    ITcpClient();
    ~ITcpClient();

    virtual void tcpClientReceiveData(void *tcp, char *buffer, int size)=0;
    virtual void tcpClientConnected(void *tcp)=0;
    virtual void tcpClientDisconnected(void *tcp)=0;
    virtual void tcpClientError(void *tcp);
};

#endif // ITCPCLIENT_H
