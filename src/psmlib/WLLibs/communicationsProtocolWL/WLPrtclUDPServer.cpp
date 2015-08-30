#include "stdafx.h"
#include "WLPrtclUDPServer.h"
#include "../communicationModel/CRC8.h"
#include "../communicationModel/UDPSocket.h"

#ifdef WL_IDE_VC6
	#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[]=__FILE__;
	#define new DEBUG_NEW
	#endif
#endif


/*********************************************************************
功能：构造函数
参数：localID，本地站ID号
	  localPort，本地服务端口；
返回：无
作者：叶文林	
备注：刘远安注释 2013-01-18
*********************************************************************/
CWLPrtclUDPServer::CWLPrtclUDPServer(int localID,WORD localPort) : CUDPServer(localPort)
{
	m_localID = localID;
	m_defTimeoutMilSec = 2000;

	m_pCSRqtRsp = new CWLCriticalSection();                    //sendRqtRecvRsp 函数临界区，保证 此函数线程安全性
	m_pEvent    = new CWLEvent(false, false);
	m_pCSSend = new CWLCriticalSection();                      //调用m_pSocket send数据及操作m_RqtRspCache的临界区
	memset(&m_RqtRspCache,0x00,sizeof(TRqtRspCacheWithAddr));  //请求(发)数据及响应(收)数据线程通信缓冲，需要与下面临界区一起使用
}

CWLPrtclUDPServer::~CWLPrtclUDPServer(void)
{
	delete m_pCSRqtRsp;
	delete m_pEvent;
	delete m_pCSSend;                                    //调用m_pSocket send数据及操作m_RqtRspCache的临界区
}


/*********************************************************************
功能：主动消息 发送心跳
参数：
返回：参考SW_XXX
作者：唐小灿	2014-07-29
*********************************************************************/
WORD CWLPrtclUDPServer::commDetect(struct sockaddr_in * pRemoteAddr)
{
	TRqtCmdIn cmdin = {CLA_RQT,INS_COMM_DETECT,0,0,0,0,0};
	int outlen = 0;
	WORD sw = 0;

	if (NULL == pRemoteAddr)
	{
		return SW_UNKNOWN_ERR;  //非测试情况下使用
	}

	sw = cmd(&cmdin,NULL,outlen,0, pRemoteAddr);
	if (SW_SUCCESS != sw)
	{
		WLOutDebug("服务端向客户端发送心跳失败");
	}
	return sw;
}


/*********************************************************************
功能：主动消息 获取远程时间
参数：
返回：参考SW_XXX
作者：唐小灿	2014-07-29
*********************************************************************/
WORD CWLPrtclUDPServer::getRemoteTime(TWLTime * pTime, struct sockaddr_in * pRemoteAddr)
{
	if (pTime == NULL)	return SW_INPUT_PARAM_ERR;

	TRqtCmdIn cmdin = {CLA_RQT,INS_GET_SVR_TIME,0,0,0,0,0};
	int outlen = 0;
	//WORD sw = cmd(&cmdin,(BYTE *)pTime,outlen,0, pRemoteAddr);
	//return sw;

	//测试
	if (NULL != pRemoteAddr)
	{
		WORD sw = cmd(&cmdin,(BYTE *)pTime,outlen,0, pRemoteAddr);
		if (SW_SUCCESS != sw)
		{
			WLOutDebug("服务端向客户端获取时间失败");
		}
		return sw;
	}
	else
	{
		//非测试情况下使用
		return SW_UNKNOWN_ERR;
	}
}


/*********************************************************************
功能：处理命令发送消息并接收反馈消息
参数：
返回：TRUE，匹配
      FALSE，不匹配
作者：唐小灿	2014-07-29
*********************************************************************/
WORD CWLPrtclUDPServer::cmd(TRqtCmdIn * pIn,BYTE * pOutData,int & outlen,int remoteID, struct sockaddr_in * pRemoteAddr)
{
	BYTE sendbuf[UDPSERVER_MAX_CMD_LEN];
	BYTE recvbuf[UDPSERVER_MAX_CMD_LEN];

	TAPDUHeader * pHeader = NULL; //报文头
	BYTE        * pBody   = NULL; //报文体
	TAPDUTailer   tailer  = {0};  //报文尾 
	WORD          sw;             //响应状态字

	static WORD sn = 0;
	WORD snCur = sn;              //用于保存本次sn，多线程有漏洞，在snCur赋值前有线程修改sn的值则不正确
	sn = (sn+1)%0xFFFF;
	snCur = sn;
	memset(sendbuf,0x00,sizeof(sendbuf));
	pHeader = (TAPDUHeader * )sendbuf;
	//填充协议头
	pHeader->syncheader = SYNC_HEADER;
	pHeader->cla        = pIn->cla;
	pHeader->ins        = pIn->ins;
	pHeader->p1         = pIn->p1;
	pHeader->p2         = pIn->p2;
	pHeader->sn         = snCur;//pIn->sn;
	pHeader->id         = m_localID;
	pHeader->remoteID   = remoteID;
	pHeader->lc         = pIn->lc;

	//填充协议体
	pBody = sendbuf + sizeof(TAPDUHeader);
	memcpy(pBody,pIn->pData,pHeader->lc);

	//填充协议尾
	int index        = sizeof(TAPDUHeader)+pHeader->lc; //报文尾位置
	tailer.crc       = CalcCRC8(sendbuf,index);
	tailer.synctailer = SYNC_TAILER;
	memcpy(sendbuf + index,&tailer,sizeof(TAPDUTailer));

	DWORD sendlen    = sizeof(TAPDUHeader) + pHeader->lc + sizeof(TAPDUTailer);
	DWORD recvlen    = sizeof(recvbuf);

	int ret = 0;
	int loop = 0;
LAB_LOOP:
	ret = sendRqtRecvRsp(sendbuf,sendlen,recvbuf,recvlen,pRemoteAddr,m_defTimeoutMilSec);
	if (ret <= 0)
	{
		//WLOutDebug("发送报文失败,错误码 = %d",ret);
		WLOutDebug("发送报文失败,错误码 = %d,cla = 0x%.2X, ins = 0x%.2X",ret,pHeader->cla,pHeader->ins);
		return SW_NO_RSP;
	}

	//处理响应包
	pHeader = (TAPDUHeader *)recvbuf;
	if (pHeader->sn != snCur)  //非所请求的回复，重新请求
	{
		loop++;
		if (loop > 3)
		{
			return SW_UNKNOWN_ERR;  //未获得请求的内容
		}
		WLOutDebug("重复请求 sn=%d,loop=%d",pHeader->sn,loop);
		goto LAB_LOOP;
	}
	pBody = recvbuf + sizeof(TAPDUHeader);
	memcpy(&sw,pBody,SW_LEN);               //取状态字
	if (pHeader->lc > SW_LEN)               //取状态字之外的响应命令报文体
	{
		//pOutData = pBody + SW_LEN;
		/**********************************************************************
		修改：更新以下代码导致的内存溢出现象
		作者：叶文林	2013-03-14
		方试：新增代码
		**********************************************************************/
		outlen   = pHeader->lc - SW_LEN;
		if (pOutData != NULL)	memcpy(pOutData,pBody + SW_LEN,outlen);
		//*******************************************************************
		//以下代码有误，会有内存溢出现象，是因为将响应码和输出数据都copy入了
		//输出数据区了
		/**********************************************************************
		if (pOutData != NULL)	memcpy(pOutData,pBody + SW_LEN,pHeader->lc);
		outlen   = pHeader->lc - SW_LEN;
		**********************************************************************/
	}
	else
	{
		pOutData = NULL;
		outlen   = 0;
	}

	//填充输入头
	pIn->cla = pHeader->cla;
	pIn->ins = pHeader->ins;
	pIn->p1  = pHeader->p1;
	pIn->p2  = pHeader->p2;
	pIn->sn  = pHeader->sn;
	pIn->lc  = pHeader->lc;
	pIn->pData = pOutData;

	return sw;
}

void CWLPrtclUDPServer::procRecvCmd(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr)             //处理收到的报文
{
	int ret = 0;
	TAPDUHeader * pHeader  = (TAPDUHeader * )pCmd;
	//if ((pHeader->cla & CLA_RQT) ==  CLA_RQT)      //客户端请求服务端响应包
	if (pHeader->cla ==  CLA_RQT)      //客户端请求服务端响应包(CLA_RQT值为0，(pHeader->cla&CLA_RQT)==CLA_RQT一直成立，所以改为直接匹配)
	{
		char ip[20] = {0};
		WORD port = 0;
		CUDPSocket::getSockAddr(ip,sizeof(ip),port,pRemoteAddr);

		//上位机 <--> 下位机
		if	    (INS_COMM_DETECT == pHeader->ins)          //通信检测，用于心跳
			onRecvCommDetect(pCmd,len,pRemoteAddr);
		else if (INS_GET_SVR_TIME == pHeader->ins)         //处理消息 获取服务器时间
			onRecvGetSvrTime(pCmd,len,pRemoteAddr);
		else 
			WLOutDebug("未实现报文");
	}
	else if ((pHeader->cla & CLA_RSP) ==  CLA_RSP) //客户端响应服务端请求包
	{
		//处理主动请求所得到的回复
		ret = procRemoteResponse(pCmd, len, pRemoteAddr);
		//return ret;
	}
	else
	{
		//未知类型
	}
}

int CWLPrtclUDPServer::analyseProtocol(BYTE *out,DWORD &outlen)                             //从m_recvBuf包中，分析出命令
{
	//因为确定为UDP报文，确定一次只发一条报文，报文间有边界，无需（也不能，如：有多个客户端）拼包，故只做最简单的检查

	TAPDUHeader * pHeader = NULL;
	TAPDUTailer * pTailer = NULL;
	WORD crc =0;
	int index = 0,cmdlen =0;
    BYTE tailer[sizeof(TAPDUTailer)] = {0};
	outlen = NULL;
LAB_HEAD:
	//检查数据长度是满足要求
	if (m_recvbuflen < sizeof(TAPDUHeader) + sizeof(TAPDUTailer))
	{
		m_recvbuflen = 0;
		return -1;            //失败(不能与下一条报文拼接，因为下条报文可能来自另一个客户端)
	}

	//检查同步头是否合法
	pHeader = (TAPDUHeader *) m_recvBuf;
	if (pHeader->syncheader != SYNC_HEADER)	goto LAB_JUMP_TO_HEAD;

	//检查长度是否合法
	cmdlen = pHeader->lc + sizeof(TAPDUHeader) + sizeof(TAPDUTailer);
	if (m_recvbuflen < cmdlen)	goto LAB_JUMP_TO_HEAD;

	//检查同步尾是否合法
	index = pHeader->lc + sizeof(TAPDUHeader);
	memcpy(tailer,&m_recvBuf[index],sizeof(TAPDUTailer));
	pTailer = (TAPDUTailer *)tailer;
	if (pTailer->synctailer != SYNC_TAILER)	goto LAB_JUMP_TO_HEAD;

	//检查CRC是否合法
	crc = CalcCRC8(m_recvBuf,index);
	if (pTailer->crc != crc)	goto LAB_JUMP_TO_HEAD;

	//成功验证出一条报文
	m_recvbuflen = m_recvbuflen - cmdlen;
	outlen = cmdlen;
	memcpy(out,m_recvBuf,outlen);
	if (m_recvbuflen <= sizeof(TAPDUHeader) + sizeof(TAPDUTailer))	return 0;	else	return 1;  

LAB_JUMP_TO_HEAD:
	memcpy(&m_recvBuf[0],&m_recvBuf[1],m_recvbuflen-1);
	m_recvbuflen--;
	goto LAB_HEAD;
}


/*********************************************************************
功能：处理远程回复的消息
参数：pCmd，回复命令
      len，回复命令长度
	  pRemoteAddr，回复命令方地址信息
返回：>=0，正确处理
      <0， 失败
作者：唐小灿	2014-07-30
*********************************************************************/
int CWLPrtclUDPServer::procRemoteResponse(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr)
{
	int ret = 0;

	//处理主动请求所得到的回复
	BOOL isMatch = isMatchRqtRspWithAddr(m_RqtRspCache.rqt, m_RqtRspCache.rqtlen, &m_RqtRspCache.addrSendTo, pCmd, len, pRemoteAddr);
	if (TRUE == isMatch)
	{
		//将响应存入缓冲区，更改缓冲状态，通知请求线程收到响应包了
		m_RqtRspCache.isExistRqt  = 0;
		m_RqtRspCache.rsplen      = len;
		memcpy(m_RqtRspCache.rsp ,pCmd,len);
		ret = m_pEvent->event_set();

		return ret;
	}

	return -1;
}


/*********************************************************************
功能：验证请求与响应是否匹配
参数：pRsp，回复命令
      rsplen，回复命令长度
	  pRemoteAddr，回复命令方地址信息
返回：TRUE，匹配
      FALSE，不匹配
作者：唐小灿	2014-07-28
*********************************************************************/
BOOL CWLPrtclUDPServer::isMatchRqtRspWithAddr(BYTE *pRqt,DWORD rqtlen, struct sockaddr_in * pRemoteAddrRqt,BYTE *pRsp,DWORD rsplen,struct sockaddr_in * pRemoteAddrRsp)
{
	if ((NULL == pRqt) || (NULL == pRsp) || (NULL == pRemoteAddrRqt) || (NULL == pRemoteAddrRsp))  return FALSE;
	if ((rqtlen < sizeof(TAPDUHeader)) || (rsplen < sizeof(TAPDUHeader)))  return FALSE;

	static TAPDUHeader * pRqtHeader = NULL;
	static TAPDUHeader * pRspHeader = NULL;
	pRqtHeader = (TAPDUHeader *)pRqt;
	pRspHeader = (TAPDUHeader *)pRsp;

	//快速判断是否是回复
	//if ((pRspHeader->cla & CLA_RSP) != CLA_RSP)  return FALSE;

	//比较地址信息，保证请求与回复为相同的目标(暂未实现)
	char rqtIP[32] = {0};
	char rspIP[32] = {0};
	WORD rqtPort   = 0;
	WORD rspPort   = 0;
	CUDPSocket::getSockAddr(rqtIP, sizeof(rqtIP), rqtPort, pRemoteAddrRqt);
	CUDPSocket::getSockAddr(rspIP, sizeof(rspIP), rspPort, pRemoteAddrRqt);
	if ( (rqtPort != rspPort) || (strlen(rqtIP) != strlen(rspIP)) || (0 != strcmp(rqtIP, rspIP)) )  
	{
		return FALSE;  //地址信息不符合
	}

	if (pRqtHeader == NULL ||pRspHeader == NULL)	return FALSE;
	if (pRqtHeader->ins == pRspHeader->ins && (pRqtHeader->cla & CLA_RQT) == CLA_RQT && (pRspHeader->cla & CLA_RSP) == CLA_RSP)	return TRUE;
	else return FALSE;
}

/*********************************************************************
功能：发送响应
参数：pCmdIn，命令输入
	  pRemoteAddr，远程命令接收者
返回：返回：<0，错误；>=0，成功发送字节数
作者：叶文林	2012-11-30
*********************************************************************/
int CWLPrtclUDPServer::responseCmd(TRspCmdIn * pIn,struct sockaddr_in * pRemoteAddr)
{
	BYTE cmd[UDPSERVER_MAX_CMD_LEN] = {0};
	DWORD sendlen = 0,index = 0,len = 0;
	TAPDUHeader *pHeader = NULL; //报文头
	BYTE        *pBody   = NULL; //报文体
	TAPDUTailer  tailer        ; //报文尾

	memset(cmd,0x00,sizeof(cmd));
	pHeader = (TAPDUHeader * )cmd;
	//填充协议头
	pHeader->syncheader = SYNC_HEADER;
	pHeader->cla        = pIn->cla | CLA_RSP;
	pHeader->ins        = pIn->ins;
	pHeader->p1         = pIn->p1;
	pHeader->p2         = pIn->p2;
	pHeader->sn         = pIn->sn;
	pHeader->id         = m_localID;
	pHeader->lc         = pIn->lc;

	//填充协议体
	index = sizeof(TAPDUHeader);
	pBody = cmd+index;
	memcpy(pBody,&(pIn->sw),2);
	memcpy(pBody+2,pIn->pData,pHeader->lc -2);

	//填充协议尾
	index        = sizeof(TAPDUHeader)+pHeader->lc; //报文尾位置
	tailer.crc       = CalcCRC8(cmd,index);
	tailer.synctailer = SYNC_TAILER;
	memcpy(cmd + index,&tailer,sizeof(TAPDUTailer));

	//发送响应报文
	len = sizeof(TAPDUHeader) + pHeader->lc + sizeof(TAPDUTailer);
	sendlen = m_pSocket->send(cmd,len,pRemoteAddr);
	//sendlen = send(cmd,len,pRemoteAddr);
	return sendlen;

}


/*********************************************************************
功能：服务端主动发送消息并接受回复
参数：无
返回：无
作者：唐小灿	2014-07-25
*********************************************************************/
int CWLPrtclUDPServer::sendRqtRecvRsp(BYTE * sendbuf, DWORD sendbuflen, BYTE * recvbuf, DWORD &recvbuflen, struct sockaddr_in * pRemoteAddr, int waitRecvMulSec,int retryCount)
{
	int ret = 0;

	ret--;//-1
	if ((NULL==sendbuf) || (NULL==recvbuf) || (NULL==pRemoteAddr))  return ret;

	ret--;//-2
	if ((NULL == m_pSocket) || (NULL==m_pCSSend))  return ret;

	//进入本函数独有临界区
	m_pCSRqtRsp->enter();

	//进发送临界区
	m_pCSSend->enter();

	//重置响应事件
	m_pEvent->event_reset();

	int retrySendCount = 0;
LAB_RETRY:
	//发送消息
	ret--;//-2
	int rev = m_pSocket->send(sendbuf, sendbuflen, pRemoteAddr);
	if (rev < 0)  
	{
		//出发送临界区
		m_pCSSend->leave();
		m_pCSRqtRsp->leave();
		return ret;
	}

	//设置请求、响应数据缓冲
	memset(&m_RqtRspCache,0x00,sizeof(TRqtRspCacheWithAddr));         //清空缓存
	memcpy(m_RqtRspCache.rqt,sendbuf,sendbuflen);
	m_RqtRspCache.rqtlen = sendbuflen;
	m_RqtRspCache.isExistRqt  = 1;
	memcpy((char *)&m_RqtRspCache.addrSendTo, (char *)pRemoteAddr, sizeof(struct sockaddr_in));
	//出发送临界区
	m_pCSSend->leave();

	//等待响应(procRecvCmd函数会通知本函数请求到达)
	//使线程阻塞在一个条件变量，等同于：阻塞等待信号量触发
	recvbuflen = 0;
	ret = m_pEvent->event_timedwait(waitRecvMulSec);
	if (ret == 1)
	{
		//离开本函数独有临界区
		retrySendCount++;
		if (retrySendCount < retryCount)
		{
			m_pCSSend->enter();
			goto LAB_RETRY;
		}

		m_pCSRqtRsp->leave();
		return -3;
	}

	if (ret != 0)
	{
		//离开本函数独有临界区
		m_pCSRqtRsp->leave();
		return -4;
	}

	//取发送回的数据
	recvbuflen = m_RqtRspCache.rsplen ;
	memcpy(recvbuf,m_RqtRspCache.rsp ,recvbuflen);
	memset(&m_RqtRspCache,0,sizeof(TRqtRspCacheWithAddr));         //清空缓存

	//离开本函数独有临界区
	m_pCSRqtRsp->leave();

	return recvbuflen;
}

/*********************************************************************
功能：响应收到通信测试包(心跳包)
参数：pCmd，命令的指针
      len，命令的长度
	  pRemoteAddr，远程命令发送者
返回：无
作者：叶文林	2012-11-30
*********************************************************************/
void CWLPrtclUDPServer::onRecvCommDetect(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr)
{
	//取请求
	TAPDUHeader *pHeader = (TAPDUHeader *)pCmd;

	//非请求不处理
	if (CLA_RQT != pHeader->cla)  return;

	//响应报文数据区拼包
	WORD lc = SW_LEN;
	WORD sw = SW_SUCCESS;

	//发送响应
	TRspCmdIn rspCmdin = {CLA_RSP,pHeader->ins,pHeader->p1,pHeader->p2,pHeader->sn,lc,sw,NULL};
	responseCmd(&rspCmdin,pRemoteAddr);
}

/*********************************************************************
功能：响应收到请求服务器本机时间
参数：pCmd，命令的指针
      len，命令的长度
	  pRemoteAddr，远程命令发送者
返回：无
作者：叶文林	2013-11-28
*********************************************************************/
void CWLPrtclUDPServer::onRecvGetSvrTime(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr)
{
	//取请求
	TAPDUHeader *pHeader = (TAPDUHeader *)pCmd;

	//处理请求

	//响应报文数据区拼包
	TWLTime tm;
	WORD lc = SW_LEN + sizeof(TWLTime);
	WORD sw = onRemoteRqtGetSvrTime(&tm);

	//发送响应
	TRspCmdIn rspCmdin = {CLA_RSP,pHeader->ins,pHeader->p1,pHeader->p2,pHeader->sn,lc,sw,(BYTE *)&tm};
	responseCmd(&rspCmdin,pRemoteAddr);
}

/*********************************************************************
功能：处理：请求服务器本机时间
参数：pTime，输出参数，本地时间
返回：响应码
作者：叶文林	2013-11-28
*********************************************************************/
WORD CWLPrtclUDPServer::onRemoteRqtGetSvrTime(TWLTime * pTime)
{
	int ret = WLGetLocalTime(pTime);
	if (ret == 1)	return SW_SUCCESS;
	else return SW_UNKNOWN_ERR;
}
