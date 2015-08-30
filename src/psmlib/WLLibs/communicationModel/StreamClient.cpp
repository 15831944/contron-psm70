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
功能：构造函数，初始化成员
参数：无
返回：无
作者：叶文林	2012-09-24
*********************************************************************/
CStreamClient::CStreamClient()
{
	memset(m_recvBuf,0,MAX_CMD_LEN*10);                  //收数据缓冲，将收到的码流存于此，然后分析出协议包，并丢充干扰数据（为最长10条命令包的长度，因此支持10条命令一次性到达）
	m_recvbuflen = 0;                                    //缓冲区当前缓冲字节数

	m_pSocket = 0;                                       //通信接口(不是 TCP/IP的socket，不要弄混)，可能是UDP、TCP、串口或其它
	memset(&m_RqtRspCache,0,sizeof(TRqtRspCache));       //请求(发)数据及响应(收)数据线程通信缓冲，需要与下面临界区一起使用

	//win32平台
	m_pCSSend = new CWLCriticalSection();                //调用m_pSocket send数据及操作m_RqtRspCache的临界区
//	m_hEventRsp = CreateEvent(NULL, false, false, NULL); //收到请求响应的事件，当响应(收)数据线程收到相关数据时，通知发送线程（在sendWaitRsp中）有响应到达了
	m_pEvent = new CWLEvent(false, false);
	m_pCSRqtRsp = new CWLCriticalSection();              //sendRqtRecvRsp 函数临界区，保证 此函数线程安全性

	//QT平台
	//暂时未定义

	m_pRecvThread = new CThdClientRecv(this);               //接收数据线程类，暂不运行 

	//m_pFuncProMemoteNotify = NULL;                       //处理c/d通信模型函数指针

	m_oneRecvMaxlen = MAX_CMD_LEN;                         //一次接收数据包的最大长度，recvThreadProc() 函数将用到
}

/*********************************************************************
功能：析构函数
参数：无
返回：无
注意：由于本函数将调用线程柝构函数，它采用TerminateThread结束线程，可能
      会不太好。所以调用者最好让线程函数返回后，再调用本函数
作者：叶文林	2012-09-24
*********************************************************************/
CStreamClient::~CStreamClient()
{
	delete m_pRecvThread;                                //本行代码可能会引起不稳定因素，因此柝构函数不要随便调用
	WLSleep(100);

	//win32平台
	delete m_pCSSend;                                    //调用m_pSocket send数据及操作m_RqtRspCache的临界区
//	CloseHandle(m_hEventRsp);                            //收到请求响应的事件，当响应(收)数据线程收到相关数据时，通知发送线程（在sendWaitRsp中）有响应到达了
	delete m_pEvent;
	delete m_pCSRqtRsp;

	//QT平台
	//暂时未定义	
}

/*********************************************************************
功能：设置通信接口
参数：pSocket，通信接口(不是 TCP/IP的socket，不要弄混)，可能是UDP、TCP、
串口或其它
返回：无
作者：叶文林	2012-09-20
*********************************************************************/
void CStreamClient::setCommunicationSocket(CCommunicationSocket * pSocket)
{
	m_pSocket = pSocket;
}

/*********************************************************************
功能：获取通信接口
参数：无
返回：通信接口指针
作者：叶文林	2012-09-20
*********************************************************************/
CCommunicationSocket * CStreamClient::getCommunicationSocket()
{
	return m_pSocket;
}
/*
/*********************************************************************
功能：设置处理通信模型c\d的回调函数。如果协议中不存在c\d模型，请不设置
参数：callbackFunc，回调函数指针
返回：无
作者：叶文林	2012-09-20
*********************************************************************/
/*void CStreamClient::setRemoteNotifyProc(TPFuncProcRemoteNotify * pCallbackFunc)
{
	
}
*/
/*********************************************************************
功能：运行收数据线程
参数：无
返回：无
作者：叶文林	2012-09-20
*********************************************************************/
int CStreamClient::runRecvThread()
{ 
	return m_pRecvThread->resume();
};    

/*********************************************************************
功能：发送请求模型，本函数多线程不完全安全(安全性：QT版 > win32版
      不安全因素在于，表现在等待回码时已释放发送临界区，这时其它线程可
	  调用本函数。如果能为本函数单独加一个临界区可以解决此问题)
参数：sendbuf，发送包指针
      sendbuflen，发送包长度
	  recvbuf，输出参数，存放接收到的数据包
	  recvbuflen，输入输出参数，输入：存放recvbuf缓冲长度；
	                            输出：存放接到的数据包长度
	  waitRecvMulSec，等待响应时间，单位 毫秒，默认值 400
返回：>=0，成功：为响应字节数
      < 0，失败：-1，未能进入本函数临界区
	             -2，发送数据失败
				 -3，等待响应超时
				 -4，等待响应失败
注意：本函数为win32版
      win32版缺陷：当发出请求包，在进入等待代码前，recv线程已收到响应
	               包，并且这时另一个线程发送了一个请求响应模型，且响
				   应包也在本函数进入等待代码前，将会出现前一个响应包
				   被覆盖
				   此缺陷发生概率极，而且如果有时间是可以修复的。
				   方法是：处理函数检查 请求响应缓冲是否有数据，如果有
				           先等待，待响应取出后，再存入。
				   在QT中，用等待条件本缺陷可以较好解决
作者：叶文林	2012-09-20
*********************************************************************/
int CStreamClient::sendRqtRecvRsp(BYTE * sendbuf,DWORD sendbuflen,BYTE * recvbuf,DWORD &recvbuflen,int waitRecvMulSec,int retryCount)
{
	//WLOutDebug("the time = %d",waitRecvMulSec);
	//m_pCSRqtRsp 为 本函数临界区
	//m_pCSSend 为 处理 m_RqtRspCache 临界区 
	//此处名字上有错误，导致代码很难懂，将来必须改


	//进入本函数独有临界区
	//BOOL bRet = m_pCSRqtRsp->tryEnter();
	//if (bRet == FALSE)	return -9;
	m_pCSRqtRsp->enter();

	int ret =0;

	if (m_pSocket == NULL || m_pCSSend == NULL)	
	{
		m_pCSRqtRsp->leave();
		return -8;  //必要变量未赋值
	}

	//进发送临界区
	m_pCSSend->enter();

	//重置响应事件
//	ResetEvent(m_hEventRsp);
	m_pEvent->event_reset();

	int retrySendCount = 0;
LAB_RETRY:
	//发送请求
	ret = m_pSocket->send(sendbuf,sendbuflen);
	if (ret <=0)	                                       //发数据失败，可能是网络断开等
	{
		//出发送临界区
		m_pCSSend->leave();
		//if (retrySendCount == 0)	m_pCSRqtRsp->leave();
		m_pCSRqtRsp->leave();
		return -2;
	}

	//设置请求、响应数据缓冲
	memset(&m_RqtRspCache,0,sizeof(TRqtRspCache));         //清空缓存
	memcpy(m_RqtRspCache.rqt,sendbuf,sendbuflen);
	m_RqtRspCache.rqtlen = sendbuflen;
	m_RqtRspCache.isExistRqt  = 1;
	//出发送临界区
	m_pCSSend->leave();

	//等待响应(procRecvCmd函数会通知本函数请求到达)
	//使线程阻塞在一个条件变量，等同于：阻塞等待信号量触发
	recvbuflen = 0;
	//printf("等着收回码了\n");
//	ret = WaitForSingleObject(m_hEventRsp,waitRecvMulSec); //等响应包
	ret = m_pEvent->event_timedwait(waitRecvMulSec);
//	if (ret == WAIT_TIMEOUT)
	if (ret == 1)
	{
		//离开本函数独有临界区
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
		//离开本函数独有临界区
		m_pCSRqtRsp->leave();
		return -4;
	}

	//取发送回的数据
	recvbuflen = m_RqtRspCache.rsplen ;
	memcpy(recvbuf,m_RqtRspCache.rsp ,recvbuflen);
	memset(&m_RqtRspCache,0,sizeof(TRqtRspCache));         //清空缓存

	//离开本函数独有临界区
	m_pCSRqtRsp->leave();

	return recvbuflen;
}

/*********************************************************************
功能：发送数据，本函数只会在通信模型：a/d中才会用到
参数：sendbuf，发送包指针
      sendbuflen，发送包长度
返回：成功发送字节数
作者：叶文林	2012-09-20
*********************************************************************/
int CStreamClient::send(BYTE * sendbuf,DWORD sendbuflen)
{
	int sendlen;

	if (m_pSocket == NULL || m_pCSSend == NULL)	return -1; //必要变量未赋值

	//进发送临界区
	m_pCSSend->enter();

	//发送数据
	sendlen = m_pSocket->send(sendbuf,sendbuflen);

	//出发送临界区
	m_pCSSend->leave();

	return sendlen;
}

/*********************************************************************
功能：收数据线程执行函数，线程类将调用本函数
参数：无
返回：意义不大
作者：叶文林	2012-09-20
*********************************************************************/
int CStreamClient::recvThreadProc()
{
	int recvlen;
	BYTE recvBuf[MAX_CMD_LEN];
	DWORD lastRecvTick = 0;
	BYTE cmd[MAX_CMD_LEN] = {0};
	DWORD  cmdlen = 0;
	int ret;

	m_recvbuflen = 0; //清空报文缓冲
	while (1)
	{
		//判断当前是否需要收数据
		//WLSleep(100000);
		//continue;
		
		//WLOutDebug("the m_pSocket is prepare................");
//		int t_ret = m_pSocket->send((void*)"12121212",6);
		//收数据
		recvlen = m_pSocket->recv(recvBuf,m_oneRecvMaxlen);//MAX_CMD_LEN

		//WLOutDebug("the m_pSocket is XXXXX");
		if (recvlen <0)
		{
			//收数据错误，可能是链接断开。线程休眠一段时间
			//WLOutDebug("收数据错误\n");
#ifdef WL_OS_CORE_WIN
			WLOutDebug("UDP 客户端 收数据错误，recvlen = %d,errCode = %d \n",recvlen,GetLastError());
#elif defined WL_OS_CORE_LINUX
			WLOutDebug("UDP 客户端 收数据错误，recvlen = %d,errno=%d \n", recvlen, errno);
#endif
			WLSleep(1000);
			
			continue;
		}
		
		if (m_recvbuflen + recvlen >= MAX_CMD_LEN * 10)	m_recvbuflen = 0; //缓冲满，丢弃前面所有缓存

		//将收到的数据包放入收数据大缓冲之中，便于后面的协议分析
		if (WLGetTickCount() - lastRecvTick >500) m_recvbuflen = 0; //判断两次收数据时间是否过大，过大说明：
		//当前收到的数据一定是一个新报文，与缓冲区存的不完整报文
		//没有关系，不完整报文已无法收完，其后半部份已丢失。
		lastRecvTick = WLGetTickCount();
		memcpy(&m_recvBuf[m_recvbuflen],recvBuf,recvlen);
		m_recvbuflen += recvlen;
		
		//分析数据,处理数据
		memset(cmd,0,MAX_CMD_LEN);
		ret = analyseProtocol(cmd,cmdlen);
		while (ret >=0)
		{
			//处理命令包
			procRecvCmd(cmd,cmdlen);

			//如果缓冲区中还可能有完整命令包，接着分析
			memset(cmd,0,MAX_CMD_LEN);
			if (ret > 0)	ret = analyseProtocol(cmd,cmdlen);
			else break;
		}
	}
}

/*********************************************************************
功能：处理接收到的命令。处理方法如下：
      (1) 先调用 pretreatmentRecvCmd 预处理命令返回1，退出
	  (2) 调用 isMatchRqtRsp 判断命令是否为“请求响应”模型的响应命令，
	      如果是处理完退出
	  (3) 当普通响应命令处理
参数：pCmd，命令的指针
      outlen，命令的长度
返回：意义不大
注意：将本函数为win32版
作者：叶文林	2012-09-20
*********************************************************************/
int CStreamClient::procRecvCmd(BYTE *pCmd,DWORD cmdlen)
{
	int ret = 0;
	
	//进入发送临界区
	m_pCSSend->enter();

	//预处理命令
	ret = pretreatmentRecvCmd(pCmd,cmdlen);
	if (ret == 1)	
	{
		//出临发送界区
		m_pCSSend->leave();
		return ret;
	}

	//处理通信模型b：请求响应模型
	ret = isMatchRqtRsp(m_RqtRspCache.rqt,m_RqtRspCache.rqtlen,pCmd,cmdlen);
	if (ret == TRUE) 
	{
		//将响应存入缓冲区，更改缓冲状态，通知请求线程收到响应包了
		m_RqtRspCache.isExistRqt  = 0;
		//printf("set m_RqtRspCache.sendbufFlag = 0\n");
		m_RqtRspCache.rsplen  = cmdlen;
		memcpy(m_RqtRspCache.rsp ,pCmd,cmdlen);
		//for (int i =0;i<10;i++) Sleep(10);
//		ret = SetEvent(m_hEventRsp);
		ret = m_pEvent->event_set();

		//出发送临界区
		m_pCSSend->leave();
		return ret;
	}

	//处理通信模型c/d：响应式、响应请求式
	//if (m_pFuncProMemoteNotify != NULL)	(*m_pFuncProMemoteNotify)(pCmd,cmdlen);
	procRemoteNotify(pCmd,cmdlen);

	//出发送临界区
	m_pCSSend->leave();

	return 0;
}

/*********************************************************************
功能：根据缓冲m_recvBuf，分析出命令包
参数：out，输出参数，分析出一条命令的指针，内存空间由外部分配
      outlen，输出参数，分析出一条命令的长度
返回：>=0，成功，其中 =0 表示缓冲区中已不可能存在一条完整命令；
                 其中 >0 表示缓冲区中可能还有完整命令，调用者可重复调
				 用本函数，分析出余下命令，直到返回值<=0为止
      <0，失败
注意：重载本函数分析出正确报文后，切记要将更改“DWORD m_recvbuflen;//缓冲区当前缓冲字节数”
作者：叶文林	2012-09-20
*********************************************************************/
int CStreamClient::analyseProtocol(BYTE *out,DWORD &outlen)
{
	outlen = (m_recvbuflen > MAX_CMD_LEN) ? MAX_CMD_LEN : m_recvbuflen;
	memcpy(out,m_recvBuf,outlen);
	m_recvbuflen = 0;
	return 0;
}

/*********************************************************************
功能：预处理响应包(接收到的数据包)，procRecvCmd函数在处理响应包之前会
      先调用本函数。本函数返回1时，procRecvCmd函数放弃对响应包进一步
	  处理。
	  子类重载本函数，可实现处理心跳等特殊包，或修改响应包
参数：pCmd，命令的指针
      outlen，命令的长度
返回：1，请求调用者(procRecvCmd函数)中止进一步处理;非1，暂无意义
作者：叶文林	2012-09-20
*********************************************************************/
int CStreamClient::pretreatmentRecvCmd(BYTE *pCmd,DWORD cmdlen)
{
	return 0;
}

/*********************************************************************
功能：验证请求与响应是否匹配
参数：pRqt，请求命令指针
      pRsp，响应命令指针
返回：1，匹配
      0，不匹配
作者：叶文林	2012-09-20
*********************************************************************/
BOOL CStreamClient::isMatchRqtRsp(BYTE *pRqt,DWORD rqtlen,BYTE *pRsp,DWORD rsplen)
{
	return 0;
}

/*********************************************************************
功能：处理对方主动发起的命令，对应c、d两种通信模型
参数：pCmd,命令指针
      cmdlen，命令长度
返回：无
作者：叶文林	2012-09-25
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
	WLOutDebug("线程退出了，线程ID = %.8X,handle = %.8X",m_threadID,m_hThread);
#elif defined WL_OS_CORE_LINUX
//	WLOutDebug("线程退出了，线程ID = %.8X,handle = %.8X",m_threadID,m_hThread);
#endif
	return ret;
}

