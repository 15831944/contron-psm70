#include "stdafx.h"
#include "TCPScoket.h"
#include "../platformLayer/WLPlatformLayer.h"

#ifdef WL_OS_CORE_WIN
#pragma comment(lib, "ws2_32.lib")
#endif


/**************************************************************************************************
功能：用于服务端主服务(即接收连接服务)的构造
参数：localPort 本地端口号
作者：唐小灿  2013-10-31
**************************************************************************************************/
CTCPScoket::CTCPScoket(DWORD localPort)
{
	m_type    = 1;
	m_hSocket = 0;
	m_port    = localPort;
	memset(m_ip, 0x00, sizeof(m_ip));
	memset(&m_addr, 0x00, sizeof(m_addr));

//	int ret = initSocket(NULL, m_port);
}


/**************************************************************************************************
功能：用于服务端接收数据服务的构造
参数：s 客户端连接产生的SOCKET
作者：唐小灿  2013-10-31
**************************************************************************************************/
CTCPScoket::CTCPScoket(int s, sockaddr_in clientAddr)
{
	m_type = 2;
	m_hSocket = s;
	m_port    = clientAddr.sin_port;
	strcpy(m_ip, inet_ntoa(clientAddr.sin_addr));
	memcpy(&m_addr, &clientAddr, sizeof(m_addr));
}


/**************************************************************************************************
功能：用于客户端服务的构造
参数：remoteIP 服务端IP
      remotePort 服务端端口
作者：唐小灿  2013-10-31
**************************************************************************************************/
CTCPScoket::CTCPScoket(const char * remoteIP, WORD remotePort)
{
	m_type    = 3;
	m_hSocket = 0;
	m_port    = remotePort;
	strcpy(m_ip, remoteIP);
	memset(&m_addr, 0x00, sizeof(m_addr));

//	int ret = initSocket(m_ip, m_port);
}


/**************************************************************************************************
功能：析构
注释：服务端接收数据服务只需要关闭对应的客户端连接，不需要关闭网络
作者：唐小灿  2013-10-31
**************************************************************************************************/
CTCPScoket::~CTCPScoket()
{
	if(m_hSocket > 0)	
	{
#ifdef WL_OS_CORE_WIN
		closesocket(m_hSocket);
#elif defined WL_OS_CORE_LINUX
		close(m_hSocket);
#endif
	}

#ifdef WL_OS_CORE_WIN
	if (2 != m_type)
	{
		WSACleanup();
	}
#endif
}


/**************************************************************************************************
功能：网络初始化，用于服务端主服务及客户端服务，服务端接收数据服务部需要做此操作
参数：pIP 服务端为NULL，客户端填写服务器IP地址
	  port 服务端为本地端口，客户端填写服务端开放的端口
返回：<0失败 0成功
作者：唐小灿  2013-10-31
**************************************************************************************************/
int CTCPScoket::initSocket(char *pIP, DWORD port)
{
	int ret = 0;

#ifdef WL_OS_CORE_WIN
	WSADATA wsaData;

	ret = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (0 != ret)
	{
		WLOutDebug("CTCPScoket::openSocket() WSAStartup() failed, error is %d", GetLastError());
		return -1;
	}
#endif

#ifdef WL_OS_CORE_WIN
    m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_hSocket)
	{
		WLOutDebug("CTCPScoket::initSocket socket() failed, error is %d", GetLastError());
		return -1;
	}
#elif defined WL_OS_CORE_LINUX
	m_hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (m_hSocket < 0)     return m_hSocket;
#endif

	m_addr.sin_family = PF_INET;//AF_INET;
	m_addr.sin_port   = htons(m_port);
	if (NULL == pIP)
	{
#ifdef WL_OS_CORE_WIN
		m_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#elif defined WL_OS_CORE_LINUX
		m_addr.sin_addr.s_addr = INADDR_ANY;
#endif
	}
	else
	{
#ifdef WL_OS_CORE_WIN
		m_addr.sin_addr.S_un.S_addr = inet_addr(pIP);
#elif defined WL_OS_CORE_LINUX
		m_addr.sin_addr.s_addr = inet_addr(pIP);
#endif
	}

	if (1 == m_type)         //服务端主服务
	{
		ret = ::bind(m_hSocket, (struct sockaddr *)&m_addr, sizeof(struct sockaddr_in));

		//if ( SOCKET_ERROR == ret )
		if ( ret < 0 )
		{
#ifdef WL_OS_CORE_WIN
			WLOutDebug("CTCPScoket::initSocket bind() failed, error is %d", GetLastError());
			closesocket(m_hSocket);
#elif defined WL_OS_CORE_LINUX
			WLOutDebug("CTCPScoket::initSocket bind() failed, errno is %d", errno);
			close(m_hSocket);
#endif
			return -1;
		}

		ret = listen(m_hSocket, 5);
		//if (SOCKET_ERROR == ret)
		if (ret < 0)
		{
#ifdef WL_OS_CORE_WIN
			WLOutDebug("CTCPScoket::initSocket listen() failed, error is %d", GetLastError());
			closesocket(m_hSocket);
#elif defined WL_OS_CORE_LINUX
			WLOutDebug("CTCPScoket::initSocket listen() failed, errno is %d", errno);
			close(m_hSocket);
#endif
			return -1;
		}
	}
	else if (3 == m_type)    //客户端
	{
		ret = ::connect(m_hSocket,(struct sockaddr*)&m_addr, sizeof(struct sockaddr_in));
		//if ( SOCKET_ERROR ==  ret )
		if ( ret < 0 )
		{
#ifdef WL_OS_CORE_WIN
			WLOutDebug("CTCPScoket::initSocket connect() failed, error is %d", GetLastError());
			closesocket(m_hSocket);
#elif defined WL_OS_CORE_LINUX
			WLOutDebug("CTCPScoket::initSocket connect() failed, errno is %d", errno);
			close(m_hSocket);
#endif
			return -1;
		}
	}

	return 0;
}


/**************************************************************************************************
功能：服务端主服务接收连接接口
参数：pClientAddr 用于反馈连接客户端地址
      pSock  接收连接的SOCKET
返回：<0失败 0成功
作者：唐小灿  2013-10-31
**************************************************************************************************/
int CTCPScoket::accept(struct sockaddr_in * pClientAddr, int * pSock)
{
	if (NULL == pClientAddr)
	{
		return -1;
	}

	int addrLen = sizeof(struct sockaddr_in);
	//SOCKET sockClient = ::accept(m_hSocket, (struct sockaddr *)pClientAddr, &addrLen);
	int sockClient = ::accept(m_hSocket, (struct sockaddr *)pClientAddr, (socklen_t*)&addrLen);  //+++++
#ifdef WL_OS_CORE_WIN
	if (INVALID_SOCKET == sockClient)
	{
		WLOutDebug("CTCPServer accept failed, error is %d", GetLastError());
		return -1;
	}
#elif defined WL_OS_CORE_LINUX
	if (sockClient < 0)
	{
		WLOutDebug("CTCPServer accept failed, errno is %d", errno);
		return -1;
	}
#endif

	*pSock = (int)sockClient;

	return 0;
}


/**************************************************************************************************
功能：发送消息，不适合服务端主服务
参数：pbuf 待发送数据
      len 待发送数据长度
返回：参照::send返回值
作者：唐小灿  2013-10-31
**************************************************************************************************/
int CTCPScoket::send(void * pbuf,int len)
{
	int ret = ::send(m_hSocket, (char*)pbuf, len, 0);
	
	return ret;
}


/**************************************************************************************************
功能：接收消息，不适合服务端主服务
参数：pbuf 接收缓冲
      len 接收缓冲大小
返回：参照::recv返回值
作者：唐小灿  2013-10-31
**************************************************************************************************/
int CTCPScoket::recv(void * pbuf,int len)
{
	int ret = ::recv(m_hSocket, (char*)pbuf, len, 0);

	return ret;
}


//int CTCPScoket::getSOCKET(int *pSocket)
//{
//	if (NULL == pSocket)
//	{
//		return -1;
//	}
//
//	*pSocket = m_hSocket;
//
//	return 0;
//}


int CTCPScoket::getAddress(struct sockaddr_in * pAddr)
{
	if (NULL == pAddr)
	{
		return -1;
	}

	memcpy(pAddr, &m_addr, sizeof(struct sockaddr_in));

	return 0;
}