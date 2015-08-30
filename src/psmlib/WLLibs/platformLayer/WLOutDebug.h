// WLOutDebug.h: interface for the CWLOutDebug class.
//
//////////////////////////////////////////////////////////////////////

/********************************************************************************************************
功能：封装关于调试打印 共公库函数
说明：
作者：叶文林	2013-11-26
********************************************************************************************************/

#if !defined(AFX_WLOUTDEBUG_H__2B7C1BC1_4509_4950_BA39_37AFC85903B7__INCLUDED_)
#define AFX_WLOUTDEBUG_H__2B7C1BC1_4509_4950_BA39_37AFC85903B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WLDEF.h"
#include <string.h>
#include <stdarg.h> 

extern int  gIsOpenConsole;			//是否打开控制台：0，不打开；1，打开

//是否禁止全部调试,以下不注释将无法输出任何调试
//#define DISABLE_ALL_DEBUG 

//定义基础调试手段，可根据情况注释以下一行至多行
#ifdef WL_DEBUG_CONSOLE
#define WL_DEF_DEBUG_CONSOLE        //一般为控制台程序
#endif

#ifdef WL_DEBUG_VS
#define WL_DEF_DEBUG_VS             //微软VS调试窗口
#endif

#ifdef WL_DEBUG_QTCREATOR
#define WL_DEF_DEBUG_QTCREATOR      //QTCreator调试窗口
#endif

#ifdef WL_DEBUG_ECLIPSE
#define WL_DEF_DEBUG_ECLIPSE        //ECLIPSE,android开发使用
#endif



void WLOutDebug(const char * fmt,...);
void WLOutDebugStream(BYTE * pStream,int len);
void WLOutDebugStream(const char * header,BYTE * pStream,int len);
void WLOutDebugStreamEx(BYTE * pStream,int len,const char * headerFmt,...);

void WLOutDebugEclipse(const char * tag,const char * fmt,...);
void WLOutDebugStreamEclipse(const char * tag,BYTE * pStream,int len);
void WLOutDebugStreamEclipse(const char * tag,const char * header,BYTE * pStream,int len);
void WLOutDebugStreamExEclipse(const char * tag,BYTE * pStream,int len,const char * headerFmt,...);

//以下函数为内部函数请不要调用
void WLOutDefDebug(const char * outStr);
void WLOutEclipseDebug(const char * tag,const char * outStr);
class CWLOutDebug  
{
public:
	CWLOutDebug();
	virtual ~CWLOutDebug();

};

#endif // !defined(AFX_WLOUTDEBUG_H__2B7C1BC1_4509_4950_BA39_37AFC85903B7__INCLUDED_)
