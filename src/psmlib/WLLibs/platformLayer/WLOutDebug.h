// WLOutDebug.h: interface for the CWLOutDebug class.
//
//////////////////////////////////////////////////////////////////////

/********************************************************************************************************
���ܣ���װ���ڵ��Դ�ӡ �����⺯��
˵����
���ߣ�Ҷ����	2013-11-26
********************************************************************************************************/

#if !defined(AFX_WLOUTDEBUG_H__2B7C1BC1_4509_4950_BA39_37AFC85903B7__INCLUDED_)
#define AFX_WLOUTDEBUG_H__2B7C1BC1_4509_4950_BA39_37AFC85903B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WLDEF.h"
#include <string.h>
#include <stdarg.h> 

extern int  gIsOpenConsole;			//�Ƿ�򿪿���̨��0�����򿪣�1����

//�Ƿ��ֹȫ������,���²�ע�ͽ��޷�����κε���
//#define DISABLE_ALL_DEBUG 

//������������ֶΣ��ɸ������ע������һ��������
#ifdef WL_DEBUG_CONSOLE
#define WL_DEF_DEBUG_CONSOLE        //һ��Ϊ����̨����
#endif

#ifdef WL_DEBUG_VS
#define WL_DEF_DEBUG_VS             //΢��VS���Դ���
#endif

#ifdef WL_DEBUG_QTCREATOR
#define WL_DEF_DEBUG_QTCREATOR      //QTCreator���Դ���
#endif

#ifdef WL_DEBUG_ECLIPSE
#define WL_DEF_DEBUG_ECLIPSE        //ECLIPSE,android����ʹ��
#endif



void WLOutDebug(const char * fmt,...);
void WLOutDebugStream(BYTE * pStream,int len);
void WLOutDebugStream(const char * header,BYTE * pStream,int len);
void WLOutDebugStreamEx(BYTE * pStream,int len,const char * headerFmt,...);

void WLOutDebugEclipse(const char * tag,const char * fmt,...);
void WLOutDebugStreamEclipse(const char * tag,BYTE * pStream,int len);
void WLOutDebugStreamEclipse(const char * tag,const char * header,BYTE * pStream,int len);
void WLOutDebugStreamExEclipse(const char * tag,BYTE * pStream,int len,const char * headerFmt,...);

//���º���Ϊ�ڲ������벻Ҫ����
void WLOutDefDebug(const char * outStr);
void WLOutEclipseDebug(const char * tag,const char * outStr);
class CWLOutDebug  
{
public:
	CWLOutDebug();
	virtual ~CWLOutDebug();

};

#endif // !defined(AFX_WLOUTDEBUG_H__2B7C1BC1_4509_4950_BA39_37AFC85903B7__INCLUDED_)
