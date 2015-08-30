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
���ܣ����캯������ʼ��һ���߳�
������createSuspended�������߳�ʱ���Ƿ��̹߳���1������0������
      �ٵ���resume�������������߳�
���أ���
���ߣ�Ҷ����	2012-08-24
�޸ģ���С��    2013-11-20
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
���ܣ���������
��������
���أ���
ע�⣺���ڱ���������TerminateThread�����̣߳����ܻ᲻̫�á����Ե�������
      �����̺߳������غ��ٵ��ñ�����
���ߣ�Ҷ����	2012-08-24
�޸ģ���С��    2013-11-20
*********************************************************************/
CWLThread::~CWLThread()
{
#ifdef WL_OS_CORE_WIN
	TerminateThread(m_hThread,0); //�˷�ʽ�����̲߳�̫�ã�������̺߳�������
	CloseHandle(m_hThread);
#elif defined WL_OS_CORE_LINUX
	if (m_pcs)  delete m_pcs;
#endif
}

/*********************************************************************
���ܣ��ָ��߳�ִ��
��������
���أ�-1��ʧ�ܣ�>=0���߳��ϴι��������
���ߣ�Ҷ����	2012-08-24
�޸ģ���С��    2013-11-20
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
���ܣ������߳�
��������
���أ�-1��ʧ�ܣ�>=0���߳��ϴι��������
���ߣ�Ҷ����	2012-08-24
�޸ģ���С��    2013-11-20
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
���ܣ���ȡ�߳����ȼ�
��������
���أ�-1��ʧ�ܣ�>=0���߳��ϴι��������
���ߣ�Ҷ����	2012-08-24
*********************************************************************/
//int CWLThread::getPriority()
//{
//	return GetThreadPriority(m_hThread);
//}

/*********************************************************************
���ܣ������߳����ȼ�
������priority �����ȼ� 
���أ�0��ʧ�ܣ�1���ɹ�
���ߣ�Ҷ����	2012-08-24
*********************************************************************/
//BOOL CWLThread::setPriority(int priority)
//{
//	return SetThreadPriority(m_hThread,priority);
//}

/*********************************************************************
���ܣ�ǿ�н����߳�
��������
���أ���
ע�⣺ǿ�н����߳̿��ܻ������ȶ����أ�������������Ҫ����
���ߣ�Ҷ����	2012-08-24
�޸ģ���С��    2013-11-20
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
���ܣ��̺߳����������߳������У��Ǳ���Ψһ�����߳������еĺ���
��������
���أ���ͬƽ̨���岻ͬ��һ�㲻�����
ע�⣺�߳�ִ�д����д�ڱ������У�����override����ʵ���߳��߼�
���ߣ�Ҷ����	2012-08-24
*********************************************************************/
int CWLThread::run()
{
	return 0;
}

/*********************************************************************
���ܣ�windows �̻߳ص�����
��������windows createthreadϵ�� ����
���أ���windows createthreadϵ�� ����
���ߣ�Ҷ����	2012-08-24
*********************************************************************/
#ifdef WL_OS_CORE_WIN
DWORD WINAPI CWLThread::ThreadProc( LPVOID lpParam )
{
	CWLThread *p = (CWLThread *)lpParam;
	return p->run();
}
#endif


/*********************************************************************
���ܣ�linux �̻߳ص�����
��������linux pthread_createϵ�� ����
���أ���linux pthread_createϵ�� ����
���ߣ���С��	2012-11-20
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
