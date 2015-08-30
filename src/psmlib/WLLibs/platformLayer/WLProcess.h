/**************************************************************************************************
��  �ܣ����ǽ��������������֧࣬�ֿ�ƽ̨

��  �ߣ���Զ��  2014-12-04
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

//�����˳��ص�����
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
    TWLHandle m_appHandle;					//windows�±�ʾ�������̵߳ľ����linux�±�ʾ�����̵߳�pid
    CAppMonitorThd *m_pAppMonitorThd;
    callbackAppExit m_callbackAppExit;
	void *m_pContext;						//�ص�����������ָ��
	int m_runState;							//����״̬��0��δ���У�1����������
	char m_appPath[MAX_PATH];
	char m_appInfoPath[MAX_PATH];
#ifdef WL_OS_CORE_WIN
	HWND m_hWnd;							//�ӽ��̿���̨���ڵľ��
#endif
};

//�̼߳���߳�
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
