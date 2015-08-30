// CommunicationSocket.h: interface for the CCommunicationSocket class.
// ����һ��ͨ��ͨ�Žӿڻ��࣬���ཫʵ�� UDP��TCP�������Լ�������ͨ�ŷ�ʽ
// Ŀ�ģ�Ϊ����Ӧ�����ͨ�ŷ�ʽ��ȷ��������������ͨ�Žӿڣ����磺
//       ĳӦ�ý���Ҫ�ô���ͨ�ţ����ǵ�ǰ�����޴��ڣ�����ʱʱ��UDP���档
//       �������ͨ�Žӿ���һ���ܺõķ�װ����������Ķ����ϴ󣬱����
//       ���ǽ�����������
// ע�⣺������Կ��������࣬�벻Ҫʵ�������ࡣ���������
//       CTCPSocket��CUDPSocket��CCOMSocket
// ���ߣ�Ҷ����	2012-08-24
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


