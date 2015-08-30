/****************************************************************
说明：	本类为服务类，作为其他具体服务类的基类
作者：	叶文林
****************************************************************/
#pragma once

#include "CommunicationPeerLink.h"
#include "CommunicationProtocol.h"

#define MAX_PEER_LINK_COUNT (1024)	//服务端最大支持客户端数量

//规约服务基类
class CCommunicationServer
{
public:
	CCommunicationServer(CCommunicationProtocol * pProtocol);
	~CCommunicationServer(void);

	virtual int onConnected(void * pContext,struct sockaddr_in * remoteAdd) = 0;
	virtual int onDisconnected(void * pContext,BOOL isDrivingOff) = 0;
	virtual int onRecv(void * pContext,void * pbuf,DWORD len) = 0;

	int getPeerCount(){return m_peerCount;};
	CCommunicationPeerLink *getPeerLink(int index);
protected:
	CCommunicationProtocol * m_pProtocol;

	CCommunicationPeerLink * m_peerlinkList[MAX_PEER_LINK_COUNT];	//存放客户端链路的列表
	DWORD32 m_peerCount;											//客户端链路的数量
};
