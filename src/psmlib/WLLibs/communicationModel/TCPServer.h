/****************************************************************
说明：	本类为TCP连接服务类
作者：	叶文林
****************************************************************/

#pragma once

#include "CommunicationServer.h"
#include "../platformLayer/WLDEF.h"
#include "../platformLayer/WLThread.h"
#include "../communicationModel/UDPSocket.h"
#include "TCPPeerLink.h"

#define MAX_TCP_PACKAGE_COUNT (2048)      //最大TCP报文长度

typedef struct _tcpContext{
	SOCKET hSocket;                       //链路socket
	struct sockaddr_in remoteAddr;        //链路中对方地址信息
}TTcpContext;

class CTCPServer : public CCommunicationServer
{
public:
	CTCPServer(CCommunicationProtocol * pProtocol);
	~CTCPServer(void);

	int startup(WORD svcPort,DWORD linkCount);
	int shutdown();

	//virtual int onConnected(void * pContext,struct sockaddr_in * remoteAdd);
	//virtual int onDisconnected(void * pContext,BOOL isDrivingOff);
	//virtual int onRecv(void * pContext,void * pbuf,DWORD len);
	virtual int onConnected(SOCKET s,struct sockaddr_in * remoteAdd);
	virtual int onDisconnected(SOCKET s,BOOL isDrivingOff);
	virtual int onRecv(SOCKET s,void * pbuf,DWORD len);

	friend class CThdTCPServerAccept;
	friend class CThdTCPPeerSvc;
private:
	int addPeer(SOCKET s,struct sockaddr_in * remoteAddr);
	int removePeer(SOCKET s);
	int findPeerIndex(SOCKET s);
	CTCPPeerLink * findPeer(SOCKET s);

	int m_hSocket;
};

//链接侦听线程（只有一个）
class CThdTCPServerAccept : public CWLThread
{
public:
	CThdTCPServerAccept(CTCPServer * pServer);

private:
	virtual int run();                           //线程函数
	CTCPServer *m_pServer;                     //服务端对象
};

//链路通信线程（有N个，N为链路数）
class CThdTCPPeerSvc : public CWLThread
{
public:
	CThdTCPPeerSvc(CTCPServer * pServer,SOCKET hSocketPeer);

private:
	virtual int run();                           //线程函数
	CTCPServer *m_pServer;                     //服务端对象
	SOCKET m_hSocketPeer;                        //链接Socket
};


//关闭连接统一接口，方便于跨平台关闭连接
int closesocketunite(SOCKET s);