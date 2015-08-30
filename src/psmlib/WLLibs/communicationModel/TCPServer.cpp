#include "stdafx.h"
#include "TCPServer.h"
#include <errno.h>

/****************************************************************
���ܣ�	���캯������ʼ������
������	��
���أ�	��
���ߣ�	Ҷ����
****************************************************************/
CTCPServer::CTCPServer(CCommunicationProtocol * pProtocol) : CCommunicationServer(pProtocol)
{
#ifdef WL_OS_CORE_WIN
	//win socket��Ҫ
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		// Tell the user that we could not find a usable 
		// WinSock DLL.                                  
		return;
	}
	//--------------------------
#endif
	m_peerCount = 0;
}

/****************************************************************
���ܣ�	����������������
������	��
���أ�	��
���ߣ�	Ҷ����
****************************************************************/
CTCPServer::~CTCPServer(void)
{
#ifdef WL_OS_CORE_WIN
	//win socket��Ҫ
	// WinSock DLL.                                  
	WSACleanup( );
	//--------------------------
#elif defined WL_OS_CORE_LINUX
	if (m_hSocket > 0)	close(m_hSocket);
#endif
}

/****************************************************************
���ܣ�	����TCP����
������	��
���أ�	-1�� ��Ч�׽���
		-2�� ��ʧ��
		-3�� ����ʧ��
		0��  ��������ɹ�
���ߣ�	Ҷ����
�޸ģ�	��ʩʩ	2014-04-14
		���������󶨡�����ʧ�ܵķ���ֵ�������ã����ڶ�λ����
�޸ģ�  ��С��  2014-07-28 �޸�Ϊ��ƽ̨
****************************************************************/
int CTCPServer::startup(WORD svcPort,DWORD linkCount)
{
	m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
#ifdef WL_OS_CORE_WIN
	if (INVALID_SOCKET == m_hSocket)
	{
		WLOutDebug("CTCPScoket::openSocket() socket() failed, error is %d", GetLastError());
		return -1;
	}
#elif defined WL_OS_CORE_LINUX
	if (m_hSocket <0 )
	{
		WLOutDebug("CTCPScoket::openSocket() socket() failed, errno is %d", errno);
		return -1;
	}
#endif

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(svcPort);
//	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr.sin_addr.s_addr=htonl(INADDR_ANY);        //2014-07-28�޸��ˣ���С��(������windows�¿���)
//	if ( SOCKET_ERROR == ::bind(m_hSocket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) )
	if ( ::bind(m_hSocket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0)
	{
		//WLOutDebug("CTCPScoket::openSocket() bind() failed, error is %d", GetLastError());
		WLOutDebug("CTCPScoket::openSocket() bind() failed, error is %d", errno);
//		closesocket(m_hSocket);
		closesocketunite(m_hSocket);
		return -2;
	}

//	if (SOCKET_ERROR == listen(m_hSocket, 1))
	if (listen(m_hSocket, 1) < 0)
	{
		//WLOutDebug("CTCPScoket::openSocket() listen() failed, error is %d", GetLastError());
		WLOutDebug("CTCPScoket::openSocket() listen() failed, error is %d", errno);
//		closesocket(m_hSocket);
		closesocketunite(m_hSocket);
		return -3;
	}

	CThdTCPServerAccept * pThdTCPServerAccept = new CThdTCPServerAccept(this);
	pThdTCPServerAccept->resume();

	return 0;	
}

/****************************************************************
���ܣ�	�ر�TCP����
������	��
���أ�	SOCKET_ERROR���رշ���ʧ��
		0��			  �رշ���ɹ�
���ߣ�	Ҷ����
****************************************************************/
int CTCPServer::shutdown()
{
	//��ֹͣ�����־λ
	//������ʱδʵ��

	//�Ͽ���������
	for (int i =0;i<m_peerCount;i++)
	{
		//closesocket(m_peerhScoketerList[i]);
		SOCKET hSocket = ((CTCPPeerLink *)m_peerlinkList[i])->getSocket();
		closesocketunite(hSocket);
	}

	//�Ͽ�����
//	return closesocket(m_hSocket);
	return closesocketunite(m_hSocket);
}

/****************************************************************
���ܣ�	���ӷ���
������	pContext���ͻ���SOCKET
		remoteAdd�� ���Ӷ˵�ַ
���أ�	1�����ӳɹ�
���ߣ�	Ҷ����
�޸ģ�  ��С��  2014-07-29 ���׸�����(void *)->(SOCKET)
****************************************************************/
//int CTCPServer::onConnected(void * pContext,struct sockaddr_in * remoteAdd)
int CTCPServer::onConnected(SOCKET s,struct sockaddr_in * remoteAdd)
{
	addPeer((SOCKET) s,remoteAdd);
	return 1;
}

/****************************************************************
���ܣ�	�Ͽ�����
������	pContextPeer���ͻ���SOCKET
		isDrivingOff���Ƿ񱾵������Ͽ���ʶ
���أ�	0��			  �Ͽ����ӳɹ�
		SICKET_ERROR, �Ͽ�����ʧ��
���ߣ�	Ҷ����
�޸ģ�  ��С��  2014-07-29 ���׸�����(void *)->(SOCKET)
****************************************************************/
//int CTCPServer::onDisconnected(void * pContextPeer,BOOL isDrivingOff)
int CTCPServer::onDisconnected(SOCKET s,BOOL isDrivingOff)
{
	removePeer((SOCKET) s);

//	return closesocket((SOCKET) pContextPeer);
	return closesocketunite((SOCKET) s);
}

/****************************************************************
���ܣ�	���ձ��Ĵ���
������	pContext�� �ͻ���SOCKET
		pbuf,	   ��������
		len��	   �������ݳ���
���أ�	-1�� �ͻ���SOCKETΪ��
		-2�� Э����Ϊ��
		>0,	 ������
���ߣ�	Ҷ����
�޸ģ�  ��С��  2014-07-29 ���׸�����(void *)->(SOCKET)
****************************************************************/
//int CTCPServer::onRecv(void * pContext,void * pbuf,DWORD len) 
int CTCPServer::onRecv(SOCKET s,void * pbuf,DWORD len) 
{
	SOCKET hSocket = (SOCKET)s;
	if (hSocket == 0)			return -1;
	if (m_pProtocol == NULL)	return -2;

	//������ݵ���·����
	CTCPPeerLink * pLink = findPeer(hSocket);
	pLink->addBufToRecvCache(hSocket,pbuf,len,MAX_TCP_PACKAGE_COUNT * 10);

	//������������
	BYTE  * pCache = NULL;
	DWORD * cacheLen = NULL;
	BYTE pCmd[MAX_TCP_PACKAGE_COUNT] = {0};
	DWORD cmdlen = MAX_TCP_PACKAGE_COUNT;
	DWORD  cmdCount = 0;
	pLink->getCache((void **)&pCache,&cacheLen);
	int ret = m_pProtocol->analyseProtocol(pCache,cacheLen,pCmd,&cmdlen);
	while (ret >= 0)
	{
		m_pProtocol->procRecvCmd(pCmd,cmdlen,pLink);
		ret = m_pProtocol->analyseProtocol(pCache,cacheLen,pCmd,&cmdlen);

		cmdCount++;
	}
	
	return cmdCount;
}

/****************************************************************
���ܣ�	��ӿͻ���
������	s��			Ҫ��ӵĿͻ���SOCKET
		remoteAddr,	Ҫ��ӵĿͻ��˵�ַ
���أ�	-1�� �ͻ��˶�������
		-2�� �ÿͻ����Ѿ�����
		>0,	 ��ӳɹ������ؿͻ�����
���ߣ�	Ҷ����
****************************************************************/
int CTCPServer::addPeer(SOCKET s,struct sockaddr_in * remoteAddr)
{
	//�ж϶����Ƿ���
	if (m_peerCount >= MAX_PEER_LINK_COUNT)	return -1;

	//�ж��Ƿ��Ѵ���
	int index = findPeerIndex(s);
	if (index >=0)	return -2;

	//���
	m_peerlinkList[m_peerCount] = new CTCPPeerLink(s,remoteAddr,MAX_TCP_PACKAGE_COUNT * 10);
	m_peerCount++;

	return m_peerCount;	
}

/****************************************************************
���ܣ�	�Ƴ��ͻ���
������	s��	 Ҫɾ���Ŀͻ���SOCKET
���أ�	-1�� δ�ҵ��ÿͻ���
		>0,	 ɾ���ͻ��˵�����
���ߣ�	Ҷ����
****************************************************************/
int CTCPServer::removePeer(SOCKET s)
{
	//���ҵ��ڵ�
	int index = findPeerIndex(s);
	if (index <0)	return -1;

	//ɾ���ڵ�
	CCommunicationPeerLink * p = m_peerlinkList[index];
	delete p;
	p = NULL;
	if (index == m_peerCount -1)	m_peerCount--;          //��β
	else                                                    //����
	{
		memcpy(&m_peerlinkList[index],m_peerlinkList[index +1],(m_peerCount - index) * sizeof(CCommunicationPeerLink * ));
		m_peerCount--;
	}

	return index;
}

/****************************************************************
���ܣ�	����ָ���ͻ��˵�����
������	s��	 Ҫ���ҵĿͻ���SOCKET
���أ�	-1�� δ�ҵ��ÿͻ���
		>0,	 �ͻ��˵�����
���ߣ�	Ҷ����
****************************************************************/
int CTCPServer::findPeerIndex(SOCKET s)
{
	for (int i =0;i<m_peerCount;i++)
	{
		if (s == ((CTCPPeerLink *)m_peerlinkList[i])->getSocket() )	return i;
	}
	return -1;
}

/****************************************************************
���ܣ�	����ָ���ͻ���
������	s��	 Ҫ���ҵĿͻ���SOCKET
���أ�	NULL�� δ�ҵ��ÿͻ���
		��NULL,ָ���ͻ���
���ߣ�	Ҷ����
****************************************************************/
CTCPPeerLink * CTCPServer::findPeer(SOCKET s)
{
	int index = findPeerIndex(s);
	if (index <0)	return NULL;
	else return (CTCPPeerLink *)m_peerlinkList[index];
}

/****************************************************************
���ܣ�	�����߳��๹�캯������ʼ������
������	��
���أ�	��
���ߣ�	Ҷ����
****************************************************************/
CThdTCPServerAccept::CThdTCPServerAccept(CTCPServer * pServer) : CWLThread(1)
{
	m_pServer = pServer;
}

/****************************************************************
���ܣ�	�����̺߳���
������	��
���أ�	1
���ߣ�	Ҷ����
****************************************************************/
int CThdTCPServerAccept::run()
{
	struct sockaddr_in peerAddr = {0};
	int addrLen = sizeof(struct sockaddr_in);

	while (1)
	{
		//�ȴ��ͻ�������
		SOCKET hSocketPeer = ::accept(m_pServer->m_hSocket, (struct sockaddr *)&peerAddr, (socklen_t*)&addrLen);
#ifdef WL_OS_CORE_WIN
		if (INVALID_SOCKET == hSocketPeer)
		{
			WLOutDebug("CTCPServer accept failed, error is %d", GetLastError());
			continue;
		}
#elif defined WL_OS_CORE_LINUX
		if (hSocketPeer < 0)
		{
			WLOutDebug("CTCPServer accept failed, error is %d", errno);
			continue;
		}
#endif
		

		//�ص�
		if (m_pServer != NULL)	
		{
			int ret = m_pServer->onConnected(//(void *)hSocketPeer,&peerAddr);
			if (ret == 0)
			{
				closesocketunite(hSocketPeer);
//				closesocket(hSocketPeer);
			}
		}


		////����һ����·�����߳�
		CThdTCPPeerSvc * pTndTCPPeerSvc = new CThdTCPPeerSvc(m_pServer,hSocketPeer);
		pTndTCPPeerSvc->resume();
	}

	return 1;
}

/****************************************************************
���ܣ�	��·ͨ���߳��๹�캯������ʼ������
������	��
���أ�	��
���ߣ�	Ҷ����
****************************************************************/
CThdTCPPeerSvc::CThdTCPPeerSvc(CTCPServer * pServer,SOCKET hSocketPeer)
{
	m_pServer = pServer;
	m_hSocketPeer = hSocketPeer;
}

/****************************************************************
���ܣ�	��·ͨ���̺߳���
������	��
���أ�	1
���ߣ�	Ҷ����
****************************************************************/
int CThdTCPPeerSvc::run()
{
	DWORD tc1 = 0;
	int fullRecvlen = 0;
	BYTE * recvbuf = new BYTE[65535];
	while(1)
	{
		int recvlen = recv(m_hSocketPeer,(char *)recvbuf,MAX_TCP_PACKAGE_COUNT,0);
		//WLOutDebugStream("recv<< ",recvbuf,recvlen);

		if (recvlen >0)
		{
			//�ص�
			if (m_pServer != NULL)	
			{
				int ret = m_pServer->onRecv(//(void *)m_hSocketPeer,recvbuf,recvlen);
				//if (ret == 0)	closesocket(m_hSocketPeer);
			}
		}
		if (0  == recvlen) 
		{
			//�ص�
			if (m_pServer != NULL)	m_pServer->onDisconnected(//(void *)m_hSocketPeer,TRUE);
			m_hSocketPeer = NULL;

			WLOutDebug("���������Ͽ�,�����߳��˳�");
			break;
		}
		else if (//SOCKET_ERROR-1 == recvlen)
		{
			//�ص�
			if (m_pServer != NULL)	m_pServer->onDisconnected(//(void *)m_hSocketPeer,FALSE);
			m_hSocketPeer = NULL;

			WLOutDebug("CTCPServer recv failed, error is %d", errno//GetLastError());
			WLOutDebug("���ӷ������Ͽ�,�����߳��˳�");
			break;
		}
	}

	delete[] recvbuf;
	return 1;
}



/****************************************************************
���ܣ�	�ر�����
������	s��	 ���ر�����(SOCKET)
���أ�	
���ߣ�	��С��  2014-07-28
****************************************************************/
int closesocketunite(SOCKET s)
{
#ifdef WL_OS_CORE_WIN
	return closesocket(s);
#elif defined WL_OS_CORE_LINUX
	return close(s);
#endif
}