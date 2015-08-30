// CommunicationSocket.h: interface for the CCommunicationSocket class.
// 定义一个通用通信接口基类，子类将实现 UDP、TCP、串口以及将来的通信方式
// 目的：为了适应程序的通信方式不确定，而定义这套通信接口，例如：
//       某应用将来要用串口通信，但是当前条件无串口，可暂时时用UDP代替。
//       如果不将通信接口作一个很好的封装，将来代码改动量较大，本类的
//       就是解决此类问题的
// 注意：本类可以看作纯虚类，请不要实例化本类。具体请见：
//       CTCPSocket、CUDPSocket、CCOMSocket
// 作者：叶文林	2012-08-24
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMUNICATIONSOCKET_H__5972555E_E568_4946_95BF_A76923726190__INCLUDED_)
#define AFX_COMMUNICATIONSOCKET_H__5972555E_E568_4946_95BF_A76923726190__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../platformLayer/WLDEF.h"

class CCommunicationSocket  
{
public:
	CCommunicationSocket();
	virtual ~CCommunicationSocket();

	virtual int send(void * pbuf,int len);
	virtual int recv(void * pbuf,int len);
	virtual int reConnect();
	virtual int setRecvTimeout(DWORD waitMilSec);
	virtual int resetRecvTimeout(DWORD waitMilSec);
};

#endif // !defined(AFX_COMMUNICATIONSOCKET_H__5972555E_E568_4946_95BF_A76923726190__INCLUDED_)


