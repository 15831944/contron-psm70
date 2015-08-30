// StreamClinet.cpp: implementation of the CStreamClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StreamClient.h"

#include "CommunicationSocket.h"
#include "UDPSocket.h"
#include "../platformLayer/WLCriticalSection.h"
#include <stdio.h>
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


/*********************************************************************
���ܣ����캯������ʼ����Ա
��������
���أ���
���ߣ�Ҷ����	2012-09-24
*********************************************************************/
CStreamClient::CStreamClient()
{
	memset(m_recvBuf,0,MAX_CMD_LEN*10);                  //�����ݻ��壬���յ����������ڴˣ�Ȼ�������Э�����������������ݣ�Ϊ�10��������ĳ��ȣ����֧��10������һ���Ե��
	m_recvbuflen = 0;                                    //��������ǰ�����ֽ���

	m_pSocket = 0;                                       //ͨ�Žӿ�(���� TCP/IP��socket����ҪŪ��)��������UDP��TCP�����ڻ�����
	memset(&m_RqtRspCache,0,sizeof(TRqtRspCache));       //����(��)���ݼ���Ӧ(��)�����߳�ͨ�Ż��壬��Ҫ�������ٽ���һ��ʹ��

	//win32ƽ̨
	m_pCSSend = new CWLCriticalSection();                //����m_pSocket send���ݼ�����m_RqtRspCache���ٽ���
//	m_hEventRsp = CreateEvent(NULL, false, false, NULL); //�յ�������Ӧ���¼�������Ӧ(��)�����߳��յ��������ʱ��֪ͨ�����̣߳���sendWaitRsp�У�����Ӧ������
	m_pEvent = new CWLEvent(false, false);
	m_pCSRqtRsp = new CWLCriticalSection();              //sendRqtRecvRsp �����ٽ�������֤ �˺����̰߳�ȫ��

	//QTƽ̨
	//��ʱδ����

	m_pRecvThread = new CThdClientRecv(this);               //���������߳��࣬�ݲ����� 

	//m_pFuncProMemoteNotify = NULL;                       //����c/dͨ��ģ�ͺ���ָ��

	m_oneRecvMaxlen = MAX_CMD_LEN;                         //һ�ν������ݰ�����󳤶ȣ�recvThreadProc() �������õ�
}

/*********************************************************************
���ܣ���������
��������
���أ���
ע�⣺���ڱ������������߳��ع�������������TerminateThread�����̣߳�����
      �᲻̫�á����Ե�����������̺߳������غ��ٵ��ñ�����
���ߣ�Ҷ����	2012-09-24
*********************************************************************/
CStreamClient::~CStreamClient()
{
	delete m_pRecvThread;                                //���д�����ܻ������ȶ����أ�����ع�������Ҫ������
	WLSleep(100);

	//win32ƽ̨
	delete m_pCSSend;                                    //����m_pSocket send���ݼ�����m_RqtRspCache���ٽ���
//	CloseHandle(m_hEventRsp);                            //�յ�������Ӧ���¼�������Ӧ(��)�����߳��յ��������ʱ��֪ͨ�����̣߳���sendWaitRsp�У�����Ӧ������
	delete m_pEvent;
	delete m_pCSRqtRsp;

	//QTƽ̨
	//��ʱδ����	
}

/*********************************************************************
���ܣ�����ͨ�Žӿ�
������pSocket��ͨ�Žӿ�(���� TCP/IP��socket����ҪŪ��)��������UDP��TCP��
���ڻ�����
���أ���
���ߣ�Ҷ����	2012-09-20
*********************************************************************/
void CStreamClient::setCommunicationSocket(CCommunicationSocket * pSocket)
{
	m_pSocket = pSocket;
}

/*********************************************************************
���ܣ���ȡͨ�Žӿ�
��������
���أ�ͨ�Žӿ�ָ��
���ߣ�Ҷ����	2012-09-20
*********************************************************************/
CCommunicationSocket * CStreamClient::getCommunicationSocket()
{
	return m_pSocket;
}
/*
/*********************************************************************
���ܣ����ô���ͨ��ģ��c\d�Ļص����������Э���в�����c\dģ�ͣ��벻����
������callbackFunc���ص�����ָ��
���أ���
���ߣ�Ҷ����	2012-09-20
*********************************************************************/
/*void CStreamClient::setRemoteNotifyProc(TPFuncProcRemoteNotify * pCallbackFunc)
{
	
}
*/
/*********************************************************************
���ܣ������������߳�
��������
���أ���
���ߣ�Ҷ����	2012-09-20
*********************************************************************/
int CStreamClient::runRecvThread()
{ 
	return m_pRecvThread->resume();
};    

/*********************************************************************
���ܣ���������ģ�ͣ����������̲߳���ȫ��ȫ(��ȫ�ԣ�QT�� > win32��
      ����ȫ�������ڣ������ڵȴ�����ʱ���ͷŷ����ٽ�������ʱ�����߳̿�
	  ���ñ������������Ϊ������������һ���ٽ������Խ��������)
������sendbuf�����Ͱ�ָ��
      sendbuflen�����Ͱ�����
	  recvbuf�������������Ž��յ������ݰ�
	  recvbuflen������������������룺���recvbuf���峤�ȣ�
	                            �������Žӵ������ݰ�����
	  waitRecvMulSec���ȴ���Ӧʱ�䣬��λ ���룬Ĭ��ֵ 400
���أ�>=0���ɹ���Ϊ��Ӧ�ֽ���
      < 0��ʧ�ܣ�-1��δ�ܽ��뱾�����ٽ���
	             -2����������ʧ��
				 -3���ȴ���Ӧ��ʱ
				 -4���ȴ���Ӧʧ��
ע�⣺������Ϊwin32��
      win32��ȱ�ݣ���������������ڽ���ȴ�����ǰ��recv�߳����յ���Ӧ
	               ����������ʱ��һ���̷߳�����һ��������Ӧģ�ͣ�����
				   Ӧ��Ҳ�ڱ���������ȴ�����ǰ���������ǰһ����Ӧ��
				   ������
				   ��ȱ�ݷ������ʼ������������ʱ���ǿ����޸��ġ�
				   �����ǣ���������� ������Ӧ�����Ƿ������ݣ������
				           �ȵȴ�������Ӧȡ�����ٴ��롣
				   ��QT�У��õȴ�������ȱ�ݿ��ԽϺý��
���ߣ�Ҷ����	2012-09-20
*********************************************************************/
int CStreamClient::sendRqtRecvRsp(BYTE * sendbuf,DWORD sendbuflen,BYTE * recvbuf,DWORD &recvbuflen,int waitRecvMulSec,int retryCount)
{
	//WLOutDebug("the time = %d",waitRecvMulSec);
	//m_pCSRqtRsp Ϊ �������ٽ���
	//m_pCSSend Ϊ ���� m_RqtRspCache �ٽ��� 
	//�˴��������д��󣬵��´�����Ѷ������������


	//���뱾���������ٽ���
	//BOOL bRet = m_pCSRqtRsp->tryEnter();
	//if (bRet == FALSE)	return -9;
	m_pCSRqtRsp->enter();

	int ret =0;

	if (m_pSocket == NULL || m_pCSSend == NULL)	
	{
		m_pCSRqtRsp->leave();
		return -8;  //��Ҫ����δ��ֵ
	}

	//�������ٽ���
	m_pCSSend->enter();

	//������Ӧ�¼�
//	ResetEvent(m_hEventRsp);
	m_pEvent->event_reset();

	int retrySendCount = 0;
LAB_RETRY:
	//��������
	ret = m_pSocket->send(sendbuf,sendbuflen);
	if (ret <=0)	                                       //������ʧ�ܣ�����������Ͽ���
	{
		//�������ٽ���
		m_pCSSend->leave();
		//if (retrySendCount == 0)	m_pCSRqtRsp->leave();
		m_pCSRqtRsp->leave();
		return -2;
	}

	//����������Ӧ���ݻ���
	memset(&m_RqtRspCache,0,sizeof(TRqtRspCache));         //��ջ���
	memcpy(m_RqtRspCache.rqt,sendbuf,sendbuflen);
	m_RqtRspCache.rqtlen = sendbuflen;
	m_RqtRspCache.isExistRqt  = 1;
	//�������ٽ���
	m_pCSSend->leave();

	//�ȴ���Ӧ(procRecvCmd������֪ͨ���������󵽴�)
	//ʹ�߳�������һ��������������ͬ�ڣ������ȴ��ź�������
	recvbuflen = 0;
	//printf("�����ջ�����\n");
//	ret = WaitForSingleObject(m_hEventRsp,waitRecvMulSec); //����Ӧ��
	ret = m_pEvent->event_timedwait(waitRecvMulSec);
//	if (ret == WAIT_TIMEOUT)
	if (ret == 1)
	{
		//�뿪�����������ٽ���
		//m_pCSRqtRsp->leave();
		retrySendCount++;
		if (retrySendCount < retryCount)
		{
			m_pCSSend->enter();
			goto LAB_RETRY;
		}
		m_pCSRqtRsp->leave();
		return -3;
	}
//	if (ret != WAIT_OBJECT_0)
	if (ret != 0)
	{
		//�뿪�����������ٽ���
		m_pCSRqtRsp->leave();
		return -4;
	}

	//ȡ���ͻص�����
	recvbuflen = m_RqtRspCache.rsplen ;
	memcpy(recvbuf,m_RqtRspCache.rsp ,recvbuflen);
	memset(&m_RqtRspCache,0,sizeof(TRqtRspCache));         //��ջ���

	//�뿪�����������ٽ���
	m_pCSRqtRsp->leave();

	return recvbuflen;
}

/*********************************************************************
���ܣ��������ݣ�������ֻ����ͨ��ģ�ͣ�a/d�вŻ��õ�
������sendbuf�����Ͱ�ָ��
      sendbuflen�����Ͱ�����
���أ��ɹ������ֽ���
���ߣ�Ҷ����	2012-09-20
*********************************************************************/
int CStreamClient::send(BYTE * sendbuf,DWORD sendbuflen)
{
	int sendlen;

	if (m_pSocket == NULL || m_pCSSend == NULL)	return -1; //��Ҫ����δ��ֵ

	//�������ٽ���
	m_pCSSend->enter();

	//��������
	sendlen = m_pSocket->send(sendbuf,sendbuflen);

	//�������ٽ���
	m_pCSSend->leave();

	return sendlen;
}

/*********************************************************************
���ܣ��������߳�ִ�к������߳��ཫ���ñ�����
��������
���أ����岻��
���ߣ�Ҷ����	2012-09-20
*********************************************************************/
int CStreamClient::recvThreadProc()
{
	int recvlen;
	BYTE recvBuf[MAX_CMD_LEN];
	DWORD lastRecvTick = 0;
	BYTE cmd[MAX_CMD_LEN] = {0};
	DWORD  cmdlen = 0;
	int ret;

	m_recvbuflen = 0; //��ձ��Ļ���
	while (1)
	{
		//�жϵ�ǰ�Ƿ���Ҫ������
		//WLSleep(100000);
		//continue;
		
		//WLOutDebug("the m_pSocket is prepare................");
//		int t_ret = m_pSocket->send((void*)"12121212",6);
		//������
		recvlen = m_pSocket->recv(recvBuf,m_oneRecvMaxlen);//MAX_CMD_LEN

		//WLOutDebug("the m_pSocket is XXXXX");
		if (recvlen <0)
		{
			//�����ݴ��󣬿��������ӶϿ����߳�����һ��ʱ��
			//WLOutDebug("�����ݴ���\n");
#ifdef WL_OS_CORE_WIN
			WLOutDebug("UDP �ͻ��� �����ݴ���recvlen = %d,errCode = %d \n",recvlen,GetLastError());
#elif defined WL_OS_CORE_LINUX
			WLOutDebug("UDP �ͻ��� �����ݴ���recvlen = %d,errno=%d \n", recvlen, errno);
#endif
			WLSleep(1000);
			
			continue;
		}
		
		if (m_recvbuflen + recvlen >= MAX_CMD_LEN * 10)	m_recvbuflen = 0; //������������ǰ�����л���

		//���յ������ݰ����������ݴ󻺳�֮�У����ں����Э�����
		if (WLGetTickCount() - lastRecvTick >500) m_recvbuflen = 0; //�ж�����������ʱ���Ƿ���󣬹���˵����
		//��ǰ�յ�������һ����һ���±��ģ��뻺������Ĳ���������
		//û�й�ϵ���������������޷����꣬���벿���Ѷ�ʧ��
		lastRecvTick = WLGetTickCount();
		memcpy(&m_recvBuf[m_recvbuflen],recvBuf,recvlen);
		m_recvbuflen += recvlen;
		
		//��������,��������
		memset(cmd,0,MAX_CMD_LEN);
		ret = analyseProtocol(cmd,cmdlen);
		while (ret >=0)
		{
			//���������
			procRecvCmd(cmd,cmdlen);

			//����������л���������������������ŷ���
			memset(cmd,0,MAX_CMD_LEN);
			if (ret > 0)	ret = analyseProtocol(cmd,cmdlen);
			else break;
		}
	}
}

/*********************************************************************
���ܣ�������յ���������������£�
      (1) �ȵ��� pretreatmentRecvCmd Ԥ���������1���˳�
	  (2) ���� isMatchRqtRsp �ж������Ƿ�Ϊ��������Ӧ��ģ�͵���Ӧ���
	      ����Ǵ������˳�
	  (3) ����ͨ��Ӧ�����
������pCmd�������ָ��
      outlen������ĳ���
���أ����岻��
ע�⣺��������Ϊwin32��
���ߣ�Ҷ����	2012-09-20
*********************************************************************/
int CStreamClient::procRecvCmd(BYTE *pCmd,DWORD cmdlen)
{
	int ret = 0;
	
	//���뷢���ٽ���
	m_pCSSend->enter();

	//Ԥ��������
	ret = pretreatmentRecvCmd(pCmd,cmdlen);
	if (ret == 1)	
	{
		//���ٷ��ͽ���
		m_pCSSend->leave();
		return ret;
	}

	//����ͨ��ģ��b��������Ӧģ��
	ret = isMatchRqtRsp(m_RqtRspCache.rqt,m_RqtRspCache.rqtlen,pCmd,cmdlen);
	if (ret == TRUE) 
	{
		//����Ӧ���뻺���������Ļ���״̬��֪ͨ�����߳��յ���Ӧ����
		m_RqtRspCache.isExistRqt  = 0;
		//printf("set m_RqtRspCache.sendbufFlag = 0\n");
		m_RqtRspCache.rsplen  = cmdlen;
		memcpy(m_RqtRspCache.rsp ,pCmd,cmdlen);
		//for (int i =0;i<10;i++) Sleep(10);
//		ret = SetEvent(m_hEventRsp);
		ret = m_pEvent->event_set();

		//�������ٽ���
		m_pCSSend->leave();
		return ret;
	}

	//����ͨ��ģ��c/d����Ӧʽ����Ӧ����ʽ
	//if (m_pFuncProMemoteNotify != NULL)	(*m_pFuncProMemoteNotify)(pCmd,cmdlen);
	procRemoteNotify(pCmd,cmdlen);

	//�������ٽ���
	m_pCSSend->leave();

	return 0;
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
���ߣ�Ҷ����	2012-09-20
*********************************************************************/
int CStreamClient::analyseProtocol(BYTE *out,DWORD &outlen)
{
	outlen = (m_recvbuflen > MAX_CMD_LEN) ? MAX_CMD_LEN : m_recvbuflen;
	memcpy(out,m_recvBuf,outlen);
	m_recvbuflen = 0;
	return 0;
}

/*********************************************************************
���ܣ�Ԥ������Ӧ��(���յ������ݰ�)��procRecvCmd�����ڴ�����Ӧ��֮ǰ��
      �ȵ��ñ�����������������1ʱ��procRecvCmd������������Ӧ����һ��
	  ����
	  �������ر���������ʵ�ִ�������������������޸���Ӧ��
������pCmd�������ָ��
      outlen������ĳ���
���أ�1�����������(procRecvCmd����)��ֹ��һ������;��1����������
���ߣ�Ҷ����	2012-09-20
*********************************************************************/
int CStreamClient::pretreatmentRecvCmd(BYTE *pCmd,DWORD cmdlen)
{
	return 0;
}

/*********************************************************************
���ܣ���֤��������Ӧ�Ƿ�ƥ��
������pRqt����������ָ��
      pRsp����Ӧ����ָ��
���أ�1��ƥ��
      0����ƥ��
���ߣ�Ҷ����	2012-09-20
*********************************************************************/
BOOL CStreamClient::isMatchRqtRsp(BYTE *pRqt,DWORD rqtlen,BYTE *pRsp,DWORD rsplen)
{
	return 0;
}

/*********************************************************************
���ܣ�����Է���������������Ӧc��d����ͨ��ģ��
������pCmd,����ָ��
      cmdlen�������
���أ���
���ߣ�Ҷ����	2012-09-25
*********************************************************************/
void CStreamClient::procRemoteNotify(BYTE *pCmd,DWORD cmdlen)
{

}

CThdClientRecv::CThdClientRecv(CStreamClient * pClient) : CWLThread(1)

{
	m_pClient = pClient;
}
/*
CThdClientRecv::~CThdClientRecv() :~CWLThread()
{

}*/

int CThdClientRecv::run()
{	
	int ret = m_pClient->recvThreadProc();
#ifdef WL_OS_CORE_WIN
	WLOutDebug("�߳��˳��ˣ��߳�ID = %.8X,handle = %.8X",m_threadID,m_hThread);
#elif defined WL_OS_CORE_LINUX
//	WLOutDebug("�߳��˳��ˣ��߳�ID = %.8X,handle = %.8X",m_threadID,m_hThread);
#endif
	return ret;
}

