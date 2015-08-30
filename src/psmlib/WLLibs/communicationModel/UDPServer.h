// UDPServer.h: interface for the CUDPServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPSERVER_H__B2079EEA_C10C_4D3C_A8BD_413D5E92FC77__INCLUDED_)
#define AFX_UDPSERVER_H__B2079EEA_C10C_4D3C_A8BD_413D5E92FC77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define UDPSERVER_MAX_CMD_LEN (4096)  //最大命令长度

#include "../platformLayer/WLDEF.h"
#include "../platformLayer/WLEvent.h"
#include "../platformLayer/WLThread.h"
#include "../platformLayer/WLPlatformLayer.h"
#include "../platformLayer/WLCriticalSection.h"
#include "CommunicationSocket.h"
#include "UDPSocket.h"

//定义：请求响应缓存结构，带地址信息
//目的：暂存一个配对的请求命令与响应命包，用于请求命令线程与接收数据线程同步
typedef struct _stRqtRspCacheWithAddr{
	BYTE  rqt[UDPSERVER_MAX_CMD_LEN];    //请求命令
	DWORD rqtlen;                        //请求命令包的长度
	DWORD isExistRqt;                    //本缓冲请求命令包是否有效（还在生命期）,收到响应包后将其设为0，请求线程copy响应包，其生命结束
	BYTE  rsp[UDPSERVER_MAX_CMD_LEN];    //响应命令
	DWORD rsplen;                        //响应命令包的长度
	struct sockaddr_in addrSendTo;       //发送消息对方地址
	struct sockaddr_in addrRecvFrom;     //接收消息对方地址
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
	virtual int analyseProtocol(BYTE *out,DWORD &outlen);                               //从m_recvBuf包中，分析出命令
	virtual void procRecvCmd(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr);    //处理收到的报文

protected:
	CUDPSocket        * m_pSocket;            //通信接口
	CThdUDPServerRecv * m_pThdRecv;           //收数据线程
protected:
	BYTE m_recvBuf[UDPSERVER_MAX_CMD_LEN];    //收数据缓冲，将收到的码流存于此，然后分析出协议包，并丢充干扰数据（为最长10条命令包的长度，因此支持10条命令一次性到达）
	DWORD m_recvbuflen;                       //缓冲区当前缓冲字节数

	int recvThreadProc();                     //接收线程调用的处理函数   
};

class CThdUDPServerRecv  : public CWLThread
{
public:
	CThdUDPServerRecv(CUDPServer * pServer);

private:
	virtual int run();              //线程函数
	CUDPServer *m_pServer;          //客户端对象
};


#endif // !defined(AFX_UDPSERVER_H__B2079EEA_C10C_4D3C_A8BD_413D5E92FC77__INCLUDED_)

