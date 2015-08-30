// COMSocket.h: interface for the CCOMSocket class.
// ���崮��ͨ���࣬�̳���CCommunicationSocket���ǶԴ���ͨ�ŷ�ʽ�ķ�װ
// Ŀ�ģ�Ϊ����Ӧ�����ͨ�ŷ�ʽ��ȷ��������������ͨ�Žӿڣ����磺
//       ĳӦ�ý���Ҫ�ô���ͨ�ţ����ǵ�ǰ�����޴��ڣ�����ʱʱ��UDP���档
//       �������ͨ�Žӿ���һ���ܺõķ�װ����������Ķ����ϴ󣬱������
//       ���ǽ�����������
// ע�⣺����ֻ�Ƿ���ͨ�ţ�����ʵ�ִ������й��ܡ�
// ���ߣ�Ҷ����	2012-08-24
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
	HANDLE m_hComm;                //���ھ��
#ifdef WL_OS_CORE_WIN
	BOOL setTimeOuts(COMMTIMEOUTS timeouts) ; //���ÿڴ��ڶ�д��ʱʱ�䣬���ı�Ĭ��ֵ��һ�������ֻ����� setRecvTime�����ص��ñ��ӿ�
#elif defined WL_OS_CORE_LINUX
#endif
	char m_portName[8];            //��������
	DWORD m_baud;                  //������
#ifdef WL_OS_CORE_WIN
	COMMTIMEOUTS m_timeOuts;       //�����շ���ʱʱ��
#elif defined WL_OS_CORE_LINUX
#endif
};

#endif // !defined(AFX_COMSOCKET_H__5E7C8613_1890_47C9_BF26_AC040E080478__INCLUDED_)
