// WLEvent.h: interface for the CWLEvent class.
// 自定义事件，为了代码的可跨平台性，封装操作系统事件
// 唐小灿	2012-11-20
//////////////////////////////////////////////////////////////////////

#pragma once

#include "WLPlatformLayer.h" 

#ifdef WL_OS_CORE_WIN
#include <Windows.h>
#define event_handle HANDLE
#elif defined WL_OS_CORE_LINUX
#include <pthread.h>
#include <errno.h> 
#include <sys/time.h>  //+++++
#include <unistd.h>    //+++++
typedef struct def_event 
{
	bool state;
	bool manual_reset;
	pthread_mutex_t mutex;
	pthread_cond_t  cond;
}event_t;
#define event_handle event_t* 
#endif

class CWLEvent
{
public:
	CWLEvent(bool manual_reset, bool init_state);
	~CWLEvent();

	//返回值：NULL 出错 
//	event_handle event_create(bool manual_reset, bool init_state);

	//返回值：0 等到事件，-1出错  
	int event_wait();

	//返回值：0 等到事件，1 超时，-1出错  
	int event_timedwait(long milliseconds);

	//返回值：0 成功，-1出错  
	int event_set();

	//返回值：0 成功，-1出错  
	int event_reset();

public:
	static int s_waitForSingleObject(TWLHandle hHandle,DWORD64 dwMilliseconds);

private:
	//返回值：NULL 出错 
	event_handle event_create(bool manual_reset, bool init_state);

	//返回值：无  
	void event_destroy();

private:
	event_handle m_event;
};