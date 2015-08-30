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

	int getLocalID()            {return m_localID;}            //获取本地唯一标志

	WORD commDetect();
	WORD getSvrTime(TWLTime * pTime);
protected:
	virtual int  analyseProtocol(BYTE *out,DWORD &outlen);                        //从m_recvBuf包中，分析出命令
	virtual int  pretreatmentRecvCmd(BYTE *pCmd,DWORD cmdlen);                    //预处理命令，返回1，告诉调用者(procRecvCmd)命令无须继续处理
	virtual BOOL isMatchRqtRsp(BYTE *pRqt,DWORD rqtlen, BYTE *pRsp,DWORD rsplen); //判断请求与响应命令是否匹配
	virtual void procRemoteNotify(BYTE *pCmd,DWORD cmdlen);                       //处理对方主动发起的命令

	int responseCmd(TRspCmdIn * pIn);  //回复

	WORD cmd(TRqtCmdIn * pIn,BYTE * pOutData,int & outlen,int remoteID=0);
	void setCommunicationSocket(CCommunicationSocket * pSocket){}; //禁止此函数调用

//private:
protected:
	virtual void onRecvCommDetect(BYTE *pCmd,DWORD len);
	virtual void onRecvGetLocalTime(BYTE *pCmd,DWORD len);

	virtual WORD onRemoteRqtGetSvrTime(TWLTime * pTime);

//private:
protected:
	//WORD cmd(BYTE cla,BYTE ins,BYTE p1,BYTE p2,WORD sn,WORD lc,BYTE * pInData,BYTE * pOutData,int & outlen);//pOutData 指向 状态字后一个字节

	int m_localID;                      //本地唯一标识，如：站号
	int m_defTimeoutMilSec;             //发送请求默认等待响应时间（单位毫秒）
};

#endif // !defined(AFX_WLPRTCLUDPCLIENT_H__44AA90FF_165D_4C77_9701_F7963BA944A5__INCLUDED_)
