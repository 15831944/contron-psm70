/****************************************************************
说明：	本类为TCP连接通道类
作者：	叶文林
****************************************************************/

#pragma once

#include "../platformLayer/WLDEF.h"
#ifdef WL_OS_CORE_WIN
//#include <winsock2.h>
#include <windows.h>
#elif defined WL_OS_CORE_LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#endif

#include "CommunicationPeerLink.h"
class CTCPPeerLink : public CCommunicationPeerLink
{
public:
//	CTCPPeerLink(SOCKET s,struct sockaddr_in * remoteAddr,DWORD maxCatheCount);
	CTCPPeerLink(int s,struct sockaddr_in * remoteAddr,DWORD maxCatheCount);
	virtual ~CTCPPeerLink(void);

	int send(void * buf,DWORD len);
	int recv(void * buf,DWORD len);

//	SOCKET getSocket();
//	int addBufToRecvCache(SOCKET s,void * pbuf,DWORD len,DWORD maxInterval);
	int getSocket();
	int addBufToRecvCache(int s,void * pbuf,DWORD len,DWORD maxInterval);
private:
//	SOCKET m_hSocket;
	int    m_hSocket;
	struct sockaddr_in m_remoteAddr;        //链路中对方地址信息
};
