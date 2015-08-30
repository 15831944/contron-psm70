#include "stdafx.h"
#include "WLProcess.h"
#include "WLOutDebug.h"
#include "../fileSysterm/WLDirect.h"

CWLProcess::CWLProcess()
{
    m_appHandle = NULL;
    m_callbackAppExit = NULL;
#ifdef WL_OS_CORE_WIN
	m_hWnd = NULL;							//子进程控制台窗口的句柄
#endif
	m_runState = 0;
	memset(m_appPath,0,MAX_PATH);
	memset(m_appInfoPath,0,MAX_PATH);
}

CWLProcess::~CWLProcess()
{
    closeProcess();
}

/************************************************************
功能：注册进程退出时的回调函数
参数：callbackFunc，回调函数
	  pContext，上下文参数指针
返回：<0，失败
     >=0，成功
作者：刘远安	2014-12-04
************************************************************/
int CWLProcess::registerCallbackAppExit(callbackAppExit callbackFunc,void *pContext)
{
    int res,ret = -1;
    if(NULL == callbackFunc)    return ret;

	m_pContext = pContext;
    m_callbackAppExit = callbackFunc;
    return 0;
}

/*********************************************************************
功能：启动应用程序
参数：appPath，待调用应用程序完整路径
      pParameters，传递给待调用应用程序的参数，可以为NULL
      pWorkDir，指定待调用应用程序的工作目录，可以为NULL
返回：FALSE，失败；TRUE，成功
作者：刘远安	2014-12-04
*********************************************************************/
int CWLProcess::startProcess(const char *appPath, char *pParameters, const char *pWorkDir)
{
	int res,ret = -1;
	//子程序正在运行，将其关闭
	if(1 == m_runState)
	{
		closeProcess();
		WLSleep(200);
	}

	BOOL isSuccess = executeApp(appPath,pParameters,pWorkDir,&m_appHandle);
	if(FALSE == isSuccess)	return ret;

	ret--;
	m_pAppMonitorThd = new CAppMonitorThd(this);
	res = m_pAppMonitorThd->startMonitor(m_appHandle);
	if(res < 0)	return ret;

	m_runState = 1;
	return 0;
}

/************************************************************
功能：本类私有函数，进程退出时被调用
参数：exitCode，进程退出码，暂未意义
返回：<0，失败
     >=0，成功
作者：刘远安	2014-12-04
************************************************************/
int CWLProcess::notifyAppExit(DWORD32 exitCode)
{
	m_runState = 0;
	int res = appExitEvent(exitCode);
	if(1 == res)	return 1;

    if(NULL == m_callbackAppExit) return -1;

    m_callbackAppExit(exitCode,m_pContext);

    return 0;
}

/************************************************************
功能：关闭进程
参数：无
返回：<0，失败
     >=0，成功
作者：刘远安	2014-12-04
************************************************************/
int CWLProcess::closeProcess()
{
    int res,ret = -1;
    if(NULL == m_appHandle)	return ret;

    ret--;
#ifdef WL_OS_CORE_WIN
    BOOL isSuccess = TerminateProcess(m_appHandle,0);
    if(FALSE == isSuccess)	return ret;
#else ifdef WL_OS_CORE_LINUX
	DWORD64 value = (DWORD64)m_appHandle;
	pid_t pid = (pid_t)value;
	kill(pid,SIGABRT);
#endif
	
    return 0;
}

#ifdef WL_OS_CORE_WIN
HWND g_hWnd = NULL;
BOOL   CALLBACK  EnumWindowsProcGetWndTitle(HWND hWnd,LPARAM lParam)
{
	DWORD ProcID = 0;
	GetWindowThreadProcessId(hWnd,&ProcID);
	if(ProcID==lParam)//如果窗口的processid等于你的ID
	{
		HWND lastValidWnd = hWnd;
		HWND pParentWnd = GetParent(hWnd);
	
		//得到父窗口的句柄
		while(NULL != pParentWnd)
		{
			lastValidWnd = pParentWnd;
			pParentWnd = GetParent(pParentWnd);
		}

		g_hWnd = lastValidWnd;
		return FALSE;
	}
	return TRUE;
}
#endif

/************************************************************
功能：本类私有函数，启动进程
参数：appPath，被启动程序的绝对路径
	  pParameters，传递给待调用应用程序的参数，可以为NULL
	  pWorkDir，指定待调用应用程序的工作目录，可以为NULL
	  outHandle，windows下表示已启动线程的句柄，linux下表示启动线程的pid
返回：<0，失败
     >=0，成功
作者：刘远安	2014-12-04
************************************************************/
BOOL CWLProcess::executeApp( const char *appPath,char *pParameters,const char *pWorkDir,TWLHandle *outHandle )
{
	if(NULL == appPath)	return FALSE;
	memset(m_appPath,0,MAX_PATH);
	strcpy(m_appPath,appPath);

#ifdef WL_OS_CORE_WIN
	wchar_t wAppPath[MAX_PATH] = {0};
	WLchar2wchar(appPath,wAppPath,MAX_PATH);

	wchar_t wCommandLine[MAX_PATH] = {0};
	if(NULL != pParameters)
	{
		WLchar2wchar(pParameters,wCommandLine,MAX_PATH);
	}

	wchar_t wWorkDir[MAX_PATH] = {0};
	if(NULL != pWorkDir)
	{
		WLchar2wchar(pWorkDir,wWorkDir,MAX_PATH);
	}

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&si,0,sizeof(si));				//清零
	si.cb=sizeof(si);
	//si.wShowWindow=SW_SHOW;
	si.wShowWindow = SW_MINIMIZE;			//最小化
	si.dwFlags = STARTF_USESHOWWINDOW;
	BOOL bResult = CreateProcess(wAppPath,	// 执行程序名
		wCommandLine,						// 参数行
		//下面两个参数描述了所创建的进程和线程的安全属性，如果为NULL则使用默认的安全属性
		NULL,								// process security attributes
		NULL,								// thread security attributes
		FALSE,								// 继承标志
		CREATE_NEW_CONSOLE,					// 创建标志:使用新的窗口
		NULL,								// 环境变量
		wWorkDir,							// 运行该进程的初始目录
		&si,								// 用于在创建子进程时设置各种属性
		&pi									// 用于在进程创建后接受相关信息
		) ;
	if (TRUE == bResult)
	{
		*outHandle = pi.hProcess;
	}

	m_hWnd = NULL;
	int nNumberDely = 1000;
	DWORD dwProcessId = pi.dwProcessId;
	// 等待窗口动行起，获取到窗口HWND
	while(EnumWindows(EnumWindowsProcGetWndTitle,dwProcessId) && nNumberDely>0 )
	{
		nNumberDely--;
	}
	if(nNumberDely > 0)
	{
		m_hWnd = g_hWnd;
		WLOutDebug("executeApp m_hWnd = %p",m_hWnd);
	}
	return bResult;

#else ifdef WL_OS_CORE_LINUX
	int res,ret = -1;
    memset(m_appInfoPath,0,MAX_PATH);
    char subAppDir[MAX_PATH] = {0};
    WLGetFolderDir(appPath,subAppDir,MAX_PATH);
    char subAppName[MAX_PATH] = {0};
    int len = MAX_PATH;
    WLGetFileNameFromDir(appPath,subAppName,len);
    char subAppInfoPath[MAX_PATH] = {0};
    sprintf(subAppInfoPath,"%s/%s",subAppDir,subAppName);
    strcat(subAppInfoPath,"FIFO");
    strcpy(m_appInfoPath,subAppInfoPath);
    printf("subAppInfoPath %s\n", subAppInfoPath);

	pid_t pid;
	pid = fork();              // 创建子进程
	if(pid < 0)
	{
		WLOutDebug("fork() failure!");
		return FALSE;
	}
	else if(pid==0)
	{
		char *ps_argv[2] = {NULL,NULL};
		ps_argv[0] = pParameters;
		//设置新进程的工作目录
		if(NULL != pWorkDir)
		{
            /*
            char curWorkDir[MAX_PATH] = {0};
            char * dir = getcwd(curWorkDir, MAX_PATH);
            if(NULL == dir)	WLOutDebug("getcwd error");

            chdir(pWorkDir);
			int res = execv(appPath,ps_argv);
            chdir(curWorkDir);
            //*/



            if((mkfifo(m_appInfoPath, O_CREAT | O_EXCL) < 0) && (errno!= EEXIST))
				printf("cannot create fifoserver\n");

//			int fd = open(subAppInfoPath, O_WRONLY | O_NONBLOCK, 0);
//			if (fd < 0)
//				printf("cannot open fifoserver\n");

			char cmd[1024] = {0};
            sprintf(cmd, "%s > %s",appPath,m_appInfoPath);
			//sprintf(cmd, "/work/PSM70/bin/BigDataTransDir/BigDataTrans > /tmp/BigDataTransFIFO", pWorkDir, pParameters);
			//sprintf(cmd, "yes > /tmp/BigDataTransFIFO", pWorkDir, pParameters);

			char curWorkDir[MAX_PATH] = {0};
			char * dir = getcwd(curWorkDir, MAX_PATH);
			if(NULL == dir)	WLOutDebug("getcwd error");

			chdir(pWorkDir);
            res = system(cmd);
			chdir(curWorkDir);

            //res = execlp("gnome-terminal","gnome-terminal","--working-directory",pWorkDir,"-x",appPath,pParameters, NULL);
            if(res < 0)
            {
                    WLOutDebug("execv error!");
                    return FALSE;
            }
		}
		else
		{
            //execlp("gnome-terminal","gnome-terminal","-x",appPath,pParameters, NULL);
		    int res = execv(appPath,ps_argv);
            if(res < 0)
            {
                    WLOutDebug("execv error!");
                    return FALSE;
            }
		}
	}
        else
        {
            WLOutDebug("new process id=%d",pid);
            *outHandle = (void *)pid;
        }

	return TRUE;
#endif
}


/************************************************************
功能：子进程退出时业务处理函数
参数：exitCode，子进程退出码，暂无意义
返回：<0，失败
     >=0，成功
注意：如果本函数的返回值为1，notifyAppExit函数将停止处理余下的业务（即不调用进程退出的回调函数）
作者：刘远安	2014-12-04
************************************************************/
int CWLProcess::appExitEvent(DWORD32 exitCode)
{
	return 0;
}

int CWLProcess::hideProcess()
{
#ifdef WL_OS_CORE_WIN
	if(NULL == m_hWnd)	return -1;
	WLOutDebug("hideProcess m_hWnd = %p",m_hWnd);
	ShowWindow(m_hWnd, SW_HIDE);
	BOOL isVisible = IsWindowVisible(m_hWnd);
	int repeatTime = 100;
	while((TRUE == isVisible) && (repeatTime > 0))
	{
		WLSleep(10);
		isVisible = IsWindowVisible(m_hWnd);
		repeatTime--;
	}
#endif
	return 0;
}

int CWLProcess::showProcess()
{
#ifdef WL_OS_CORE_WIN
	if(NULL == m_hWnd)	return -1;

	WLOutDebug("showProcess m_hWnd = %p",m_hWnd);
	ShowWindow(m_hWnd, SW_SHOW);
	BOOL isVisible = IsWindowVisible(m_hWnd);
	int repeatTime = 100;
	while((FALSE == isVisible) && (repeatTime > 0))
	{
		WLSleep(10);
		isVisible = IsWindowVisible(m_hWnd);
		repeatTime--;
	}
#else ifdef WL_OS_CORE_LINUX
    /*
    memset(m_appInfoPath,0,MAX_PATH);
    char subAppDir[MAX_PATH] = {0};
    WLGetFolderDir(m_appPath,subAppDir,MAX_PATH);
    char subAppName[MAX_PATH] = {0};
    int len = MAX_PATH;
    WLGetFileNameFromDir(m_appPath,subAppName,len);
    char subAppInfoPath[MAX_PATH] = {0};
    sprintf(subAppInfoPath,"%s/%s",subAppDir,subAppName);
    strcat(subAppInfoPath,"FIFO");
    strcpy(m_appInfoPath,subAppInfoPath);
    //*/

    char cmd[MAX_PATH] = {0};
    printf("m_appInfoPath = %s\n",m_appInfoPath);
    sprintf(cmd,"gnome-terminal -x bash -c 'cat %s'",m_appInfoPath);
    system(cmd);
//    system("gnome-terminal -x bash -c 'cat /liuyuanan/testFIFO'");
                //gnome-terminal -x bash -c "echo \"OK now\"; read"
//        execlp("gnome-terminal","-x","cat liuyuanan/testFIFO", NULL);
#endif
	return 0;
}

CAppMonitorThd::CAppMonitorThd(CWLProcess *pWLProcess)
{
    m_pWLProcess = pWLProcess;
    m_isStart = false;
}

CAppMonitorThd::~CAppMonitorThd()
{
}

/************************************************************
功能：启动子进程监控
参数：handle，，windows下表示已启动线程的句柄，linux下表示启动线程的pid
返回：<0，失败
     >=0，成功
作者：刘远安	2014-12-04
************************************************************/
int CAppMonitorThd::startMonitor(TWLHandle handle)
{
    int ret = -1;
    if(NULL == handle)	return ret;

    //是否已经启动了监控
    ret--;
    if(m_isStart)	return ret;

    m_handle  = handle;
    m_isStart = true;
    return 0;
}

/************************************************************
功能：线程执行函数
参数：无
返回：<0，失败
     >=0，成功
作者：刘远安	2014-12-04
************************************************************/
int CAppMonitorThd::run()
{
    int res,ret = -1;
    DWORD32 exitCode = 0;
    while(1)
    {
        if(!m_isStart)
        {
            WLSleep(10);
            continue;
        }

        if(NULL == m_handle)	continue;

#ifdef WL_OS_CORE_WIN
        DWORD objectCode = WaitForSingleObject(m_handle, INFINITE);//无限超时
    //	if (objectCode == WAIT_OBJECT_0)  return 0;
    //	return -1;

#else ifdef WL_OS_CORE_LINUX
        DWORD64 value = (DWORD64)m_handle;
        pid_t pid = (pid_t)value;
        int status;
        waitpid(pid, &status, 0);    //等待子进程退出
        exitCode = status;

#endif
        WLOutDebug("子程序退出了：exitCode=%d",exitCode);
        break;
    }

    //通知主线程：有子程序退出了
    m_pWLProcess->notifyAppExit(exitCode);
    return 0;
}
