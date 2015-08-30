#pragma once
#include "../communicationModel/UDPServer.h"
#include "WLProtocolDataType.h"


class CWLPrtclUDPServer :public CUDPServer
{
public:
	CWLPrtclUDPServer(int localID,WORD localPort);
	virtual ~CWLPrtclUDPServer(void);

	//主动请求
	WORD commDetect(struct sockaddr_in * pRemoteAddr=NULL);
	WORD getRemoteTime(TWLTime * pTime, struct sockaddr_in * pRemoteAddr=NULL);

protected:
	virtual void procRecvCmd(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr);       //处理收到的报文
	virtual int analyseProtocol(BYTE *out,DWORD &outlen);                                 //从m_recvBuf包中，分析出命令
	virtual int procRemoteResponse(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr);//处理远程回复的消息
	//判断请求与响应命令是否匹配
	virtual BOOL isMatchRqtRspWithAddr(BYTE *pRqt,DWORD rqtlen, struct sockaddr_in * pRemoteAddrRqt,BYTE *pRsp,DWORD rsplen,struct sockaddr_in * pRemoteAddrRsp);
	//组装消息发送并接收回复消息
	WORD cmd(TRqtCmdIn * pIn,BYTE * pOutData,int & outlen,int remoteID=0, struct sockaddr_in * pRemoteAddr=NULL);
	//组装并发送回复消息
	int  responseCmd(TRspCmdIn * pIn,struct sockaddr_in * pRemoteAddr); 
	//发送消息并接收回复消息
	int sendRqtRecvRsp(BYTE * sendbuf, DWORD sendbuflen, BYTE * recvbuf, DWORD &recvbuflen, struct sockaddr_in * pRemoteAddr=NULL, int waitRecvMulSec=2000,int retryCount=3);


	//处理对方请求
	virtual void onRecvCommDetect(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr);
	virtual void onRecvGetSvrTime(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr);

	virtual WORD onRemoteRqtGetSvrTime(TWLTime * pTime);

protected:
	TRqtRspCacheWithAddr m_RqtRspCache;       //请求(发)数据及响应(收)数据线程通信缓冲，需要与下面临界区一起使用

private:
	DWORD m_localID;                          //本地唯一标识
	int m_defTimeoutMilSec;                   //发送请求默认等待响应时间（单位毫秒）

	CWLCriticalSection *m_pCSRqtRsp;          //sendRqtRecvRsp 函数临界区，保证 此函数线程安全性
	CWLEvent           *m_pEvent;             //事件
	CWLCriticalSection *m_pCSSend;            //调用m_pSocket send数据及操作m_RqtRspCache的临界区，
	                                          //保证一次只有一个线程调用m_pSocket发送数据
	                                          //在sendWaitRsp函数中将发送数据和操作m_RqtRspCache封装为一个原子

};



/*
功能：CWLPrtclUDPServer类内部发送请求并接收返回处理线程
说明：CWLPrtclUDPServer中发送跟接收在同一线程中可能造成堵塞而无法接收到消息，因为sendRqtRecvRsp方法等待时阻塞，
      本类将sendRqtRecvRsp与CWLPrtclUDPServer中线程分离等待，防止堵塞
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