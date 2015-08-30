// COMSocket.cpp: implementation of the CCOMSocket class.
// 定义串口类，基类为：CCommunicationSocket，实现其 send、recv接口
// 目的：为了适应程序的通信方式不确定，而定义这套通信接口，例如：
//       某应用将来要用串口通信，但是当前条件无串口，可暂时时用UDP代替。
//       如果不将通信接口作一个很好的封装，将来代码改动量较大，本类的
//       就是解决此类问题的
// 注意：兄弟类CTCPSocket、CUDPSocket、CCOMSocket
// 作者：叶文林	2012-08-24
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
功能：构造函数
参数：无
返回：无
作者：叶文林	2012-09-19
*********************************************************************/
CCOMSocket::CCOMSocket()
{
	m_hComm = 0;
}

/*********************************************************************
功能：析构函数
参数：无
返回：无
作者：叶文林	2012-09-19
*********************************************************************/
CCOMSocket::~CCOMSocket()
{

}

/*********************************************************************
功能：打开串口
参数：portName，串口名称；baud，波特率。串口校验位等设：N，8，1
      dtr，rts，取标为0或1，禁止流量控制
返回：1，成功；其它，失败
作者：叶文林	2012-09-19
*********************************************************************/
int CCOMSocket::open(const char * portName,DWORD baud,int dtr,int rts)
{
#ifdef WL_OS_CORE_WIN
	//保存参数
	memset(m_portName,0,sizeof(m_portName));
	sprintf(m_portName,"%s",portName);
	m_baud = baud;

	if (m_hComm != 0)	close(); //防止重复打开

	BOOL ret = FALSE;
	TCHAR wPortName[10] = {0};

	DCB dcb;                                     // 串口控制块
	COMMTIMEOUTS timeouts = {                    // 串口超时控制参数
								10,             // 读字符间隔超时时间: 100 ms
								MAXDWORD,              // 读操作时每字符的时间: 1 ms (n个字符总共为n ms)
								MAXDWORD,        // 基本的(额外的)读超时时间: 500 ms 
								1,               // 写操作时每字符的时间: 1 ms (n个字符总共为n ms)
								100};            // 基本的(额外的)写超时时间: 100 ms
#ifdef WL_OS_WINCE
	gAnsiToUnicode(portName,wPortName);
#endif
	//打开串口
	m_hComm = CreateFile(wPortName,              // 串口名称或设备路径
		GENERIC_READ | GENERIC_WRITE,            // 读写方式
		0,               // 共享方式：独占
		NULL,            // 默认的安全描述符
		OPEN_EXISTING,   // 创建方式
		0,               // 不需设置文件属性
		NULL);           // 不需参照模板文件
	if(m_hComm == INVALID_HANDLE_VALUE)
	{
		WLOutDebug("打开串口失败，串口号为:%s\n",portName);
		return FALSE;        // 打开串口失败
	}

	//设置波特率
	ret = GetCommState(m_hComm, &dcb);                       // 取DCB
	if (ret == FALSE)	goto LAB_ERR;
	dcb.BaudRate = baud;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.fDtrControl = dtr;								     // 禁止流量控制 
	dcb.fRtsControl = rts;
	ret = SetCommState(m_hComm, &dcb);                       // 设置DCB
	if (ret == FALSE)	goto LAB_ERR;

//	ret = SetupComm(m_hComm, 4096, 1024);                    // 设置输入输出缓冲区大小
	if (ret == FALSE)	goto LAB_ERR;

	ret = setTimeOuts(timeouts);                             // 设置超时,并改成默认超时参数
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

	////保存参数
	//memset(m_portName,0,sizeof(m_portName));
	//sprintf(m_portName,"%s",portName);
	//m_baud = baud;
	//
	//if (m_hComm != 0)	close(); //防止重复打开
	//TCHAR wPortName[10] = {0};
	//gAnsiToUnicode(portName,wPortName);
	////打开串口
	//m_hComm = CreateFile(wPortName, 
	//	GENERIC_READ | GENERIC_WRITE, 
	//	0, // com cannot be shared
	//	NULL, 
	//	OPEN_EXISTING, 
	//	0,//FILE_ATTRIBUTE_NORMAL, 
	//	NULL);
	//if(m_hComm == INVALID_HANDLE_VALUE)
	//{
	//	//MessageBox(TEXT("打开文件失败"),_T("ERROR"));
	//	printf("createfile err\n");
	//	return -1;
	//} 
	//
	////SetCommMask(m_hComm, EV_RXCHAR);					// 串口事件:接收到一个字符 	
 //   blRet = ::SetupComm(m_hComm,1024,1024);  //设置输入缓冲区和输出缓冲区的大小 
	//if (!blRet)	
	//{	
	//	CloseHandle(m_hComm);
	//	printf("PurgeComm err!");
	//	return 0;
	//}
 //   blRet = ::PurgeComm(m_hComm,PURGE_TXCLEAR | PURGE_RXCLEAR);  //清空输入输出缓冲区 
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
	////设置波特率
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
	//	dcbComPort.fParity  = FALSE;								// 禁止奇偶校验 
	//	dcbComPort.fBinary  = TRUE;
	//	dcbComPort.fDtrControl = 0;								// 禁止流量控制 
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
功能：关闭串口
参数：无
返回：1，参见操作系统
作者：叶文林	2012-09-19
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
功能：设置超时函数
参数：无
返回：无意义
作者：叶文林	2012-09-19
*********************************************************************/ 
#ifdef WL_OS_CORE_WIN
BOOL CCOMSocket::setTimeOuts(COMMTIMEOUTS timeouts) 
{ 
#ifdef WL_OS_CORE_WIN
#ifdef WL_OS_WIN32
	//win32下由于 两个线程一个读，另一个同时写会导致 死锁，故改小操时时间
	COMMTIMEOUTS t;
	memcpy(&t,&timeouts,sizeof(COMMTIMEOUTS));
	if (t.ReadIntervalTimeout == 0)	t.ReadIntervalTimeout = 100;  //改变读两个字节之间最大间隔,原因见 recv
	if (t.ReadIntervalTimeout >500)	t.ReadIntervalTimeout = 500;
	t.ReadTotalTimeoutMultiplier = 0;
	t.ReadTotalTimeoutConstant = timeouts.ReadIntervalTimeout;
	BOOL ret = ::SetCommTimeouts(m_hComm,&t);         //设置超时 
	if (ret == FALSE)	return ret;
#endif

#ifdef WL_OS_WINCE
	BOOL ret = ::SetCommTimeouts(m_hComm,&timeouts);  //设置超时 
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
功能：发送数据
参数：pbuf，发送缓冲
      len，发送字节数
返回：<0，错误；>=0，成功发送字节数
作者：叶文林	2012-09-19
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
功能：接收数据
参数：pbuf，接收缓冲
      len，接收字节数
返回：<0，错误；>=0，成功接收字节数
作者：叶文林	2012-09-19
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
		WLOutDebug("串口没有读到数据返回 len = %d,tc = %d",len, GetTickCount() - tc);
	}*/
#endif

#ifdef WL_OS_WIN32
	//如果是Winxp
	static BYTE b[1024] = {0};
	static DWORD l =0;
	static DWORD tc = WLGetTickCount();
	memset(b,0,1024);
	nNumberOfBytesRead = 0;
	while (1)
	{
		if (m_timeOuts.ReadTotalTimeoutConstant != MAXDWORD && WLGetTickCount() - tc > m_timeOuts.ReadTotalTimeoutConstant)	break; //总读时间超时
		//WLSleep(m_timeOuts.ReadIntervalTimeout);                                   //sleep一个时间间隔

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
			break;  //一包命令读完了	
		if (l > 0)
		{
			if (l + nNumberOfBytesRead >= len)	l = len - nNumberOfBytesRead;
			memcpy(&((char *)pbuf)[nNumberOfBytesRead],b,l);
			nNumberOfBytesRead += l;
			if (nNumberOfBytesRead >= len)	break; //收完了全部数据了
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
功能：重新连接（对串口、TCP有效、UDP无效）
参数：无
返回：<0，错误；>=0，成功
作者：叶文林	2012-09-19
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
功能：复位收数据超时时间到默认值
参数：waitMilSec，等待时间，单位毫秒
返回：<0，错误；>=0，成功
注意：请子类设计默认读数据不超时，一直阻塞。
      一般情况下，不要调用本接口。
作者：叶文林	2012-09-28
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
功能：重新设置一次收数据超时时间
参数：waitMilSec，等待时间，单位毫秒
返回：<0，错误；>=0，成功
注意：请子类设计默认读数据不超时，一直阻塞。
      一般情况下，不要调用本接口。
作者：叶文林	2012-09-28
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

