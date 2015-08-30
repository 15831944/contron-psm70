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
���ܣ�	���캯������ʼ������
������	��
���أ�	��
���ߣ�	Ҷ����
****************************************************************/
CTCPPeerLink::CTCPPeerLink(int s,struct sockaddr_in * remoteAddr,DWORD maxCatheCount) : CCommunicationPeerLink(maxCatheCount)
{
	m_hSocket = s;
	memcpy(&m_remoteAddr,remoteAddr,sizeof(struct sockaddr_in));
}

/****************************************************************
���ܣ�	����������������
������	��
���أ�	��
���ߣ�	Ҷ����
****************************************************************/
CTCPPeerLink::~CTCPPeerLink(void)
{
}

/****************************************************************
���ܣ�	������
������	buf������Դ�׵�ַ
		len�����ͳ���
���أ�	>0���ɹ������ֽ���
		SOSCKET_ERROR,ʧ��
���ߣ�	Ҷ����
****************************************************************/
int CTCPPeerLink::send(void * buf,DWORD len)
{
	return ::send(m_hSocket,(char *)buf,len,0);
}

/****************************************************************
���ܣ�	������
������	buf������Դ�׵�ַ
		len�����ճ���
���أ�	>0���ɹ������ֽ���
		SOSCKET_ERROR,ʧ��
���ߣ�	Ҷ����
****************************************************************/
int CTCPPeerLink::recv(void * buf,DWORD len)
{
	return ::recv(m_hSocket,(char *)buf,len,0);
}

/****************************************************************
���ܣ�	��ȡSOCKET
������	��
���أ�	��ǰSOCKET
���ߣ�	Ҷ����
****************************************************************/
int CTCPPeerLink::getSocket()
{
	return m_hSocket;
}

/****************************************************************
���ܣ�	�������ݵ������ݻ���
������	s��			  SOCKET����
		pbuf��		  ���Ƶ�Դ����
		len��		  Դ���ݵĳ���
		maxInterval�� �����ʱ��
���أ�  -1�� SOCKET����ͬ��ǰSOCKET��һ��
		>0�� �����ݻ��������ݵ�����Ч����
���ߣ�	Ҷ����
****************************************************************/
int CTCPPeerLink::addBufToRecvCache(int s,void * pbuf,DWORD len,DWORD maxInterval)
{
	if (s != m_hSocket)	return -1;

	//�����������ø����ͬ����������Ȼ�����ͬ��������������
	return CCommunicationPeerLink::addBufToRecvCache(pbuf,len,maxInterval);
}