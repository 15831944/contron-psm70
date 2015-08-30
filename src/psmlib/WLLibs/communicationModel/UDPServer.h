// UDPServer.h: interface for the CUDPServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPSERVER_H__B2079EEA_C10C_4D3C_A8BD_413D5E92FC77__INCLUDED_)
#define AFX_UDPSERVER_H__B2079EEA_C10C_4D3C_A8BD_413D5E92FC77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define UDPSERVER_MAX_CMD_LEN (4096)  //��������

#include "../platformLayer/WLDEF.h"
#include "../platformLayer/WLEvent.h"
#include "../platformLayer/WLThread.h"
#include "../platformLayer/WLPlatformLayer.h"
#include "../platformLayer/WLCriticalSection.h"
#include "CommunicationSocket.h"
#include "UDPSocket.h"

//���壺������Ӧ����ṹ������ַ��Ϣ
//Ŀ�ģ��ݴ�һ����Ե�������������Ӧ�������������������߳�����������߳�ͬ��
typedef struct _stRqtRspCacheWithAddr{
	BYTE  rqt[UDPSERVER_MAX_CMD_LEN];    //��������
	DWORD rqtlen;                        //����������ĳ���
	DWORD isExistRqt;                    //����������������Ƿ���Ч�����������ڣ�,�յ���Ӧ��������Ϊ0�������߳�copy��Ӧ��������������
	BYTE  rsp[UDPSERVER_MAX_CMD_LEN];    //��Ӧ����
	DWORD rsplen;                        //��Ӧ������ĳ���
	struct sockaddr_in addrSendTo;       //������Ϣ�Է���ַ
	struct sockaddr_in addrRecvFrom;     //������Ϣ�Է���ַ
}TRqtRspCacheWithAddr;

class CUDPSocket;
class CThdUDPServerRecv;
class CUDPServer  
{
public:
	CUDPServer(WORD localPort);
	virtual ~CUDPServer();

	int runRecvThread();
	friend class CThdUDPServerRecv;
protected:
	virtual int analyseProtocol(BYTE *out,DWORD &outlen);                               //��m_recvBuf���У�����������
	virtual void procRecvCmd(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr);    //�����յ��ı���

protected:
	CUDPSocket        * m_pSocket;            //ͨ�Žӿ�
	CThdUDPServerRecv * m_pThdRecv;           //�������߳�
protected:
	BYTE m_recvBuf[UDPSERVER_MAX_CMD_LEN];    //�����ݻ��壬���յ����������ڴˣ�Ȼ�������Э�����������������ݣ�Ϊ�10��������ĳ��ȣ����֧��10������һ���Ե��
	DWORD m_recvbuflen;                       //��������ǰ�����ֽ���

	int recvThreadProc();                     //�����̵߳��õĴ�����   
};

class CThdUDPServerRecv  : public CWLThread
{
public:
	CThdUDPServerRecv(CUDPServer * pServer);

private:
	virtual int run();              //�̺߳���
	CUDPServer *m_pServer;          //�ͻ��˶���
};


#endif // !defined(AFX_UDPSERVER_H__B2079EEA_C10C_4D3C_A8BD_413D5E92FC77__INCLUDED_)

