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
#include "../communicationModel/UDPServer.h"  //Ϊȡ�꣬�ɽ����޸�λ��

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

	TAPDUHeader * pHeader = NULL; //����ͷ
	BYTE        * pBody   = NULL; //������
	TAPDUTailer   tailer  = {0};  //����β 
	WORD          sw;             //��Ӧ״̬��

	static WORD sn = 0;
	WORD snCur = sn;              //���ڱ��汾��sn�����߳���©������snCur��ֵǰ���߳��޸�sn��ֵ����ȷ
	sn = (sn+1)%0xFFFF;
	snCur = sn;
	memset(sendbuf,0x00,sizeof(sendbuf));
	pHeader = (TAPDUHeader * )sendbuf;
	//���Э��ͷ
	pHeader->syncheader = SYNC_HEADER;
	pHeader->cla        = pIn->cla;
	pHeader->ins        = pIn->ins;
	pHeader->p1         = pIn->p1;
	pHeader->p2         = pIn->p2;
	pHeader->sn         = snCur;//pIn->sn;
	pHeader->id         = m_localID;
	pHeader->remoteID   = remoteID;
	pHeader->lc         = pIn->lc;

	//���Э����
	pBody = sendbuf + sizeof(TAPDUHeader);
	memcpy(pBody,pIn->pData,pHeader->lc);

	//���Э��β
	int index        = sizeof(TAPDUHeader)+pHeader->lc; //����βλ��
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
		//WLOutDebug("���ͱ���ʧ��,������ = %d",ret);
		WLOutDebug("���ͱ���ʧ��,������ = %d,cla = 0x%.2X, ins = 0x%.2X",ret,pHeader->cla,pHeader->ins);
		return SW_NO_RSP;
	}

	//������Ӧ��
	pHeader = (TAPDUHeader *)recvbuf;
	if (pHeader->sn != snCur)  //��������Ļظ�����������
	{
		loop++;
		if (loop > 3)
		{
			return SW_UNKNOWN_ERR;  //δ������������
		}
		WLOutDebug("�ظ����� sn=%d,loop=%d",pHeader->sn,loop);
		goto LAB_LOOP;
	}
	pBody = recvbuf + sizeof(TAPDUHeader);
	memcpy(&sw,pBody,SW_LEN);               //ȡ״̬��
	if (pHeader->lc > SW_LEN)               //ȡ״̬��֮�����Ӧ�������
	{
		//pOutData = pBody + SW_LEN;
		/**********************************************************************
		�޸ģ��������´��뵼�µ��ڴ��������
		���ߣ�Ҷ����	2013-03-14
		���ԣ���������
		**********************************************************************/
		outlen   = pHeader->lc - SW_LEN;
		if (pOutData != NULL)	memcpy(pOutData,pBody + SW_LEN,outlen);
		//*******************************************************************
		//���´������󣬻����ڴ������������Ϊ����Ӧ���������ݶ�copy����
		//�����������
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

	//�������ͷ
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
���ܣ����ݻ���m_recvBuf�������������
������out�����������������һ�������ָ�룬�ڴ�ռ����ⲿ����
      outlen�����������������һ������ĳ���
���أ�>=0���ɹ������� =0 ��ʾ���������Ѳ����ܴ���һ���������
                 ���� >0 ��ʾ�������п��ܻ���������������߿��ظ���
				 �ñ��������������������ֱ������ֵ<=0Ϊֹ
      <0��ʧ��
ע�⣺��������������ȷ���ĺ��м�Ҫ�����ġ�DWORD m_recvbuflen;//��������ǰ�����ֽ�����
���ߣ�Ҷ����	2012-09-20
*********************************************************************/
int CWLPrtclUDPClient::analyseProtocol(BYTE *out,DWORD &outlen)
{
	//��ʱ��ʵ��֮
	outlen = (m_recvbuflen > MAX_CMD_LEN) ? MAX_CMD_LEN : m_recvbuflen;
	memcpy(out,m_recvBuf,outlen);
	m_recvbuflen = 0;
	return 0;
}


/*********************************************************************
���ܣ�Ԥ������Ӧ��(���յ������ݰ�)��procRecvCmd�����ڴ�����Ӧ��֮ǰ��
      �ȵ��ñ�����������������1ʱ��procRecvCmd������������Ӧ����һ��
	  ����
	  �������ر���������ʵ�ִ�������������������޸���Ӧ��
������pCmd�������ָ��
      outlen������ĳ���
���أ�1�����������(procRecvCmd����)��ֹ��һ������;��1����������
���ߣ���С��	2014-07-29
*********************************************************************/
int CWLPrtclUDPClient::pretreatmentRecvCmd(BYTE *pCmd,DWORD cmdlen)
{
	return 0;
}


/*********************************************************************
���ܣ���֤��������Ӧ�Ƿ�ƥ��
������pRqt����������ָ��
      pRsp����Ӧ����ָ��
���أ�1��ƥ��
      0����ƥ��
���ߣ�Ҷ����	2012-09-20
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
���ܣ�����Է���������������Ӧc��d����ͨ��ģ��
������pCmd,����ָ��
      cmdlen�������
���أ���
���ߣ���С��	2014-07-29
*********************************************************************/
void CWLPrtclUDPClient::procRemoteNotify(BYTE *pCmd,DWORD cmdlen)
{
	if (NULL == pCmd)  return;

	TAPDUHeader * pHeader = (TAPDUHeader * )pCmd;

	//ֻ��������
	if (CLA_RQT != pHeader->cla)  return;

	if (pHeader->ins == INS_COMM_DETECT)                   //ͨ�ż�⣬��������
		onRecvCommDetect(pCmd,cmdlen);
	else if (INS_GET_SVR_TIME == pHeader->ins)
		onRecvGetLocalTime(pCmd,cmdlen);
	else 
		WLOutDebug("δʵ�ֱ���");
}


/*********************************************************************
���ܣ�������Ӧ
������pCmdIn����������
���أ����أ�<0������
      >=0���ɹ������ֽ���
���ߣ���С��	2014-07-30
*********************************************************************/
int CWLPrtclUDPClient::responseCmd(TRspCmdIn * pIn)
{
	BYTE cmd[UDPSERVER_MAX_CMD_LEN] = {0};
	DWORD sendlen = 0,index = 0,len = 0;
	TAPDUHeader *pHeader = NULL; //����ͷ
	BYTE        *pBody   = NULL; //������
	TAPDUTailer  tailer        ; //����β

	memset(cmd,0x00,sizeof(cmd));
	pHeader = (TAPDUHeader * )cmd;
	//���Э��ͷ
	pHeader->syncheader = SYNC_HEADER;
	pHeader->cla        = pIn->cla | CLA_RSP;
	pHeader->ins        = pIn->ins;
	pHeader->p1         = pIn->p1;
	pHeader->p2         = pIn->p2;
	pHeader->sn         = pIn->sn;
	pHeader->id         = m_localID;
	pHeader->lc         = pIn->lc;

	//���Э����
	index = sizeof(TAPDUHeader);
	pBody = cmd+index;
	memcpy(pBody,&(pIn->sw),2);
	memcpy(pBody+2,pIn->pData,pHeader->lc -2);

	//���Э��β
	index        = sizeof(TAPDUHeader)+pHeader->lc; //����βλ��
	tailer.crc       = CalcCRC8(cmd,index);
	tailer.synctailer = SYNC_TAILER;
	memcpy(cmd + index,&tailer,sizeof(TAPDUTailer));

	//������Ӧ����
	len = sizeof(TAPDUHeader) + pHeader->lc + sizeof(TAPDUTailer);
	sendlen = m_pSocket->send(cmd,len);

	return sendlen;
}


/*
���ܣ�����Է�������Ϣ ����
*/
void CWLPrtclUDPClient::onRecvCommDetect(BYTE *pCmd,DWORD len)
{
	//ȡ����
	TAPDUHeader *pHeader = (TAPDUHeader *)pCmd;

	//��������

	//��Ӧ����������ƴ��
	WORD lc = SW_LEN;
	WORD sw = SW_SUCCESS;

	//������Ӧ
	TRspCmdIn rspCmdin = {CLA_RSP,pHeader->ins,pHeader->p1,pHeader->p2,pHeader->sn,lc,sw,NULL};
	responseCmd(&rspCmdin);
}


/*
���ܣ�����Է�������Ϣ ��ȡ����ʱ��
*/
void CWLPrtclUDPClient::onRecvGetLocalTime(BYTE *pCmd,DWORD len)
{
	//ȡ����
	TAPDUHeader *pHeader = (TAPDUHeader *)pCmd;

	//��������

	//��Ӧ����������ƴ��
	TWLTime tm;
	WORD lc = SW_LEN + sizeof(TWLTime);
	WORD sw = onRemoteRqtGetSvrTime(&tm);

	//������Ӧ
	TRspCmdIn rspCmdin = {CLA_RSP,pHeader->ins,pHeader->p1,pHeader->p2,pHeader->sn,lc,sw,(BYTE *)&tm};
	responseCmd(&rspCmdin);
}


/*********************************************************************
���ܣ������������������ʱ��
������pTime���������������ʱ��
���أ���Ӧ��
���ߣ�Ҷ����	2013-11-28
*********************************************************************/
WORD CWLPrtclUDPClient::onRemoteRqtGetSvrTime(TWLTime * pTime)
{
	int ret = WLGetLocalTime(pTime);
	if (ret == 1)	return SW_SUCCESS;
	else return SW_UNKNOWN_ERR;
}