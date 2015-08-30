/**************************************************************************************************
TCPSocket.h
功能：服务于TCP网络操作，使用于TCP服务端接收连接服务，服务端接收数据服务及客户端服务三种情况。
      其中TCP服务端接收连接服务不能用于收发数据，其地址为服务器本地地址。
作者：唐小灿  2013-10-31
**************************************************************************************************/
#ifndef __TCPSOCKET_H__
#define __TCPSOCKET_H__

#include "CommunicationSocket.h"
#include "../platformLayer/WLDEF.h"
#ifdef WL_OS_CORE_WIN
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")
#elif defined WL_OS_CORE_LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#endif

class CTCPScoket : public CCommunicationSocket  
{
public:
	CTCPScoket(DWORD localPort = 0);                     //服务端主服务，即接收连接服务,禁止收发数据
	CTCPScoket(int s, sockaddr_in clientAddr);           //服务端接收数据服务
	CTCPScoket(const char * remoteIP, WORD remotePort);  //客户端
	virtual ~CTCPScoket();

	virtual int accept(struct sockaddr_in * pClientAddr, int * pSock);    //服务端接收连接
	virtual int send(void * pbuf,int len);
	virtual int recv(void * pbuf,int len);

//	int getSOCKET(int *pSocket);
	int getAddress(struct sockaddr_in * pAddr);

	int initSocket(char *pIP=NULL, DWORD port=0);

private:
//	int initSocket(char *pIP=NULL, DWORD port=0);

private:
	int   m_type;                     //类型标识。1：服务端主服务，2：服务端接收数据服务，3：客户端
	int   m_hSocket;
	WORD  m_port;
	char  m_ip[64];
	struct sockaddr_in m_addr;        //服务端主服务时为本地地址，服务端接收数据服务为客户端地址，客户端服务为服务器地址
};

#endif