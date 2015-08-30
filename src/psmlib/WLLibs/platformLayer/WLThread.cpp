// WLThread.cpp: implementation of the CWLThread class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WLThread.h"

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
功能：构造函数，初始化一个线程
参数：createSuspended，创建线程时，是否将线程挂起。1，挂起，0，运行
      再调用resume函数可再运行线程
返回：无
作者：叶文林	2012-08-24
修改：唐小灿    2013-11-20
*********************************************************************/
CWLThread::CWLThread(int createSuspended)
{
#ifdef WL_OS_CORE_WIN
	if (createSuspended != 0)	createSuspended = CREATE_SUSPENDED;
	else createSuspended = 0;
	m_hThread = CreateThread(NULL,0,ThreadProc,this,createSuspended,&m_threadID);
#elif defined WL_OS_CORE_LINUX
	if(createSuspended != 0)    m_runFlag = -1;
	else m_runFlag = 1;
	m_pcs = new CWLCriticalSection();
	m_hThread = pthread_create(&m_pthread_t, NULL, ThreadProc, this);
#endif
}

/*********************************************************************
功能：析构函数
参数：无
返回：无
注意：由于本函数采用TerminateThread结束线程，可能会不太好。所以调用者最
      好让线程函数返回后，再调用本函数
作者：叶文林	2012-08-24
修改：唐小灿    2013-11-20
*********************************************************************/
CWLThread::~CWLThread()
{
#ifdef WL_OS_CORE_WIN
	TerminateThread(m_hThread,0); //此方式结束线程不太好，最好是线程函数返回
	CloseHandle(m_hThread);
#elif defined WL_OS_CORE_LINUX
	if (m_pcs)  delete m_pcs;
#endif
}

/*********************************************************************
功能：恢复线程执行
参数：无
返回：-1，失败；>=0，线程上次挂起记数器
作者：叶文林	2012-08-24
修改：唐小灿    2013-11-20
*********************************************************************/
int CWLThread::resume()
{
#ifdef WL_OS_CORE_WIN
	return ResumeThread(m_hThread);
#elif defined WL_OS_CORE_LINUX
	m_runFlag = 1;
	return 0;
#endif
}

/*********************************************************************
功能：挂起线程
参数：无
返回：-1，失败；>=0，线程上次挂起记数器
作者：叶文林	2012-08-24
修改：唐小灿    2013-11-20
*********************************************************************/
int CWLThread::suspend()
{
#ifdef WL_OS_CORE_WIN
	return SuspendThread(m_hThread);
#elif defined WL_OS_CORE_LINUX
	m_runFlag = -1;
	return 0;
#endif
}

/*********************************************************************
功能：获取线程优先级
参数：无
返回：-1，失败；>=0，线程上次挂起记数器
作者：叶文林	2012-08-24
*********************************************************************/
//int CWLThread::getPriority()
//{
//	return GetThreadPriority(m_hThread);
//}

/*********************************************************************
功能：设置线程优先级
参数：priority ，优先级 
返回：0，失败；1，成功
作者：叶文林	2012-08-24
*********************************************************************/
//BOOL CWLThread::setPriority(int priority)
//{
//	return SetThreadPriority(m_hThread,priority);
//}

/*********************************************************************
功能：强行结束线程
参数：无
返回：无
注意：强行结束线程可能会引起不稳定因素，本函数尽量不要调用
作者：叶文林	2012-08-24
修改：唐小灿    2013-11-20
*********************************************************************/
void CWLThread::forceTerminate()
{
#ifdef WL_OS_CORE_WIN
	DWORD exitCode = 0;
	if ( GetExitCodeThread(m_hThread,&exitCode) ) 	TerminateThread(m_hThread,exitCode); 
#elif defined WL_OS_CORE_LINUX
	pthread_kill(m_pthread_t, 0);
#endif
}

/*********************************************************************
功能：线程函数，在新线程中运行，是本类唯一在新线程中运行的函数
参数：无
返回：不同平台意义不同，一般不用理会
注意：线程执行代码就写在本函数中，子类override它，实现线程逻辑
作者：叶文林	2012-08-24
*********************************************************************/
int CWLThread::run()
{
	return 0;
}

/*********************************************************************
功能：windows 线程回调函数
参数：见windows createthread系列 函数
返回：见windows createthread系列 函数
作者：叶文林	2012-08-24
*********************************************************************/
#ifdef WL_OS_CORE_WIN
DWORD WINAPI CWLThread::ThreadProc( LPVOID lpParam )
{
	CWLThread *p = (CWLThread *)lpParam;
	return p->run();
}
#endif


/*********************************************************************
功能：linux 线程回调函数
参数：见linux pthread_create系列 函数
返回：见linux pthread_create系列 函数
作者：唐小灿	2012-11-20
*********************************************************************/
#ifdef WL_OS_CORE_LINUX
void * CWLThread::ThreadProc(void* lpParam)
{
	CWLThread *p = (CWLThread *)lpParam;
	while (p->m_runFlag < 0)
	{
        //WLSleep(10);
	}

	p->run();

	return NULL;
}
#endif
