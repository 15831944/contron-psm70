// CommunicationSocket.cpp: implementation of the CCommunicationSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommunicationSocket.h"

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
功能：构造函数，作初始化工作
参数：无
返回：无
作者：叶文林	2012-08-24
*********************************************************************/
CCommunicationSocket::CCommunicationSocket()
{

}

/*********************************************************************
功能：析构函数
参数：无
返回：无
作者：叶文林	2012-08-24
*********************************************************************/
CCommunicationSocket::~CCommunicationSocket()
{

}

/*********************************************************************
功能：发送数据
参数：pbuf，发送缓冲
      len，发送字节数
返回：<0，错误；>=0，成功发送字节数
作者：叶文林	2012-08-24
*********************************************************************/
int CCommunicationSocket::send(void * pbuf,int len)
{
	return 0;
}

/*********************************************************************
功能：接收数据
参数：pbuf，接收缓冲
      len，接收字节数
返回：<0，错误；>=0，成功接收字节数
作者：叶文林	2012-08-24
*********************************************************************/
int CCommunicationSocket::recv(void * pbuf,int len)
{
	return 0;
}

/*********************************************************************
功能：重新连接（对串口、TCP有效、UDP无效）
参数：无
返回：<0，错误；>=0，成功
作者：叶文林	2012-08-24
*********************************************************************/
int CCommunicationSocket::reConnect()
{
	return 0;
}

/*********************************************************************
功能：复位收数据超时时间
参数：waitMilSec，等待时间，单位毫秒
返回：<0，错误；>=0，成功
注意：请子类设计默认读数据不超时，一直阻塞。
      一般情况下，不要调用本接口。
作者：叶文林	2012-09-28
*********************************************************************/
int CCommunicationSocket::resetRecvTimeout(DWORD waitMilSec)
{
	return 0;
}

/*********************************************************************
功能：设置收数据超时时间
参数：waitMilSec，等待时间，单位毫秒
返回：<0，错误；>=0，成功
注意：请子类设计默认读数据不超时，一直阻塞。
      一般情况下，不要调用本接口。
作者：叶文林	2012-09-28
*********************************************************************/
int CCommunicationSocket::setRecvTimeout(DWORD waitMilSec)
{
	return 0;
}