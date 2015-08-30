#include "stdafx.h"
#include "TCPServer.h"
#include <errno.h>

/****************************************************************
功能：	构造函数，初始化工作
参数：	无
返回：	无
作者：	叶文林
****************************************************************/
CTCPServer::CTCPServer(CCommunicationProtocol * pProtocol) : CCommunicationServer(pProtocol)
{
#ifdef WL_OS_CORE_WIN
	//win socket需要
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
功能：	析构函数，清理工作
参数：	无
返回：	无
作者：	叶文林
****************************************************************/
CTCPServer::~CTCPServer(void)
{
#ifdef WL_OS_CORE_WIN
	//win socket需要
	// WinSock DLL.                                  
	WSACleanup( );
	//--------------------------
#elif defined WL_OS_CORE_LINUX
	if (m_hSocket > 0)	close(m_hSocket);
#endif
}

/****************************************************************
功能：	启动TCP服务
参数：	无
返回：	-1， 无效套接字
		-2， 绑定失败
		-3， 监听失败
		0，  启动服务成功
作者：	叶文林
修改：	李施施	2014-04-14
		将启动、绑定、监听失败的返回值区分设置，便于定位问题
修改：  唐小灿  2014-07-28 修改为跨平台
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
	addr.sin_addr.s_addr=htonl(INADDR_ANY);        //2014-07-28修改人：唐小灿(经测试windows下可用)
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
功能：	关闭TCP服务
参数：	无
返回：	SOCKET_ERROR，关闭服务失败
		0，			  关闭服务成功
作者：	叶文林
****************************************************************/
int CTCPServer::shutdown()
{
	//置停止服务标志位
	//代码暂时未实现

	//断开所有链接
	for (int i =0;i<m_peerCount;i++)
	{
		//closesocket(m_peerhScoketerList[i]);
		SOCKET hSocket = ((CTCPPeerLink *)m_peerlinkList[i])->getSocket();
		closesocketunite(hSocket);
	}

	//断开服务
//	return closesocket(m_hSocket);
	return closesocketunite(m_hSocket);
}

/****************************************************************
功能：	连接服务
参数：	pContext，客户端SOCKET
		remoteAdd， 连接端地址
返回：	1，连接成功
作者：	叶文林
修改：  唐小灿  2014-07-29 将首个参数(void *)->(SOCKET)
****************************************************************/
//int CTCPServer::onConnected(void * pContext,struct sockaddr_in * remoteAdd)
int CTCPServer::onConnected(SOCKET s,struct sockaddr_in * remoteAdd)
{
	addPeer((SOCKET) s,remoteAdd);
	return 1;
}

/****************************************************************
功能：	断开连接
参数：	pContextPeer，客户端SOCKET
		isDrivingOff，是否本地主动断开标识
返回：	0，			  断开连接成功
		SICKET_ERROR, 断开连接失败
作者：	叶文林
修改：  唐小灿  2014-07-29 将首个参数(void *)->(SOCKET)
****************************************************************/
//int CTCPServer::onDisconnected(void * pContextPeer,BOOL isDrivingOff)
int CTCPServer::onDisconnected(SOCKET s,BOOL isDrivingOff)
{
	removePeer((SOCKET) s);

//	return closesocket((SOCKET) pContextPeer);
	return closesocketunite((SOCKET) s);
}

/****************************************************************
功能：	接收报文处理
参数：	pContext， 客户端SOCKET
		pbuf,	   接收数据
		len，	   接收数据长度
返回：	-1， 客户端SOCKET为空
		-2， 协议类为空
		>0,	 报文数
作者：	叶文林
修改：  唐小灿  2014-07-29 将首个参数(void *)->(SOCKET)
****************************************************************/
//int CTCPServer::onRecv(void * pContext,void * pbuf,DWORD len) 
int CTCPServer::onRecv(SOCKET s,void * pbuf,DWORD len) 
{
	SOCKET hSocket = (SOCKET)s;
	if (hSocket == 0)			return -1;
	if (m_pProtocol == NULL)	return -2;

	//添加数据到链路缓存
	CTCPPeerLink * pLink = findPeer(hSocket);
	pLink->addBufToRecvCache(hSocket,pbuf,len,MAX_TCP_PACKAGE_COUNT * 10);

	//分析及处理报文
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
功能：	添加客户端
参数：	s，			要添加的客户端SOCKET
		remoteAddr,	要添加的客户端地址
返回：	-1， 客户端队列已满
		-2， 该客户端已经存在
		>0,	 添加成功，返回客户端数
作者：	叶文林
****************************************************************/
int CTCPServer::addPeer(SOCKET s,struct sockaddr_in * remoteAddr)
{
	//判断队列是否满
	if (m_peerCount >= MAX_PEER_LINK_COUNT)	return -1;

	//判断是否已存在
	int index = findPeerIndex(s);
	if (index >=0)	return -2;

	//添加
	m_peerlinkList[m_peerCount] = new CTCPPeerLink(s,remoteAddr,MAX_TCP_PACKAGE_COUNT * 10);
	m_peerCount++;

	return m_peerCount;	
}

/****************************************************************
功能：	移除客户端
参数：	s，	 要删除的客户端SOCKET
返回：	-1， 未找到该客户端
		>0,	 删除客户端的索引
作者：	叶文林
****************************************************************/
int CTCPServer::removePeer(SOCKET s)
{
	//查找到节点
	int index = findPeerIndex(s);
	if (index <0)	return -1;

	//删除节点
	CCommunicationPeerLink * p = m_peerlinkList[index];
	delete p;
	p = NULL;
	if (index == m_peerCount -1)	m_peerCount--;          //队尾
	else                                                    //队中
	{
		memcpy(&m_peerlinkList[index],m_peerlinkList[index +1],(m_peerCount - index) * sizeof(CCommunicationPeerLink * ));
		m_peerCount--;
	}

	return index;
}

/****************************************************************
功能：	查找指定客户端的索引
参数：	s，	 要查找的客户端SOCKET
返回：	-1， 未找到该客户端
		>0,	 客户端的索引
作者：	叶文林
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
功能：	查找指定客户端
参数：	s，	 要查找的客户端SOCKET
返回：	NULL， 未找到该客户端
		非NULL,指定客户端
作者：	叶文林
****************************************************************/
CTCPPeerLink * CTCPServer::findPeer(SOCKET s)
{
	int index = findPeerIndex(s);
	if (index <0)	return NULL;
	else return (CTCPPeerLink *)m_peerlinkList[index];
}

/****************************************************************
功能：	监听线程类构造函数，初始化工作
参数：	无
返回：	无
作者：	叶文林
****************************************************************/
CThdTCPServerAccept::CThdTCPServerAccept(CTCPServer * pServer) : CWLThread(1)
{
	m_pServer = pServer;
}

/****************************************************************
功能：	监听线程函数
参数：	无
返回：	1
作者：	叶文林
****************************************************************/
int CThdTCPServerAccept::run()
{
	struct sockaddr_in peerAddr = {0};
	int addrLen = sizeof(struct sockaddr_in);

	while (1)
	{
		//等待客户端连接
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
		

		//回调
		if (m_pServer != NULL)	
		{
			int ret = m_pServer->onConnected(//(void *)hSocketPeer,&peerAddr);
			if (ret == 0)
			{
				closesocketunite(hSocketPeer);
//				closesocket(hSocketPeer);
			}
		}


		////开启一条链路服务线程
		CThdTCPPeerSvc * pTndTCPPeerSvc = new CThdTCPPeerSvc(m_pServer,hSocketPeer);
		pTndTCPPeerSvc->resume();
	}

	return 1;
}

/****************************************************************
功能：	链路通信线程类构造函数，初始化工作
参数：	无
返回：	无
作者：	叶文林
****************************************************************/
CThdTCPPeerSvc::CThdTCPPeerSvc(CTCPServer * pServer,SOCKET hSocketPeer)
{
	m_pServer = pServer;
	m_hSocketPeer = hSocketPeer;
}

/****************************************************************
功能：	链路通信线程函数
参数：	无
返回：	1
作者：	叶文林
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
			//回调
			if (m_pServer != NULL)	
			{
				int ret = m_pServer->onRecv(//(void *)m_hSocketPeer,recvbuf,recvlen);
				//if (ret == 0)	closesocket(m_hSocketPeer);
			}
		}
		if (0  == recvlen) 
		{
			//回调
			if (m_pServer != NULL)	m_pServer->onDisconnected(//(void *)m_hSocketPeer,TRUE);
			m_hSocketPeer = NULL;

			WLOutDebug("链接正常断开,服务线程退出");
			break;
		}
		else if (//SOCKET_ERROR-1 == recvlen)
		{
			//回调
			if (m_pServer != NULL)	m_pServer->onDisconnected(//(void *)m_hSocketPeer,FALSE);
			m_hSocketPeer = NULL;

			WLOutDebug("CTCPServer recv failed, error is %d", errno//GetLastError());
			WLOutDebug("链接非正常断开,服务线程退出");
			break;
		}
	}

	delete[] recvbuf;
	return 1;
}



/****************************************************************
功能：	关闭连接
参数：	s，	 待关闭连接(SOCKET)
返回：	
作者：	唐小灿  2014-07-28
****************************************************************/
int closesocketunite(SOCKET s)
{
#ifdef WL_OS_CORE_WIN
	return closesocket(s);
#elif defined WL_OS_CORE_LINUX
	return close(s);
#endif
}