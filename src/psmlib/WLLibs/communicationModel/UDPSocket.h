// UDPSocket.h: interface for the CUDPSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPSOCKET_H__E7D82C82_F265_46DF_94B4_6BCF7575CF2A__INCLUDED_)
#define AFX_UDPSOCKET_H__E7D82C82_F265_46DF_94B4_6BCF7575CF2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../platformLayer/WLPlatformLayer.h"
#include "CommunicationSocket.h"
#include "../platformLayer/WLDEF.h"
#ifdef WL_OS_CORE_WIN
#include "Winsock2.h"
#include "windows.h"
#elif defined WL_OS_CORE_LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#endif

typedef void (* TCallbackDebugOutCMD) (const char * str,BYTE * pStrem,int len,const char * ip,WORD port);//打印命令码回调函数



class CUDPSocket : public CCommunicationSocket  
{
public:
	CUDPSocket(const char * remoteIP,WORD remotePort,WORD loaclPort = 0);
	virtual ~CUDPSocket();

	virtual int send(void * pbuf,int len);
	virtual int recv(void * pbuf,int len);
	virtual int send(void * pbuf,int len,struct sockaddr_in * pRemoteAddr);
	virtual int recv(void * pbuf,int len,struct sockaddr_in * pRemoteAddr);
	//struct sockaddr_in 
	int getSock(){return m_hSocket;}
	static void setDebugoutCallback(TCallbackDebugOutCMD pFunc);
	int getSendRemoteAddr(struct sockaddr_in * pOutRemoteAddr);

private:
	int m_hSocket;
	struct sockaddr_in m_sendRemoteAddr; //发送到远程地址，注意：一个socket可以在两个线程中又收又发，但是不能用同一个地址结构
	struct sockaddr_in m_recvRemoteAddr; //接收自远程地址

	static TCallbackDebugOutCMD m_callbackDebugOutCmd;
public:
	static int initSock(int * sock,struct sockaddr_in *addr,short port = 0);

	static void setSockaddr(const char *ip,short port,struct sockaddr_in * pAddr);
	static void getSockAddr(char * ip,int iplen,WORD & port,const struct sockaddr_in * pAddr);
	static void debugout(const char * str,BYTE * pStrem,int len,const struct sockaddr_in * pAddr);
};

#endif // !defined(AFX_UDPSOCKET_H__E7D82C82_F265_46DF_94B4_6BCF7575CF2A__INCLUDED_)
