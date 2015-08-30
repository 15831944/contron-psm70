// WLThread.h: interface for the CWLThread class.
// ���ܣ����߳��ࣨwinodws�棩��������ܳ��ְ汾�У�windows�桢QT�桢linux��
//       ���а汾public��protected�ӿڶ�������ͬ
// Ŀ�ģ�Ϊ����Ӧ��ƽ̨��ֵ�������� WLCriticalSection ����һ��ϵ�е�
// ���ߣ�Ҷ����	2012-08-24
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WLTHREAD_H__50869A6C_59AA_49F5_85D4_C0B34ABF5EB9__INCLUDED_)
#define AFX_WLTHREAD_H__50869A6C_59AA_49F5_85D4_C0B34ABF5EB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WLPlatformLayer.h" 
#include "WLCriticalSection.h"

#ifdef WL_OS_CORE_WIN
#include "windows.h"
#elif defined WL_OS_CORE_LINUX
#include <pthread.h>
#include <signal.h>  //+++++
#endif
//#include "windows.h"

class CWLThread  
{
public:
	CWLThread(int createSuspended = 0);
	virtual ~CWLThread();

	int resume();
	int suspend();
	//int getPriority();
	//BOOL setPriority(int priority);
	void forceTerminate();
protected:
	virtual int run(); //�̺߳���(����override����ʵ���߳��߼�)

#ifdef WL_OS_CORE_WIN
	//����ֻ��ƽ̨��أ����涨���Ϊ windowsƽ̨
	HANDLE m_hThread;                 //�߳̾��
	DWORD  m_threadID;                //�߳�ID
#elif defined WL_OS_CORE_LINUX
	int  m_runFlag;                   //�߳����б�־��1�����У�-1����
	int  m_hThread;
	pthread_t           m_pthread_t;
	CWLCriticalSection *m_pcs;
#endif

private:
#ifdef WL_OS_CORE_WIN
	static DWORD WINAPI ThreadProc( LPVOID lpParam );
#elif defined WL_OS_CORE_LINUX
	static void * ThreadProc(void* lpParam);
#endif
};

#endif // !defined(AFX_WLTHREAD_H__50869A6C_59AA_49F5_85D4_C0B34ABF5EB9__INCLUDED_)
