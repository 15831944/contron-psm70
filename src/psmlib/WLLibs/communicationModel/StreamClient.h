// StreamClinet.h: interface for the CStreamClient class.
// ����:���ͻ���ģ�ͻ���
// Ӧ�ó��ϣ�ͨ��ģ�ͱ�����Գ���Ϊ�ͻ��˺ͷ������һ������·���������£�
//           (1) �ͻ��˶�Ӧһ��Ψһ�ķ����
//           (2) ���˷����֮��ͻ��˲����յ������˵㷢�͹���������
//           (3) �������������UDP�Ȳ���������·�����ͨ���ǲ���ȫ��ģ�͵ģ����Ǿ�������߱�����
// ���ڲ���������·�������������������һ��Ҳ����ʹ�ã�
// A:�ͻ���ֻ�����յ����������
// B:�ͻ�������յ�����˵�����ݣ����ǿ��Ա�֤ÿ�η��͹�ҵ������Ϊһ����������������������֮�䲻����ƴ��������
//   �����ע����д analyseProtocol
//   ���ڱ���UDP�ȶ�ʽ������ȫ����Ҫ��
// C:������ʱ�������жϷ�����Ƿ�Ϸ�����UDP��ʽ��֤�Է�IP��ַ���˿ںţ����ܶ������Ϸ�IP
// ���ͻ����ص����£�
// (1) һ�����������Ϊһ�����Ļ������ķ��ͽ��գ�Ҳ����һ�����Ķ�Ӧ
//     ��������;
// (2) ��Ҫʵ��һ��Э�����������analyseProtocol�������з����������;
// (3) ��Ҫʵ��һ����Ӧ���������procRspCmd�������ڽ�������Ӧ����
//     ����(������Ӧʽ����Ӧʽ)����
// (3) ������һ���߳����ڽ��շ��������ݣ�
// (4) ֧����������ͨ��ģʽ�����壺���󣺷������ģ���Ӧ���յ�����
//     a. ������Ӧʽ���������������ģ��ȴ��Է���Ӧ����
//     b. ����ʽ���������ͱ��ģ����ǶԷ����Ա�������Ӧ
//     c. ��Ӧʽ���Է������ϱ����ģ����˲���Ӧ
//     d. ��Ӧ����ʽ���Է������ϱ����ģ�������Ӧ
//     ע�⣺aģ�ͣ��ӿ�Ϊ: sendRqtRecvRsp
//           bģ�ͣ��ӿ�Ϊ: send
//           c��dģ��ͨ���ӿ�setRemoteNotifyProc���ûص����� 
// ע�⣺������QT����δ�ҵ��������滻win32���¼��ķ�������֮��ش��뽫
//       �ṩWin32��QT�����棬��ش���Ϊ��
//       (1) �����Ա�����������̵߳����к��� m_pRecvThread->run();
//       (2) �����Ա��sendRqtRecvRsp();
//       (3) �����Ա��send(); 
//       (4) �����Ա���ԣ�m_pCSSend��m_hEventRsp��
// ���ߣ�Ҷ����	2012-09-19
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STREAMCLINET_H__76C17349_CEFD_4D5B_9660_15071BAEADE6__INCLUDED_)
#define AFX_STREAMCLINET_H__76C17349_CEFD_4D5B_9660_15071BAEADE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../platformLayer/WLPlatformLayer.h"
//#include "windows.h"
#include "../platformLayer/WLDEF.h"
#include "../platformLayer/WLThread.h"
#include "../platformLayer/WLEvent.h"

#define MAX_CMD_LEN (4096)     //ͨ�����ݰ���󳤶�

//���壺������Ӧ����ṹ
//Ŀ�ģ��ݴ�һ����Ե�������������Ӧ�������������������߳�����������߳�ͬ��
typedef struct  {
	BYTE  rqt[MAX_CMD_LEN];    //��������
	DWORD rqtlen;              //����������ĳ���
	DWORD isExistRqt;          //����������������Ƿ���Ч�����������ڣ�,�յ���Ӧ��������Ϊ0�������߳�copy��Ӧ��������������
	BYTE  rsp[MAX_CMD_LEN];    //��Ӧ����
	DWORD rsplen;              //��Ӧ������ĳ���
}TRqtRspCache;//TRqtRspCache;

//���庯��ָ�����ͣ�����c.dͨ��ģ����Ӧ���ݰ�
//typedef int (WINAPI * TPFuncProcRemoteNotify)(void * cmd,DWORD cmdlen);
//typedef int (* TPFuncProcRemoteNotify)(void * cmd,DWORD cmdlen);

class CWLCriticalSection;
class CCommunicationSocket;
class CThdClientRecv;
class CStreamClient  
{
public:
	CStreamClient();
	virtual ~CStreamClient();

	void setCommunicationSocket(CCommunicationSocket * pSocket);
	CCommunicationSocket * getCommunicationSocket();
	//void setRemoteNotifyProc(TPFuncProcRemoteNotify * pCallbackFunc);
	int runRecvThread();
	int sendRqtRecvRsp(BYTE * sendbuf,DWORD sendbuflen,BYTE * recvbuf,DWORD &recvbuflen,int waitRecvMulSec = 2000,int retryCount = 3);
	int send(BYTE * sendbuf,DWORD sendbuflen);
protected:
	void setOneRecvMaxLen(int maxlen);
	virtual int analyseProtocol(BYTE *out,DWORD &outlen);     //��m_recvBuf���У�����������
	virtual int pretreatmentRecvCmd(BYTE *pCmd,DWORD cmdlen); //Ԥ�����������1�����ߵ�����(procRecvCmd)���������������
	virtual BOOL isMatchRqtRsp(BYTE *pRqt,DWORD rqtlen,
		                       BYTE *pRsp,DWORD rsplen);      //�ж���������Ӧ�����Ƿ�ƥ��
	virtual void procRemoteNotify(BYTE *pCmd,DWORD cmdlen);   //����Է��������������

	BYTE m_recvBuf[MAX_CMD_LEN*10];                      //�����ݻ��壬���յ����������ڴˣ�Ȼ�������Э�����������������ݣ�Ϊ�10��������ĳ��ȣ����֧��10������һ���Ե��
	DWORD m_recvbuflen;                                  //��������ǰ�����ֽ���

	CCommunicationSocket *m_pSocket;                     //ͨ�Žӿ�(���� TCP/IP��socket����ҪŪ��)��������UDP��TCP�����ڻ�����
	TRqtRspCache m_RqtRspCache;                          //����(��)���ݼ���Ӧ(��)�����߳�ͨ�Ż��壬��Ҫ�������ٽ���һ��ʹ��

	//win32ƽ̨
	CWLCriticalSection *m_pCSSend;                       //����m_pSocket send���ݼ�����m_RqtRspCache���ٽ�����
	                                                     //��֤һ��ֻ��һ���̵߳���m_pSocket��������
	                                                     //��sendWaitRsp�����н��������ݺͲ���m_RqtRspCache��װΪһ��ԭ��
	CWLCriticalSection *m_pCSRqtRsp;                     //sendRqtRecvRsp �����ٽ�������֤ �˺����̰߳�ȫ��
//	HANDLE m_hEventRsp;                                  //�յ�������Ӧ���¼�������Ӧ(��)�����߳��յ��������ʱ��֪ͨ�����̣߳���sendWaitRsp�У�����Ӧ������
	CWLEvent *m_pEvent;
	
	//QTƽ̨
	//��ʱδ����

	CWLThread * m_pRecvThread;                           //����������  

	//DWORD m_rqtWaitRspTimeout;                           //�������Ӧ֮��ȴ���ʱʱ��
	
	//TPFuncProcRemoteNotify *m_pFuncProMemoteNotify;      //����c/dͨ��ģ�ͺ���ָ��
	int m_oneRecvMaxlen;                                 //һ�ν������ݰ�����󳤶�
public:
	int recvThreadProc();                                //�����̵߳��õĴ�����    
private:
	int procRecvCmd(BYTE *pCmd,DWORD cmdlen);            //������յ�������ȴ������������ٸ�����Ӧ��������(������Ӧʽ����Ӧʽ)����
	
};

class CThdClientRecv  : public CWLThread
{
public:
	CThdClientRecv(CStreamClient * pClient);
	//virtual ~CThdClientRecv();
private:
	virtual int run(); //�̺߳���
	CStreamClient *m_pClient;                            //�ͻ��˶���
};
 
#endif // !defined(AFX_STREAMCLINET_H__76C17349_CEFD_4D5B_9660_15071BAEADE6__INCLUDED_)
