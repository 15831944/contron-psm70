#include "stdafx.h"
#include "WLEvent.h"

#ifdef WL_IDE_VC6
	#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[]=__FILE__;
	#define new DEBUG_NEW
	#endif
#endif
//manual_resetָ�����¼����󴴽����ֶ���ԭ�����Զ���ԭ�������TRUE����ô������ResetEvent�������ֹ����¼���״̬��ԭ�����ź�״̬���������ΪFALSE����һ���ȴ��̱߳��ͷ��Ժ�ϵͳ�����Զ����¼�״̬��ԭΪ���ź�״̬��
	//init_stateָ���¼�����ĳ�ʼ״̬�����ΪTRUE����ʼ״̬Ϊ���ź�״̬������Ϊ���ź�״̬��
CWLEvent::CWLEvent(bool manual_reset, bool init_state)
{
	m_event = event_create(manual_reset, init_state);
}


CWLEvent::~CWLEvent()
{
	event_destroy();
}


event_handle CWLEvent::event_create(bool manual_reset, bool init_state)
{
#ifdef WL_OS_CORE_WIN

	m_event = CreateEvent(NULL, manual_reset, init_state, NULL);

#elif defined WL_OS_CORE_LINUX

	m_event = new event_t;
	if (NULL == m_event)  return NULL;

	m_event->state = init_state;
	m_event->manual_reset = manual_reset;

	if (pthread_mutex_init(&m_event->mutex, NULL))
	{
		delete m_event;
		return NULL;
	}

	if (pthread_cond_init(&m_event->cond, NULL))  
	{  
		pthread_mutex_destroy(&m_event->mutex);  
		delete m_event;  
		return NULL;  
	}  

#endif

	return m_event;
}


int CWLEvent::event_wait()
{
#ifdef WL_OS_CORE_WIN

	DWORD ret = WaitForSingleObject(m_event, INFINITE);  
	if (ret == WAIT_OBJECT_0)  return 0; 
	return -1;

#elif defined WL_OS_CORE_LINUX

	if (pthread_mutex_lock(&m_event->mutex))  return -1;      
	while (!m_event->state)      
	{        
		if (pthread_cond_wait(&m_event->cond, &m_event->mutex))     
		{     
			pthread_mutex_unlock(&m_event->mutex);   
			return -1;     
		}     
	}

	if (!m_event->manual_reset)                 m_event->state = false;
	if (pthread_mutex_unlock(&m_event->mutex))  return -1; 
	return 0; 

#endif
}


int CWLEvent::event_timedwait(long milliseconds)
{
#ifdef WL_OS_CORE_WIN

	DWORD ret = WaitForSingleObject(m_event, milliseconds);  
	if (ret == WAIT_OBJECT_0)  return 0;
	if (ret == WAIT_TIMEOUT)   return 1;
	return -1; 

#elif defined WL_OS_CORE_LINUX

	int rc = 0;     
	struct timespec abstime;     
	struct timeval tv;     
	gettimeofday(&tv, NULL);     
	abstime.tv_sec  = tv.tv_sec + milliseconds / 1000;     
	abstime.tv_nsec = tv.tv_usec*1000 + (milliseconds % 1000)*1000000;     
	if (abstime.tv_nsec >= 1000000000)     
	{     
		abstime.tv_nsec -= 1000000000;     
		abstime.tv_sec++;     
	}     

	if (pthread_mutex_lock(&m_event->mutex) != 0)     
	{       
		return -1;     
	}     
	while (!m_event->state)      
	{        
		if (rc = pthread_cond_timedwait(&m_event->cond, &m_event->mutex, &abstime))     
		{     
			if (rc == ETIMEDOUT) break;     
			pthread_mutex_unlock(&m_event->mutex);      
			return -1;     
		}     
	}     
	if (rc == 0 && !m_event->manual_reset)     
	{  
		m_event->state = false;  
	}  
	if (pthread_mutex_unlock(&m_event->mutex) != 0)     
	{        
		return -1;     
	}  
	if (rc == ETIMEDOUT)  
	{  
		//timeout return 1  
		return 1;  
	}  
	//wait event success return 0  
	return 0; 

#endif
}


int CWLEvent::event_set()
{
#ifdef WL_OS_CORE_WIN

	return !SetEvent(m_event);

#elif defined WL_OS_CORE_LINUX

	if (pthread_mutex_lock(&m_event->mutex) != 0)  
	{  
		return -1;  
	}  

	m_event->state = true;  

	if (m_event->manual_reset)  
	{  
		if(pthread_cond_broadcast(&m_event->cond))  
		{  
			return -1;  
		}  
	}  
	else  
	{  
		if(pthread_cond_signal(&m_event->cond))  
		{  
			return -1;  
		}  
	}  

	if (pthread_mutex_unlock(&m_event->mutex) != 0)  
	{  
		return -1;  
	}  

	return 0;

#endif
}


int CWLEvent::event_reset()
{
#ifdef WL_OS_CORE_WIN

	//ResetEvent ���ط����ʾ�ɹ�  
	if (ResetEvent(m_event))  
	{  
		return 0;  
	}   
	return -1;

#elif defined WL_OS_CORE_LINUX

	if (pthread_mutex_lock(&m_event->mutex) != 0)  
	{  
		return -1;  
	}  

	m_event->state = false;  

	if (pthread_mutex_unlock(&m_event->mutex) != 0)  
	{        
		return -1;  
	}  
	return 0;

#endif
}


void CWLEvent::event_destroy()
{
#ifdef WL_OS_CORE_WIN

	CloseHandle(m_event);

#elif defined WL_OS_CORE_LINUX

	pthread_cond_destroy(&m_event->cond);  
	pthread_mutex_destroy(&m_event->mutex);  
	delete m_event;

#endif
}

/************************************************************
���ܣ��ȴ�����¼�
������hHandle�����ȴ�����¼�
	  dwMilliseconds���ȴ���ʱʱ�䣬windows��INFINITE������ʾ���޳�ʱ
���أ�<0��ʧ��
     >=0���ɹ�
ע�⣺������Ӧ�ó����·�������пո�����пո���Ҫ����ִ���ļ�����·������˫����
���ߣ���Զ��	2014-11-24
************************************************************/
int CWLEvent::s_waitForSingleObject( TWLHandle hHandle,DWORD64 dwMilliseconds )
{
#ifdef WL_OS_CORE_WIN
	DWORD ret = WaitForSingleObject(hHandle, dwMilliseconds);  
	if (ret == WAIT_OBJECT_0)  return 0; 
	return -1;

#elif defined WL_OS_CORE_LINUX

	return -1; 

#endif
}
