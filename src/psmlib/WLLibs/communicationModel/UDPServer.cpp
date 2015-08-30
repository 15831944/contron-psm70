// UDPServer.cpp: implementation of the CUDPServer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UDPServer.h"
#include <stdio.h>

#include "../platformLayer/WLPlatformLayer.h"
#include "../platformLayer/WLThread.h"
#include "../platformLayer/WLOutDebug.h"

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

CUDPServer::CUDPServer(WORD localPort)
{
	m_pSocket  = new CUDPSocket(NULL,0,localPort);
	m_pThdRecv = new CThdUDPServerRecv(this);               //���������߳��࣬�ݲ����� 
}

CUDPServer::~CUDPServer()
{
	delete m_pSocket;
}

/*********************************************************************
���ܣ������������߳�
��������
���أ���
���ߣ�Ҷ����	2012-09-20
*********************************************************************/
int CUDPServer::runRecvThread()
{ 
	return m_pThdRecv->resume();
}

/*********************************************************************
���ܣ����ݻ���m_recvBuf�������������
������out�����������������һ�������ָ�룬�ڴ�ռ����ⲿ����
      outlen�����������������һ������ĳ���
���أ�>=0���ɹ������� =0 ��ʾ���������Ѳ����ܴ���һ���������
                 ���� >0 ��ʾ�������п��ܻ���������������߿��ظ���
				 �ñ��������������������ֱ������ֵ<=0Ϊֹ
      <0��ʧ��
ע�⣺���ر�������������ȷ���ĺ��м�Ҫ�����ġ�DWORD m_recvbuflen;//��������ǰ�����ֽ�����
���ߣ�Ҷ����	2012-11-28
*********************************************************************/
int CUDPServer::analyseProtocol(BYTE *out,DWORD &outlen)
{
	return 0;
}

/*********************************************************************
���ܣ�������յ������
������pCmd�������ָ��
      len������ĳ���
	  pRemoteAddr��Զ���������
���أ����岻��
ע�⣺��������Ϊwin32��
���ߣ�Ҷ����	2012-11-28
*********************************************************************/
void CUDPServer::procRecvCmd(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr)
{

}

int CUDPServer::recvThreadProc()
{
	int recvlen;
	BYTE recvBuf[UDPSERVER_MAX_CMD_LEN];
	DWORD lastRecvTick = 0;
	BYTE cmd[UDPSERVER_MAX_CMD_LEN] = {0};
	DWORD  cmdlen = 0;
	int ret;
	struct sockaddr_in remoteAddr;

	m_recvbuflen = 0; //��ձ��Ļ���
	while (1)
	{
		//�жϵ�ǰ�Ƿ���Ҫ������
		//WLSleep(100000);
		//continue;

		//������
		recvlen = m_pSocket->recv(recvBuf,UDPSERVER_MAX_CMD_LEN,&remoteAddr);//
		if (recvlen <0)
		{
			//�����ݴ��󣬿��������ӶϿ����߳�����һ��ʱ��
			//WLOutDebug("�����ݴ���\n");
#ifdef WL_OS_CORE_WIN
			WLOutDebug("UDP����� �����ݴ���recvlen = %d,errCode = %d \n",recvlen,GetLastError());
#elif defined WL_OS_CORE_LINUX
#endif
			WLSleep(1000);

			continue;
		}

//		if (m_recvbuflen + recvlen >= UDPSERVER_MAX_CMD_LEN)	m_recvbuflen = 0; //������������ǰ�����л���
		//������������ǰ�����л���
		if (m_recvbuflen + recvlen >= UDPSERVER_MAX_CMD_LEN)	
		{
			m_recvbuflen = 0; 
			WLOutDebug("CUDPServer::recvThreadProc�������������ǰ�����л���");
		}

		//���յ������ݰ����������ݴ󻺳�֮�У����ں����Э�����
		if (WLGetTickCount() - lastRecvTick >500) m_recvbuflen = 0; //�ж�����������ʱ���Ƿ���󣬹���˵����
		                                                            //��ǰ�յ�������һ����һ���±��ģ��뻺������Ĳ���������
		                                                            //û�й�ϵ���������������޷����꣬���벿���Ѷ�ʧ��
		lastRecvTick = WLGetTickCount();
		memcpy(&m_recvBuf[m_recvbuflen],recvBuf,recvlen);
		m_recvbuflen += recvlen;

		//��������,��������
		memset(cmd,0,UDPSERVER_MAX_CMD_LEN);
		ret = analyseProtocol(cmd,cmdlen);
		while (ret >=0)
		{
			//���������
			procRecvCmd(cmd,cmdlen,&remoteAddr);

			//����������л���������������������ŷ���
			memset(cmd,0,UDPSERVER_MAX_CMD_LEN);
			if (ret > 0)	ret = analyseProtocol(cmd,cmdlen);
			else break;
		}
	}
	return 0;
}


CThdUDPServerRecv::CThdUDPServerRecv(CUDPServer * pServer) : CWLThread(1)

{
	m_pServer = pServer;
}

int CThdUDPServerRecv::run()
{	
	int ret = m_pServer->recvThreadProc();
#ifdef WL_OS_CORE_WIN
	WLOutDebug("�߳��˳��ˣ��߳�ID = %.8X,handle = %.8X",m_threadID,m_hThread);
#elif defined WL_OS_CORE_LINUX
#endif
	return ret;
}
