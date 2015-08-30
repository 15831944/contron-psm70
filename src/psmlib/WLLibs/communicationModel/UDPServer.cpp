// UDPServer.cpp: implementation of the CUDPServer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UDPServer.h"
#include <stdio.h>

#include "../platformLayer/WLPlatformLayer.h"
#include "../platformLayer/WLThread.h"
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

CUDPServer::CUDPServer(WORD localPort)
{
	m_pSocket  = new CUDPSocket(NULL,0,localPort);
	m_pThdRecv = new CThdUDPServerRecv(this);               //接收数据线程类，暂不运行 
}

CUDPServer::~CUDPServer()
{
	delete m_pSocket;
}

/*********************************************************************
功能：运行收数据线程
参数：无
返回：无
作者：叶文林	2012-09-20
*********************************************************************/
int CUDPServer::runRecvThread()
{ 
	return m_pThdRecv->resume();
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
作者：叶文林	2012-11-28
*********************************************************************/
int CUDPServer::analyseProtocol(BYTE *out,DWORD &outlen)
{
	return 0;
}

/*********************************************************************
功能：处理接收到的命令。
参数：pCmd，命令的指针
      len，命令的长度
	  pRemoteAddr，远程命令发送者
返回：意义不大
注意：将本函数为win32版
作者：叶文林	2012-11-28
*********************************************************************/
void CUDPServer::procRecvCmd(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr)
{

}

int CUDPServer::recvThreadProc()
{
	int recvlen;
	BYTE recvBuf[UDPSERVER_MAX_CMD_LEN];
	DWORD lastRecvTick = 0;
	BYTE cmd[UDPSERVER_MAX_CMD_LEN] = {0};
	DWORD  cmdlen = 0;
	int ret;
	struct sockaddr_in remoteAddr;

	m_recvbuflen = 0; //清空报文缓冲
	while (1)
	{
		//判断当前是否需要收数据
		//WLSleep(100000);
		//continue;

		//收数据
		recvlen = m_pSocket->recv(recvBuf,UDPSERVER_MAX_CMD_LEN,&remoteAddr);//
		if (recvlen <0)
		{
			//收数据错误，可能是链接断开。线程休眠一段时间
			//WLOutDebug("收数据错误\n");
#ifdef WL_OS_CORE_WIN
			WLOutDebug("UDP服务端 收数据错误，recvlen = %d,errCode = %d \n",recvlen,GetLastError());
#elif defined WL_OS_CORE_LINUX
#endif
			WLSleep(1000);

			continue;
		}

//		if (m_recvbuflen + recvlen >= UDPSERVER_MAX_CMD_LEN)	m_recvbuflen = 0; //缓冲满，丢弃前面所有缓存
		//缓冲满，丢弃前面所有缓存
		if (m_recvbuflen + recvlen >= UDPSERVER_MAX_CMD_LEN)	
		{
			m_recvbuflen = 0; 
			WLOutDebug("CUDPServer::recvThreadProc缓冲溢出，丢弃前面所有缓存");
		}

		//将收到的数据包放入收数据大缓冲之中，便于后面的协议分析
		if (WLGetTickCount() - lastRecvTick >500) m_recvbuflen = 0; //判断两次收数据时间是否过大，过大说明：
		                                                            //当前收到的数据一定是一个新报文，与缓冲区存的不完整报文
		                                                            //没有关系，不完整报文已无法收完，其后半部份已丢失。
		lastRecvTick = WLGetTickCount();
		memcpy(&m_recvBuf[m_recvbuflen],recvBuf,recvlen);
		m_recvbuflen += recvlen;

		//分析数据,处理数据
		memset(cmd,0,UDPSERVER_MAX_CMD_LEN);
		ret = analyseProtocol(cmd,cmdlen);
		while (ret >=0)
		{
			//处理命令包
			procRecvCmd(cmd,cmdlen,&remoteAddr);

			//如果缓冲区中还可能有完整命令包，接着分析
			memset(cmd,0,UDPSERVER_MAX_CMD_LEN);
			if (ret > 0)	ret = analyseProtocol(cmd,cmdlen);
			else break;
		}
	}
	return 0;
}


CThdUDPServerRecv::CThdUDPServerRecv(CUDPServer * pServer) : CWLThread(1)

{
	m_pServer = pServer;
}

int CThdUDPServerRecv::run()
{	
	int ret = m_pServer->recvThreadProc();
#ifdef WL_OS_CORE_WIN
	WLOutDebug("线程退出了，线程ID = %.8X,handle = %.8X",m_threadID,m_hThread);
#elif defined WL_OS_CORE_LINUX
#endif
	return ret;
}
