// COMSocket.cpp: implementation of the CCOMSocket class.
// ���崮���࣬����Ϊ��CCommunicationSocket��ʵ���� send��recv�ӿ�
// Ŀ�ģ�Ϊ����Ӧ�����ͨ�ŷ�ʽ��ȷ��������������ͨ�Žӿڣ����磺
//       ĳӦ�ý���Ҫ�ô���ͨ�ţ����ǵ�ǰ�����޴��ڣ�����ʱʱ��UDP���档
//       �������ͨ�Žӿ���һ���ܺõķ�װ����������Ķ����ϴ󣬱����
//       ���ǽ�����������
// ע�⣺�ֵ���CTCPSocket��CUDPSocket��CCOMSocket
// ���ߣ�Ҷ����	2012-08-24
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "COMSocket.h"
#include <stdio.h>
#include "../platformLayer/WLPlatformLayer.h"

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
���ܣ����캯��
��������
���أ���
���ߣ�Ҷ����	2012-09-19
*********************************************************************/
CCOMSocket::CCOMSocket()
{
	m_hComm = 0;
}

/*********************************************************************
���ܣ���������
��������
���أ���
���ߣ�Ҷ����	2012-09-19
*********************************************************************/
CCOMSocket::~CCOMSocket()
{

}

/*********************************************************************
���ܣ��򿪴���
������portName���������ƣ�baud�������ʡ�����У��λ���裺N��8��1
      dtr��rts��ȡ��Ϊ0��1����ֹ��������
���أ�1���ɹ���������ʧ��
���ߣ�Ҷ����	2012-09-19
*********************************************************************/
int CCOMSocket::open(const char * portName,DWORD baud,int dtr,int rts)
{
#ifdef WL_OS_CORE_WIN
	//�������
	memset(m_portName,0,sizeof(m_portName));
	sprintf(m_portName,"%s",portName);
	m_baud = baud;

	if (m_hComm != 0)	close(); //��ֹ�ظ���

	BOOL ret = FALSE;
	TCHAR wPortName[10] = {0};

	DCB dcb;                                     // ���ڿ��ƿ�
	COMMTIMEOUTS timeouts = {                    // ���ڳ�ʱ���Ʋ���
								10,             // ���ַ������ʱʱ��: 100 ms
								MAXDWORD,              // ������ʱÿ�ַ���ʱ��: 1 ms (n���ַ��ܹ�Ϊn ms)
								MAXDWORD,        // ������(�����)����ʱʱ��: 500 ms 
								1,               // д����ʱÿ�ַ���ʱ��: 1 ms (n���ַ��ܹ�Ϊn ms)
								100};            // ������(�����)д��ʱʱ��: 100 ms
#ifdef WL_OS_WINCE
	gAnsiToUnicode(portName,wPortName);
#endif
	//�򿪴���
	m_hComm = CreateFile(wPortName,              // �������ƻ��豸·��
		GENERIC_READ | GENERIC_WRITE,            // ��д��ʽ
		0,               // ����ʽ����ռ
		NULL,            // Ĭ�ϵİ�ȫ������
		OPEN_EXISTING,   // ������ʽ
		0,               // ���������ļ�����
		NULL);           // �������ģ���ļ�
	if(m_hComm == INVALID_HANDLE_VALUE)
	{
		WLOutDebug("�򿪴���ʧ�ܣ����ں�Ϊ:%s\n",portName);
		return FALSE;        // �򿪴���ʧ��
	}

	//���ò�����
	ret = GetCommState(m_hComm, &dcb);                       // ȡDCB
	if (ret == FALSE)	goto LAB_ERR;
	dcb.BaudRate = baud;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.fDtrControl = dtr;								     // ��ֹ�������� 
	dcb.fRtsControl = rts;
	ret = SetCommState(m_hComm, &dcb);                       // ����DCB
	if (ret == FALSE)	goto LAB_ERR;

//	ret = SetupComm(m_hComm, 4096, 1024);                    // �������������������С
	if (ret == FALSE)	goto LAB_ERR;

	ret = setTimeOuts(timeouts);                             // ���ó�ʱ,���ĳ�Ĭ�ϳ�ʱ����
	if (ret == FALSE)	goto LAB_ERR;

	return TRUE;

LAB_ERR:
	CloseHandle(m_hComm);
	return FALSE;
#else
	return -1;
#endif


	////m_hComm = OpenCommExt(TEXT("COM:5"),19200);
	////return 1;

	//DCB	dcbComPort;
	//BOOL blRet = TRUE;
	//DWORD dwRet;

	////�������
	//memset(m_portName,0,sizeof(m_portName));
	//sprintf(m_portName,"%s",portName);
	//m_baud = baud;
	//
	//if (m_hComm != 0)	close(); //��ֹ�ظ���
	//TCHAR wPortName[10] = {0};
	//gAnsiToUnicode(portName,wPortName);
	////�򿪴���
	//m_hComm = CreateFile(wPortName, 
	//	GENERIC_READ | GENERIC_WRITE, 
	//	0, // com cannot be shared
	//	NULL, 
	//	OPEN_EXISTING, 
	//	0,//FILE_ATTRIBUTE_NORMAL, 
	//	NULL);
	//if(m_hComm == INVALID_HANDLE_VALUE)
	//{
	//	//MessageBox(TEXT("���ļ�ʧ��"),_T("ERROR"));
	//	printf("createfile err\n");
	//	return -1;
	//} 
	//
	////SetCommMask(m_hComm, EV_RXCHAR);					// �����¼�:���յ�һ���ַ� 	
 //   blRet = ::SetupComm(m_hComm,1024,1024);  //�������뻺����������������Ĵ�С 
	//if (!blRet)	
	//{	
	//	CloseHandle(m_hComm);
	//	printf("PurgeComm err!");
	//	return 0;
	//}
 //   blRet = ::PurgeComm(m_hComm,PURGE_TXCLEAR | PURGE_RXCLEAR);  //���������������� 
	////blRet = PurgeComm(m_hComm,PURGE_TXABORT | PURGE_RXABORT |
	////	PURGE_TXCLEAR | PURGE_RXCLEAR );
	//if (!blRet)	
	//{	
	//	CloseHandle(m_hComm);
	//	printf("PurgeComm err!");
	//	return 0;
	//}
	//
	//
	//
	////���ò�����
	//dwRet = GetCommState(m_hComm,&dcbComPort);
	//if(!dwRet)
	//{
	//	CloseHandle(m_hComm);
	//	printf("GetCommState failed");
	//	return -2;
	//}
	//else
	//{
	///*
	//dcbComPort.BaudRate = Baud;
	//dcbComPort.ByteSize = 8;
	//dcbComPort.Parity   = NOPARITY;
	//dcbComPort.StopBits = ONESTOPBIT;
	//	*/
	//	dcbComPort.BaudRate = baud;			
	//	dcbComPort.ByteSize = 8;
	//	dcbComPort.Parity   = NOPARITY;
	//	dcbComPort.StopBits = ONESTOPBIT;	
	//	dcbComPort.fParity  = FALSE;								// ��ֹ��żУ�� 
	//	dcbComPort.fBinary  = TRUE;
	//	dcbComPort.fDtrControl = 0;								// ��ֹ�������� 
	//	dcbComPort.fRtsControl = 0;
	//	dcbComPort.fOutX       = 0;
	//	dcbComPort.fInX        = 0;
	//	dcbComPort.fTXContinueOnXoff = 0;
	//	
	//	dwRet = SetCommState(m_hComm,&dcbComPort);
	//	if(!dwRet)
	//	{
	//		//MessageBox(_T("SetCommState failed"),_T("WARNING"),MB_ICONWARNING);
	//		printf("SetCommState failed\n");
	//		return -3;
	//	}
	//}	

	//SetTimeOuts();
	//
	//return 1;
}

/*********************************************************************
���ܣ��رմ���
��������
���أ�1���μ�����ϵͳ
���ߣ�Ҷ����	2012-09-19
*********************************************************************/
int CCOMSocket::close()
{
#ifdef WL_OS_CORE_WIN
	if(m_hComm)	return CloseHandle(m_hComm);
	return 1;
#else
	return -1;
#endif

}

/*********************************************************************
���ܣ����ó�ʱ����
��������
���أ�������
���ߣ�Ҷ����	2012-09-19
*********************************************************************/ 
#ifdef WL_OS_CORE_WIN
BOOL CCOMSocket::setTimeOuts(COMMTIMEOUTS timeouts) 
{ 
#ifdef WL_OS_CORE_WIN
#ifdef WL_OS_WIN32
	//win32������ �����߳�һ��������һ��ͬʱд�ᵼ�� �������ʸ�С��ʱʱ��
	COMMTIMEOUTS t;
	memcpy(&t,&timeouts,sizeof(COMMTIMEOUTS));
	if (t.ReadIntervalTimeout == 0)	t.ReadIntervalTimeout = 100;  //�ı�������ֽ�֮�������,ԭ��� recv
	if (t.ReadIntervalTimeout >500)	t.ReadIntervalTimeout = 500;
	t.ReadTotalTimeoutMultiplier = 0;
	t.ReadTotalTimeoutConstant = timeouts.ReadIntervalTimeout;
	BOOL ret = ::SetCommTimeouts(m_hComm,&t);         //���ó�ʱ 
	if (ret == FALSE)	return ret;
#endif

#ifdef WL_OS_WINCE
	BOOL ret = ::SetCommTimeouts(m_hComm,&timeouts);  //���ó�ʱ 
	if (ret == FALSE)	return ret;
#endif

	memcpy(&m_timeOuts,&timeouts,sizeof(COMMTIMEOUTS));
	return TRUE;
#else
	return FALSE;
#endif

} 
#elif defined WL_OS_CORE_LINUX
#endif
/*********************************************************************
���ܣ���������
������pbuf�����ͻ���
      len�������ֽ���
���أ�<0������>=0���ɹ������ֽ���
���ߣ�Ҷ����	2012-09-19
*********************************************************************/
int CCOMSocket::send(void * pbuf,int len)
{
#ifdef WL_OS_CORE_WIN
	BOOL ret;
	DWORD nNumberOfBytesWritten;

	//ret = MyWriteGramComm(m_hComm,(BYTE *)pbuf,len);
	//WLOutDebug("send tick = %d",GetTickCount());
	ret = WriteFile(m_hComm,pbuf,len,&nNumberOfBytesWritten,NULL);
	//WLOutDebugStream("comm>>",(BYTE *)pbuf,nNumberOfBytesWritten);
	if (ret)	return nNumberOfBytesWritten;	else return -1;
#else
	return -1;
#endif
}

/*********************************************************************
���ܣ���������
������pbuf�����ջ���
      len�������ֽ���
���أ�<0������>=0���ɹ������ֽ���
���ߣ�Ҷ����	2012-09-19
*********************************************************************/
int CCOMSocket::recv(void * pbuf,int len)
{
#ifdef WL_OS_CORE_WIN
	BOOL ret;
	DWORD nNumberOfBytesRead=9;

#ifdef WL_OS_WINCE
	DWORD tc = GetTickCount();
	ret = ReadFile(m_hComm,pbuf,len,&nNumberOfBytesRead,NULL);
	/*if (nNumberOfBytesRead == 0)
	{
		WLOutDebug("����û�ж������ݷ��� len = %d,tc = %d",len, GetTickCount() - tc);
	}*/
#endif

#ifdef WL_OS_WIN32
	//�����Winxp
	static BYTE b[1024] = {0};
	static DWORD l =0;
	static DWORD tc = WLGetTickCount();
	memset(b,0,1024);
	nNumberOfBytesRead = 0;
	while (1)
	{
		if (m_timeOuts.ReadTotalTimeoutConstant != MAXDWORD && WLGetTickCount() - tc > m_timeOuts.ReadTotalTimeoutConstant)	break; //�ܶ�ʱ�䳬ʱ
		//WLSleep(m_timeOuts.ReadIntervalTimeout);                                   //sleepһ��ʱ����

		l = 0;
		ret = ReadFile(m_hComm,b,1024,&l,NULL);
		if (ret == FALSE)
		{
			if (nNumberOfBytesRead == 0)	
			{
				return -1;	
			}
			else 
			{
				ret = TRUE;
				break;
			}
		}
		if (l ==0 && nNumberOfBytesRead >0)	
			break;  //һ�����������	
		if (l > 0)
		{
			if (l + nNumberOfBytesRead >= len)	l = len - nNumberOfBytesRead;
			memcpy(&((char *)pbuf)[nNumberOfBytesRead],b,l);
			nNumberOfBytesRead += l;
			if (nNumberOfBytesRead >= len)	break; //������ȫ��������
		}
		
	}
#endif
	

	//PurgeComm(m_hComm, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	static int i = 0;
	if (nNumberOfBytesRead != 0)
	{
		//WLOutDebug("%d",i++);
		//WLOutDebug("recv tick = %d",GetTickCount());
		//WLOutDebugStream("comm<<",(BYTE *)pbuf,nNumberOfBytesRead);
		BYTE *pByte = (BYTE *)pbuf;
		pByte[nNumberOfBytesRead] = NULL;
		//WLOutDebug("==================================>\n%s\n<==================================\n",(char *)pbuf);
	}

	if (ret)	return nNumberOfBytesRead;	else return -1;
#else
	return -1;
#endif
}

/*********************************************************************
���ܣ��������ӣ��Դ��ڡ�TCP��Ч��UDP��Ч��
��������
���أ�<0������>=0���ɹ�
���ߣ�Ҷ����	2012-09-19
*********************************************************************/
int CCOMSocket::reConnect()
{
#ifdef WL_OS_CORE_WIN
	return open(m_portName,m_baud);
#elif defined WL_OS_CORE_LINUX
	return -1;
#endif
}

/*********************************************************************
���ܣ���λ�����ݳ�ʱʱ�䵽Ĭ��ֵ
������waitMilSec���ȴ�ʱ�䣬��λ����
���أ�<0������>=0���ɹ�
ע�⣺���������Ĭ�϶����ݲ���ʱ��һֱ������
      һ������£���Ҫ���ñ��ӿڡ�
���ߣ�Ҷ����	2012-09-28
*********************************************************************/
int CCOMSocket::resetRecvTimeout(DWORD waitMilSec)
{
#ifdef WL_OS_CORE_WIN
	return SetCommTimeouts(m_hComm,&m_timeOuts);
#elif defined WL_OS_CORE_LINUX
	return -1;
#endif
	
}

/*********************************************************************
���ܣ���������һ�������ݳ�ʱʱ��
������waitMilSec���ȴ�ʱ�䣬��λ����
���أ�<0������>=0���ɹ�
ע�⣺���������Ĭ�϶����ݲ���ʱ��һֱ������
      һ������£���Ҫ���ñ��ӿڡ�
���ߣ�Ҷ����	2012-09-28
*********************************************************************/
int CCOMSocket::setRecvTimeout(DWORD waitMilSec)
{
#ifdef WL_OS_CORE_WIN
	COMMTIMEOUTS stTimeOuts; 

	BOOL ret = GetCommTimeouts(m_hComm,&stTimeOuts);
	if(ret == FALSE)	return ret;

	stTimeOuts.ReadTotalTimeoutMultiplier = 0;
	stTimeOuts.WriteTotalTimeoutConstant  = waitMilSec;

	ret = SetCommTimeouts(m_hComm,&stTimeOuts);
	return ret;
#elif defined WL_OS_CORE_LINUX
	return -1;
#endif

}

