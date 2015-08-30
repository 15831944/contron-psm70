// WLCriticalSection.h: interface for the CWLCriticalSection class.
// 自定义临界区，为了代码的可跨平台性，封装操作系统临界区
// 叶文林	2012-08-22
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WLCRITICALSECTION_H__51278281_C11E_4D0F_ADC2_B67F4D16FD55__INCLUDED_)
#define AFX_WLCRITICALSECTION_H__51278281_C11E_4D0F_ADC2_B67F4D16FD55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WLDEF.h" 

#ifdef WL_OS_CORE_WIN
#include "windows.h"
#elif defined (WL_OS_CORE_LINUX)
#include <pthread.h>
#endif

class CWLCriticalSection
{
public:
	CWLCriticalSection(){
#ifdef WL_OS_CORE_WIN
		InitializeCriticalSection(&m_CriticalSection);
#elif defined WL_OS_CORE_LINUX
		pthread_mutex_init(&m_CriticalSection, NULL);
#endif

	};
	virtual ~CWLCriticalSection(){
#ifdef WL_OS_CORE_WIN
		DeleteCriticalSection(&m_CriticalSection);
#elif defined WL_OS_CORE_LINUX
		pthread_mutex_destroy(&m_CriticalSection);
#endif
//		DeleteCriticalSection(&m_CriticalSection);
	};

	void enter()
	{
#ifdef WL_OS_CORE_WIN
		EnterCriticalSection(&m_CriticalSection);
#elif defined WL_OS_CORE_LINUX
		pthread_mutex_lock(&m_CriticalSection);
#endif
		//EnterCriticalSection(&m_CriticalSection);
	};


	void leave()
	{
#ifdef WL_OS_CORE_WIN
		LeaveCriticalSection(&m_CriticalSection);
#elif defined WL_OS_CORE_LINUX
		pthread_mutex_unlock(&m_CriticalSection);
#endif
//		LeaveCriticalSection(&m_CriticalSection);
	};


	BOOL tryEnter()
	{
		int ret = 0;
#ifdef WL_OS_CORE_WIN
		ret = TryEnterCriticalSection(&m_CriticalSection);
		if (0 == ret)  return FALSE;
		else           return TRUE;
#elif defined WL_OS_CORE_LINUX
		ret = pthread_mutex_trylock(&m_CriticalSection);
		if (0 == ret)  return TRUE;
		else           return FALSE;
#endif
//		return TryEnterCriticalSection(&m_CriticalSection);
	}; 


private:
#ifdef WL_OS_CORE_WIN
	CRITICAL_SECTION m_CriticalSection;  //windows下的临界区结构
#elif defined WL_OS_CORE_LINUX
	pthread_mutex_t m_CriticalSection;
#endif
//	CRITICAL_SECTION m_CriticalSection;  //windows下的临界区结构
};

#endif // !defined(AFX_WLCRITICALSECTION_H__51278281_C11E_4D0F_ADC2_B67F4D16FD55__INCLUDED_)

