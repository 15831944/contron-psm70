#include "itcpclient.h"

#include "sys/platform.h"

ITcpClient::ITcpClient()
{

}

ITcpClient::~ITcpClient()
{

}

void ITcpClient::tcpClientError(void *tcp)
{
    UN_USE(tcp);
}

