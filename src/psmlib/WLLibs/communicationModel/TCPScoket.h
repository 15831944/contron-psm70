/**************************************************************************************************
TCPSocket.h
���ܣ�������TCP���������ʹ����TCP����˽������ӷ��񣬷���˽������ݷ��񼰿ͻ��˷������������
      ����TCP����˽������ӷ����������շ����ݣ����ַΪ���������ص�ַ��
���ߣ���С��  2013-10-31
**************************************************************************************************/
#ifndef __TCPSOCKET_H__
#define __TCPSOCKET_H__

#include "CommunicationSocket.h"
#include "../platformLayer/WLDEF.h"
#ifdef WL_OS_CORE_WIN
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")
#elif defined WL_OS_CORE_LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#endif

class CTCPScoket : public CCommunicationSocket  
{
public:
	CTCPScoket(DWORD localPort = 0);                     //����������񣬼��������ӷ���,��ֹ�շ�����
	CTCPScoket(int s, sockaddr_in clientAddr);           //����˽������ݷ���
	CTCPScoket(const char * remoteIP, WORD remotePort);  //�ͻ���
	virtual ~CTCPScoket();

	virtual int accept(struct sockaddr_in * pClientAddr, int * pSock);    //����˽�������
	virtual int send(void * pbuf,int len);
	virtual int recv(void * pbuf,int len);

//	int getSOCKET(int *pSocket);
	int getAddress(struct sockaddr_in * pAddr);

	int initSocket(char *pIP=NULL, DWORD port=0);

private:
//	int initSocket(char *pIP=NULL, DWORD port=0);

private:
	int   m_type;                     //���ͱ�ʶ��1�������������2������˽������ݷ���3���ͻ���
	int   m_hSocket;
	WORD  m_port;
	char  m_ip[64];
	struct sockaddr_in m_addr;        //�����������ʱΪ���ص�ַ������˽������ݷ���Ϊ�ͻ��˵�ַ���ͻ��˷���Ϊ��������ַ
};

#endif