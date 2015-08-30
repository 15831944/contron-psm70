// WLThread.h: interface for the CWLThread class.
// 功能：封线程类（winodws版），本类可能出现版本有：windows版、QT版、linux版
//       所有版本public、protected接口都必须相同
// 目的：为了适应跨平台移值。本类与 WLCriticalSection 等是一个系列的
// 作者：叶文林	2012-08-24
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
	virtual int run(); //线程函数(子类override它，实现线程逻辑)

#ifdef WL_OS_CORE_WIN
	//以下只与平台相关，下面定义的为 windows平台
	HANDLE m_hThread;                 //线程句柄
	DWORD  m_threadID;                //线程ID
#elif defined WL_OS_CORE_LINUX
	int  m_runFlag;                   //线程运行标志，1运行中，-1挂起
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
