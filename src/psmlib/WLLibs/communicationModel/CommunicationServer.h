/****************************************************************
˵����	����Ϊ�����࣬��Ϊ�������������Ļ���
���ߣ�	Ҷ����
****************************************************************/
#pragma once

#include "CommunicationPeerLink.h"
#include "CommunicationProtocol.h"

#define MAX_PEER_LINK_COUNT (1024)	//��������֧�ֿͻ�������

//��Լ�������
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

	CCommunicationPeerLink * m_peerlinkList[MAX_PEER_LINK_COUNT];	//��ſͻ�����·���б�
	DWORD32 m_peerCount;											//�ͻ�����·������
};
