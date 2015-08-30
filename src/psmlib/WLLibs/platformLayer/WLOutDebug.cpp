// WLOutDebug.cpp: implementation of the CWLOutDebug class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WLOutDebug.h"
#include <stdio.h>
#ifdef WL_DEBUG_ECLIPSE
#include "charactersetconversion.h"
//#include <jni.h>
#include <android/log.h>
#endif
#ifdef WL_IDE_VC6
	#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[]=__FILE__;
	#define new DEBUG_NEW
	#endif
#endif

//#define  DISABLE_ALL_DEBUG

	//修改：刘远安 在做GUI程序的时候可以通过此方法打开控制台
    int  gIsOpenConsole = 1;			//是否打开控制台：0，不打开；1，打开
#ifdef WL_OS_WIN32 
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <windows.h>
	//打开控制台应用程序
	void InitConsoleWindow()
	{
		int nCrt = 0;
		FILE* fp;
		AllocConsole();
		nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
		fp = _fdopen(nCrt, "w");
		*stdout = *fp;
		setvbuf(stdout, NULL, _IONBF, 0);
	}
#endif

#ifdef WL_DEF_DEBUG_QTCREATOR
#include <QDebug>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void WLOutDefDebug(const char * outStr)
{
#ifdef DISABLE_ALL_DEBUG
	return;
#endif

#ifdef WL_DEF_DEBUG_CONSOLE
	//修改：刘远安 在做GUI程序的时候可以通过此方法打开控制台
  #ifdef WL_OS_WIN32 
	//打印到控制台
	static int isFirst = TRUE;
	if (isFirst)	
	{
		isFirst = FALSE;
		if(1 == gIsOpenConsole)
		{
			InitConsoleWindow();
		}
	}
  #endif
	printf(outStr);
	printf("\n");
	fflush(stdout);
#endif


#ifdef WL_DEF_DEBUG_VS
	outputVSDebug(outStr);
#endif

#ifdef WL_DEF_DEBUG_QTCREATOR
	qDebug(outStr);
#endif

#ifdef WL_DEF_DEBUG_ECLIPSE
	__android_log_print(ANDROID_LOG_WARN, "C++", "%s",outStr);
#endif
	
}

void WLOutEclipseDebug(const char * tag,const char * outStr)
{
#ifdef WL_DEBUG_ECLIPSE
	char tagUft8[2048] = {0};
	char outStrUtf8[2048] = {0};
	DWORD utflen = 0;

	gb2312StrToUTF8((char *)tag,tagUft8,sizeof(tagUft8),utflen);
	gb2312StrToUTF8((char *)outStr,outStrUtf8,sizeof(outStrUtf8),utflen);

	__android_log_print(ANDROID_LOG_WARN,tagUft8, "%s",outStrUtf8);
#else
	WLOutDefDebug(outStr);
#endif
}

void WLOutDebug(const char * fmt,...)
{
#ifdef DISABLE_ALL_DEBUG                  //禁止所有调试信息输出，发布后可以考虑启用本项
	return;
#else
	static char sprint_buf[1024];

	memset(sprint_buf,0,1024);
	va_list args; 
	int n; 
	va_start(args, fmt); 
	n = vsprintf(sprint_buf, fmt, args); 
	va_end(args);  

	WLOutDefDebug(sprint_buf);
#endif
}

//打印流，结尾自动换行
void WLOutDebugStream(BYTE * pStream,int len)
{
#ifdef DISABLE_ALL_DEBUG                  //禁止所有调试信息输出，发布后可以考虑启用本项
	return;
#else
	static char buf[1024] = {0};
	if (len >511) len = 511;
	memset(buf,0,1024);
	for (int i =0;i<len;i++)	sprintf(&buf[i*2],"%.2X",pStream[i]);

	WLOutDefDebug(buf);
#endif
}

void WLOutDebugStream(const char * header,BYTE * pStream,int len)
{
#ifdef DISABLE_ALL_DEBUG                  //禁止所有调试信息输出，发布后可以考虑启用本项
	return;
#else
	static char buf[1024] = {0};
	int headerlen = strlen(header);
	if (len - headerlen > 511)	len = 511 - headerlen;
	memset(buf,0,1024);
	memcpy(buf,header,headerlen);
	for (int i = 0;i<len;i++) sprintf(&buf[headerlen + i*2],"%.2X",pStream[i]);

	WLOutDefDebug(buf);
#endif
}

void WLOutDebugStreamEx(BYTE * pStream,int len,const char * headerFmt,...)
{
#ifdef DISABLE_ALL_DEBUG                  //禁止所有调试信息输出，发布后可以考虑启用本项
	return;
#else
	static char buf[1024]; 

	va_list args; 
	int n; 
	va_start(args, headerFmt); 
	n = vsprintf(buf, headerFmt, args); 
	va_end(args);  

	int headerlen = strlen(buf);
	if (len - headerlen > 511)	len = 511 - headerlen;
	for (int i = 0;i<len;i++) sprintf(&buf[headerlen + i*2],"%.2X",pStream[i]);

	WLOutDefDebug(buf);
#endif
}

void WLOutDebugEclipse(const char * tag,const char * fmt,...)
{
#ifdef DISABLE_ALL_DEBUG                  //禁止所有调试信息输出，发布后可以考虑启用本项
	return;
#else
	static char sprint_buf[1024]; 

	memset(sprint_buf,0,1024);
	va_list args; 
	int n; 
	va_start(args, fmt); 
	n = vsprintf(sprint_buf, fmt, args); 
	va_end(args);  

	WLOutEclipseDebug(tag,sprint_buf);
#endif
}

void WLOutDebugStreamEclipse(const char * tag,BYTE * pStream,int len)
{
#ifdef DISABLE_ALL_DEBUG                  //禁止所有调试信息输出，发布后可以考虑启用本项
	return;
#else
	static char buf[1024] = {0};
	if (len >511) len = 511;
	memset(buf,0,1024);
	for (int i =0;i<len;i++)	sprintf(&buf[i*2],"%.2X",pStream[i]);

	WLOutEclipseDebug(tag,buf);
#endif
}

void WLOutDebugStreamEclipse(const char * tag,const char * header,BYTE * pStream,int len)
{
#ifdef DISABLE_ALL_DEBUG                  //禁止所有调试信息输出，发布后可以考虑启用本项
	return;
#else
	static char buf[1024] = {0};
	int headerlen = strlen(header);
	if (len - headerlen > 511)	len = 511 - headerlen;
	memset(buf,0,1024);
	memcpy(buf,header,headerlen);
	for (int i = 0;i<len;i++) sprintf(&buf[headerlen + i*2],"%.2X",pStream[i]);

	WLOutDefDebug(buf);
#endif
}

void WLOutDebugStreamExEclipse(const char * tag,BYTE * pStream,int len,const char * headerFmt,...)
{
#ifdef DISABLE_ALL_DEBUG                  //禁止所有调试信息输出，发布后可以考虑启用本项
	return;
#else
	static char buf[1024]; 

	va_list args; 
	int n; 
	va_start(args, headerFmt); 
	n = vsprintf(buf, headerFmt, args); 
	va_end(args);  

	int headerlen = strlen(buf);
	if (len - headerlen > 511)	len = 511 - headerlen;
	for (int i = 0;i<len;i++) sprintf(&buf[headerlen + i*2],"%.2X",pStream[i]);

	WLOutDefDebug(buf);
#endif
}

CWLOutDebug::CWLOutDebug()
{

}

CWLOutDebug::~CWLOutDebug()
{

}
