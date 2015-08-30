// NetSvrClient.cpp: implementation of the CNetSvrClient class.7
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WLPrtclUDPClient.h"
#include "WLProtocolDataType.h"
#include "../communicationModel/CRC8.h"
#include "../platformLayer/WLPlatformLayer.h"
//#include "../BusinessData/BusinessDataType.h"
//#include "../BusinessData/DSKeySocket.h"
#include "../communicationModel/UDPServer.h"  //为取宏，可将宏修改位置

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

CWLPrtclUDPClient::CWLPrtclUDPClient(int localID,const char * remoteIP,WORD remotePort,WORD localPort) : CStreamClient()
{
	m_localID = localID;
	m_defTimeoutMilSec = 2000;

	m_pSocket  = new CUDPSocket(remoteIP,remotePort,localPort);
}

CWLPrtclUDPClient::~CWLPrtclUDPClient()
{

}

WORD CWLPrtclUDPClient::commDetect()
{
	TRqtCmdIn cmdin = {CLA_RQT,INS_COMM_DETECT,0,0,0,0,0};
	int outlen = 0;
	WORD sw = cmd(&cmdin,NULL,outlen,0);
	return sw;
}

WORD CWLPrtclUDPClient::getSvrTime(TWLTime * pTime)
{
	if (pTime == NULL)	return SW_INPUT_PARAM_ERR;

	TRqtCmdIn cmdin = {CLA_RQT,INS_COMM_DETECT,0,0,0,0,0};
	int outlen = 0;
	WORD sw = cmd(&cmdin,(BYTE *)pTime,outlen,0);
	return sw;
}


WORD CWLPrtclUDPClient::cmd(TRqtCmdIn * pIn,BYTE * pOutData,int & outlen,int remoteID)
//WORD CNetSvrClient::cmd(BYTE cla,BYTE ins,BYTE p1,BYTE p2,WORD sn,WORD lc,BYTE * pInData,BYTE * pOutData,int & outlen)
{
	BYTE sendbuf[MAX_CMD_LEN];
	BYTE recvbuf[MAX_CMD_LEN];

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
	ret = sendRqtRecvRsp(sendbuf,sendlen,recvbuf,recvlen,m_defTimeoutMilSec);
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

/*********************************************************************
功能：根据缓冲m_recvBuf，分析出命令包
参数：out，输出参数，分析出一条命令的指针，内存空间由外部分配
      outlen，输出参数，分析出一条命令的长度
返回：>=0，成功，其中 =0 表示缓冲区中已不可能存在一条完整命令；
                 其中 >0 表示缓冲区中可能还有完整命令，调用者可重复调
				 用本函数，分析出余下命令，直到返回值<=0为止
      <0，失败
注意：本函数分析出正确报文后，切记要将更改“DWORD m_recvbuflen;//缓冲区当前缓冲字节数”
作者：叶文林	2012-09-20
*********************************************************************/
int CWLPrtclUDPClient::analyseProtocol(BYTE *out,DWORD &outlen)
{
	//暂时简单实现之
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
作者：唐小灿	2014-07-29
*********************************************************************/
int CWLPrtclUDPClient::pretreatmentRecvCmd(BYTE *pCmd,DWORD cmdlen)
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
BOOL CWLPrtclUDPClient::isMatchRqtRsp(BYTE *pRqt,DWORD rqtlen,BYTE *pRsp,DWORD rsplen)
{
	static TAPDUHeader * pRqtHeader = NULL;
	static TAPDUHeader * pRspHeader = NULL;
	pRqtHeader = (TAPDUHeader *)pRqt;
	pRspHeader = (TAPDUHeader *)pRsp;

	if (pRqtHeader == NULL ||pRspHeader == NULL)	return FALSE;
	if (pRqtHeader->ins == pRspHeader->ins && (pRqtHeader->cla & CLA_RQT) == CLA_RQT && (pRspHeader->cla & CLA_RSP) == CLA_RSP)	return TRUE;
	else return FALSE;
}


/*********************************************************************
功能：处理对方主动发起的命令，对应c、d两种通信模型
参数：pCmd,命令指针
      cmdlen，命令长度
返回：无
作者：唐小灿	2014-07-29
*********************************************************************/
void CWLPrtclUDPClient::procRemoteNotify(BYTE *pCmd,DWORD cmdlen)
{
	if (NULL == pCmd)  return;

	TAPDUHeader * pHeader = (TAPDUHeader * )pCmd;

	//只处理请求
	if (CLA_RQT != pHeader->cla)  return;

	if (pHeader->ins == INS_COMM_DETECT)                   //通信检测，用于心跳
		onRecvCommDetect(pCmd,cmdlen);
	else if (INS_GET_SVR_TIME == pHeader->ins)
		onRecvGetLocalTime(pCmd,cmdlen);
	else 
		WLOutDebug("未实现报文");
}


/*********************************************************************
功能：发送响应
参数：pCmdIn，命令输入
返回：返回：<0，错误；
      >=0，成功发送字节数
作者：唐小灿	2014-07-30
*********************************************************************/
int CWLPrtclUDPClient::responseCmd(TRspCmdIn * pIn)
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
	sendlen = m_pSocket->send(cmd,len);

	return sendlen;
}


/*
功能：处理对方主动消息 心跳
*/
void CWLPrtclUDPClient::onRecvCommDetect(BYTE *pCmd,DWORD len)
{
	//取请求
	TAPDUHeader *pHeader = (TAPDUHeader *)pCmd;

	//处理请求

	//响应报文数据区拼包
	WORD lc = SW_LEN;
	WORD sw = SW_SUCCESS;

	//发送响应
	TRspCmdIn rspCmdin = {CLA_RSP,pHeader->ins,pHeader->p1,pHeader->p2,pHeader->sn,lc,sw,NULL};
	responseCmd(&rspCmdin);
}


/*
功能：处理对方主动消息 获取本端时间
*/
void CWLPrtclUDPClient::onRecvGetLocalTime(BYTE *pCmd,DWORD len)
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
	responseCmd(&rspCmdin);
}


/*********************************************************************
功能：处理：请求服务器本机时间
参数：pTime，输出参数，本地时间
返回：响应码
作者：叶文林	2013-11-28
*********************************************************************/
WORD CWLPrtclUDPClient::onRemoteRqtGetSvrTime(TWLTime * pTime)
{
	int ret = WLGetLocalTime(pTime);
	if (ret == 1)	return SW_SUCCESS;
	else return SW_UNKNOWN_ERR;
}