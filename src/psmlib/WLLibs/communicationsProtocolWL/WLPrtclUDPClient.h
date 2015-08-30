// WLPrtclUDPClient.h: interface for the CWLPrtclUDPClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WLPRTCLUDPCLIENT_H__44AA90FF_165D_4C77_9701_F7963BA944A5__INCLUDED_)
#define AFX_WLPRTCLUDPCLIENT_H__44AA90FF_165D_4C77_9701_F7963BA944A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../communicationModel/StreamClient.h"
#include "WLProtocolDataType.h"
#include "../communicationModel/UDPSocket.h"

class CWLPrtclUDPClient : public CStreamClient  
{
public:
	CWLPrtclUDPClient(int localID,const char * remoteIP,WORD remotePort,WORD localPort = 0);
	virtual ~CWLPrtclUDPClient();

	int getLocalID()            {return m_localID;}            //��ȡ����Ψһ��־

	WORD commDetect();
	WORD getSvrTime(TWLTime * pTime);
protected:
	virtual int  analyseProtocol(BYTE *out,DWORD &outlen);                        //��m_recvBuf���У�����������
	virtual int  pretreatmentRecvCmd(BYTE *pCmd,DWORD cmdlen);                    //Ԥ�����������1�����ߵ�����(procRecvCmd)���������������
	virtual BOOL isMatchRqtRsp(BYTE *pRqt,DWORD rqtlen, BYTE *pRsp,DWORD rsplen); //�ж���������Ӧ�����Ƿ�ƥ��
	virtual void procRemoteNotify(BYTE *pCmd,DWORD cmdlen);                       //����Է��������������

	int responseCmd(TRspCmdIn * pIn);  //�ظ�

	WORD cmd(TRqtCmdIn * pIn,BYTE * pOutData,int & outlen,int remoteID=0);
	void setCommunicationSocket(CCommunicationSocket * pSocket){}; //��ֹ�˺�������

//private:
protected:
	virtual void onRecvCommDetect(BYTE *pCmd,DWORD len);
	virtual void onRecvGetLocalTime(BYTE *pCmd,DWORD len);

	virtual WORD onRemoteRqtGetSvrTime(TWLTime * pTime);

//private:
protected:
	//WORD cmd(BYTE cla,BYTE ins,BYTE p1,BYTE p2,WORD sn,WORD lc,BYTE * pInData,BYTE * pOutData,int & outlen);//pOutData ָ�� ״̬�ֺ�һ���ֽ�

	int m_localID;                      //����Ψһ��ʶ���磺վ��
	int m_defTimeoutMilSec;             //��������Ĭ�ϵȴ���Ӧʱ�䣨��λ���룩
};

#endif // !defined(AFX_WLPRTCLUDPCLIENT_H__44AA90FF_165D_4C77_9701_F7963BA944A5__INCLUDED_)
