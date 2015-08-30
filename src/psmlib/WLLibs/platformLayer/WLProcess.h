/**************************************************************************************************
功  能：这是进程启动及管理类，支持跨平台

作  者：刘远安  2014-12-04
**************************************************************************************************/
#ifndef WLPROCESS_H
#define WLPROCESS_H

#include "WLDEF.h"
#include "WLThread.h"

#ifdef WL_OS_CORE_WIN

#elif defined WL_OS_CORE_LINUX
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#endif

//进程退出回调函数
typedef int (* callbackAppExit)(DWORD32 exitCode,void *pContext);

class CAppMonitorThd;
class CWLProcess
{
public:
    CWLProcess();
    ~CWLProcess();

    int registerCallbackAppExit(callbackAppExit callbackFunc,void *pContext);
    int startProcess(const char *appPath,char *pParameters,const char *pWorkDir);
    int closeProcess();
	int hideProcess();
	int showProcess();
    friend class CAppMonitorThd;

protected:
	virtual int appExitEvent(DWORD32 exitCode);
    int notifyAppExit(DWORD32 exitCode);
	BOOL executeApp(const char *appPath,char *pParameters,const char *pWorkDir,TWLHandle *outHandle);

protected:
    TWLHandle m_appHandle;					//windows下表示已启动线程的句柄，linux下表示启动线程的pid
    CAppMonitorThd *m_pAppMonitorThd;
    callbackAppExit m_callbackAppExit;
	void *m_pContext;						//回调函数上下文指针
	int m_runState;							//运行状态：0，未运行；1，正在运行
	char m_appPath[MAX_PATH];
	char m_appInfoPath[MAX_PATH];
#ifdef WL_OS_CORE_WIN
	HWND m_hWnd;							//子进程控制台窗口的句柄
#endif
};

//线程监控线程
class CAppMonitorThd :public CWLThread
{
public:
    CAppMonitorThd(CWLProcess *pWLProcess);
    ~CAppMonitorThd();
    int startMonitor(TWLHandle handle);

protected:
    virtual int run();

private:
    CWLProcess *m_pWLProcess;
    bool m_isStart;
    TWLHandle m_handle;
};

#endif // WLPROCESS_H
