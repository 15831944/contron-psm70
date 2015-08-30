// StreamClinet.h: interface for the CStreamClient class.
// 定义:流客户端模型基类
// 应用场合：通信模型必须可以抽像为客户端和服务端有一条虚链路，解释如下：
//           (1) 客户端对应一个唯一的服务端
//           (2) 除了服务端之外客户端不能收到其它端点发送过来的数据
//           (3) 理论上如果采用UDP等不存在虚链路的情况通信是不适全本模型的，但是具体情况具本分析
// 对于不存在虚链路情况，满足以下条件任一种也可以使用：
// A:客户端只可能收到服务端数据
// B:客户端虽可收到多个端点的数据，但是可以保证每次发送过业的数据为一条或多条命令包，两条命令之间不存在拼包操作。
//   此情况注意重写 analyseProtocol
//   关于本点UDP等段式报文完全符合要求
// C:收数据时，可以判断服务端是否合法（如UDP方式验证对方IP地址及端口号），能丢弃不合法IP
// 流客户端特点如下：
// (1) 一条命令可能作为一个报文或多个报文发送接收，也可能一个报文对应
//     多条命令;
// (2) 需要实现一个协议分析函数：analyseProtocol，从流中分析出命令包;
// (3) 需要实现一个响应命令处理函数：procRspCmd，函数内将根据响应命令
//     类型(请求响应式，响应式)处理；
// (3) 单独开一个线程用于接收服务器数据；
// (4) 支持以下四种通信模式，定义：请求：发出报文；响应：收到报文
//     a. 请求响应式：主动发送请求报文，等待对方响应报文
//     b. 请求式：主动发送报文，但是对方不对报文作响应
//     c. 响应式：对方主动上报报文，本端不响应
//     d. 响应请求式：对方主动上报报文，本端响应
//     注意：a模型，接口为: sendRqtRecvRsp
//           b模型，接口为: send
//           c、d模型通过接口setRemoteNotifyProc设置回调函数 
// 注意：由于在QT中暂未找到能完美替换win32中事件的方法，与之相关代码将
//       提供Win32和QT两个版，相关代码为：
//       (1) 本类成员：接收数据线程的运行函数 m_pRecvThread->run();
//       (2) 本类成员：sendRqtRecvRsp();
//       (3) 本类成员：send(); 
//       (4) 本类成员属性：m_pCSSend，m_hEventRsp；
// 作者：叶文林	2012-09-19
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

#define MAX_CMD_LEN (4096)     //通信数据包最大长度

//定义：请求响应缓存结构
//目的：暂存一个配对的请求命令与响应命包，用于请求命令线程与接收数据线程同步
typedef struct  {
	BYTE  rqt[MAX_CMD_LEN];    //请求命令
	DWORD rqtlen;              //请求命令包的长度
	DWORD isExistRqt;          //本缓冲请求命令包是否有效（还在生命期）,收到响应包后将其设为0，请求线程copy响应包，其生命结束
	BYTE  rsp[MAX_CMD_LEN];    //响应命令
	DWORD rsplen;              //响应命令包的长度
}TRqtRspCache;//TRqtRspCache;

//定义函数指针类型：处理c.d通信模型响应数据包
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
	virtual int analyseProtocol(BYTE *out,DWORD &outlen);     //从m_recvBuf包中，分析出命令
	virtual int pretreatmentRecvCmd(BYTE *pCmd,DWORD cmdlen); //预处理命令，返回1，告诉调用者(procRecvCmd)命令无须继续处理
	virtual BOOL isMatchRqtRsp(BYTE *pRqt,DWORD rqtlen,
		                       BYTE *pRsp,DWORD rsplen);      //判断请求与响应命令是否匹配
	virtual void procRemoteNotify(BYTE *pCmd,DWORD cmdlen);   //处理对方主动发起的命令

	BYTE m_recvBuf[MAX_CMD_LEN*10];                      //收数据缓冲，将收到的码流存于此，然后分析出协议包，并丢充干扰数据（为最长10条命令包的长度，因此支持10条命令一次性到达）
	DWORD m_recvbuflen;                                  //缓冲区当前缓冲字节数

	CCommunicationSocket *m_pSocket;                     //通信接口(不是 TCP/IP的socket，不要弄混)，可能是UDP、TCP、串口或其它
	TRqtRspCache m_RqtRspCache;                          //请求(发)数据及响应(收)数据线程通信缓冲，需要与下面临界区一起使用

	//win32平台
	CWLCriticalSection *m_pCSSend;                       //调用m_pSocket send数据及操作m_RqtRspCache的临界区，
	                                                     //保证一次只有一个线程调用m_pSocket发送数据
	                                                     //在sendWaitRsp函数中将发送数据和操作m_RqtRspCache封装为一个原子
	CWLCriticalSection *m_pCSRqtRsp;                     //sendRqtRecvRsp 函数临界区，保证 此函数线程安全性
//	HANDLE m_hEventRsp;                                  //收到请求响应的事件，当响应(收)数据线程收到相关数据时，通知发送线程（在sendWaitRsp中）有响应到达了
	CWLEvent *m_pEvent;
	
	//QT平台
	//暂时未定义

	CWLThread * m_pRecvThread;                           //接收数据类  

	//DWORD m_rqtWaitRspTimeout;                           //请求和响应之间等待超时时间
	
	//TPFuncProcRemoteNotify *m_pFuncProMemoteNotify;      //处理c/d通信模型函数指针
	int m_oneRecvMaxlen;                                 //一次接收数据包的最大长度
public:
	int recvThreadProc();                                //接收线程调用的处理函数    
private:
	int procRecvCmd(BYTE *pCmd,DWORD cmdlen);            //处理接收到的命令，先处理特殊命令再根据响应命令类型(请求响应式，响应式)处理
	
};

class CThdClientRecv  : public CWLThread
{
public:
	CThdClientRecv(CStreamClient * pClient);
	//virtual ~CThdClientRecv();
private:
	virtual int run(); //线程函数
	CStreamClient *m_pClient;                            //客户端对象
};
 
#endif // !defined(AFX_STREAMCLINET_H__76C17349_CEFD_4D5B_9660_15071BAEADE6__INCLUDED_)
