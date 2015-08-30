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
���ܣ����캯��
������localID������վID��
	  localPort�����ط���˿ڣ�
���أ���
���ߣ�Ҷ����	
��ע����Զ��ע�� 2013-01-18
*********************************************************************/
CWLPrtclUDPServer::CWLPrtclUDPServer(int localID,WORD localPort) : CUDPServer(localPort)
{
	m_localID = localID;
	m_defTimeoutMilSec = 2000;

	m_pCSRqtRsp = new CWLCriticalSection();                    //sendRqtRecvRsp �����ٽ�������֤ �˺����̰߳�ȫ��
	m_pEvent    = new CWLEvent(false, false);
	m_pCSSend = new CWLCriticalSection();                      //����m_pSocket send���ݼ�����m_RqtRspCache���ٽ���
	memset(&m_RqtRspCache,0x00,sizeof(TRqtRspCacheWithAddr));  //����(��)���ݼ���Ӧ(��)�����߳�ͨ�Ż��壬��Ҫ�������ٽ���һ��ʹ��
}

CWLPrtclUDPServer::~CWLPrtclUDPServer(void)
{
	delete m_pCSRqtRsp;
	delete m_pEvent;
	delete m_pCSSend;                                    //����m_pSocket send���ݼ�����m_RqtRspCache���ٽ���
}


/*********************************************************************
���ܣ�������Ϣ ��������
������
���أ��ο�SW_XXX
���ߣ���С��	2014-07-29
*********************************************************************/
WORD CWLPrtclUDPServer::commDetect(struct sockaddr_in * pRemoteAddr)
{
	TRqtCmdIn cmdin = {CLA_RQT,INS_COMM_DETECT,0,0,0,0,0};
	int outlen = 0;
	WORD sw = 0;

	if (NULL == pRemoteAddr)
	{
		return SW_UNKNOWN_ERR;  //�ǲ��������ʹ��
	}

	sw = cmd(&cmdin,NULL,outlen,0, pRemoteAddr);
	if (SW_SUCCESS != sw)
	{
		WLOutDebug("�������ͻ��˷�������ʧ��");
	}
	return sw;
}


/*********************************************************************
���ܣ�������Ϣ ��ȡԶ��ʱ��
������
���أ��ο�SW_XXX
���ߣ���С��	2014-07-29
*********************************************************************/
WORD CWLPrtclUDPServer::getRemoteTime(TWLTime * pTime, struct sockaddr_in * pRemoteAddr)
{
	if (pTime == NULL)	return SW_INPUT_PARAM_ERR;

	TRqtCmdIn cmdin = {CLA_RQT,INS_GET_SVR_TIME,0,0,0,0,0};
	int outlen = 0;
	//WORD sw = cmd(&cmdin,(BYTE *)pTime,outlen,0, pRemoteAddr);
	//return sw;

	//����
	if (NULL != pRemoteAddr)
	{
		WORD sw = cmd(&cmdin,(BYTE *)pTime,outlen,0, pRemoteAddr);
		if (SW_SUCCESS != sw)
		{
			WLOutDebug("�������ͻ��˻�ȡʱ��ʧ��");
		}
		return sw;
	}
	else
	{
		//�ǲ��������ʹ��
		return SW_UNKNOWN_ERR;
	}
}


/*********************************************************************
���ܣ������������Ϣ�����շ�����Ϣ
������
���أ�TRUE��ƥ��
      FALSE����ƥ��
���ߣ���С��	2014-07-29
*********************************************************************/
WORD CWLPrtclUDPServer::cmd(TRqtCmdIn * pIn,BYTE * pOutData,int & outlen,int remoteID, struct sockaddr_in * pRemoteAddr)
{
	BYTE sendbuf[UDPSERVER_MAX_CMD_LEN];
	BYTE recvbuf[UDPSERVER_MAX_CMD_LEN];

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
	ret = sendRqtRecvRsp(sendbuf,sendlen,recvbuf,recvlen,pRemoteAddr,m_defTimeoutMilSec);
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

void CWLPrtclUDPServer::procRecvCmd(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr)             //�����յ��ı���
{
	int ret = 0;
	TAPDUHeader * pHeader  = (TAPDUHeader * )pCmd;
	//if ((pHeader->cla & CLA_RQT) ==  CLA_RQT)      //�ͻ�������������Ӧ��
	if (pHeader->cla ==  CLA_RQT)      //�ͻ�������������Ӧ��(CLA_RQTֵΪ0��(pHeader->cla&CLA_RQT)==CLA_RQTһֱ���������Ը�Ϊֱ��ƥ��)
	{
		char ip[20] = {0};
		WORD port = 0;
		CUDPSocket::getSockAddr(ip,sizeof(ip),port,pRemoteAddr);

		//��λ�� <--> ��λ��
		if	    (INS_COMM_DETECT == pHeader->ins)          //ͨ�ż�⣬��������
			onRecvCommDetect(pCmd,len,pRemoteAddr);
		else if (INS_GET_SVR_TIME == pHeader->ins)         //������Ϣ ��ȡ������ʱ��
			onRecvGetSvrTime(pCmd,len,pRemoteAddr);
		else 
			WLOutDebug("δʵ�ֱ���");
	}
	else if ((pHeader->cla & CLA_RSP) ==  CLA_RSP) //�ͻ�����Ӧ����������
	{
		//���������������õ��Ļظ�
		ret = procRemoteResponse(pCmd, len, pRemoteAddr);
		//return ret;
	}
	else
	{
		//δ֪����
	}
}

int CWLPrtclUDPServer::analyseProtocol(BYTE *out,DWORD &outlen)                             //��m_recvBuf���У�����������
{
	//��Ϊȷ��ΪUDP���ģ�ȷ��һ��ֻ��һ�����ģ����ļ��б߽磬���裨Ҳ���ܣ��磺�ж���ͻ��ˣ�ƴ������ֻ����򵥵ļ��

	TAPDUHeader * pHeader = NULL;
	TAPDUTailer * pTailer = NULL;
	WORD crc =0;
	int index = 0,cmdlen =0;
    BYTE tailer[sizeof(TAPDUTailer)] = {0};
	outlen = NULL;
LAB_HEAD:
	//������ݳ���������Ҫ��
	if (m_recvbuflen < sizeof(TAPDUHeader) + sizeof(TAPDUTailer))
	{
		m_recvbuflen = 0;
		return -1;            //ʧ��(��������һ������ƴ�ӣ���Ϊ�������Ŀ���������һ���ͻ���)
	}

	//���ͬ��ͷ�Ƿ�Ϸ�
	pHeader = (TAPDUHeader *) m_recvBuf;
	if (pHeader->syncheader != SYNC_HEADER)	goto LAB_JUMP_TO_HEAD;

	//��鳤���Ƿ�Ϸ�
	cmdlen = pHeader->lc + sizeof(TAPDUHeader) + sizeof(TAPDUTailer);
	if (m_recvbuflen < cmdlen)	goto LAB_JUMP_TO_HEAD;

	//���ͬ��β�Ƿ�Ϸ�
	index = pHeader->lc + sizeof(TAPDUHeader);
	memcpy(tailer,&m_recvBuf[index],sizeof(TAPDUTailer));
	pTailer = (TAPDUTailer *)tailer;
	if (pTailer->synctailer != SYNC_TAILER)	goto LAB_JUMP_TO_HEAD;

	//���CRC�Ƿ�Ϸ�
	crc = CalcCRC8(m_recvBuf,index);
	if (pTailer->crc != crc)	goto LAB_JUMP_TO_HEAD;

	//�ɹ���֤��һ������
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
���ܣ�����Զ�̻ظ�����Ϣ
������pCmd���ظ�����
      len���ظ������
	  pRemoteAddr���ظ������ַ��Ϣ
���أ�>=0����ȷ����
      <0�� ʧ��
���ߣ���С��	2014-07-30
*********************************************************************/
int CWLPrtclUDPServer::procRemoteResponse(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr)
{
	int ret = 0;

	//���������������õ��Ļظ�
	BOOL isMatch = isMatchRqtRspWithAddr(m_RqtRspCache.rqt, m_RqtRspCache.rqtlen, &m_RqtRspCache.addrSendTo, pCmd, len, pRemoteAddr);
	if (TRUE == isMatch)
	{
		//����Ӧ���뻺���������Ļ���״̬��֪ͨ�����߳��յ���Ӧ����
		m_RqtRspCache.isExistRqt  = 0;
		m_RqtRspCache.rsplen      = len;
		memcpy(m_RqtRspCache.rsp ,pCmd,len);
		ret = m_pEvent->event_set();

		return ret;
	}

	return -1;
}


/*********************************************************************
���ܣ���֤��������Ӧ�Ƿ�ƥ��
������pRsp���ظ�����
      rsplen���ظ������
	  pRemoteAddr���ظ������ַ��Ϣ
���أ�TRUE��ƥ��
      FALSE����ƥ��
���ߣ���С��	2014-07-28
*********************************************************************/
BOOL CWLPrtclUDPServer::isMatchRqtRspWithAddr(BYTE *pRqt,DWORD rqtlen, struct sockaddr_in * pRemoteAddrRqt,BYTE *pRsp,DWORD rsplen,struct sockaddr_in * pRemoteAddrRsp)
{
	if ((NULL == pRqt) || (NULL == pRsp) || (NULL == pRemoteAddrRqt) || (NULL == pRemoteAddrRsp))  return FALSE;
	if ((rqtlen < sizeof(TAPDUHeader)) || (rsplen < sizeof(TAPDUHeader)))  return FALSE;

	static TAPDUHeader * pRqtHeader = NULL;
	static TAPDUHeader * pRspHeader = NULL;
	pRqtHeader = (TAPDUHeader *)pRqt;
	pRspHeader = (TAPDUHeader *)pRsp;

	//�����ж��Ƿ��ǻظ�
	//if ((pRspHeader->cla & CLA_RSP) != CLA_RSP)  return FALSE;

	//�Ƚϵ�ַ��Ϣ����֤������ظ�Ϊ��ͬ��Ŀ��(��δʵ��)
	char rqtIP[32] = {0};
	char rspIP[32] = {0};
	WORD rqtPort   = 0;
	WORD rspPort   = 0;
	CUDPSocket::getSockAddr(rqtIP, sizeof(rqtIP), rqtPort, pRemoteAddrRqt);
	CUDPSocket::getSockAddr(rspIP, sizeof(rspIP), rspPort, pRemoteAddrRqt);
	if ( (rqtPort != rspPort) || (strlen(rqtIP) != strlen(rspIP)) || (0 != strcmp(rqtIP, rspIP)) )  
	{
		return FALSE;  //��ַ��Ϣ������
	}

	if (pRqtHeader == NULL ||pRspHeader == NULL)	return FALSE;
	if (pRqtHeader->ins == pRspHeader->ins && (pRqtHeader->cla & CLA_RQT) == CLA_RQT && (pRspHeader->cla & CLA_RSP) == CLA_RSP)	return TRUE;
	else return FALSE;
}

/*********************************************************************
���ܣ�������Ӧ
������pCmdIn����������
	  pRemoteAddr��Զ�����������
���أ����أ�<0������>=0���ɹ������ֽ���
���ߣ�Ҷ����	2012-11-30
*********************************************************************/
int CWLPrtclUDPServer::responseCmd(TRspCmdIn * pIn,struct sockaddr_in * pRemoteAddr)
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
	sendlen = m_pSocket->send(cmd,len,pRemoteAddr);
	//sendlen = send(cmd,len,pRemoteAddr);
	return sendlen;

}


/*********************************************************************
���ܣ����������������Ϣ�����ܻظ�
��������
���أ���
���ߣ���С��	2014-07-25
*********************************************************************/
int CWLPrtclUDPServer::sendRqtRecvRsp(BYTE * sendbuf, DWORD sendbuflen, BYTE * recvbuf, DWORD &recvbuflen, struct sockaddr_in * pRemoteAddr, int waitRecvMulSec,int retryCount)
{
	int ret = 0;

	ret--;//-1
	if ((NULL==sendbuf) || (NULL==recvbuf) || (NULL==pRemoteAddr))  return ret;

	ret--;//-2
	if ((NULL == m_pSocket) || (NULL==m_pCSSend))  return ret;

	//���뱾���������ٽ���
	m_pCSRqtRsp->enter();

	//�������ٽ���
	m_pCSSend->enter();

	//������Ӧ�¼�
	m_pEvent->event_reset();

	int retrySendCount = 0;
LAB_RETRY:
	//������Ϣ
	ret--;//-2
	int rev = m_pSocket->send(sendbuf, sendbuflen, pRemoteAddr);
	if (rev < 0)  
	{
		//�������ٽ���
		m_pCSSend->leave();
		m_pCSRqtRsp->leave();
		return ret;
	}

	//����������Ӧ���ݻ���
	memset(&m_RqtRspCache,0x00,sizeof(TRqtRspCacheWithAddr));         //��ջ���
	memcpy(m_RqtRspCache.rqt,sendbuf,sendbuflen);
	m_RqtRspCache.rqtlen = sendbuflen;
	m_RqtRspCache.isExistRqt  = 1;
	memcpy((char *)&m_RqtRspCache.addrSendTo, (char *)pRemoteAddr, sizeof(struct sockaddr_in));
	//�������ٽ���
	m_pCSSend->leave();

	//�ȴ���Ӧ(procRecvCmd������֪ͨ���������󵽴�)
	//ʹ�߳�������һ��������������ͬ�ڣ������ȴ��ź�������
	recvbuflen = 0;
	ret = m_pEvent->event_timedwait(waitRecvMulSec);
	if (ret == 1)
	{
		//�뿪�����������ٽ���
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
		//�뿪�����������ٽ���
		m_pCSRqtRsp->leave();
		return -4;
	}

	//ȡ���ͻص�����
	recvbuflen = m_RqtRspCache.rsplen ;
	memcpy(recvbuf,m_RqtRspCache.rsp ,recvbuflen);
	memset(&m_RqtRspCache,0,sizeof(TRqtRspCacheWithAddr));         //��ջ���

	//�뿪�����������ٽ���
	m_pCSRqtRsp->leave();

	return recvbuflen;
}

/*********************************************************************
���ܣ���Ӧ�յ�ͨ�Ų��԰�(������)
������pCmd�������ָ��
      len������ĳ���
	  pRemoteAddr��Զ���������
���أ���
���ߣ�Ҷ����	2012-11-30
*********************************************************************/
void CWLPrtclUDPServer::onRecvCommDetect(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr)
{
	//ȡ����
	TAPDUHeader *pHeader = (TAPDUHeader *)pCmd;

	//�����󲻴���
	if (CLA_RQT != pHeader->cla)  return;

	//��Ӧ����������ƴ��
	WORD lc = SW_LEN;
	WORD sw = SW_SUCCESS;

	//������Ӧ
	TRspCmdIn rspCmdin = {CLA_RSP,pHeader->ins,pHeader->p1,pHeader->p2,pHeader->sn,lc,sw,NULL};
	responseCmd(&rspCmdin,pRemoteAddr);
}

/*********************************************************************
���ܣ���Ӧ�յ��������������ʱ��
������pCmd�������ָ��
      len������ĳ���
	  pRemoteAddr��Զ���������
���أ���
���ߣ�Ҷ����	2013-11-28
*********************************************************************/
void CWLPrtclUDPServer::onRecvGetSvrTime(BYTE *pCmd,DWORD len,struct sockaddr_in * pRemoteAddr)
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
	responseCmd(&rspCmdin,pRemoteAddr);
}

/*********************************************************************
���ܣ������������������ʱ��
������pTime���������������ʱ��
���أ���Ӧ��
���ߣ�Ҷ����	2013-11-28
*********************************************************************/
WORD CWLPrtclUDPServer::onRemoteRqtGetSvrTime(TWLTime * pTime)
{
	int ret = WLGetLocalTime(pTime);
	if (ret == 1)	return SW_SUCCESS;
	else return SW_UNKNOWN_ERR;
}
