/****************************************************************
˵����	����ΪTCP���ӷ�����
���ߣ�	Ҷ����
****************************************************************/

#pragma once

#include "CommunicationServer.h"
#include "../platformLayer/WLDEF.h"
#include "../platformLayer/WLThread.h"
#include "../communicationModel/UDPSocket.h"
#include "TCPPeerLink.h"

#define MAX_TCP_PACKAGE_COUNT (2048)      //���TCP���ĳ���

typedef struct _tcpContext{
	SOCKET hSocket;                       //��·socket
	struct sockaddr_in remoteAddr;        //��·�жԷ���ַ��Ϣ
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

//���������̣߳�ֻ��һ����
class CThdTCPServerAccept : public CWLThread
{
public:
	CThdTCPServerAccept(CTCPServer * pServer);

private:
	virtual int run();                           //�̺߳���
	CTCPServer *m_pServer;                     //����˶���
};

//��·ͨ���̣߳���N����NΪ��·����
class CThdTCPPeerSvc : public CWLThread
{
public:
	CThdTCPPeerSvc(CTCPServer * pServer,SOCKET hSocketPeer);

private:
	virtual int run();                           //�̺߳���
	CTCPServer *m_pServer;                     //����˶���
	SOCKET m_hSocketPeer;                        //����Socket
};


//�ر�����ͳһ�ӿڣ������ڿ�ƽ̨�ر�����
int closesocketunite(SOCKET s);