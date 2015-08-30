#pragma once
#include "../communicationModel/UDPServer.h"
#include "WLProtocolDataType.h"


class CWLPrtclUDPServer :public CUDPServer
{
public:
	CWLPrtclUDPServer(int localID,WORD localPort);
	virtual ~CWLPrtclUDPServer(void);

	//��������
	WORD commDetect(struct sockaddr_in * pRemoteAddr=NULL);
	WORD getRemoteTime(TWLTime * pTime, struct sockaddr_in * pRemoteAddr=NULL);

protected:
	virtual void procRecvCmd(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr);       //�����յ��ı���
	virtual int analyseProtocol(BYTE *out,DWORD &outlen);                                 //��m_recvBuf���У�����������
	virtual int procRemoteResponse(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr);//����Զ�̻ظ�����Ϣ
	//�ж���������Ӧ�����Ƿ�ƥ��
	virtual BOOL isMatchRqtRspWithAddr(BYTE *pRqt,DWORD rqtlen, struct sockaddr_in * pRemoteAddrRqt,BYTE *pRsp,DWORD rsplen,struct sockaddr_in * pRemoteAddrRsp);
	//��װ��Ϣ���Ͳ����ջظ���Ϣ
	WORD cmd(TRqtCmdIn * pIn,BYTE * pOutData,int & outlen,int remoteID=0, struct sockaddr_in * pRemoteAddr=NULL);
	//��װ�����ͻظ���Ϣ
	int  responseCmd(TRspCmdIn * pIn,struct sockaddr_in * pRemoteAddr); 
	//������Ϣ�����ջظ���Ϣ
	int sendRqtRecvRsp(BYTE * sendbuf, DWORD sendbuflen, BYTE * recvbuf, DWORD &recvbuflen, struct sockaddr_in * pRemoteAddr=NULL, int waitRecvMulSec=2000,int retryCount=3);


	//����Է�����
	virtual void onRecvCommDetect(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr);
	virtual void onRecvGetSvrTime(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr);

	virtual WORD onRemoteRqtGetSvrTime(TWLTime * pTime);

protected:
	TRqtRspCacheWithAddr m_RqtRspCache;       //����(��)���ݼ���Ӧ(��)�����߳�ͨ�Ż��壬��Ҫ�������ٽ���һ��ʹ��

private:
	DWORD m_localID;                          //����Ψһ��ʶ
	int m_defTimeoutMilSec;                   //��������Ĭ�ϵȴ���Ӧʱ�䣨��λ���룩

	CWLCriticalSection *m_pCSRqtRsp;          //sendRqtRecvRsp �����ٽ�������֤ �˺����̰߳�ȫ��
	CWLEvent           *m_pEvent;             //�¼�
	CWLCriticalSection *m_pCSSend;            //����m_pSocket send���ݼ�����m_RqtRspCache���ٽ�����
	                                          //��֤һ��ֻ��һ���̵߳���m_pSocket��������
	                                          //��sendWaitRsp�����н��������ݺͲ���m_RqtRspCache��װΪһ��ԭ��

};



/*
���ܣ�CWLPrtclUDPServer���ڲ��������󲢽��շ��ش����߳�
˵����CWLPrtclUDPServer�з��͸�������ͬһ�߳��п�����ɶ������޷����յ���Ϣ����ΪsendRqtRecvRsp�����ȴ�ʱ������
      ���ཫsendRqtRecvRsp��CWLPrtclUDPServer���̷߳���ȴ�����ֹ����
*/
//class CThdUDPServerReqRsp : public CWLThread
//{
//public:
//	CThdUDPServerReqRsp(CWLPrtclUDPServer * pServer);
//	~CThdUDPServerReqRsp();
//
//private:
//	virtual int run();
//	CWLPrtclUDPServer * m_pServer;
//};