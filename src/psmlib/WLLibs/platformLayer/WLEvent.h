// WLEvent.h: interface for the CWLEvent class.
// �Զ����¼���Ϊ�˴���Ŀɿ�ƽ̨�ԣ���װ����ϵͳ�¼�
// ��С��	2012-11-20
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

	//����ֵ��NULL ���� 
//	event_handle event_create(bool manual_reset, bool init_state);

	//����ֵ��0 �ȵ��¼���-1����  
	int event_wait();

	//����ֵ��0 �ȵ��¼���1 ��ʱ��-1����  
	int event_timedwait(long milliseconds);

	//����ֵ��0 �ɹ���-1����  
	int event_set();

	//����ֵ��0 �ɹ���-1����  
	int event_reset();

public:
	static int s_waitForSingleObject(TWLHandle hHandle,DWORD64 dwMilliseconds);

private:
	//����ֵ��NULL ���� 
	event_handle event_create(bool manual_reset, bool init_state);

	//����ֵ����  
	void event_destroy();

private:
	event_handle m_event;
};