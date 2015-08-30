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
���ܣ���ȡ����ʱ��
������TWLTime������ʱ��
�޸ģ���С��  2014-08-04
˵����ͳһ�˸���ƽ̨�¸��Ե��ýӿڣ�����C++�ӿ�ͳһʵ��
���ԣ�windows
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
	//���ߣ���С��  2014-08-04
	//��ȡ��ǰϵͳʱ�䣬����c++�ӿڣ�windows�¿�ͳһʹ��
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

	//���ߣ���С��  2014-08-04
	//��ȡ��ǰϵͳʱ�䣬����c++�ӿڣ�windows�¿�ͳһʹ��
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
���ܣ����ñ���ʱ��
������TWLTime��ʱ��
�޸ģ���С��  2014-08-04
���ԣ�
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

//7,��ʾ�����죬1-6���ֱ��ʾ����һ����
//-1,��ʾʧ��
/******************************************************************************
���ܣ���ȡ��ʱ�������ܼ���1-7������һ������
��������
�޸ģ���С��  2014-08-04
˵����ͳһ�˸���ƽ̨�¸��Ե��ýӿڣ�����C++�ӿ�ͳһʵ��
���ԣ�windows
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

	//p->tm_wdayȡֵ[0-6]��0��ʾ����
	if (p->tm_wday < 0 || p->tm_wday > 6)  return -1;
	if (p->tm_wday == 0)  return 7;
	return p->tm_wday;
	
	//return -1;
}

int getLocalWeekStr(char * weekStr,DWORD // len )
{
    int dayOfWeek = getLocalWeek();
    if		(dayOfWeek == 7)	strcat(weekStr,"������");
    else if (dayOfWeek == 1)	strcat(weekStr,"����һ");
    else if (dayOfWeek == 2)	strcat(weekStr,"���ڶ�");
    else if (dayOfWeek == 3)	strcat(weekStr,"������");
    else if (dayOfWeek == 4)	strcat(weekStr,"������");
    else if (dayOfWeek == 5)	strcat(weekStr,"������");
    else if (dayOfWeek == 6)	strcat(weekStr,"������");
    else strcat(weekStr,"δ֪");
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

//����64λʱ�������δʵ��
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

//��ʱ�����ǲ������������Կ��Ի��������Ϣ��������ʱ��ʱһ��ʱ���ȴ�������Ϣ��Ȼ������ʱ
//��ʱ��ʱ��λ10MS���������ϵͳ��С��ʱ��λ����10ms�����Բ���ϵͳ��С��ʱ��λΪ׼��
//����ʱ������׼�Ƚϵͣ�һ�㶼���������ʱʱ���
//�������æʱ����ʱ�ϴ󣬷�֮�෴��
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
���ܣ���ȡ��������ʱ��
��������
���أ���
���ߣ�Ҷ����
��ע����Զ��ע��	2013-01-21
*********************************************************************/
void getCompileDateTime( TWLTime * pTime )
{
    if (pTime == NULL)	return;

    char dateStr[32] = {0};
    char timeStr[32] = {0};
    char * yearStr,* monthStr,* dayStr;
    char * hourStr,* minStr,  *secStr;

    //ȡ�����ַ��� __DATE__ ���򱻱��������, ��"Mmm dd yyyy"��ʽ���ַ�����ע.
    sprintf(dateStr,"%s",__DATE__);
    //yearStr  = strtok_s(dateStr," ",&context);
    //monthStr = strtok_s(0," ",&context);
    //dayStr   = strtok_s(0," ",&context);
    monthStr  = strtok(dateStr," ");
    dayStr   = strtok(0," ");
    yearStr = strtok(0," ");

    //ȡʱ���ַ���__TIME__ ���򱻱����ʱ��, ��"hh:mm:ss"��ʽ���ַ�����ע
    sprintf(timeStr,"%s",__TIME__);
    //hourStr  = strtok_s(timeStr,":",&context);
    //minStr   = strtok_s(0,":",&context);
    //secStr   = strtok_s(0,":",&context);
    hourStr  = strtok(timeStr,":");
    minStr   = strtok(0,":");
    secStr   = strtok(0,":");

    //�ṹ�帳ֵ
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

//�����ַ���ת����������
int str2int(const char* pstr)
{
    return atoi(pstr);
}


//double�ַ���ת����double
double str2double(const char* pstr)
{
    //_CRT_DOUBLE res;
    ////double res;
    //int ret = _atodbl(&res, (char*)pstr);
    //return res.x;
    return strtod(pstr, (char**)NULL);
}

//timeStr��ʽ:yyyyMMddhhmmss
int timeStr2TWLTime(const char * timeStr, TWLTime * pDestTime)
{
    if ((NULL==timeStr) || (NULL==pDestTime))  return -1;

    char buf[8] = {0};
    //��
    memcpy(buf, timeStr, 4);
    pDestTime->year = (WORD)atoi(buf);

    //��
    memset(buf, 0x00, sizeof(buf));
    memcpy(buf, timeStr+4, 2);
    pDestTime->month = (BYTE)atoi(buf);

    //��
    memset(buf, 0x00, sizeof(buf));
    memcpy(buf, timeStr+6, 2);
    pDestTime->day = (BYTE)atoi(buf);

    //ʱ
    memset(buf, 0x00, sizeof(buf));
    memcpy(buf, timeStr+8, 2);
    pDestTime->hour = (BYTE)atoi(buf);

    //��
    memset(buf, 0x00, sizeof(buf));
    memcpy(buf, timeStr+10, 2);
    pDestTime->minute = (BYTE)atoi(buf);

    //��
    memset(buf, 0x00, sizeof(buf));
    memcpy(buf, timeStr+12, 2);
    pDestTime->second = (BYTE)atoi(buf);

    return 0;
}

//�����ڴ��ַƫ��
//Ҷ����
TMemAddr WLGetNewOffsetMemAddr(TMemAddr org,DWORD32 offset)
{
	TMemAddr dest = (TMemAddr)((BYTE *)org + offset);
	return dest;
}

//Ҷ����
TMemAddr WLPointerToTMemAddr(void * p)
{
	////ȡCPU�ֳ�
	//static WORD wordLen = 0;
	//if (wordLen == 0)	wordLen = sizeof(void *); //ȡֵֻ����Ϊ32��64	

	////���ݲ�ͬCPU��������ͬ����
	//if (wordLen == 32)	return (DWORD32)p;
	//else return (DWORD64)p;

	return (DWORD64)p;
}

void * WLTMemAddrToPointer(TMemAddr addr)
{
	////ȡCPU�ֳ�
	//static WORD wordLen = 0;
	//if (wordLen == 0)	wordLen = sizeof(void *); //ȡֵֻ����Ϊ32��64	

	////���ݲ�ͬCPU��������ͬ����
	//if (wordLen == 32)	return (void *)(DWORD32)addr;
	//else return (void *)(DWORD64)addr;
	return (void *)addr;
}



//��С�� 2014-01-03
void * WLGetNewOffsetMemAddr(void * pAddrBase, DWORD32 offset)
{
	TMemAddr base = WLPointerToTMemAddr(pAddrBase);
	TMemAddr dest = (TMemAddr)(base + offset);
	return WLTMemAddrToPointer(dest);
}


/*********************************************************************
���ܣ�void *ת����TMemAddr����
������p��void����ָ��
���أ�TMemAddr��������
���ߣ���С��  2014-01-09
*********************************************************************/
TMemAddr  WLPointer2MemAddr(void * p)
{
	return (DWORD64)p;
}


/*********************************************************************
���ܣ�TMemAddrת����void����ָ���ַ
������addr��TMemAddr��������
���أ�void����ָ��
���ߣ���С��  2014-01-09
*********************************************************************/
void    * WLMemAddr2Pointer(TMemAddr addr)
{
	return (void *)addr;
}


/*********************************************************************
���ܣ�����TMemAddr��ƫ�Ƽ����µ�TMemAddr����
������addr��TMemAddr��������
      offset��ƫ��
���أ�TMemAddr��������
���ߣ���С��  2014-01-09
*********************************************************************/
TMemAddr  WLMemAddrInc(TMemAddr addr,DWORD32 offset)
{
	return (TMemAddr)(addr + offset);
}


/*********************************************************************
���ܣ�����void����ָ���ƫ�Ƽ����µ�void����ָ���ַ
������addr��void����ָ���ַ
      offset��ƫ��
���أ�void����ָ���ַ
���ߣ���С��  2014-01-09
*********************************************************************/
void    * WLPointerInc(void * p ,DWORD32 offset)
{
	TMemAddr memAddrOld = WLPointer2MemAddr(p);
	TMemAddr memAddrNew = WLMemAddrInc(memAddrOld, offset);
	return WLMemAddr2Pointer(memAddrNew);
}


/*********************************************************************
���ܣ����������
��������
���أ�DWORD32�������
���ߣ���С��  2014-05-16
*********************************************************************/
DWORD32 getRand()
{
	srand(WLGetTickCount());
	DWORD32 num = rand()%0xFFFFFFFF;
	return num;
}

/*********************************************************************
���ܣ�����Ӧ�ó���
������appPath��������Ӧ�ó�������·��
	  pParameters�����ݸ�������Ӧ�ó���Ĳ���
	  pWorkDir��ָ��������Ӧ�ó���Ĺ���Ŀ¼
	  outHandle�����������������Ӧ�ó���������ľ��
���أ�FALSE��ʧ�ܣ�TRUE���ɹ�
���ߣ���Զ��	2014-11-24
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
	memset(&si,0,sizeof(si));				//����
	si.cb=sizeof(si);
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
	return bResult;

#elif defined WL_OS_CORE_LINUX
        pid_t pid;
        pid = fork();              // �����ӽ���
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
        if (waitpid(pid, NULL, 0) != pid)    //�����̵ȵ��ӽ����˳�
            return FALSE;
                //*/

        return TRUE;
#endif
}
