#include "stdafx.h"
#include "TCPPeerLink.h"
//#include <winsock2.h>
#ifdef WL_OS_CORE_WIN
#include <winsock2.h>
#ifdef WL_OS_WIN32
#pragma comment(lib, "ws2_32.lib")
#else ifdef WL_OS_WINCE
#pragma comment(lib, "ws2.lib")
#endif
#endif

/****************************************************************
功能：	构造函数，初始化工作
参数：	无
返回：	无
作者：	叶文林
****************************************************************/
CTCPPeerLink::CTCPPeerLink(int s,struct sockaddr_in * remoteAddr,DWORD maxCatheCount) : CCommunicationPeerLink(maxCatheCount)
{
	m_hSocket = s;
	memcpy(&m_remoteAddr,remoteAddr,sizeof(struct sockaddr_in));
}

/****************************************************************
功能：	析构函数，清理工作
参数：	无
返回：	无
作者：	叶文林
****************************************************************/
CTCPPeerLink::~CTCPPeerLink(void)
{
}

/****************************************************************
功能：	发数据
参数：	buf，发送源首地址
		len，发送长度
返回：	>0，成功发送字节数
		SOSCKET_ERROR,失败
作者：	叶文林
****************************************************************/
int CTCPPeerLink::send(void * buf,DWORD len)
{
	return ::send(m_hSocket,(char *)buf,len,0);
}

/****************************************************************
功能：	收数据
参数：	buf，接收源首地址
		len，接收长度
返回：	>0，成功接收字节数
		SOSCKET_ERROR,失败
作者：	叶文林
****************************************************************/
int CTCPPeerLink::recv(void * buf,DWORD len)
{
	return ::recv(m_hSocket,(char *)buf,len,0);
}

/****************************************************************
功能：	获取SOCKET
参数：	无
返回：	当前SOCKET
作者：	叶文林
****************************************************************/
int CTCPPeerLink::getSocket()
{
	return m_hSocket;
}

/****************************************************************
功能：	复制数据到收数据缓存
参数：	s，			  SOCKET对象
		pbuf，		  复制的源数据
		len，		  源数据的长度
		maxInterval， 最大间隔时间
返回：  -1， SOCKET对象同当前SOCKET不一致
		>0， 收数据缓存中数据的有有效长度
作者：	叶文林
****************************************************************/
int CTCPPeerLink::addBufToRecvCache(int s,void * pbuf,DWORD len,DWORD maxInterval)
{
	if (s != m_hSocket)	return -1;

	//必须这样调用父类的同名函数，不然父类的同名函数被隐藏了
	return CCommunicationPeerLink::addBufToRecvCache(pbuf,len,maxInterval);
}