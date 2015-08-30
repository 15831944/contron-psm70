// UDPSocket.cpp: implementation of the CUDPSocket class.
// 定义UDP类，基类为：CCommunicationSocket，实现其 send、recv接口
// 目的：为了适应程序的通信方式不确定，而定义这套通信接口，例如：
//       某应用将来要用串口通信，但是当前条件无串口，可暂时时用UDP代替。
//       如果不将通信接口作一个很好的封装，将来代码改动量较大，本类的
//       就是解决此类问题的
// 注意：兄弟类CTCPSocket、CUDPSocket、CCOMSocket
// 作者：叶文林	2012-08-24
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UDPSocket.h"
//#include "../PlatformLayer/WLDEF.h"
//#include "../PlatformLayer/WLPlatformLayer.h"
#include "../platformLayer/WLOutDebug.h"
#ifdef WL_OS_CORE_WIN
#ifdef WL_OS_WIN32
#pragma comment(lib, "ws2_32.lib")
#else ifdef WL_OS_WINCE
#pragma comment(lib, "ws2.lib")
#endif
#endif

#ifdef WL_IDE_VC6
	#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[]=__FILE__;
	#define new DEBUG_NEW
	#endif
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TCallbackDebugOutCMD CUDPSocket::m_callbackDebugOutCmd = NULL;


/*********************************************************************
功能：构造函数，作初始化工作
参数：remoteIP，对方IP址
      remotePort，本方端口号
	  loaclPort，本地端口号，默认值为0。当为0时，表示任意绑定一个端口号
返回：无
作者：叶文林	2012-08-24
*********************************************************************/
CUDPSocket::CUDPSocket(const char * remoteIP,WORD remotePort,WORD loaclPort)
				: CCommunicationSocket()
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

	//设置发送地址
	setSockaddr(remoteIP,remotePort,&m_sendRemoteAddr);
	//初始化SOCKET
	struct sockaddr_in addr;
	initSock(&m_hSocket,&addr,loaclPort);
}

/*********************************************************************
功能：析构函数
参数：无
返回：无
作者：叶文林	2012-08-24
*********************************************************************/
CUDPSocket::~CUDPSocket()
{
#ifdef WL_OS_CORE_WIN
	closesocket(m_hSocket);

	//win socket需要
	// WinSock DLL.                                  
    WSACleanup( );
	//--------------------------
#elif defined WL_OS_CORE_LINUX
	close(m_hSocket);
#endif
}

/*********************************************************************
功能：发送数据
参数：pbuf，发送缓冲
      len，发送字节数
返回：<0，错误；>=0，成功发送字节数
作者：叶文林	2012-08-24
*********************************************************************/
int CUDPSocket::send(void * pbuf,int len)
{
	int ret = ::sendto(m_hSocket,(char *)pbuf,len,0,(struct sockaddr *)&m_sendRemoteAddr,sizeof(struct sockaddr_in));
//	debugout(">>",(BYTE *)pbuf,ret,&m_sendRemoteAddr);
	return ret;
}

/*********************************************************************
功能：接收数据
参数：pbuf，接收缓冲
      len，接收字节数
返回：<0，错误；>=0，成功接收字节数
作者：叶文林	2012-08-24
*********************************************************************/
int CUDPSocket::recv(void * pbuf,int len)
{
	int fromlen = sizeof(struct sockaddr_in);
	int ret = ::recvfrom(m_hSocket,(char *)pbuf,len,0,(struct sockaddr *)&m_recvRemoteAddr,(socklen_t*)&fromlen);
//	debugout("<<",(BYTE *)pbuf,ret,&m_recvRemoteAddr);
	return ret;
}

/*********************************************************************
功能：发送数据到指定接收者
参数：pbuf，发送缓冲
      len，发送字节数
	  pRemoteAddr，远程地址
返回：<0，错误；>=0，成功发送字节数
作者：叶文林	2012-08-24
*********************************************************************/
int CUDPSocket::send(void * pbuf,int len,struct sockaddr_in * pRemoteAddr)
	{
	int ret = ::sendto(m_hSocket,(char *)pbuf,len,0,(struct sockaddr *)pRemoteAddr,sizeof(struct sockaddr_in));
//	debugout(">>",(BYTE *)pbuf,ret,pRemoteAddr);
	return ret;
}

/*********************************************************************
功能：接收数据，并返回发送者。本函数与 recv(void * pbuf,int len)区别是：
      recv(void * pbuf,int len,struct sockaddr_in * pRemoteAddr) ：所有发送者发过来的都可以接收
	  recv(void * pbuf,int len) ： 只接收初始化时指定的远程方数据，其它发送过来的全忽略
参数：pbuf，接收缓冲
      len，接收字节数
返回：<0，错误；>=0，成功接收字节数
作者：叶文林	2012-08-24
*********************************************************************/
int CUDPSocket::recv(void * pbuf,int len,struct sockaddr_in * pRemoteAddr)
{
	int fromlen = sizeof(struct sockaddr_in);
	int ret = ::recvfrom(m_hSocket,(char *)pbuf,len,0,(struct sockaddr *)pRemoteAddr,(socklen_t*)&fromlen);
//	debugout("<<",(BYTE *)pbuf,ret,pRemoteAddr);
	return ret;
}

/*********************************************************************
功能：创建UDP Socket
参数：sock，socket
      addr，本地地址值
	  port，端口号。0，随机邦定。0为默认值。
返回：<=0，失败；1，成功
作者：叶文林	2012-08-24
*********************************************************************/
int CUDPSocket::initSock(int * sock,struct sockaddr_in *addr,short port)
{
	int addrLen;
	int rtn;
	//	struct timeval sockBlockTime;                                    //收一次数据包超时时间
	
	*sock = socket(AF_INET,SOCK_DGRAM,0);                            //API创建Socket，为UDP

#ifdef WL_OS_CORE_WIN
	if (* sock == -1)  return *sock;
#elif defined WL_OS_CORE_LINUX
	if (*sock < 0)     return *sock;
	memset(addr, 0x00, sizeof(struct sockaddr_in));
//	bzero(addr,sizeof(struct sockaddr_in));
#endif
	
	(addr->sin_addr).s_addr = htonl(INADDR_ANY);
	(addr->sin_family)      = AF_INET;
	(addr->sin_port)        = htons(port);
	addrLen = sizeof(struct sockaddr_in);
	
	rtn = bind(* sock,(struct sockaddr *)addr,addrLen);              //邦定到固定的端口号上
	if (rtn <0)  return rtn;
	
	int   len   =   sizeof(struct   sockaddr_in);   

#ifdef WL_OS_CORE_WIN
	rtn = getsockname(* sock,(struct sockaddr *)addr,&len);          //测试sock与IP、端口号是否绑定成功
	if (rtn < 0) return rtn;
#endif
	
	//wince 设置sock超时无效，用select可实现
	//	if (uTimeOut == 0)	return 1;
	//设置接收超时时间
	//	(sockBlockTime).tv_sec = uTimeOut /1000;
	//	(sockBlockTime).tv_usec= uTimeOut %1000;
	//	setsockopt(*sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&sockBlockTime,          //设置命令接收超时时间
	//		   sizeof(struct timeval));
	
	return 1;
}

/*********************************************************************
功能：根据IP地址及端口号，设置网络地址结构
参数：ip，IP地址(a.b.c.d形式)；
      port，端口号
	  pAddr，地址结构体
返回：无意义
作者：叶文林	2012-08-24
*********************************************************************/
void CUDPSocket::setSockaddr(const char *ip,short port,struct sockaddr_in * pAddr)
{
	if (ip == NULL || pAddr == NULL)	return;

	memset(pAddr,0x00,sizeof(struct sockaddr_in));
	pAddr->sin_family        = AF_INET;
	pAddr->sin_port          = htons(port);
#ifdef WL_OS_CORE_WIN
	pAddr->sin_addr.s_addr   = inet_addr(ip);
#elif defined WL_OS_CORE_LINUX
	int ret = inet_aton(ip, &pAddr->sin_addr);
	//if (ret < 0)  
	//{
	//	//错误
	//}
#endif
}

void CUDPSocket::getSockAddr(char * ip,int iplen,WORD & port,const struct sockaddr_in * pAddr)
{
	if (ip == NULL || pAddr == NULL)	return;
	memset(ip,0,iplen);
	strcat(ip,inet_ntoa(pAddr->sin_addr));
	port = ntohs(pAddr->sin_port);
}

void CUDPSocket::debugout(const char * str,BYTE * pStrem,int len,const struct sockaddr_in * pAddr)
{
	char ip[20] = {0};
	WORD port = 0;
	CUDPSocket::getSockAddr(ip,20,port,pAddr);

	if (m_callbackDebugOutCmd != NULL)	m_callbackDebugOutCmd(str,pStrem,len,ip,port);
	return;

	WLOutDebugStreamEx(pStrem,len,"%.8d %s:%d UDP(%.4d)[ %.2X ]%s",WLGetTickCount(),ip,port,len,pStrem[3],str);
}

void CUDPSocket::setDebugoutCallback(TCallbackDebugOutCMD pFunc)
{
	m_callbackDebugOutCmd = pFunc;
}

int CUDPSocket::getSendRemoteAddr( struct sockaddr_in * pOutRemoteAddr )
{
	int res,ret = -1;
	if(NULL == pOutRemoteAddr)	return ret;

	memcpy(pOutRemoteAddr,&m_sendRemoteAddr,sizeof(struct sockaddr_in));

	return 0;
}
