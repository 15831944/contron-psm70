// COMSocket.h: interface for the CCOMSocket class.
// 定义串口通信类，继承于CCommunicationSocket。是对串口通信方式的封装
// 目的：为了适应程序的通信方式不确定，而定义这套通信接口，例如：
//       某应用将来要用串口通信，但是当前条件无串口，可暂时时用UDP代替。
//       如果不将通信接口作一个很好的封装，将来代码改动量较大，本类家族
//       就是解决此类问题的
// 注意：本类只是方便通信，并不实现串口所有功能。
// 作者：叶文林	2012-08-24
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMSOCKET_H__5E7C8613_1890_47C9_BF26_AC040E080478__INCLUDED_)
#define AFX_COMSOCKET_H__5E7C8613_1890_47C9_BF26_AC040E080478__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../platformLayer/WLDEF.h"
#include "../platformLayer/WLPlatformLayer.h"
#include "CommunicationSocket.h"

#ifdef WL_OS_CORE_WIN
#include <windows.h>
#elif defined WL_OS_CORE_LINUX
//
#define HANDLE DWORD 
#endif


class CCOMSocket : public CCommunicationSocket  
{
public:
	CCOMSocket();
	virtual ~CCOMSocket();

	int open(const char * portName,DWORD baud,int dtr = 0,int rts = 0);
	int close();
	
	virtual int send(void * pbuf,int len);
	virtual int recv(void * pbuf,int len);
	virtual int reConnect();
	virtual int setRecvTimeout(DWORD waitMilSec);
	virtual int resetRecvTimeout(DWORD waitMilSec);
private:
	HANDLE m_hComm;                //串口句柄
#ifdef WL_OS_CORE_WIN
	BOOL setTimeOuts(COMMTIMEOUTS timeouts) ; //设置口串口读写超时时间，并改变默认值。一般情况下只需调用 setRecvTime，不必调用本接口
#elif defined WL_OS_CORE_LINUX
#endif
	char m_portName[8];            //串口名称
	DWORD m_baud;                  //波特率
#ifdef WL_OS_CORE_WIN
	COMMTIMEOUTS m_timeOuts;       //串口收发超时时间
#elif defined WL_OS_CORE_LINUX
#endif
};

#endif // !defined(AFX_COMSOCKET_H__5E7C8613_1890_47C9_BF26_AC040E080478__INCLUDED_)
