// WLPlatformLayer.cpp: implementation of the CWLPlatformLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WLPlatformLayer.h"
#include "../fileSysterm/WLDirect.h"
#ifdef WL_OS_CORE_WIN
#include "windows.h"
#elif defined WL_OS_CORE_LINUX
//#include <sys/timeb.h>
//#include<time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#endif
#include <time.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <stdlib.h>

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
#ifdef WL_3LIB_QT
#include <QDateTime>
#endif
/******************************************************************************
功能：获取本地时间
参数：TWLTime，接收时间
修改：唐小灿  2014-08-04
说明：统一了各个平台下各自调用接口，采用C++接口统一实现
测试：windows
******************************************************************************/
int WLGetLocalTime(TWLTime * pWLTime)
{
	/*
#ifdef WL_OS_CORE_WIN
	SYSTEMTIME tm = {0};
	GetLocalTime(&tm);

	pWLTime->year  = tm.wYear;
	pWLTime->month = tm.wMonth;
	pWLTime->day   = tm.wDay;

	pWLTime->hour   = tm.wHour;
	pWLTime->minute = tm.wMinute;
	pWLTime->second = tm.wSecond;
	pWLTime->RFU    = tm.wSecond / 4;

	return 1;
#endif

#ifdef WL_OS_CORE_LINUX
	//作者：唐小灿  2014-08-04
	//获取当前系统时间，采用c++接口，windows下可统一使用
	struct tm * p  = NULL;
	time_t      tt = time(NULL);
	p = localtime(&tt);

	pWLTime->year   = p->tm_year + 1900;
	pWLTime->month  = p->tm_mon + 1;
	pWLTime->day    = p->tm_mday;
	pWLTime->hour   = p->tm_hour;
	pWLTime->minute = p->tm_min;
	pWLTime->second = p->tm_sec;
	pWLTime->RFU    = p->tm_sec / 4;

	return 1;
#endif

#ifdef WL_3LIB_QT
	QDate d = QDate::currentDate ();
	pWLTime->year = d.year();
	pWLTime->month = d.month();
	pWLTime->day = d.day();

	QTime t = QTime::currentTime ();
	pWLTime->hour = t.hour();
	pWLTime->minute = t.minute();
	pWLTime->second = t.second();
	pWLTime->RFU = t.minute() / 4;

	return 1;
#endif
//*/

	//作者：唐小灿  2014-08-04
	//获取当前系统时间，采用c++接口，windows下可统一使用
	struct tm * p  = NULL;
	time_t      tt = time(NULL);
	p = localtime(&tt);

	pWLTime->year   = p->tm_year + 1900;
	pWLTime->month  = p->tm_mon + 1;
	pWLTime->day    = p->tm_mday;
	pWLTime->hour   = p->tm_hour;
	pWLTime->minute = p->tm_min;
	pWLTime->second = p->tm_sec;
	pWLTime->RFU    = p->tm_sec / 4;

	return 1;

//	return 0;
}


/******************************************************************************
功能：设置本地时间
参数：TWLTime，时间
修改：唐小灿  2014-08-04
测试：
******************************************************************************/
int WLSetLocalTime(const TWLTime * pTime)
{
	if (NULL == pTime)  return -1;

#ifdef WL_OS_CORE_WIN
	SYSTEMTIME tm = {0};
	tm.wYear = pTime->year;
	tm.wMonth = pTime->month;
	tm.wDay   = pTime->day;
	tm.wHour  = pTime->hour;
	tm.wMinute= pTime->minute;
	tm.wSecond= pTime->second;

	BOOL ret = SetLocalTime(&tm);
	if (FALSE == ret)
	{
		printf("SetLocalTime errno = %d\n", GetLastError());
		return -1;
	}
	else
		return 0;

	return 1;
#endif

#ifdef WL_OS_CORE_LINUX
	struct tm *    sTime = NULL;
	struct timeval tv;
	time_t timep, timeq;
	int ret = 0;

	time(&timep);
	sTime = localtime(&timep);
	sTime->tm_year = pTime->year;
	sTime->tm_mon  = pTime->month;
	sTime->tm_mday = pTime->day;
	sTime->tm_hour = pTime->hour;
	sTime->tm_min  = pTime->minute;
	sTime->tm_sec  = pTime->second;

	timeq = mktime(sTime);
	tv.tv_sec  = (DWORD32)timeq;
	tv.tv_usec = 0;

//	ret = settimeofday(&tv, NULL);
	if (ret < 0)  return -1;
	else          return 0;
#endif

	return 0;
}

//7,表示星期天，1-6，分别表示星期一到六
//-1,表示失败
/******************************************************************************
功能：获取当时间所属周几，1-7代表周一到周天
参数：无
修改：唐小灿  2014-08-04
说明：统一了各个平台下各自调用接口，采用C++接口统一实现
测试：windows
******************************************************************************/
int getLocalWeek()
{
	/*
#ifdef WL_OS_CORE_WIN
	SYSTEMTIME tm = {0};
	GetLocalTime(&tm);

	return tm.wDayOfWeek;
#endif

#ifdef WL_OS_CORE_LINUX
	struct tm * p  = NULL;
	time_t      tt = time(NULL);
	p = localtime(&tt);

	return p->tm_wday;

	//return 1;
#endif
	//*/

	struct tm * p  = NULL;
	time_t      tt = time(NULL);
	p = localtime(&tt);

	//p->tm_wday取值[0-6]，0表示周天
	if (p->tm_wday < 0 || p->tm_wday > 6)  return -1;
	if (p->tm_wday == 0)  return 7;
	return p->tm_wday;
	
	//return -1;
}

int getLocalWeekStr(char * weekStr,DWORD // len )
{
    int dayOfWeek = getLocalWeek();
    if		(dayOfWeek == 7)	strcat(weekStr,"星期天");
    else if (dayOfWeek == 1)	strcat(weekStr,"星期一");
    else if (dayOfWeek == 2)	strcat(weekStr,"星期二");
    else if (dayOfWeek == 3)	strcat(weekStr,"星期三");
    else if (dayOfWeek == 4)	strcat(weekStr,"星期四");
    else if (dayOfWeek == 5)	strcat(weekStr,"星期五");
    else if (dayOfWeek == 6)	strcat(weekStr,"星期六");
    else strcat(weekStr,"未知");
    return TRUE;
}


DWORD WLGetTickCount()
{
#ifdef WL_OS_CORE_WIN
	return GetTickCount();
#elif defined WL_OS_CORE_LINUX
	struct timeb tb;
	struct tm * p;
	time_t timep;
	DWORD millSec = 0;
	ftime(&tb);
	p=localtime(&tb.time);
	timep = mktime(p);
	millSec = timep*1000+tb.millitm;
	return millSec;
#endif
	/*linux 
	tms tm;
	return times(&tm);
	*/
	return 0;
}

//计算64位时间戳，暂未实现
DWORD64 WLGetTickCount64()
{
#ifdef WL_OS_CORE_WIN
	struct timeb tb;
	struct tm * p;
	time_t timep;
	DWORD64 millSec = 0;
	ftime(&tb);
	p=localtime(&tb.time);
	timep = mktime(p);
	millSec = timep*1000+tb.millitm;
	return millSec;
//	return GetTickCount64();
//	return GetTickCount();
#elif defined WL_OS_CORE_LINUX
	struct timeb tb;
	struct tm * p;
	time_t timep;
	DWORD millSec = 0;
	ftime(&tb);
	p=localtime(&tb.time);
	timep = mktime(p);
	millSec = timep*1000+tb.millitm;
	return millSec;
#endif
	/*linux 
	tms tm;
	return times(&tm);
	*/
	return 0;
}

void WLSleep(DWORD mSec)
{
#ifdef WL_OS_CORE_WIN
	Sleep(mSec);
#elif defined WL_OS_CORE_LINUX
	usleep( mSec * 1000 ); 
#endif
}

//延时，但是不死掉（窗口仍可以活动，处理消息），本质时延时一段时间先处理窗口消息，然后在延时
//最时延时单位10MS（如果操作系统最小延时单位大于10ms，则以操作系统最小延时单位为准）
//此延时函数精准度较低，一般都会比期望延时时间大。
//当界面较忙时，延时较大，反之相反。
#ifdef WL_3LIB_QT
#include <QApplication>
#endif
void WLNoFreezeSleep(DWORD mSec)
{
	DWORD tc = WLGetTickCount();//GetTickCount();
	while (TRUE)
	{
		if (WLGetTickCount() - tc >= mSec)	break;
#ifdef WL_3LIB_QT
		qApp->processEvents();
#endif
		WLSleep(10);
	}
}

//---------------------------------------------------------------------
//Description:
//	Execute the application
//
//Parameters:
//	pszPath:[in] Long pointer to a null-terminated string that specifies the absolute name of the file to run
//	pParameters:[in] Long pointer to a null-terminated string that contains the application parameters. 
//				The parameters must be separated by spaces. To include double quotation marks, 
//				you must enclose the marks in double quotation marks, as in the following example. 
//				TCHAR *pParameters = "An example: """quoted text"""";
//				In this case, the application receives three parameters: An, example:, and "quoted text". 
//				If the pszPath specifies a document file, this member should be NULL. 
//
//---------------------------------------------------------------------
//BOOL ExecuteApp(char *pszPath,char *pParameters)
//{
//	TCHAR wPath[MAX_PATH] = {0};
//	TCHAR wParam[100] = {0};
//	char s[100] = {0};
//	//sprintf(s,"%s %d %d",">>param",WLGetCurrentProcess(),1);
//	//sprintf(s,"%s %d %d",">>param",WLGetCurrentProcessId(),1);
//	WLOutDebug(s);
//	//sprintf(wParam,"%s","010203040506070809");
//
//	//GetCurrentProcess();
//
//	gAnsiToUnicode(pszPath,wPath);
//	if (pParameters != NULL)	gAnsiToUnicode(pParameters,wParam);
//	//gAnsiToUnicode(s,wParam);
//	//memcpy(wParam,s,10);
//
//	SHELLEXECUTEINFO info;	
//	memset(&info,0,sizeof(info));
//	info.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
//	info.lpVerb = NULL;
//	info.lpFile = wPath;
//	if (pParameters != NULL)	info.lpParameters = wParam;	else info.lpParameters = NULL;
//	info.lpDirectory = NULL;
//	info.nShow = SW_SHOW;
//	info.hInstApp = NULL;
//	info.cbSize = sizeof( info );
//
//	BOOL bResult = ShellExecuteEx( &info );	
//
//	return bResult;
//	return 0;
//}

//DWORD WLGetCurrentProcess()
//{
//	HANDLE h = GetCurrentProcess();
//	DWORD ret = 0;
//	memcpy(&ret,&h,4);
//	return ret;
//}

//BOOL WLTerminateProcess(DWORD hProcess)
//{
//	HANDLE h = 0;
//	memcpy(&h,&hProcess,4);
//	return TerminateProcess(h,0);
//}

//BOOL WLTerminateProcessWaitEnd(DWORD hProcess)
//{
//	HANDLE h = 0;
//	memcpy(&h,&hProcess,4);
//
//	BOOL ret = TerminateProcess(h,0);
//	if (ret == FALSE)	return FALSE;
//
//	DWORD retDWord = WaitForSingleObject(h,100000);
//	CloseHandle(h);
//	if (WAIT_OBJECT_0 != retDWord)	return FALSE;	else return TRUE;
//}

//DWORD WLGetCurrentProcessId()
//{
//	return GetCurrentProcessId();
//}

//BOOL WLKillProcessWaitEnd(DWORD processId)
//{
//	HANDLE hProcess = 0;
//	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
//	if (hProcess <= 0)	return FALSE;
//
//	BOOL ret = TerminateProcess(hProcess,0);
//	if (ret == FALSE)	
//	{
//		CloseHandle(hProcess);
//		return 0;
//	}
//
//	DWORD retDWord = WaitForSingleObject(hProcess,100000);
//	CloseHandle(hProcess);
//	if (WAIT_OBJECT_0 != retDWord)	return FALSE;	else return TRUE;
//}

//#define _UNICODE
//void gAnsiToUnicode(const char* ansistr,TCHAR* unicodestr)
//{
//	int a = strlen(ansistr)+1;
//#ifdef _UNICODE
//	MultiByteToWideChar(CP_ACP, 0, ansistr, a, unicodestr, a);
//#else
//	strcpy(unicodestr,ansistr);
//#endif
//	//... when done, free the BSTR
//}

//void gWideCharToAnsi(const TCHAR* unicodestr,char* ansistr)
//{
//	int a;
//#ifdef _UNICODE
//	//a= _tcslen(unicodestr)*2+1;
//	a = wcslen(unicodestr)*2+1;
//	WideCharToMultiByte(CP_ACP,0,unicodestr,-1,ansistr,a,NULL,NULL);
//	//ansistr[a]='\0';
//#else
//	a= strlen(unicodestr)+1;
//	strncpy(ansistr,unicodestr,a);
//	//ansistr[a]='\0';
//
//#endif
//}

/*********************************************************************
功能：获取编译日期时间
参数：无
返回：无
作者：叶文林
备注：刘远安注释	2013-01-21
*********************************************************************/
void getCompileDateTime( TWLTime * pTime )
{
    if (pTime == NULL)	return;

    char dateStr[32] = {0};
    char timeStr[32] = {0};
    char * yearStr,* monthStr,* dayStr;
    char * hourStr,* minStr,  *secStr;

    //取日期字符串 __DATE__ 程序被编译的日期, 以"Mmm dd yyyy"格式的字符串标注.
    sprintf(dateStr,"%s",__DATE__);
    //yearStr  = strtok_s(dateStr," ",&context);
    //monthStr = strtok_s(0," ",&context);
    //dayStr   = strtok_s(0," ",&context);
    monthStr  = strtok(dateStr," ");
    dayStr   = strtok(0," ");
    yearStr = strtok(0," ");

    //取时间字符串__TIME__ 程序被编译的时间, 以"hh:mm:ss"格式的字符串标注
    sprintf(timeStr,"%s",__TIME__);
    //hourStr  = strtok_s(timeStr,":",&context);
    //minStr   = strtok_s(0,":",&context);
    //secStr   = strtok_s(0,":",&context);
    hourStr  = strtok(timeStr,":");
    minStr   = strtok(0,":");
    secStr   = strtok(0,":");

    //结构体赋值
    pTime->year = atoi(yearStr);
    const char months[][4]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    for (int i=0;i<12;i++)
    {
        if (strcmp(monthStr,months[i])==0)
        {
            pTime->month=i+1;
            break;
        }
    }
    pTime->day = atoi(dayStr);
    pTime->hour= atoi(hourStr);
    pTime->minute = atoi(minStr);
    pTime->second =
    atoi(secStr);
}

//整形字符串转换成整形数
int str2int(const char* pstr)
{
    return atoi(pstr);
}


//double字符串转换成double
double str2double(const char* pstr)
{
    //_CRT_DOUBLE res;
    ////double res;
    //int ret = _atodbl(&res, (char*)pstr);
    //return res.x;
    return strtod(pstr, (char**)NULL);
}

//timeStr格式:yyyyMMddhhmmss
int timeStr2TWLTime(const char * timeStr, TWLTime * pDestTime)
{
    if ((NULL==timeStr) || (NULL==pDestTime))  return -1;

    char buf[8] = {0};
    //年
    memcpy(buf, timeStr, 4);
    pDestTime->year = (WORD)atoi(buf);

    //月
    memset(buf, 0x00, sizeof(buf));
    memcpy(buf, timeStr+4, 2);
    pDestTime->month = (BYTE)atoi(buf);

    //日
    memset(buf, 0x00, sizeof(buf));
    memcpy(buf, timeStr+6, 2);
    pDestTime->day = (BYTE)atoi(buf);

    //时
    memset(buf, 0x00, sizeof(buf));
    memcpy(buf, timeStr+8, 2);
    pDestTime->hour = (BYTE)atoi(buf);

    //分
    memset(buf, 0x00, sizeof(buf));
    memcpy(buf, timeStr+10, 2);
    pDestTime->minute = (BYTE)atoi(buf);

    //秒
    memset(buf, 0x00, sizeof(buf));
    memcpy(buf, timeStr+12, 2);
    pDestTime->second = (BYTE)atoi(buf);

    return 0;
}

//计算内存地址偏移
//叶文林
TMemAddr WLGetNewOffsetMemAddr(TMemAddr org,DWORD32 offset)
{
	TMemAddr dest = (TMemAddr)((BYTE *)org + offset);
	return dest;
}

//叶文林
TMemAddr WLPointerToTMemAddr(void * p)
{
	////取CPU字长
	//static WORD wordLen = 0;
	//if (wordLen == 0)	wordLen = sizeof(void *); //取值只可能为32或64	

	////根据不同CPU类型作不同计算
	//if (wordLen == 32)	return (DWORD32)p;
	//else return (DWORD64)p;

	return (DWORD64)p;
}

void * WLTMemAddrToPointer(TMemAddr addr)
{
	////取CPU字长
	//static WORD wordLen = 0;
	//if (wordLen == 0)	wordLen = sizeof(void *); //取值只可能为32或64	

	////根据不同CPU类型作不同计算
	//if (wordLen == 32)	return (void *)(DWORD32)addr;
	//else return (void *)(DWORD64)addr;
	return (void *)addr;
}



//唐小灿 2014-01-03
void * WLGetNewOffsetMemAddr(void * pAddrBase, DWORD32 offset)
{
	TMemAddr base = WLPointerToTMemAddr(pAddrBase);
	TMemAddr dest = (TMemAddr)(base + offset);
	return WLTMemAddrToPointer(dest);
}


/*********************************************************************
功能：void *转换成TMemAddr类型
参数：p，void类型指针
返回：TMemAddr类型内容
作者：唐小灿  2014-01-09
*********************************************************************/
TMemAddr  WLPointer2MemAddr(void * p)
{
	return (DWORD64)p;
}


/*********************************************************************
功能：TMemAddr转换成void类型指针地址
参数：addr，TMemAddr类型内容
返回：void类型指针
作者：唐小灿  2014-01-09
*********************************************************************/
void    * WLMemAddr2Pointer(TMemAddr addr)
{
	return (void *)addr;
}


/*********************************************************************
功能：根据TMemAddr跟偏移计算新的TMemAddr内容
参数：addr，TMemAddr类型内容
      offset，偏移
返回：TMemAddr类型内容
作者：唐小灿  2014-01-09
*********************************************************************/
TMemAddr  WLMemAddrInc(TMemAddr addr,DWORD32 offset)
{
	return (TMemAddr)(addr + offset);
}


/*********************************************************************
功能：根据void类型指针跟偏移计算新的void类型指针地址
参数：addr，void类型指针地址
      offset，偏移
返回：void类型指针地址
作者：唐小灿  2014-01-09
*********************************************************************/
void    * WLPointerInc(void * p ,DWORD32 offset)
{
	TMemAddr memAddrOld = WLPointer2MemAddr(p);
	TMemAddr memAddrNew = WLMemAddrInc(memAddrOld, offset);
	return WLMemAddr2Pointer(memAddrNew);
}


/*********************************************************************
功能：产生随机数
参数：无
返回：DWORD32的随机数
作者：唐小灿  2014-05-16
*********************************************************************/
DWORD32 getRand()
{
	srand(WLGetTickCount());
	DWORD32 num = rand()%0xFFFFFFFF;
	return num;
}

/*********************************************************************
功能：调用应用程序
参数：appPath，待调用应用程序完整路径
	  pParameters，传递给待调用应用程序的参数
	  pWorkDir，指定待调用应用程序的工作目录
	  outHandle，输出参数，待调用应用程序启动后的句柄
返回：FALSE，失败；TRUE，成功
作者：刘远安	2014-11-24
*********************************************************************/
BOOL WLExecuteApp(const char *appPath,char *pParameters,const char *pWorkDir,TWLHandle *outHandle)
{
	if(NULL == appPath)	return FALSE;

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
	return bResult;

#elif defined WL_OS_CORE_LINUX
        pid_t pid;
        pid = fork();              // 创建子进程
        if(pid < 0)
        {
                printf("failure!\n ");
        }
        else if(pid==0)
        {
                char *ps_argv[2] = {NULL,NULL};
                ps_argv[0] = pParameters;

                if(NULL != pWorkDir)
                {
                        char curWorkDir[MAX_PATH] = {0};
                        char * dir = getcwd(curWorkDir, MAX_PATH);
                        if(NULL == dir)	printf("getcwd error\n");

                        chdir(pWorkDir);
                        int res = execv(appPath,ps_argv);
                        chdir(curWorkDir);
                        if(res < 0)
                        {
                                printf("execv error!\n");
                                return FALSE;
                        }
                }
                else
                {
                        int res = execv(appPath,ps_argv);
                        if(res < 0)
                        {
                                printf("execv error!\n");
                                return FALSE;
                        }
                }
        }
        else
        {
            *outHandle = (TWLHandle)pid;
        }
        //wait(&status);
        //*
        if (waitpid(pid, NULL, 0) != pid)    //父进程等到子进程退出
            return FALSE;
                //*/

        return TRUE;
#endif
}
