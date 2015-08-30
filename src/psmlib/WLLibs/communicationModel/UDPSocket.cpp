// UDPSocket.cpp: implementation of the CUDPSocket class.
// ����UDP�࣬����Ϊ��CCommunicationSocket��ʵ���� send��recv�ӿ�
// Ŀ�ģ�Ϊ����Ӧ�����ͨ�ŷ�ʽ��ȷ��������������ͨ�Žӿڣ����磺
//       ĳӦ�ý���Ҫ�ô���ͨ�ţ����ǵ�ǰ�����޴��ڣ�����ʱʱ��UDP���档
//       �������ͨ�Žӿ���һ���ܺõķ�װ����������Ķ����ϴ󣬱����
//       ���ǽ�����������
// ע�⣺�ֵ���CTCPSocket��CUDPSocket��CCOMSocket
// ���ߣ�Ҷ����	2012-08-24
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
���ܣ����캯��������ʼ������
������remoteIP���Է�IPַ
      remotePort�������˿ں�
	  loaclPort�����ض˿ںţ�Ĭ��ֵΪ0����Ϊ0ʱ����ʾ�����һ���˿ں�
���أ���
���ߣ�Ҷ����	2012-08-24
*********************************************************************/
CUDPSocket::CUDPSocket(const char * remoteIP,WORD remotePort,WORD loaclPort)
				: CCommunicationSocket()
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

	//���÷��͵�ַ
	setSockaddr(remoteIP,remotePort,&m_sendRemoteAddr);
	//��ʼ��SOCKET
	struct sockaddr_in addr;
	initSock(&m_hSocket,&addr,loaclPort);
}

/*********************************************************************
���ܣ���������
��������
���أ���
���ߣ�Ҷ����	2012-08-24
*********************************************************************/
CUDPSocket::~CUDPSocket()
{
#ifdef WL_OS_CORE_WIN
	closesocket(m_hSocket);

	//win socket��Ҫ
	// WinSock DLL.                                  
    WSACleanup( );
	//--------------------------
#elif defined WL_OS_CORE_LINUX
	close(m_hSocket);
#endif
}

/*********************************************************************
���ܣ���������
������pbuf�����ͻ���
      len�������ֽ���
���أ�<0������>=0���ɹ������ֽ���
���ߣ�Ҷ����	2012-08-24
*********************************************************************/
int CUDPSocket::send(void * pbuf,int len)
{
	int ret = ::sendto(m_hSocket,(char *)pbuf,len,0,(struct sockaddr *)&m_sendRemoteAddr,sizeof(struct sockaddr_in));
//	debugout(">>",(BYTE *)pbuf,ret,&m_sendRemoteAddr);
	return ret;
}

/*********************************************************************
���ܣ���������
������pbuf�����ջ���
      len�������ֽ���
���أ�<0������>=0���ɹ������ֽ���
���ߣ�Ҷ����	2012-08-24
*********************************************************************/
int CUDPSocket::recv(void * pbuf,int len)
{
	int fromlen = sizeof(struct sockaddr_in);
	int ret = ::recvfrom(m_hSocket,(char *)pbuf,len,0,(struct sockaddr *)&m_recvRemoteAddr,(socklen_t*)&fromlen);
//	debugout("<<",(BYTE *)pbuf,ret,&m_recvRemoteAddr);
	return ret;
}

/*********************************************************************
���ܣ��������ݵ�ָ��������
������pbuf�����ͻ���
      len�������ֽ���
	  pRemoteAddr��Զ�̵�ַ
���أ�<0������>=0���ɹ������ֽ���
���ߣ�Ҷ����	2012-08-24
*********************************************************************/
int CUDPSocket::send(void * pbuf,int len,struct sockaddr_in * pRemoteAddr)
	{
	int ret = ::sendto(m_hSocket,(char *)pbuf,len,0,(struct sockaddr *)pRemoteAddr,sizeof(struct sockaddr_in));
//	debugout(">>",(BYTE *)pbuf,ret,pRemoteAddr);
	return ret;
}

/*********************************************************************
���ܣ��������ݣ������ط����ߡ��������� recv(void * pbuf,int len)�����ǣ�
      recv(void * pbuf,int len,struct sockaddr_in * pRemoteAddr) �����з����߷������Ķ����Խ���
	  recv(void * pbuf,int len) �� ֻ���ճ�ʼ��ʱָ����Զ�̷����ݣ��������͹�����ȫ����
������pbuf�����ջ���
      len�������ֽ���
���أ�<0������>=0���ɹ������ֽ���
���ߣ�Ҷ����	2012-08-24
*********************************************************************/
int CUDPSocket::recv(void * pbuf,int len,struct sockaddr_in * pRemoteAddr)
{
	int fromlen = sizeof(struct sockaddr_in);
	int ret = ::recvfrom(m_hSocket,(char *)pbuf,len,0,(struct sockaddr *)pRemoteAddr,(socklen_t*)&fromlen);
//	debugout("<<",(BYTE *)pbuf,ret,pRemoteAddr);
	return ret;
}

/*********************************************************************
���ܣ�����UDP Socket
������sock��socket
      addr�����ص�ֵַ
	  port���˿ںš�0��������0ΪĬ��ֵ��
���أ�<=0��ʧ�ܣ�1���ɹ�
���ߣ�Ҷ����	2012-08-24
*********************************************************************/
int CUDPSocket::initSock(int * sock,struct sockaddr_in *addr,short port)
{
	int addrLen;
	int rtn;
	//	struct timeval sockBlockTime;                                    //��һ�����ݰ���ʱʱ��
	
	*sock = socket(AF_INET,SOCK_DGRAM,0);                            //API����Socket��ΪUDP

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
	
	rtn = bind(* sock,(struct sockaddr *)addr,addrLen);              //����̶��Ķ˿ں���
	if (rtn <0)  return rtn;
	
	int   len   =   sizeof(struct   sockaddr_in);   

#ifdef WL_OS_CORE_WIN
	rtn = getsockname(* sock,(struct sockaddr *)addr,&len);          //����sock��IP���˿ں��Ƿ�󶨳ɹ�
	if (rtn < 0) return rtn;
#endif
	
	//wince ����sock��ʱ��Ч����select��ʵ��
	//	if (uTimeOut == 0)	return 1;
	//���ý��ճ�ʱʱ��
	//	(sockBlockTime).tv_sec = uTimeOut /1000;
	//	(sockBlockTime).tv_usec= uTimeOut %1000;
	//	setsockopt(*sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&sockBlockTime,          //����������ճ�ʱʱ��
	//		   sizeof(struct timeval));
	
	return 1;
}

/*********************************************************************
���ܣ�����IP��ַ���˿ںţ����������ַ�ṹ
������ip��IP��ַ(a.b.c.d��ʽ)��
      port���˿ں�
	  pAddr����ַ�ṹ��
���أ�������
���ߣ�Ҷ����	2012-08-24
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
	//	//����
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
