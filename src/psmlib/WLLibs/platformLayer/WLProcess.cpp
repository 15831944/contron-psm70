#include "stdafx.h"
#include "WLProcess.h"
#include "WLOutDebug.h"
#include "../fileSysterm/WLDirect.h"

CWLProcess::CWLProcess()
{
    m_appHandle = NULL;
    m_callbackAppExit = NULL;
#ifdef WL_OS_CORE_WIN
	m_hWnd = NULL;							//�ӽ��̿���̨���ڵľ��
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
���ܣ�ע������˳�ʱ�Ļص�����
������callbackFunc���ص�����
	  pContext�������Ĳ���ָ��
���أ�<0��ʧ��
     >=0���ɹ�
���ߣ���Զ��	2014-12-04
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
���ܣ�����Ӧ�ó���
������appPath��������Ӧ�ó�������·��
      pParameters�����ݸ�������Ӧ�ó���Ĳ���������ΪNULL
      pWorkDir��ָ��������Ӧ�ó���Ĺ���Ŀ¼������ΪNULL
���أ�FALSE��ʧ�ܣ�TRUE���ɹ�
���ߣ���Զ��	2014-12-04
*********************************************************************/
int CWLProcess::startProcess(const char *appPath, char *pParameters, const char *pWorkDir)
{
	int res,ret = -1;
	//�ӳ����������У�����ر�
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
���ܣ�����˽�к����������˳�ʱ������
������exitCode�������˳��룬��δ����
���أ�<0��ʧ��
     >=0���ɹ�
���ߣ���Զ��	2014-12-04
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
���ܣ��رս���
��������
���أ�<0��ʧ��
     >=0���ɹ�
���ߣ���Զ��	2014-12-04
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
	if(ProcID==lParam)//������ڵ�processid�������ID
	{
		HWND lastValidWnd = hWnd;
		HWND pParentWnd = GetParent(hWnd);
	
		//�õ������ڵľ��
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
���ܣ�����˽�к�������������
������appPath������������ľ���·��
	  pParameters�����ݸ�������Ӧ�ó���Ĳ���������ΪNULL
	  pWorkDir��ָ��������Ӧ�ó���Ĺ���Ŀ¼������ΪNULL
	  outHandle��windows�±�ʾ�������̵߳ľ����linux�±�ʾ�����̵߳�pid
���أ�<0��ʧ��
     >=0���ɹ�
���ߣ���Զ��	2014-12-04
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
	memset(&si,0,sizeof(si));				//����
	si.cb=sizeof(si);
	//si.wShowWindow=SW_SHOW;
	si.wShowWindow = SW_MINIMIZE;			//��С��
	si.dwFlags = STARTF_USESHOWWINDOW;
	BOOL bResult = CreateProcess(wAppPath,	// ִ�г�����
		wCommandLine,						// ������
		//�������������������������Ľ��̺��̵߳İ�ȫ���ԣ����ΪNULL��ʹ��Ĭ�ϵİ�ȫ����
		NULL,								// process security attributes
		NULL,								// thread security attributes
		FALSE,								// �̳б�־
		CREATE_NEW_CONSOLE,					// ������־:ʹ���µĴ���
		NULL,								// ��������
		wWorkDir,							// ���иý��̵ĳ�ʼĿ¼
		&si,								// �����ڴ����ӽ���ʱ���ø�������
		&pi									// �����ڽ��̴�������������Ϣ
		) ;
	if (TRUE == bResult)
	{
		*outHandle = pi.hProcess;
	}

	m_hWnd = NULL;
	int nNumberDely = 1000;
	DWORD dwProcessId = pi.dwProcessId;
	// �ȴ����ڶ����𣬻�ȡ������HWND
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
	pid = fork();              // �����ӽ���
	if(pid < 0)
	{
		WLOutDebug("fork() failure!");
		return FALSE;
	}
	else if(pid==0)
	{
		char *ps_argv[2] = {NULL,NULL};
		ps_argv[0] = pParameters;
		//�����½��̵Ĺ���Ŀ¼
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
���ܣ��ӽ����˳�ʱҵ������
������exitCode���ӽ����˳��룬��������
���أ�<0��ʧ��
     >=0���ɹ�
ע�⣺����������ķ���ֵΪ1��notifyAppExit������ֹͣ�������µ�ҵ�񣨼������ý����˳��Ļص�������
���ߣ���Զ��	2014-12-04
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
���ܣ������ӽ��̼��
������handle����windows�±�ʾ�������̵߳ľ����linux�±�ʾ�����̵߳�pid
���أ�<0��ʧ��
     >=0���ɹ�
���ߣ���Զ��	2014-12-04
************************************************************/
int CAppMonitorThd::startMonitor(TWLHandle handle)
{
    int ret = -1;
    if(NULL == handle)	return ret;

    //�Ƿ��Ѿ������˼��
    ret--;
    if(m_isStart)	return ret;

    m_handle  = handle;
    m_isStart = true;
    return 0;
}

/************************************************************
���ܣ��߳�ִ�к���
��������
���أ�<0��ʧ��
     >=0���ɹ�
���ߣ���Զ��	2014-12-04
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
        DWORD objectCode = WaitForSingleObject(m_handle, INFINITE);//���޳�ʱ
    //	if (objectCode == WAIT_OBJECT_0)  return 0;
    //	return -1;

#else ifdef WL_OS_CORE_LINUX
        DWORD64 value = (DWORD64)m_handle;
        pid_t pid = (pid_t)value;
        int status;
        waitpid(pid, &status, 0);    //�ȴ��ӽ����˳�
        exitCode = status;

#endif
        WLOutDebug("�ӳ����˳��ˣ�exitCode=%d",exitCode);
        break;
    }

    //֪ͨ���̣߳����ӳ����˳���
    m_pWLProcess->notifyAppExit(exitCode);
    return 0;
}
