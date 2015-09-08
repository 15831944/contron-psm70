#ifndef PLATFORM_H
#define PLATFORM_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

//UCHAR
#ifndef UCHAR
typedef unsigned char UCHAR;
#endif

//USHORT
#ifndef USHORT
typedef unsigned short USHORT;
#endif

typedef union {
    double d;
    char bytes[sizeof(double)];
} DOUBLE_CONVERTER;

//日志
#define APP_LOG _log

/*!
    使用winsock 2.2
*/
#if WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Winsock2.h>
typedef SOCKET SOCKET_HANDLE;
#ifndef __MINGW32__
#pragma comment(lib, "Ws2_32.lib")
#endif
#ifndef SOL_TCP
#define SOL_TCP 6
#endif
#ifndef TCP_KEEPIDLE
#define TCP_KEEPIDLE 4
#endif
#ifndef TCP_KEEPINTVL
#define TCP_KEEPINTVL 5
#endif
#ifndef TCP_KEEPCNT
#define TCP_KEEPCNT 6
#endif
#ifndef socklen_t
#define socklen_t int
#endif
#else
typedef int SOCKET_HANDLE;
#include <netinet/tcp.h>
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif
#define closesocket(socket) close(socket)
#define INVALID_SOCKET -1
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#if WIN32

#ifndef __MINGW32__
#define snprintf _snprintf
#define usleep Sleep
#define strcasecmp stricmp
#endif

#else

//usleep
#include <unistd.h>
#define Sleep usleep

#endif

/*!
    线程
*/
#if WIN32 && !defined(__MINGW32__)

typedef HANDLE pthread_t;

#define THREAD_API DWORD WINAPI
#define THREAD_CREATE(handle, func, param, ret) \
    *handle = CreateThread(NULL, 0, func, param, 0, NULL); \
    ret = (NULL != handle);
#define THREAD_RUN(handle, block) \
	if(block) \
	{ \
		WaitForSingleObject(handle, INFINITE); \
	} \
	else \
	{ \
		WaitForSingleObject(handle, 0); \
	} 
/*!
  \brief 等待线程退出
  \param handle 线程句柄
  \note 只能用在循环内，线程如果已退出，则退出当前循环
 */
#define THREAD_WAITEXIT(handle) \
    { \
        DWORD exit_code; \
        pthread_t thread_handle = GetCurrentThread(); \
        int ret = GetExitCodeThread(thread_handle, &exit_code); \
        if(0==ret) break; \
        if((0!=ret) && (STILL_ACTIVE!=exit_code)) break; \
    }
#define THREAD_CLOSE(handle) \
	TerminateThread(handle, 0); \
    CloseHandle(handle); \
	handle = NULL;

#else

#include <pthread.h>
#define THREAD_CREATE(handle, func, param, ret) \
    ret = (0 == pthread_create(handle, NULL, func, param));
#define THREAD_RUN(handle, block) \
    if(block) \
    { \
		pthread_join(handle, NULL); \
    } \
    else \
    { \
		pthread_detach(handle); \
    }
/*!
  \brief 等待线程退出
  \param handle 此参数无用，只为了与windows下调用兼容
  \note 调用THREAD_CLOSE只是向线程发送了CANCEL信号，线程接收到CANCEL信号默认执行到取消点才退出
 */
#define THREAD_WAITEXIT(handle) \
    pthread_testcancel();
#define THREAD_CLOSE(handle) \
    pthread_cancel(handle);

#define THREAD_API void *

#endif

/*!
  临界区
*/
#if WIN32 && !defined(__MINGW32__)

#define MUTEX_INIT(critical) \
    InitializeCriticalSection(critical);

#define MUTEX_FREE(critical) \
    DeleteCriticalSection(critical);

#define MUTEX_LOCK(critical) \
    EnterCriticalSection(critical);

#define MUTEX_UNLOCK(critical) \
    LeaveCriticalSection(critical);
#else
#include <pthread.h>
#define  CRITICAL_SECTION   pthread_mutex_t
/*!
  临界区初始化，允许递归加锁
*/
#define MUTEX_INIT(critical) \
    pthread_mutex_init((CRITICAL_SECTION *)critical, NULL); \
    { \
        pthread_mutexattr_t attr; \
        pthread_mutexattr_init(&attr); \
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE_NP); \
        pthread_mutex_init((CRITICAL_SECTION *)critical, &attr); \
        pthread_mutexattr_destroy(&attr); \
    } \

#define MUTEX_FREE(critical) \
    pthread_mutex_destroy((CRITICAL_SECTION *)critical);

#define MUTEX_LOCK(critical) \
    pthread_mutex_lock((CRITICAL_SECTION *)critical);

#define MUTEX_UNLOCK(critical) \
    pthread_mutex_unlock((CRITICAL_SECTION *)critical);

#endif

/*!
    时间
*/
//gettimeofday
#if WIN32 && defined(__MINGW32__)
#include <sys/time.h>
#endif
#if !WIN32
#include <sys/time.h>
#endif

#include <time.h>
#include <sys/timeb.h>

#define MAX_STRING          10240
#define NUMBER_ERROR        -1
#define NUMBER_NOERROR      0
#define NUMBER_SUCCESS      1
#define NUMBER_FALSE        0
#define NUMBER_TRUE         1


#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

/*!
 * \brief GET_TIME  取当前时间
 * \param now  当前时间
 * 当前时间整形为秒值，小数为毫秒
 */
#if WIN32 && !defined(__MINGW32__)
#define GET_TIME(now) \
	{ \
		struct timeval tp[1]; \
		time_t clock; \
		struct tm tm; \
		SYSTEMTIME wtm; \
		GetLocalTime(&wtm); \
		tm.tm_year     = wtm.wYear - 1900; \
		tm.tm_mon     = wtm.wMonth - 1; \
		tm.tm_mday     = wtm.wDay; \
		tm.tm_hour     = wtm.wHour; \
		tm.tm_min     = wtm.wMinute; \
		tm.tm_sec     = wtm.wSecond; \
		tm. tm_isdst    = -1; \
		clock = mktime(&tm); \
		tp->tv_sec = clock; \
		tp->tv_usec = wtm.wMilliseconds * 1000; \
		now = ( (double) tp->tv_sec + (double) tp->tv_usec / 1000000 ); \
	} 
#else
#define GET_TIME(now) \
	{ \
		struct timeval time[1]; \
		gettimeofday( time, 0 ); \
		now = ( (double) time->tv_sec + (double) time->tv_usec / 1000000 ); \
	} 
#endif

#define UN_USE(x) (void)x


#endif // PLATFORM_H

