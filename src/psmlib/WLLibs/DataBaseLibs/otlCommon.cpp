#include "stdafx.h"
#include "otlCommon.h"
#include <odbcinst.h> 
#include <stdio.h>


//Unicode与ASCII互转
#define _UNICODE
void gAnsiToUnicode(const char* ansistr,TCHAR* unicodestr)
{
	int a = strlen(ansistr)+1;
#ifdef _UNICODE
	MultiByteToWideChar(CP_ACP, 0, ansistr, a, unicodestr, a);
#else
	strcpy(unicodestr,ansistr);
#endif
	//... when done, free the BSTR
}

void gWideCharToAnsi(const TCHAR* unicodestr,char* ansistr)
{
	int a;
#ifdef _UNICODE
	//a= _tcslen(unicodestr)*2+1;
	a = wcslen(unicodestr)*2+1;
	WideCharToMultiByte(CP_ACP,0,unicodestr,-1,ansistr,a,NULL,NULL);
	//ansistr[a]='\0';
#else
	a= strlen(unicodestr)+1;
	strncpy(ansistr,unicodestr,a);
	//ansistr[a]='\0';

#endif
}


/**************************************************************************************************
修改：唐小灿  2014-10-30 增加驱动参数
**************************************************************************************************/
BOOL configDataSoure(const char * pDB, const char * pDBName, const char * pDNS, const char * pDriver)
{
	int   len             = 0;
	char  buf[128]        = {0};
	TCHAR wbufSingle1[512] = {0};
	TCHAR wbufSingle2[512] = {0};
	TCHAR wbufSingle3[512] = {0};
	TCHAR wbuf[1024]      = {0};

	sprintf(buf, "DSN=%s", pDNS);
	gAnsiToUnicode(buf, wbufSingle1);

	memset(buf, 0x00, sizeof(buf));
	sprintf(buf, "SERVER=%s", pDB);
	gAnsiToUnicode(buf, wbufSingle2);

	memset(buf, 0x00, sizeof(buf));
	sprintf(buf, "DATABASE=%s", pDBName);
	gAnsiToUnicode(buf, wbufSingle3);

	len += wsprintf(&wbuf[len], TEXT("%s"), wbufSingle1);
	len += 1;
	len += wsprintf(&wbuf[len], TEXT(" %s"), wbufSingle2);
	len += 1;
	len += wsprintf(&wbuf[len], TEXT(" %s"), wbufSingle3);

	wchar_t wDriver[1024] = {0};
#ifdef _UNICODE
	{
		gAnsiToUnicode(pDriver, wDriver);
	}
#else
	{
		;  //未定义_UNICODE情况下为单字节处理
	}
#endif

	//配制数据源
	//BOOL ret = SQLConfigDataSource(NULL,ODBC_ADD_DSN, TEXT("SQL Server\0"), wbuf);  //SQL Server
	BOOL ret = SQLConfigDataSource(NULL,ODBC_ADD_DSN, wDriver//TEXT("MySQL ODBC 5.2 ANSI Driver\0"), wbuf);  //MySQL
	return ret;
}


BOOL configMySqlDataSoure(const char * pAddress, const char * pDBName, const char * pUid, const char * pPwd, const char * pDsn)
{
    if((NULL == pAddress) || (NULL == pDBName) || (NULL == pUid) || (NULL == pPwd) || (NULL == pDsn))   return FALSE;

    int   len             = 0;
    char  buf[128]        = {0};
    TCHAR wbufSingle1[512] = {0};
    TCHAR wbufSingle2[512] = {0};
    TCHAR wbufSingle3[512] = {0};
    TCHAR wbufSingle4[512] = {0};
    TCHAR wbufSingle5[512] = {0};
    TCHAR wbuf[1024]      = {0};

    sprintf(buf, "DSN=%s", pDsn);
    gAnsiToUnicode(buf, wbufSingle1);

    memset(buf, 0x00, sizeof(buf));
    sprintf(buf, "SERVER=%s", pAddress);
    gAnsiToUnicode(buf, wbufSingle2);

    memset(buf, 0x00, sizeof(buf));
    sprintf(buf, "DATABASE=%s", pDBName);
    gAnsiToUnicode(buf, wbufSingle3);

    memset(buf, 0x00, sizeof(buf));
    sprintf(buf, "USER=%s", pUid);
    gAnsiToUnicode(buf, wbufSingle4);

    memset(buf, 0x00, sizeof(buf));
    sprintf(buf, "PASSWORD=%s", pPwd);
    gAnsiToUnicode(buf, wbufSingle5);

    len += wsprintf(&wbuf[len], TEXT("%s"), wbufSingle1);
    len += 1;
    len += wsprintf(&wbuf[len], TEXT(" %s"), wbufSingle2);
    len += 1;
    len += wsprintf(&wbuf[len], TEXT(" %s"), wbufSingle3);
    len += 1;
    len += wsprintf(&wbuf[len], TEXT(" %s"), wbufSingle4);
    len += 1;
    len += wsprintf(&wbuf[len], TEXT(" %s"), wbufSingle5);

//	wsprintf(wbuf, TEXT("%s%d %s%d %s"), wbufSingle1, 0xFF, wbufSingle2, 0xFF, wbufSingle3);

    //测试
    //TCHAR * p = TEXT("DSN=OTLTestwl\0 SERVER=10.7.5.136\\sqlexpress\0 DATABASE=FieryDrag1104");

    //配制数据源
    BOOL ret = SQLConfigDataSource(NULL,ODBC_ADD_DSN, TEXT("MySQL ODBC 5.2 ANSI Driver\0"), wbuf);  //MySQL
    return ret;
}
